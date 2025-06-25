#include <JuceHeader.h>
#include "BasePanel.h"
#include "Utils.h"

//==============================================================================
BasePanel::BasePanel(juce::AudioProcessorValueTreeState& apvts) :
        lfoComponent(apvts),
        masterComponent(apvts)
{
    setMainStyleLabel(titleLabel, 25);
    
    companyLabel.setFont(juce::FontOptions(18));
    addAndMakeVisible(companyLabel);
    
    addAndMakeVisible(lfoComponent);
    addAndMakeVisible(midiComponent);
    addAndMakeVisible(masterComponent);
}

BasePanel::~BasePanel()
{
}

void BasePanel::paint (juce::Graphics& g)
{
    const int rectWidth = 185;
    const int x = (getWidth() - rectWidth) / 2;
    
    g.setColour(BaseColours::darkPink);
    
    g.fillRoundedRectangle(x, getHeight() - 190, rectWidth, 160, 10);
}

void BasePanel::resized()
{
    const float titleLabelWidth = juce::GlyphArrangement::getStringWidth(
                                                                         titleLabel.getFont(),
                                                                         titleLabel.getText()
                                                                         );
    const auto companyLabelWidth = juce::GlyphArrangement::getStringWidth(
                                                                          companyLabel.getFont(),
                                                                          companyLabel.getText()
                                                                         );
    
    titleLabel.setBounds(
                       (getWidth() - titleLabelWidth) / 2,
                       500,
                       titleLabelWidth,
                       titleLabel.getFont().getHeight()
                   );
    companyLabel.setBounds(
                       (getWidth() - companyLabelWidth) / 2,
                       titleLabel.getY() + 25,
                       companyLabelWidth,
                       companyLabel.getFont().getHeight()
                   );
    
    const int controlComponentWidth = 185;
    const int x = (getWidth() - controlComponentWidth) / 2;
    const int pad = 20;
    
    lfoComponent.setBounds(x, 10, controlComponentWidth, 100);
    midiComponent.setBounds(x, lfoComponent.getBottom() + pad, controlComponentWidth, 65);
    masterComponent.setBounds(
                              x,
                              midiComponent.getBottom() + pad,
                              controlComponentWidth,
                              110
                            );
}
 

void BasePanel::setMainStyleLabel(juce::Label& label, float fontSize)
{
    label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    juce::FontOptions comicSansOptions {
        typeface->getName(),
        fontSize,
        juce::Font::bold
    };
    label.setFont(comicSansOptions);
    label.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(label);
}
