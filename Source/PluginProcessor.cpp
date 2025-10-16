#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PanCakeAudioProcessor::PanCakeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                         ),
      apvts(*this, nullptr, "Parameters", createParameterLayout()), pitchDetector(3 * 2048, 512),
      ringBuffer(pitchDetector.getFrameSize() * 4), pitchThread(pitchDetector, ringBuffer)
#endif
{
}

PanCakeAudioProcessor::~PanCakeAudioProcessor() {}

//==============================================================================
const juce::String PanCakeAudioProcessor::getName() const { return JucePlugin_Name; }

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

double PanCakeAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int PanCakeAudioProcessor::getNumPrograms() { return 1; }

int PanCakeAudioProcessor::getCurrentProgram() { return 0; }

void PanCakeAudioProcessor::setCurrentProgram(int index) {}

const juce::String PanCakeAudioProcessor::getProgramName(int index) { return {}; }

void PanCakeAudioProcessor::changeProgramName(int index, const juce::String &newName) {}

//==============================================================================
void PanCakeAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    pitchThread.prepare();
    pitchDetector.prepare(sampleRate, 0.15);
}

void PanCakeAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PanCakeAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void PanCakeAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                         juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    const juce::ScopedLock sl(bufferLock);
    sharedBuffer.makeCopyOf(buffer);

    fillRingBuffer(buffer);

    if (auto *playHead = getPlayHead())
    {
        if (auto pos = playHead->getPosition())
        {
            panner.update(apvts, *pos, pitchThread.getPitch());
            panner.process(buffer);
        }
    }
}

//==============================================================================
bool PanCakeAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor *PanCakeAudioProcessor::createEditor()
{
    return new PanCakeAudioProcessorEditor(*this);
}

//==============================================================================
void PanCakeAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void PanCakeAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);

    if (tree.isValid())
    {
        apvts.replaceState(tree);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() { return new PanCakeAudioProcessor(); }

juce::AudioProcessorValueTreeState::ParameterLayout PanCakeAudioProcessor::createParameterLayout()
{
    using namespace juce;

    AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID("MIX", 1), "Mix", NormalisableRange<float>(0, 100.f, 1.f, 1.f), 100.f));

    layout.add(std::make_unique<AudioParameterChoice>(ParameterID("WAVETYPE", 1), "Wave Type",
                                                      StringArray{"Sine", "Triangle"}, 0));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID("HERTZRATE", 1), "Speed", NormalisableRange<float>(0, 100.f, 0.01f, 0.4f), 0));

    StringArray modeChoices = {"Hertz Retrig.", "Beat Retrig.", "Hertz Synced", "Beat Synced",
                               "Pitch -> Rate"};
    layout.add(
        std::make_unique<AudioParameterChoice>(ParameterID("MODE", 1), "Mode", modeChoices, 1));

    StringArray noteDurationChoices = {
        "32 Bars", "16 Bars", "8 Bars", "4 Bars", "3 Bars", "2 Bars", "1.5 Bars", "1 Bar", "3/4",
        "1/2",     "3/8",     "1/4",    "3/16",   "1/8",    "1/16",   "1/32",     "1/64",  "1/128"};
    layout.add(std::make_unique<AudioParameterChoice>(ParameterID("DURATION", 1), "Note Duration",
                                                      noteDurationChoices, 10));

    return layout;
}

void PanCakeAudioProcessor::fillRingBuffer(const juce::AudioBuffer<float> &buffer)
{
    const int numSamples = buffer.getNumSamples();
    auto *left = buffer.getReadPointer(0);
    auto *right = buffer.getReadPointer(1);

    if (buffer.getNumChannels() > 1)
    {
        std::vector<float> mono(numSamples);

        for (int i = 0; i < numSamples; i++)
        {
            mono[i] = (left[i] + right[i]) / 2;
        }

        ringBuffer.push(mono.data(), numSamples);
    }
    else
        ringBuffer.push(left, numSamples);
}
