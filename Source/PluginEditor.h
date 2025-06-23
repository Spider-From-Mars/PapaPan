#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "BasePanel.h"
#include "BaseColours.h"

//==============================================================================
/**
*/
class PanCakeAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PanCakeAudioProcessorEditor (PanCakeAudioProcessor&);
    ~PanCakeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PanCakeAudioProcessor& audioProcessor;
    
    BasePanel basePanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanCakeAudioProcessorEditor)
};


