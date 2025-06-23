#pragma once

#include <JuceHeader.h>
#include "ControlComponent.h"

//==============================================================================
/*
*/
class MasterComponent  : public ControlComponent
{
public:
    MasterComponent(juce::AudioProcessorValueTreeState& apvts);
    ~MasterComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MasterComponent)
    
    RotarySliderWithLabels mixSlider;
    
    juce::AudioProcessorValueTreeState::SliderAttachment mixSliderAttachment;
};
