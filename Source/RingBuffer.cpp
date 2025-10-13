#include "RingBuffer.h"

RingBuffer::RingBuffer(int size)
    : fifo(size), buffer(size, 0.f)
{}

void RingBuffer::push(const float *input, int numSamples)
{
    int start1, size1, start2, size2;
    fifo.prepareToWrite(numSamples, start1, size1, start2, size2);
    
    if (size1 > 0)
        std::copy(input, input + size1, buffer.begin() + start1);
    if (size2 > 0)
        std::copy(input + size1, input + size1 + size2, buffer.begin() + start2);

    fifo.finishedWrite(size1 + size2);
}


bool RingBuffer::getFrame(std::vector<float> &out, int frameSize, int hopSize)
{
    if (fifo.getNumReady() < frameSize)
        return false;
    
    out.resize(frameSize);
    
    int start1, size1, start2, size2;
    fifo.prepareToRead(frameSize, start1, size1, start2, size2);
    
    if (size1 > 0)
        std::copy(buffer.begin() + start1, buffer.begin() + start1 + size1, out.begin());
    if (size2 > 0)
        std::copy(buffer.begin() + start2, buffer.begin() + start2 + size2, out.begin() + size1);
    
    fifo.finishedRead(hopSize);
    
    return true;
}
