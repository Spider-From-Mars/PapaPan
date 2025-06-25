#pragma once

#include <JuceHeader.h>
#include "Utils.h"

//==============================================================================
/*
*/
class ControlComponent  : public juce::Component
{
public:
    ControlComponent();
    ~ControlComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void setRotatedLabel(juce::Graphics& g, juce::String title);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlComponent)
    
    juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor(
                                                                           BinaryData::Comic_Sans_MS_Bold_ttf,
                                                                           BinaryData::Comic_Sans_MS_Bold_ttfSize
                                                                           );

};


// RotarySliderWithLabels
//==============================================================================

class LookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(
                            juce::Graphics& g,
                            int x, int y, int width, int height,
                            float sliderPosProportional,
                            float rotaryStartAngle,
                            float rotaryEndAngle,
                            juce::Slider& slider
                          ) override;
    
private:
    void drawRotarySliderLabel(juce::Graphics& g, juce::Rectangle<float> bounds, const juce::String& displayString);
    
    juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor(
                                                                           BinaryData::Comic_Sans_MS_Bold_ttf,
                                                                           BinaryData::Comic_Sans_MS_Bold_ttfSize
                                                                           );
};


class RotarySliderWithLabels : public juce::Slider
{
public: 
    RotarySliderWithLabels(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(&rap),
        suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }
    
    ~RotarySliderWithLabels()
    {
        setLookAndFeel(nullptr);
    }
    
    void paint(juce::Graphics& g) override;
    bool hitTest(int x, int y) override;
    juce::Rectangle<int> getSliderBounds() const;
    juce::String getDisplayString() const;
    
private:
    LookAndFeel lnf;
    
    juce::RangedAudioParameter* param;
    juce::String suffix;
};
