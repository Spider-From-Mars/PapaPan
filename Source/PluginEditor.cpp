#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PanCakeAudioProcessorEditor::PanCakeAudioProcessorEditor (PanCakeAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p), basePanel(audioProcessor.apvts), plot(audioProcessor.apvts)
{
    addAndMakeVisible(basePanel);
    addAndMakeVisible(plot);
        
    setSize(530, 630);
}

PanCakeAudioProcessorEditor::~PanCakeAudioProcessorEditor()
{
}

//==============================================================================
void PanCakeAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(BaseColours::basePink);
}

void PanCakeAudioProcessorEditor::resized()
{
    basePanel.setBounds(0, 0, 220, getHeight());
    plot.setBounds(basePanel.getWidth(),
                   0,
                   getWidth() - basePanel.getWidth() - 8,
                   getHeight());
}
