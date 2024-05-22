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
 *          |   --------  vol                                 |
 *   SID 0 --->| Buffer |----->                               |
 *          |   --------       |                              |
 *          |                  |                              |
 *          |   --------  vol  |             --------------   |
 *   SID 1 --->| Buffer |----->|          ->|  AudioPort  |-----> Speaker
 *          |   --------       |   pan   |   --------------   |
 *          |                  |-------->|                    |
 *          |   --------  vol  |  l vol  |   --------------
 *   SID 2 --->| Buffer |----->|  r vol   ->|  AudioPort  |-----> Recorder
 *          |   --------       |             --------------   |
 *          |                  |                              |
 *          |   --------  vol  |                              |
 *   SID 3 --->| Buffer |----->                               |
 *          |   --------                                      |
 *           -------------------------------------------------
 */

class SIDBridge final : public SubComponent, public Dumpable {

    friend C64Memory;
    friend AudioPort;
    
    Descriptions descriptions = {{

        .name           = "SIDBridge",
        .shellName      = "SIDBridge",
        .description    = "SID Bridge"
    }};

    ConfigOptions options = {

        OPT_SID_POWER_SAVE,
        OPT_SID_ENGINE,
        OPT_AUD_VOL_L,
        OPT_AUD_VOL_R
    };
    
    // Current configuration
    SIDBridgeConfig config = { };
    
    // Statistics
    SIDBridgeStats stats = { };

    
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

private:

    // Master volumes
    float volL;
    float volR;

public:


    //
    // Audio streams
    //
    
    /* The four SID streams. Each stream stores the sound samples produced by
     * one of the four supported SIDs.
     */
    SampleStream sidStream[4];

    
    //
    // Methods
    //
    
public:

    SIDBridge(C64 &ref);
    const Descriptions &getDescriptions() const override { return descriptions; }

    void _dump(Category category, std::ostream& os) const override;
    void _dump(Category category, std::ostream& os, isize nr) const;

    SIDBridge& operator= (const SIDBridge& other) {

        CLONE_ARRAY(sid)
        CLONE(config)

        return *this;
    }

    template <class T>
    void serialize(T& worker)
    {
        worker
        
        << sid;

        if (isResetter(worker)) return;

        worker

        << config.engine
        << config.volL
        << config.volR;
    }
    
    void operator << (SerResetter &worker) override { serialize(worker); }
    void operator << (SerChecker &worker) override { serialize(worker); }
    void operator << (SerCounter &worker) override { serialize(worker); }
    void operator << (SerReader &worker) override;
    void operator << (SerWriter &worker) override { serialize(worker); }


    //
    // Configuring
    //

    const SIDBridgeConfig &getConfig() const { return config; }
    const ConfigOptions &getOptions() const override { return options; }
    i64 getOption(Option opt) const override;
    void setOption(Option opt, i64 value) override;


    //
    // Inspecting
    //

public:

    CoreComponent &getSID(isize nr);
    SIDBridgeStats getStats();
    
    bool isEnabled(isize nr) const { return sid[nr].config.enabled; }
    bool isMuted() const;


    //
    // Managing the four sample buffers
    //
    
public:

    // Clears the SID sample buffers
    void clearSampleBuffers();
    void clearSampleBuffer(long nr);

    
    //
    // Managing the ring buffer
    //
    
public:

    // Reads a audio sample pair without moving the read pointer
    void ringbufferData(isize offset, float *left, float *right);

    // Prepares for a new frame
    void beginFrame();

    // Finishes the current frame
    void endFrame();

    /* Executes SID until a certain cycle is reached. The function returns the
     * number of produced sound samples (not yet).
     */
    void executeUntil(Cycle targetCycle);

    // Indicates if sample synthesis should be skipped
    bool powerSave() const;

    
    //
    // Accessig memory
    //
    
public:
    
    // Translates a memory address to the mapped SID
    isize mappedSID(u16 addr) const;
    
    // Special peek function for the I/O memory range
    u8 peek(u16 addr);

    // Same as peek without side effects
    u8 spypeek(u16 addr) const;
    
    // Reads the pot bits that show up in register 0x19 and 0x1A
    u8 readPotX() const;
    u8 readPotY() const;
    
    // Special poke function for the I/O memory range
    void poke(u16 addr, u8 value);


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
