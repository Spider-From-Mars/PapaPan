#include <cmath>

#include "YinFFT.h"

YinFFT::YinFFT(int frameSize, int hopSize) :
    frameSize(frameSize),
    hopSize(hopSize),
    fftSize(1 << getFftOrder(frameSize)),
    fft(getFftOrder(frameSize))
{
    fftBuffer.allocate(2 * fftSize, true);
    cmndValues.allocate(frameSize, true);
    cmndValues[0] = 1;
}


void YinFFT::prepare(double sampleRate_, float threshold_, float fMin_, float fMax_)
{
    sampleRate = sampleRate_;
    threshold = threshold_;
    fMin = fMin_;
    fMax = fMax_;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate_;
    spec.maximumBlockSize = frameSize;
    spec.numChannels = 1;
    
    auto setBand = [sampleRate_](juce::dsp::IIR::Filter<float>& filter,
                                 float fLow,
                                 float fHigh)
    {
        auto center = 0.5 * (fLow + fHigh);
        auto bandWidth = std::abs(fHigh - fLow);
        auto quality = center / bandWidth;
        
        filter.coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate_,
                                                                                center,
                                                                                quality);
    };
    
    for (auto filter : filters)
    {
        filter->prepare(spec);
    }
    
    setBand(lowPassFilter, 30, 250);
    setBand(lowPassFilter, 200, 800);
    setBand(lowPassFilter, 600, 3000);
}


juce::Optional<float> YinFFT::getF0(RingBuffer& ringBuffer)
{
    std::vector<float> frame(frameSize);
    
    if (ringBuffer.getFrame(frame, frameSize, hopSize))
    {
        for (auto filter : filters)
        {
            juce::AudioBuffer<float> tmp{1, frameSize};
            tmp.copyFrom(0, 0, frame.data(), frameSize);
            juce::dsp::AudioBlock<float> block(tmp);
            juce::dsp::ProcessContextReplacing<float> ctx(block);
            filter->process(ctx);

            juce::Optional<float> f0 = calculateF0(tmp.getReadPointer(0));
            
            if (f0)
                return f0;
        }
    }
    
    return std::nullopt;
}


void YinFFT::ACF(const float *frame)
{
    for (int i = 0; i < frameSize; i++)
    {
        fftBuffer[i] = frame[i];
    }
    
    fft.performRealOnlyForwardTransform(fftBuffer.getData());
    
    for (int i = 0; i <= fftSize / 2; i++)
    {
        float re = fftBuffer[2*i];
        float img = fftBuffer[2*i + 1];
        
        fftBuffer[2*i] = re * re + img * img;
        fftBuffer[2*i + 1] = 0;
    }
    
    fft.performRealOnlyInverseTransform(fftBuffer.getData());
}


void YinFFT::DF(const float *frame)
{
    ACF(frame);
    
    // First frameSize values of fftBuffer is acf values
    float r0 = fftBuffer[0];
    
    for (int i = 0; i < frameSize; i++)
    {
        fftBuffer[i] = 2 * (r0 - fftBuffer[i]);
    }
}


void YinFFT::CMND()
{
    size_t runningSum = 0;
    
    // First frameSize values of fftBuffer is diff values
    for (int tau = 1; tau < frameSize; tau++)
    {
        runningSum += fftBuffer[tau];
        cmndValues[tau] = fftBuffer[tau] / (runningSum / tau);
    }
    
    std::fill(fftBuffer.getData(), fftBuffer.getData() + 2 * fftSize, 0.f);
}


float YinFFT::minInterpolation(int tau)
{
    if (1 < tau and tau < frameSize-1) {
        float a = cmndValues[tau-1];
        float b = cmndValues[tau];
        float c = cmndValues[tau+1];
        
        auto denom = a - 2 * b + c;
        if (denom != 0)
            tau = tau + 0.5 * (a - c) / denom;
    }
    
    return tau;
}


juce::Optional<float> YinFFT::calculateF0(const float* frame)
{
    DF(frame);
    CMND();
    
    int discreteTau = 0;
    float candidate = 0;
    
    for (; discreteTau < frameSize; discreteTau++)
    {
        candidate = cmndValues[discreteTau];
        if (candidate < threshold)
            break;
    }
    
    if (candidate > threshold)
        return std::nullopt;
    
    float tau = minInterpolation(discreteTau);
    
    float f0 = sampleRate / tau;
    if (f0 < fMin or f0 > fMax)
        return std::nullopt;
    
    return f0;
}
