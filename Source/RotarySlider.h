#pragma once

#include "BaseColours.h"
#include <JuceHeader.h>

class RotarySliderLookAndFeel : public juce::LookAndFeel_V4
{
  public:
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider &slider) override;

  private:
    void drawRotarySliderLabel(juce::Graphics &g, juce::Rectangle<float> bounds,
                               const juce::String &displayString);

    juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor(
        BinaryData::ComicNeueBold_ttf, BinaryData::ComicNeueBold_ttfSize);
};

class RotarySliderWithLabels : public juce::Slider
{
  public:
    RotarySliderWithLabels(juce::RangedAudioParameter &rap, const juce::String &unitSuffix)
        : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag,
                       juce::Slider::TextEntryBoxPosition::NoTextBox),
          param(&rap), suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }

    ~RotarySliderWithLabels() { setLookAndFeel(nullptr); }

    void paint(juce::Graphics &g) override;
    bool hitTest(int x, int y) override;
    juce::Rectangle<int> getSliderBounds() const;
    juce::String getDisplayString() const;

  private:
    RotarySliderLookAndFeel lnf;

    juce::RangedAudioParameter *param;
    juce::String suffix;
};
