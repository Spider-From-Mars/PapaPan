#pragma once

#include "ControlComponent.h"
#include "RotarySlider.h"
#include <JuceHeader.h>

class MasterComponent : public ControlComponent
{
  public:
    MasterComponent(juce::AudioProcessorValueTreeState &apvts);
    ~MasterComponent() override;

    void paint(juce::Graphics &) override;
    void resized() override;

  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterComponent)

    RotarySliderWithLabels mixSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixSliderAttachment;
};
