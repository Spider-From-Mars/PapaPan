#pragma once

#include "BaseColours.h"
#include "BasePanel.h"
#include "PlotComponent.h"
#include "PluginProcessor.h"
#include <JuceHeader.h>

class PanCakeAudioProcessorEditor : public juce::AudioProcessorEditor
{
  public:
    PanCakeAudioProcessorEditor(PanCakeAudioProcessor &);
    ~PanCakeAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

  private:
    PanCakeAudioProcessor &audioProcessor;

    BasePanel basePanel;
    PlotComponent plot;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PanCakeAudioProcessorEditor)
};
