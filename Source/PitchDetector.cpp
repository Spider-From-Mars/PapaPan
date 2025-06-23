#include "PitchDetector.h"

void PitchDetector::prepare(juce::dsp::ProcessSpec& spec)
{
    jassert (spec.sampleRate > 0);
    jassert (spec.numChannels > 0);

    sampleRate = spec.sampleRate;
}

float PitchDetector::detectPitch(const float* audioData, int startBound, int endBound) {
    int size = endBound - startBound;
    std::vector<float> CMNDFValues(size, 0.0);
    
    long sample = -1;
    
    for (int i = startBound; i < endBound; i++)
    {
        float currentVal = CMNDF(audioData, i - startBound);
        CMNDFValues[i - startBound] = currentVal;
        
        if (currentVal < threshold && sample == -1)
        {
            sample = i + startBound;
            break;
        }
    }
    
    if (sample == -1)
    {
        auto it = std::min_element(CMNDFValues.begin(), CMNDFValues.end());
        sample = std::distance(CMNDFValues.begin(), it) + startBound;
    }
    
    return sampleRate / sample;
}

float PitchDetector::DF(const float *audioData, int lag)
{
    float sum = 0;
    
    for (int j = 0; j < windowSize; j++)
    {
        float delta = audioData[j] - audioData[j + lag];
        sum += delta * delta;
    }
    
    return sum;
}


float PitchDetector::CMNDF(const float *audioData, int lag)
{
    if (lag == 0)
        return 1;
    
    float DFSum = 0;
    for (int j = 0; j < lag; j++)
    {
        DFSum += DF(audioData, j + 1);
    }
    
    return DF(audioData, lag) / DFSum * lag;
}
