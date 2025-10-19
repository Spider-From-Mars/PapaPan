#include "PitchThread.h"

PitchDetectionThread::PitchDetectionThread(YinFFT &pitchDetector, RingBuffer &buffer)
    : juce::Thread("PitchDetector"), pitchDetector(pitchDetector), buffer(buffer)
{
}

PitchDetectionThread::~PitchDetectionThread()
{
    signalThreadShouldExit();
    waitEvent.signal();
    stopThread(1000);
}

void PitchDetectionThread::prepare() { startThread(); }

void PitchDetectionThread::run()
{
    while (!threadShouldExit())
    {
        waitEvent.wait();
        auto pitch = pitchDetector.getF0(buffer);
        detectedPitch.store(pitch.orFallback(0), std::memory_order_relaxed);
        wait(5);
    }
}
