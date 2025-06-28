#include <JuceHeader.h>
#include "MasterComponent.h"

//==============================================================================
MasterComponent::MasterComponent(juce::AudioProcessorValueTreeState& apvts) :
        mixSlider(*apvts.getParameter("MIX"), "%")
{
    mixSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "MIX", mixSlider);
    
    addAndMakeVisible(mixSlider);
    
}

MasterComponent::~MasterComponent()
{
}

void MasterComponent::paint (juce::Graphics& g)
{
    ControlComponent::paint(g);
    setRotatedLabel(g, "MASTER");
}

void MasterComponent::resized()
{
    mixSlider.setBounds(getLocalBounds());
}
