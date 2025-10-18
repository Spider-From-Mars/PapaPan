#include "BasePanel.h"

BasePanel::BasePanel(juce::AudioProcessorValueTreeState &apvts)
    : lfoComponent(apvts), masterComponent(apvts)
{
    setMainStyleLabel(titleLabel, 25);

    companyLabel.setFont(juce::FontOptions(18));
    addAndMakeVisible(companyLabel);

    addAndMakeVisible(lfoComponent);
    addAndMakeVisible(masterComponent);
}

BasePanel::~BasePanel() {}

void BasePanel::paint(juce::Graphics &g)
{
    g.setColour(BaseColours::darkPink);

    g.fillRoundedRectangle((getWidth() - roundedRectWidth) / 2, getHeight() - 190, roundedRectWidth,
                           160, 10);
}

void BasePanel::resized()
{
    static float titleLabelWidth =
        juce::GlyphArrangement::getStringWidth(titleLabel.getFont(), titleLabel.getText());
    static float companyLabelWidth =
        juce::GlyphArrangement::getStringWidth(companyLabel.getFont(), companyLabel.getText());

    titleLabel.setBounds((getWidth() - titleLabelWidth) / 2, 500, titleLabelWidth,
                         titleLabel.getFont().getHeight());
    companyLabel.setBounds((getWidth() - companyLabelWidth) / 2, titleLabel.getY() + 25,
                           companyLabelWidth, companyLabel.getFont().getHeight());

    static float x = (getWidth() - roundedRectWidth) / 2;
    lfoComponent.setBounds(x, 10, roundedRectWidth, 100);
    masterComponent.setBounds(x, lfoComponent.getBottom() + 20, roundedRectWidth, 110);
}

void BasePanel::setMainStyleLabel(juce::Label &label, float fontSize)
{
    label.setColour(juce::Label::ColourIds::textColourId, BaseColours::white);
    juce::FontOptions comicNeueOptions{typeface->getName(), fontSize, juce::Font::bold};
    label.setFont(comicNeueOptions);
    label.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(label);
}
