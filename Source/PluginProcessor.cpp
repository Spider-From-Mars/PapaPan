#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PanCakeAudioProcessor::PanCakeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
        apvts(*this, nullptr, "Parameters", createParameterLayout()),
        panner(modulation)
#endif
{
}

PanCakeAudioProcessor::~PanCakeAudioProcessor()
{
}

//==============================================================================
const juce::String PanCakeAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PanCakeAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PanCakeAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PanCakeAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PanCakeAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PanCakeAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PanCakeAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PanCakeAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PanCakeAudioProcessor::getProgramName (int index)
{
    return {};
}

void PanCakeAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PanCakeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
//    pitchThread.prepare(spec);
}

void PanCakeAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PanCakeAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PanCakeAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
//    pitchThread.setBuffer(buffer);
//    pitch = pitchThread.getPitch();
//    if (pitch != 2205)
//        juce::Logger::outputDebugString("Pitch: " + juce::String(pitch));
    
    
    // TODO: Here and in setStateInformation() make one function for this
    // TODO: Add handler when waveType and other changed
    auto waveIndex = static_cast<int>(*apvts.getRawParameterValue("WAVETYPE"));
    auto wave = static_cast<Panner::waveType>(waveIndex);
    panner.update(
                  *apvts.getRawParameterValue("MIX"),
                  wave,
                  *apvts.getRawParameterValue("MODE"),
                  *apvts.getRawParameterValue("DURATION"),
                  *apvts.getRawParameterValue("HERTZRATE"),
                  *apvts
                  .getRawParameterValue("MODE") == Modes::Beat_Synced ? getCurrentBpm() : 120.0
                );
    panner.process(buffer);
}

//==============================================================================
bool PanCakeAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* PanCakeAudioProcessor::createEditor()
{
    return new PanCakeAudioProcessorEditor (*this);
}

//==============================================================================
void PanCakeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void PanCakeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    
    if (tree.isValid())
    {
        apvts.replaceState(tree);
        auto waveIndex = static_cast<int>(*apvts.getRawParameterValue("WAVETYPE"));
        auto wave = static_cast<Panner::waveType>(waveIndex);
        panner.update(
                      *apvts.getRawParameterValue("MIX"),
                      wave,
                      *apvts.getRawParameterValue("MODE"),
                      *apvts.getRawParameterValue("DURATION"),
                      *apvts.getRawParameterValue("HERTZRATE"),
                      *apvts
                      .getRawParameterValue("MODE") == Modes::Beat_Synced ? getCurrentBpm() : 120.0
                    );
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PanCakeAudioProcessor();
}


juce::AudioProcessorValueTreeState::ParameterLayout PanCakeAudioProcessor::createParameterLayout()
{
    using namespace juce;
    
    AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>( ParameterID("MIX", 1),
                                                            "Mix",
                                                            juce::NormalisableRange<float>(0.f, 100.f, 1.f, 1.f),
                                                            100.f));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(ParameterID("WAVETYPE", 1),
                                                            "Wave Type",
                                                            juce::StringArray {"Sine", "Triangle"},
                                                            0
                                                            ));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID("HERTZRATE", 1),
                                                           "Speed",
                                                           juce::NormalisableRange<float>(0.f, 100.f, 0.01f, 0.4f),
                                                           0));
    
    juce::StringArray modeChoices = {"Hertz Synced", "Beat Synced"};
    layout.add(std::make_unique<juce::AudioParameterChoice>(ParameterID("MODE", 1),
                                                            "Mode",
                                                            modeChoices,
                                                            1));
    
    juce::StringArray noteDurationChoices = {
        "32 Bars", "16 Bars", "8 Bars", "4 Bars",
        "3 Bars", "2 Bars", "1.5 Bars", "1 Bar",
        "3/4", "1/2", "3/8", "1/4", "3/16", "1/8", "1/16",
        "1/32", "1/64", "1/128"
    };
    layout.add(std::make_unique<juce::AudioParameterChoice>(ParameterID("DURATION", 1),
                                                            "Note Duration",
                                                            noteDurationChoices,
                                                            10));
    
    return layout;
}


double PanCakeAudioProcessor::getCurrentBpm()
{
    if (auto* playHead = getPlayHead())
    {
        auto posInfo = playHead->getPosition();
        auto bpm = posInfo->getBpm();
        if (bpm)
            return *bpm;
    }
    
    return 120.0;
}

// ============== PitchDetectionThread ==============

PitchDetectionThread::~PitchDetectionThread()
{
    signalThreadShouldExit();
    stopThread(1000);
}

void PitchDetectionThread::prepare(juce::dsp::ProcessSpec &spec)
{
    pitchDetector.prepare(spec);
    startThread();
}

void PitchDetectionThread::run()
{
    while (!threadShouldExit()) {
        if (bufferToProcess.getNumSamples() > 0)
        {
            float pitch = pitchDetector.detectPitch(bufferToProcess.getReadPointer(0),
                                                    20,
                                                    bufferToProcess.getNumSamples() / 2);
            detectedPitch.store(pitch, std::memory_order_relaxed);
            bufferToProcess.clear();
        }
        wait(50);
    }
}
