#pragma once

#include "BaseColours.h"
#include <JuceHeader.h>

class ControlComponent : public juce::Component
{
  public:
    ControlComponent() {};
    ~ControlComponent() override {};

    void paint(juce::Graphics &) override;
    void resized() override;
    void setRotatedLabel(juce::Graphics &g, const juce::String &title);

  protected:
    static constexpr int rotatedLabelHeight = 25;

  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlComponent)

    juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor(
        BinaryData::ComicNeueBold_ttf, BinaryData::ComicNeueBold_ttfSize);
};

// DropdownMenuLookAndFeel
//==============================================================================

class DropdownMenuLookAndFeel : public juce::LookAndFeel_V4
{
  public:
    DropdownMenuLookAndFeel() : juce::LookAndFeel_V4() { setPopupMenuColors(); }

    void drawComboBox(juce::Graphics &g, int width, int height, bool isButtonDown, int buttonX,
                      int buttonY, int buttonW, int buttonH, juce::ComboBox &box) override;
    void drawPopupMenuBackground(juce::Graphics &g, int width, int height) override;
    juce::Font getComboBoxFont(juce::ComboBox &box) override;
    void positionComboBoxText(juce::ComboBox &box, juce::Label &label) override;
    void drawPopupMenuItem(juce::Graphics &g, const juce::Rectangle<int> &area,
                           const bool isSeparator, const bool isActive, const bool isHighlighted,
                           const bool isTicked, const bool hasSubMenu, const juce::String &text,
                           const juce::String &shortcutKeyText, const juce::Drawable *icon,
                           const juce::Colour *const textColourToUse) override;

  private:
    void setPopupMenuColors();

    juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor(
        BinaryData::ComicNeueBold_ttf, BinaryData::ComicNeueBold_ttfSize);
};

// DropdownMenu
//==============================================================================
class DropdownMenu : public juce::ComboBox
{
  public:
    DropdownMenu(const juce::String &componentName = {}) : juce::ComboBox(componentName)
    {
        setLookAndFeel(&lnf);
    }

    ~DropdownMenu() { setLookAndFeel(nullptr); }

  private:
    DropdownMenuLookAndFeel lnf;
};
