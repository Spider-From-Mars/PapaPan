#include "ControlComponent.h"

// ControlComponent
//==============================================================================

void ControlComponent::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour(BaseColours::darkPink);
    g.fillRoundedRectangle(bounds, 10);

    g.setColour(BaseColours::white);
    auto line =
        juce::Rectangle<float>(bounds.getRight() - 25, bounds.getY(), 1.5, bounds.getHeight());
    g.fillRect(line);
}

void ControlComponent::resized() {}

void ControlComponent::setRotatedLabel(juce::Graphics &g, const juce::String &title)
{
    juce::Graphics::ScopedSaveState state(g);

    auto bounds = getLocalBounds().toFloat();

    float pad = rotatedLabelHeight / 2;
    float deltaX = bounds.getWidth() / 2 - pad;
    g.addTransform(juce::AffineTransform::rotation(juce::MathConstants<float>::halfPi,
                                                   bounds.getCentre().getX(),
                                                   bounds.getCentre().getY())
                       .translated(deltaX, 0));

    static juce::FontOptions comicNeueOptions{typeface->getName(), 19, juce::Font::bold};

    g.setColour(BaseColours::white);
    g.setFont(juce::Font(comicNeueOptions));
    g.drawText(title, bounds, juce::Justification::centred, true);
}

// DropdownMenuLookAndFeel
//==============================================================================

void DropdownMenuLookAndFeel::drawComboBox(juce::Graphics &g, int width, int height,
                                           bool isButtonDown, int buttonX, int buttonY, int buttonW,
                                           int buttonH, juce::ComboBox &box)
{
    g.fillAll(juce::Colours::transparentBlack);
}

juce::Font DropdownMenuLookAndFeel::getComboBoxFont(juce::ComboBox &box)
{
    juce::FontOptions comicNeueOptions{typeface->getName(), 17, juce::Font::bold};

    return juce::Font(comicNeueOptions);
}

void DropdownMenuLookAndFeel::positionComboBoxText(juce::ComboBox &box, juce::Label &label)
{
    label.setBounds(box.getLocalBounds());
    label.setFont(getComboBoxFont(box));
    label.setJustificationType(juce::Justification::centred);

    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    label.setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
}

void DropdownMenuLookAndFeel::drawPopupMenuBackground(juce::Graphics &g, int width, int height)
{
    g.setColour(BaseColours::transparentPink);
    g.fillRoundedRectangle(0, 0, width, height, 10);

    g.setColour(BaseColours::white);
    g.drawRoundedRectangle(0, 0, width, height, 8, 2);
}

void DropdownMenuLookAndFeel::setPopupMenuColors()
{
    setColour(juce::PopupMenu::highlightedBackgroundColourId, BaseColours::darkPink);
    setColour(juce::PopupMenu::backgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::PopupMenu::textColourId, BaseColours::white);
}

void DropdownMenuLookAndFeel::drawPopupMenuItem(juce::Graphics &g, const juce::Rectangle<int> &area,
                                                const bool isSeparator, const bool isActive,
                                                const bool isHighlighted, const bool isTicked,
                                                const bool hasSubMenu, const juce::String &text,
                                                const juce::String &shortcutKeyText,
                                                const juce::Drawable *icon,
                                                const juce::Colour *const textColourToUse)
{
    if (isSeparator)
    {
        auto r = area.reduced(5, 0).toFloat();
        r.removeFromTop(juce::roundToInt((r.getHeight() * 0.5) - 0.5));

        g.setColour(findColour(juce::PopupMenu::textColourId).withAlpha(0.8f));
        g.fillRect(r.removeFromTop(1));
    }
    else
    {
        juce::LookAndFeel_V4::drawPopupMenuItem(g, area, isSeparator, isActive, isHighlighted,
                                                isTicked, hasSubMenu, text, shortcutKeyText, icon,
                                                textColourToUse);
    }
}
