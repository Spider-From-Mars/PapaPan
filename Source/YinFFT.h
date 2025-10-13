#pragma once

#include <JuceHeader.h>
#include <vector>

#include "RingBuffer.h"

class YinFFT
{
public:
    YinFFT(int frameSize, int hopSize);
    
    void prepare(double sampleRate_=44100, float threshold_=0.1, float fMin_=50, float fMax_=2000);
    
    int getFrameSize() const { return frameSize; }
    juce::Optional<float> getF0(RingBuffer& ringBuffer);
    
private:
    int frameSize;
    int hopSize;
    
    double sampleRate;
    float threshold;
    float fMin, fMax;
    
    int fftSize;
    juce::dsp::FFT fft;
    juce::HeapBlock<float> fftBuffer, cmndValues;
    
    juce::dsp::IIR::Filter<float> lowPassFilter, midPassFilter, highPassFilter;
    std::vector<juce::dsp::IIR::Filter<float>*> filters {&midPassFilter, &lowPassFilter, &highPassFilter};
    
    static int getFftOrder(int frameSize) { return juce::roundToInt(std::ceil(std::log2(frameSize * 2))); }
    
    juce::Optional<float> calculateF0(const float* frame);
    void ACF(const float* frame);
    void DF(const float* frame);
    void CMND();
    float minInterpolation(int tau);
};
