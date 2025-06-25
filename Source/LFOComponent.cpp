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
    lfoSlider.setBounds(getLocalBounds());
}
