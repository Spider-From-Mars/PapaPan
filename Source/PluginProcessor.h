#pragma once

#include <JuceHeader.h>
#include "Panner.h"
#include "PitchDetector.h"

class PitchDetectionThread : juce::Thread
{
public:
    PitchDetectionThread() : juce::Thread("PitchDetector") {};
    ~PitchDetectionThread();
    
    void prepare(juce::dsp::ProcessSpec& spec);
    
    void run() override;
    void setBuffer(const juce::AudioBuffer<float>& buffer) { bufferToProcess.makeCopyOf(buffer); }
    
    float getPitch() const { return detectedPitch.load(std::memory_order_relaxed); }
    
private:
    PitchDetector pitchDetector;
    
    juce::AudioBuffer<float> bufferToProcess;
    std::atomic<float> detectedPitch = 0;
};

//==============================================================================
/**
*/
class PanCakeAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    PanCakeAudioProcessor();
    ~PanCakeAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    Modulation& getModulation() { return modulation; }
    
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioBuffer<float> sharedBuffer;
    juce::CriticalSection bufferLock;
    float pitch = 0;

private:
    Panner panner;
    PitchDetectionThread pitchThread;
    Modulation modulation;
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanCakeAudioProcessor)
};
