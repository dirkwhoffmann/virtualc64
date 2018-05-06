//
//  SIDbase.cpp
//  VirtualC64
//
//  Created by Dirk Hoffmann on 06.05.18.
//

#include "C64.h"

void
SIDbase::run()
{
    clearRingbuffer();
}

void
SIDbase::halt()
{
    clearRingbuffer();
}

void
SIDbase::clearRingbuffer()
{
    debug(4,"Clearing ringbuffer\n");
    
    // Reset ringbuffer contents
    for (unsigned i = 0; i < bufferSize; i++) {
        ringBuffer[i] = 0.0f;
    }
    
    // Reset pointer positions
    readPtr = 0;
    alignWritePtr();
}

float
SIDbase::readData()
{
    // Read sound sample
    float value = ringBuffer[readPtr];
    
    // Adjust volume
    if (volume != targetVolume) {
        if (volume < targetVolume) {
            volume += MIN(volumeDelta, targetVolume - volume);
        } else {
            volume -= MIN(volumeDelta, volume - targetVolume);
        }
    }
    value = (volume <= 0) ? 0.0f : value * (float)volume / 100000.0f;
    
    // Advance read pointer
    advanceReadPtr();
    
    return value;
}

void
SIDbase::handleBufferUnderflow()
{
    debug(4, "SID RINGBUFFER UNDERFLOW (%ld)\n", readPtr);
}

void
SIDbase::readMonoSamples(float *target, size_t n)
{
    // Check for buffer underflow
    if (samplesInBuffer() < n) {
        handleBufferUnderflow();
    }
    
    // Read samples
    for (size_t i = 0; i < n; i++) {
        float value = readData();
        target[i] = value;
    }
}

void
SIDbase::readStereoSamples(float *target1, float *target2, size_t n)
{
    // Check for buffer underflow
    if (samplesInBuffer() < n) {
        handleBufferUnderflow();
    }
    
    // Read samples
    for (unsigned i = 0; i < n; i++) {
        float value = readData();
        target1[i] = target2[i] = value;
    }
}

void
SIDbase::readStereoSamplesInterleaved(float *target, size_t n)
{
    // Check for buffer underflow
    if (samplesInBuffer() < n) {
        handleBufferUnderflow();
    }
    
    // Read samples
    for (unsigned i = 0; i < n; i++) {
        float value = readData();
        target[i*2] = value;
        target[i*2+1] = value;
    }
}

void
SIDbase::writeData(short *data, size_t count)
{
    // Check for buffer overflow
    if (bufferCapacity() < count) {
        handleBufferOverflow();
    }
    
    // Convert sound samples to floating point values and write into ringbuffer
    for (unsigned i = 0; i < count; i++) {
        ringBuffer[writePtr] = float(data[i]) * scale;
        advanceWritePtr();
    }
}

void
SIDbase::handleBufferOverflow()
{
    debug(4, "SID RINGBUFFER OVERFLOW (%ld)\n", writePtr);
    
    if (!c64->getWarp()) {
        // In real-time mode, we readjust the write pointer
        alignWritePtr();
    } else {
        // In warp mode, we don't advance the write ptr to avoid crack noises
        return;
    }
}

