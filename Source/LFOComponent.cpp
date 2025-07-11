#include <JuceHeader.h>
#include "LFOComponent.h"

//==============================================================================
LFOComponent::LFOComponent(juce::AudioProcessorValueTreeState& apvts) :
        lfoSlider(*apvts.getParameter("HERTZRATE"), "HZ")
{
    juce::AudioParameterChoice* mode = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("MODE"));
    modeSelector.addItemList(mode->choices, 2);
    modeSelector.addSeparator();
    
    lfoSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "HERTZRATE", lfoSlider);
    modeSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts,
                                                                                                      "MODE",
                                                                                                      modeSelector);
    
    addAndMakeVisible(lfoSlider);
    addAndMakeVisible(modeSelector);
}

LFOComponent::~LFOComponent()
{
}

void LFOComponent::paint (juce::Graphics& g)
{
    ControlComponent::paint(g);
    setRotatedLabel(g, "LFO");
}

void LFOComponent::resized()
{
    auto bounds = getLocalBounds();
    auto activeArea = bounds.withTrimmedRight(getrotatedLabelHeight());
    const float sliderWidth = activeArea.getWidth() / 2 - 10;
    
    lfoSlider.setBounds(activeArea.removeFromRight(sliderWidth));
    
    activeArea.setHeight(20);
    activeArea.setCentre(activeArea.getWidth() / 2, bounds.getCentreY() - 15);
    modeSelector.setBounds(activeArea);
}
