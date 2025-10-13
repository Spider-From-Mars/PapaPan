#pragma once

#include <JuceHeader.h>

#include "YinFFT.h"
#include "RingBuffer.h"

class PitchDetectionThread : juce::Thread
{
public:
    PitchDetectionThread(YinFFT& pitchDetector, RingBuffer& buffer);
    ~PitchDetectionThread();
    
    void prepare();
    
    void run() override;
    float getPitch() const { return detectedPitch.load(std::memory_order_relaxed); }
    
private:
    YinFFT& pitchDetector;
    RingBuffer& buffer;
    std::atomic<float> detectedPitch = 0;
};
