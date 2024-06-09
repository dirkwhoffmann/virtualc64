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
#include "SIDBridge.h"
#include "Emulator.h"
#include "IOUtils.h"

#include <algorithm>
#include <cmath>

namespace vc64 {

SIDBridge::SIDBridge(C64 &ref) : SubComponent(ref)
{
    subComponents = std::vector<CoreComponent *> {

        &sid[0],
        &sid[1],
        &sid[2],
        &sid[3]
    };
}

isize
SIDBridge::mappedSID(u16 addr) const
{
    addr &= 0xFFE0;
    
    if (sid1.isEnabled() && addr == sid1.config.address) return 1;
    if (sid2.isEnabled() && addr == sid2.config.address) return 2;
    if (sid3.isEnabled() && addr == sid3.config.address) return 3;

    return 0;
}

u8 
SIDBridge::peek(u16 addr)
{
    // Select the target SID
    isize sidNr = mappedSID(addr);

    // Get the target SID up to date
    sid[sidNr].executeUntil(cpu.clock);

    addr &= 0x1F;

    if (sidNr == 0) {
        
        if (addr == 0x19) {
            
            port1.updatePotX();
            port2.updatePotX();
            return readPotX();
        }
        if (addr == 0x1A) {

            port1.updatePotY();
            port2.updatePotY();
            return readPotY();
        }
    }
    
    return sid[sidNr].resid.peek(addr);
}

u8
SIDBridge::spypeek(u16 addr) const
{
    // Select the target SID
    isize sidNr = mappedSID(addr);

    addr &= 0x1F;

    if (sidNr == 0) {

        if (addr == 0x19) { return port1.readPotX() & port2.readPotX(); }
        if (addr == 0x1A) { return port1.readPotY() & port2.readPotY(); }
    }

    return sid[sidNr].spypeek(addr);
}

u8
SIDBridge::readPotX() const
{
    u8 result = 0xFF;

    if (GET_BIT(cia1.getPA(), 7) == 0) result &= port1.readPotX();
    if (GET_BIT(cia1.getPA(), 6) == 0) result &= port2.readPotX();

    return result;
}

u8
SIDBridge::readPotY() const
{
    u8 result = 0xFF;

    if (GET_BIT(cia1.getPA(), 7) == 0) result &= port1.readPotY();
    if (GET_BIT(cia1.getPA(), 6) == 0) result &= port2.readPotY();

    return result;
}

void 
SIDBridge::poke(u16 addr, u8 value)
{
    trace(SIDREG_DEBUG, "poke(%x,%x)\n", addr, value);

    // Select the target SID
    isize sidNr = mappedSID(addr);

    // Get the target SID up to date
    sid[sidNr].executeUntil(cpu.clock);

    // Write the register
    sid[sidNr].poke(addr, value);
}

void 
SIDBridge::setClockFrequency(u32 frequency)
{
    for (isize i = 0; i < 4; i++) {
        sid[i].setClockFrequency(frequency);
    }
}

void
SIDBridge::setSampleRate(double rate)
{
    for (isize i = 0; i < 4; i++) {
        sid[i].setSampleRate(rate);
    }
}

void
SIDBridge::beginFrame()
{
    // Update the CPU clock frequency
    c64.updateClockFrequency();

    // Update the audio sample rate
    setSampleRate(host.getOption(OPT_HOST_SAMPLE_RATE) + audioPort.getSampleRateCorrection());
}

void 
SIDBridge::endFrame()
{
    // Execute all remaining SID cycles
    sid0.executeUntil(cpu.clock);
    sid1.executeUntil(cpu.clock);
    sid2.executeUntil(cpu.clock);
    sid3.executeUntil(cpu.clock);

    // Generate sound sampes
    audioPort.generateSamples();
}

float
SIDBridge::draw(u32 *buffer, isize width, isize height,
            float maxAmp, u32 color, isize nr) const
{
    auto samples = new float[width][2];
    isize hheight = height / 2;
    float newMaxAmp = 0.001f, dw;

    // Gather data
    switch (nr) {

        case 0: case 1: case 2: case 3:

            dw = sid[nr].stream.cap() / float(width);

            for (isize w = 0; w < width; w++) {

                auto sample = sid[nr].stream.current(isize(w * dw));
                samples[w][0] = float(abs(sample));
                samples[w][1] = float(abs(sample));
            }
            break;

        default:

            dw = audioPort.cap() / float(width);

            for (isize w = 0; w < width; w++) {

                auto sample = audioPort.current(isize(w * dw));
                samples[w][0] = abs(sample.left);
                samples[w][1] = abs(sample.right);
            }
            break;
    }

    // Clear buffer
    for (isize i = 0; i < width * height; i++) buffer[i] = 0;

    // Draw waveform
    for (isize w = 0; w < width; w++) {

        u32 *ptr = buffer + width * hheight + w;

        if (samples[w][0] == 0 && samples[w][1] == 0) {

            // Draw some noise to make it look sexy
            *ptr = color;
            if (rand() % 2) *(ptr + width) = color;
            if (rand() % 2) *(ptr - width) = color;

        } else {

            // Remember the highest amplitude
            if (samples[w][0] > newMaxAmp) newMaxAmp = samples[w][0];
            if (samples[w][1] > newMaxAmp) newMaxAmp = samples[w][1];

            // Scale the sample
            isize scaledL = std::min(isize(samples[w][0] * hheight / maxAmp), hheight);
            isize scaledR = std::min(isize(samples[w][1] * hheight / maxAmp), hheight);

            // Draw vertical lines
            for (isize j = 0; j < scaledL; j++) *(ptr - j * width) = color;
            for (isize j = 0; j < scaledR; j++) *(ptr + j * width) = color;
        }
    }

    delete[] samples;
    return newMaxAmp;
}

}
