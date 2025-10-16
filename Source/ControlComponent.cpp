#include <JuceHeader.h>
#include "ControlComponent.h"
#include "BaseColours.h"

//==============================================================================
ControlComponent::ControlComponent()
{

}

ControlComponent::~ControlComponent()
{
}

void ControlComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour(BaseColours::darkPink);
    g.fillRoundedRectangle(bounds, 10);
    
    g.setColour(BaseColours::white);
    auto line = juce::Rectangle<float>(bounds.getRight() - 25, bounds.getY(), 1.5, bounds.getHeight());
    g.fillRect(line);
}

void ControlComponent::resized()
{
    
}

void ControlComponent::setRotatedLabel(juce::Graphics& g, juce::String title)
{
    juce::Graphics::ScopedSaveState state(g);
    
    auto localBounds = getLocalBounds().toFloat();
    
    float pad = rotatedLabelHeight / 2;
    float deltaX = localBounds.getWidth() / 2 - pad;
    g.addTransform(
        juce::AffineTransform::rotation(
                                        juce::MathConstants<float>::halfPi,
                                        localBounds.getCentre().getX(),
                                        localBounds.getCentre().getY()
                                    ).translated(deltaX, 0)
    );
    
    juce::FontOptions comicSansOptions {
        typeface->getName(),
        19,
        juce::Font::bold
    };
    
    g.setColour(BaseColours::white);
    g.setFont(juce::Font(comicSansOptions));
    g.drawText(
        title,
        localBounds,
        juce::Justification::centred,
        true
    );
}


// RotarySlider LookAndFeel
//==============================================================================

void LookAndFeel::drawRotarySlider(juce::Graphics& g,
                                   int x, int y, int width, int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   juce::Slider& slider)
{
    using namespace juce;
    
    auto bounds = Rectangle<float>(x, y, width, height);
    
    Path p;
    auto center = bounds.getCentre();
    
    const float borderThickness = 5.f;
    const auto arcRadius = bounds.getHeight() / 2;
    
    p.addCentredArc(
                    center.getX(),
                    center.getY(),
                    arcRadius,
                    arcRadius,
                    0.0f,
                    rotaryStartAngle,
                    rotaryEndAngle,
                    true);
    g.setColour(BaseColours::yellow);
    g.strokePath(p, juce::PathStrokeType(borderThickness, PathStrokeType::curved, PathStrokeType::rounded));
    
    g.setColour(BaseColours::basePink);
    g.fillEllipse(bounds.reduced(borderThickness));
    
    g.setColour(BaseColours::white);
    g.drawEllipse(bounds.reduced(borderThickness), 5);
    
    if (auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider))
    {
        p.clear();
        
        Rectangle<float> r;
        r.setLeft(center.getX() - 2);
        r.setRight(center.getX() + 2);
        r.setTop(bounds.reduced(borderThickness).getY());
        r.setBottom(center.getY());
        
        p.addRectangle(r);
        
        jassert(rotaryStartAngle < rotaryEndAngle);
        
        auto sliderAngRad = jmap(sliderPosProportional,
                                 0.f,
                                 1.f,
                                 rotaryStartAngle,
                                 rotaryEndAngle);
        
        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));
        
        g.setColour(BaseColours::white);
        g.fillPath(p);
        
        drawRotarySliderLabel(g, bounds, rswl->getDisplayString());
    }
}


void LookAndFeel::drawRotarySliderLabel(juce::Graphics& g, juce::Rectangle<float> bounds, const juce::String& displayString)
{
    juce::Graphics::ScopedSaveState state(g);
    
    juce::FontOptions comicSansOptions {
        typeface->getName(),
        15,
        juce::Font::bold
    };
    auto font = juce::Font(comicSansOptions);
    
    g.setColour(BaseColours::white);
    
    auto getMaxLineWidth = [&font](const juce::String& text)
    {
        juce::StringArray lines;
        lines.addLines(text);
        float maxWidth = 0;
        
        for (auto line : lines)
        {
            float lineWidth = juce::GlyphArrangement::getStringWidth(font, line);
            if (lineWidth > maxWidth)
            {
                maxWidth = lineWidth;
            }
        }
        
        return maxWidth;
    };
    
    const float maxLineWidth = getMaxLineWidth(displayString);
    juce::GlyphArrangement ga;
    ga.addJustifiedText(
                        font,
                        displayString,
                        bounds.getCentre().getX() - maxLineWidth / 2,
                        bounds.getBottom() + comicSansOptions.getHeight(),
                        maxLineWidth,
                        juce::Justification::centred
                    );
    ga.draw(g);
}


// RotarySliderWithLabels
//==============================================================================

void RotarySliderWithLabels::paint(juce::Graphics &g)
{
    using namespace juce;
    
    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;
    
    auto range = getRange();
    
    auto sliderBounds = getSliderBounds();
    
    getLookAndFeel().drawRotarySlider(g,
                                      sliderBounds.getX(),
                                      sliderBounds.getY(),
                                      sliderBounds.getWidth(),
                                      sliderBounds.getHeight(),
                                      jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                      startAng,
                                      endAng,
                                      *this);
}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    size -= size * 0.5;
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), bounds.getCentreY() - 15);
    
    return r;
}

bool RotarySliderWithLabels::hitTest(int x, int y)
{
    juce::Point<float> mousePos(x, y);
    juce::Point<float> center = getSliderBounds().getCentre().toFloat();
    float radius = getSliderBounds().getWidth() / 2;

    return mousePos.getDistanceFrom(center) <= radius;
}

juce::String RotarySliderWithLabels::getDisplayString() const
{
    juce::String str;
    
    if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
    {
        str << param->name.toUpperCase() << "\n" << getValue() << suffix;
    }
    else
    {
        jassertfalse;
    }
    
    return str;
}

// DropdownMenu LookAndFeel
//==============================================================================
void LookAndFeel::drawComboBox(juce::Graphics& g,
                               int width, int height,
                               bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH,
                               juce::ComboBox& box
                               )
{
    g.fillAll(juce::Colours::transparentBlack);
}

juce::Font LookAndFeel::getComboBoxFont(juce::ComboBox& box)
{
    juce::FontOptions comicSansOptions {
        typeface->getName(),
        17,
        juce::Font::bold
    };
    
    return juce::Font(comicSansOptions);
}

void LookAndFeel::positionComboBoxText(juce::ComboBox& box, juce::Label& label)
{
    label.setBounds(box.getLocalBounds());
    label.setFont(getComboBoxFont(box));
    label.setJustificationType(juce::Justification::centred);
    
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    label.setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
}

void LookAndFeel::drawPopupMenuBackground(juce::Graphics &g, int width, int height)
{
    g.setColour(BaseColours::transparentPink);
    g.fillRoundedRectangle(0, 0, width, height, 10.f);
    
    g.setColour(BaseColours::white);
    g.drawRoundedRectangle(0, 0, width, height, 8.f, 2);
}

void LookAndFeel::setPopupMenuColors()
{
    setColour(juce::PopupMenu::highlightedBackgroundColourId, BaseColours::darkPink);
    setColour(juce::PopupMenu::backgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::PopupMenu::textColourId, BaseColours::white);
}

void LookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                       const bool isSeparator, const bool isActive,
                       const bool isHighlighted, const bool isTicked,
                       const bool hasSubMenu, const juce::String& text,
                       const juce::String& shortcutKeyText,
                       const juce::Drawable* icon, const juce::Colour* const textColourToUse)
{
    if (isSeparator)
    {
        auto r = area.reduced(5, 0);
        r.removeFromTop(juce::roundToInt(((float) r.getHeight() * 0.5f) - 0.5f));
        
        g.setColour(findColour(juce::PopupMenu::textColourId).withAlpha(0.8f));
        g.fillRect(r.removeFromTop(1));
    }
    else
    {
        juce::LookAndFeel_V4::drawPopupMenuItem(g, area, isSeparator, isActive, isHighlighted,
                                                isTicked, hasSubMenu, text, shortcutKeyText,
                                                icon, textColourToUse);
    }
}

// DropdownMenu
//==============================================================================

