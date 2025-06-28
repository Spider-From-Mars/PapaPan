#include <JuceHeader.h>
#include "LFOComponent.h"

//==============================================================================
LFOComponent::LFOComponent(juce::AudioProcessorValueTreeState& apvts) :
        lfoSlider(*apvts.getParameter("HERTZRATE"), "HZ"),
        lfoSliderAttachment(apvts, "HERTZRATE", lfoSlider)
{
    addAndMakeVisible(lfoSlider);
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
    float sliderWidth = (bounds.getWidth() - getrotatedLabelHeight()) / 2;
    lfoSlider.setBounds(
                        bounds
                        .withTrimmedRight(getrotatedLabelHeight())
                        .removeFromRight(sliderWidth)
                    );
}
