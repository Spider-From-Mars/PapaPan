#pragma once

#include <JuceHeader.h>
#include "ControlComponent.h"

//==============================================================================
/*
*/
class LFOComponent  : public ControlComponent
{
public:
    LFOComponent(juce::AudioProcessorValueTreeState& apvts);
    ~LFOComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOComponent)
    
    RotarySliderWithLabels lfoSlider;
    DropdownMenu modeSelector;
    DropdownMenu durationSelector;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeSelectorAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> durationSelectorAttachment;
};
