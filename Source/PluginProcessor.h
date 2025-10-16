#pragma once

#include <JuceHeader.h>

#include "Panner.h"
#include "PitchThread.h"
#include "RingBuffer.h"
#include "YinFFT.h"


class PanCakeAudioProcessor : public juce::AudioProcessor
{
  public:
    //==============================================================================
    PanCakeAudioProcessor();
    ~PanCakeAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    //==============================================================================
    juce::AudioProcessorEditor *createEditor() override;
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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String &newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    const Panner &getPanner() const { return panner; }

    juce::AudioProcessorValueTreeState apvts;
    juce::AudioBuffer<float> sharedBuffer;
    juce::CriticalSection bufferLock;

  private:
    YinFFT pitchDetector;
    RingBuffer ringBuffer;
    PitchDetectionThread pitchThread;
    Panner panner;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void fillRingBuffer(const juce::AudioBuffer<float> &buffer);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PanCakeAudioProcessor)
};
