#pragma once

#include "RingBuffer.h"
#include "YinFFT.h"
#include <JuceHeader.h>

class PitchDetectionThread : juce::Thread
{
  public:
    PitchDetectionThread(YinFFT &pitchDetector, RingBuffer &buffer);
    ~PitchDetectionThread();

    void prepare();

    void run() override;
    float getPitch() const { return detectedPitch.load(std::memory_order_relaxed); }
    juce::WaitableEvent waitEvent;
    
  private:
    YinFFT &pitchDetector;
    RingBuffer &buffer;
    std::atomic<float> detectedPitch = 0;
};
