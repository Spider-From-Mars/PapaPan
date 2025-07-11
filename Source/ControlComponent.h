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
    int getrotatedLabelHeight() const {return rotatedLabelHeight;}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlComponent)
    
    juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor(
                                                                           BinaryData::Comic_Sans_MS_Bold_ttf,
                                                                           BinaryData::Comic_Sans_MS_Bold_ttfSize
                                                                           );
    
    const int rotatedLabelHeight = 25;

};


// LookAndFeel
//==============================================================================

class LookAndFeel : public juce::LookAndFeel_V4
{
public:
    LookAndFeel() : juce::LookAndFeel_V4() { setPopupMenuColors(); }
    
    void drawRotarySlider(
                            juce::Graphics& g,
                            int x, int y, int width, int height,
                            float sliderPosProportional,
                            float rotaryStartAngle,
                            float rotaryEndAngle,
                            juce::Slider& slider
                          ) override;
    
    void drawComboBox(juce::Graphics& g,
                      int width, int height,
                      bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box
                      ) override;
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;
    juce::Font getComboBoxFont(juce::ComboBox& box) override;
    void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override;
    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                           const bool isSeparator, const bool isActive,
                           const bool isHighlighted, const bool isTicked,
                           const bool hasSubMenu, const juce::String& text,
                           const juce::String& shortcutKeyText,
                           const juce::Drawable* icon, const juce::Colour* const textColourToUse) override;

private:
    void drawRotarySliderLabel(juce::Graphics& g, juce::Rectangle<float> bounds, const juce::String& displayString);
    void setPopupMenuColors();
    
    juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor(
                                                                           BinaryData::Comic_Sans_MS_Bold_ttf,
                                                                           BinaryData::Comic_Sans_MS_Bold_ttfSize
                                                                           );
};


// RotarySliderWithLabels
//==============================================================================

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


// DropdownMenu
//==============================================================================
class DropdownMenu : public juce::ComboBox
{
public:
    DropdownMenu(const juce::String& componentName = {}) : juce::ComboBox(componentName)
    {
        setLookAndFeel(&lnf);
    }
    
    ~DropdownMenu()
    {
        setLookAndFeel(nullptr);
    }

private:
    LookAndFeel lnf;
};
