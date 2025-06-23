#pragma once
#include <JuceHeader.h>
#include <numeric>

class PitchDetector
{
public:
    void prepare(juce::dsp::ProcessSpec& spec);
    float detectPitch(const float* audioData, int startBound, int endBound);
    
private:
    double sampleRate = 44100.0;
    int windowSize = 1024;
    float threshold = 0.15;
    
    float DF(const float *audioData, int lag);
    float CMNDF(const float *audioData, int lag);    
};
