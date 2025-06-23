#include <JuceHeader.h>
#include "MidiComponent.h"

//==============================================================================
MidiComponent::MidiComponent()
{

}

MidiComponent::~MidiComponent()
{
}

void MidiComponent::paint (juce::Graphics& g)
{
    ControlComponent::paint(g);
    setRotatedLabel(g, "MIDI");
}

void MidiComponent::resized()
{

}
