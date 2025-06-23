#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class SliderComponent  : public juce::Component
{
public:
    SliderComponent();
    ~SliderComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderComponent)
};
