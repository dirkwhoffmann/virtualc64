// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "AudioStream.h"
#include <algorithm>

namespace vc64 {

void
AudioStream::wipeOut()
{
    {   SYNCHRONIZED

        clear(SamplePair{0,0});
    }
}

void
AudioStream::eliminateCracks()
{
    {   SYNCHRONIZED

        loginfo(AUDVOL_DEBUG, "Eliminating cracks (%ld samples)...\n", count());

        float scale = 1.0f;
        float delta = 1.0f / count();

        // Rescale the existing samples
        for (isize i = begin(); i != end(); i = next(i)) {

            scale -= delta;
            assert(scale >= -0.1 && scale < 1.0);

            elements[i].l *= scale;
            elements[i].r *= scale;
        }

        // Wipe out the rest of the buffer
        for (isize i = end(); i != begin(); i = next(i)) {

            elements[i] = { 0, 0 };
        }
    }
}

void
AudioStream::alignWritePtr()
{
    {   SYNCHRONIZED

        align(cap() / 2);
    }
}

isize
AudioStream::copyMono(float *buffer, isize n)
{
    {   SYNCHRONIZED

        // If a buffer underflow occurs ...
        if (auto cnt = count(); cnt < n) {

            // ... copy all we have while stepwise lowering the volume ...
            for (isize i = 0; i < cnt; i++) {

                auto pair = read();
                *buffer++ = (pair.l + pair.r) * float(cnt - i) / float(cnt);
            }
            assert(isEmpty());

            // ... and fill the rest with zeroes.
            for (isize i = cnt; i < n; i++) *buffer++ = 0;

            return cnt;
        }

        // The standard case: The buffer contains enough samples
        for (isize i = 0; i < n; i++) {

            auto sample = read();
            buffer[i] = 0.5f * (sample.l + sample.r);
        }

        return n;
    }
}

isize
AudioStream::copyStereo(float *left, float *right, isize n)
{
    {   SYNCHRONIZED

        // If a buffer underflow occurs ...
        if (auto cnt = count(); cnt < n) {

            // ... copy all we have while stepwise lowering the volume ...
            for (isize i = 0; i < cnt; i++) {

                auto pair = read();
                *left++ = pair.l * float(cnt - i) / float(cnt);
                *right++ = pair.r * float(cnt - i) / float(cnt);
            }
            assert(isEmpty());

            // ... and fill the rest with zeroes.
            for (isize i = cnt; i < n; i++) *left++ = *right++ = 0;

            return cnt;
        }

        // The standard case: The buffer contains enough samples
        for (isize i = 0; i < n; i++) {

            auto sample = read();
            left[i] = sample.l;
            right[i] = sample.r;
        }

        return n;
    }
}

isize
AudioStream::copyInterleaved(float *buffer, isize n)
{
    {   SYNCHRONIZED
        
        // If a buffer underflow occurs ...
        if (auto cnt = count(); cnt < n) {
            
            // ... copy all we have while stepwise lowering the volume ...
            for (isize i = 0; i < cnt; i++) {
                
                auto pair = read();
                *buffer++ = pair.l * float(cnt - i) / float(cnt);
                *buffer++ = pair.r * float(cnt - i) / float(cnt);
            }
            assert(isEmpty());
            
            // ... and fill the rest with zeroes.
            for (isize i = cnt; i < n; i++) { *buffer++ = 0; *buffer++ = 0; }
            
            return cnt;
        }
        
        // The standard case: The buffer contains enough samples
        for (isize i = 0; i < n; i++) {
            
            auto sample = read();
            *buffer++ = sample.l;
            *buffer++ = sample.r;
        }
        
        return n;
    }
}

void
AudioStream::drawL(u32 *buffer, isize width, isize height, u32 color) const
{
    static float highest = 0.01f;
    highest = drawL(buffer, width, height, highest, color);
}

void
AudioStream::drawR(u32 *buffer, isize width, isize height, u32 color) const
{
    static float highest = 0.01f;
    highest = drawR(buffer, width, height, highest, color);
}

float
AudioStream::drawL(u32 *buffer, isize width, isize height, float highest, u32 color) const
{
    return draw(buffer, width, height, highest,
                [this](float x) { return std::abs(current(isize(cap() * x)).l); }, color);
}

float
AudioStream::drawR(u32 *buffer, isize width, isize height, float highest, u32 color) const
{
    return draw(buffer, width, height, highest,
                [this](float x) { return std::abs(current(isize(cap() * x)).r); }, color);
}

float
AudioStream::draw(u32 *buffer, isize width, isize height, float highest,
                  std::function<float(float x)> data, u32 color) const
{
    SYNCHRONIZED

    float newHighestAmplitude = 0.001f;

    // Clear buffer
    for (isize i = 0; i < width * height; i++) {
        buffer[i] = 0; // 0xFF000000;
    }

    // Draw waveform
    for (isize w = 0; w < width; w++) {

        // Get a sample from the data provider
        auto sample = data((float)w / (float)width);

        if (sample == 0) {

            // Draw some noise to make it look sexy
            unsigned *ptr = buffer + width * height / 2 + w;
            *ptr = color;
            if (rand() % 2) *(ptr + width) = color;
            if (rand() % 2) *(ptr - width) = color;

        } else {

            // Remember the highest amplitude
            if (sample > newHighestAmplitude) newHighestAmplitude = sample;

            // Scale the sample
            isize scaled = isize(sample * height / highest);
            if (scaled > height) scaled = height;

            // Draw vertical line
            u32 *ptr = buffer + width * ((height - scaled) / 2) + w;
            for (isize j = 0; j < scaled; j++, ptr += width) *ptr = color;
        }
    }
    return newHighestAmplitude;
}

}
