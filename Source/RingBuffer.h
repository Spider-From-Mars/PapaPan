#pragma once

#include <JuceHeader.h>
#include <vector>

class RingBuffer
{
public:
    RingBuffer(int size);
    
    void push(const float* input, int numSamples);
    bool getFrame(std::vector<float>& out, int frameSize, int hopSize);
    
private:
    juce::AbstractFifo fifo;
    std::vector<float> buffer;
};
