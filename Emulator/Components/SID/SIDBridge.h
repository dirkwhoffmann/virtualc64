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

#pragma once

#include "SubComponent.h"
#include "SIDTypes.h"
#include "Constants.h"
#include "Volume.h"
#include "AudioPort.h"
#include "SID.h"
#include "Chrono.h"

namespace vc64 {

/* Architecture of the audio pipeline
 *
 *           SidBridge
 *           -------------------------------------------------
 *          |   --------                                      |
 *          |  |  SID0  |----->                               |
 *          |   --------       |                              |
 *          |                  |                              |
 *          |   --------       |                              |
 *          |  |  SID1  |----->|                          ---------> Speaker
 *          |   --------       |      --------------     |    |
*          |                   |---->|  AudioPort   |----|    |
 *          |   --------       |      --------------     |    |
 *          |  |  SID2  |----->|       Volume, pan        ---------> Recorder
 *          |   --------       |                              |
 *          |                  |                              |
 *          |   --------       |                              |
 *          |  |  SID3  |----->                               |
 *          |   --------                                      |
 *           -------------------------------------------------
 */

class SIDBridge final : public SubComponent {

    friend C64Memory;
    friend AudioPort;
    
    Descriptions descriptions = {{

        .name           = "SIDBridge",
        .description    = "SID Bridge"
    }};

    
    //
    // Sub components
    //

public:
    
    SID sid[4] =  {

        SID(c64, 0),
        SID(c64, 1),
        SID(c64, 2),
        SID(c64, 3)
    };


    //
    // Methods
    //
    
public:

    SIDBridge(C64 &ref);

    SIDBridge& operator= (const SIDBridge& other) {

        CLONE_ARRAY(sid)

        return *this;
    }


    //
    // Methods from Serializable
    //

public:

    template <class T>
    void serialize(T& worker)
    {
        worker 

        << sid;

    } SERIALIZERS(serialize);


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }


    //
    // Parameterizing
    //

public:

    // Adjusts the clock frequency for all SIDs
    void setClockFrequency(u32 frequency);

    // Adjusts the sample rate for all SIDs
    void setSampleRate(double rate);


    //
    // Running the device
    //

public:

    // Prepares for a new frame
    void beginFrame();

    // Finishes the current frame
    void endFrame();

    
    //
    // Accessig memory
    //
    
public:

    // Special peek function for the I/O memory range
    u8 peek(u16 addr);

    // Same as peek without side effects
    u8 spypeek(u16 addr) const;
    
    // Reads the pot bits that show up in register 0x19 and 0x1A
    u8 readPotX() const;
    u8 readPotY() const;
    
    // Special poke function for the I/O memory range
    void poke(u16 addr, u8 value);

private:

    // Translates a memory address to the mapped SID
    isize mappedSID(u16 addr) const;


    //
    // Visualizing the waveform
    //

public:

    /* Plots a graphical representation of the waveform. Returns the highest
     * amplitute that was found in the ringbuffer. To implement auto-scaling,
     * pass the returned value as parameter maxAmp in the next call to this
     * function.
     */
    float draw(u32 *buffer, isize width, isize height,
               float maxAmp, u32 color, isize sid = -1) const;
};

}
