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

#include "DatasetteTypes.h"
#include "C64Types.h"
#include "SubComponent.h"
#include "Constants.h"
#include "Chrono.h"

namespace vc64 {

class Pulse {
    
public:
    
    i32 cycles;

public:

    Pulse() : cycles(0) { };
    Pulse(i32 value) : cycles(value) { };
    
    util::Time delay() const;
};

class Datasette : public SubComponent {

    // Current configuration
    DatasetteConfig config = { };

    //
    // Tape
    //

    /* Data format (as specified in the TAP file)
     *
     *   - In TAP format 0, data byte 0 signals a long pulse without stating
     *     its length precisely.
     *
     *   - In TAP format 1, each 0 is followed by three bytes stating the
     *     precise length in LO_LO_HI_00 format.
     */
    u8 type = 0;

    // The pulse buffer
    Pulse *pulses = nullptr;
    
    // Number of pulses stored in the pulse buffer
    isize size = 0;


    //
    // Tape drive
    //
    
    // The position of the read/write head inside the pulse buffer (0 ... size)
    isize head = 0;

    // The tape counter (time between start and the current head position)
    util::Time counter;
    
    // State of the play key
    bool playKey = false;
    
    // State of the drive motor
    bool motor = false;

    // Next scheduled rising edge on data line
    i64 nextRisingEdge = 0;
    
    // Next scheduled falling edge on data line
    i64 nextFallingEdge = 0;
    
    
    //
    // Initializing
    //
    
public:

    Datasette(C64 &ref) : SubComponent(ref) { };
    ~Datasette();
    
    void alloc(isize capacity);
    void dealloc();

    
    //
    // Methods from CoreObject
    //

private:
    
    const char *getDescription() const override { return "Datasette"; }
    void _dump(Category category, std::ostream& os) const override;

    
    //
    // Methods from CoreComponent
    //

private:

    void _reset(bool hard) override;
        
    template <class T>
    void serialize(T& worker)
    {
        worker
        
        << head
        << counter.ticks
        << playKey
        << motor
        << nextRisingEdge
        << nextFallingEdge;

        if (util::isResetter(worker)) return;

        worker

        << type;
    }
    
    isize _size() override;
    u64 _checksum() override { COMPUTE_SNAPSHOT_CHECKSUM }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    isize didLoadFromBuffer(const u8 *buffer) override;
    isize didSaveToBuffer(u8 *buffer) override;


    //
    // Configuring
    //

public:

    const DatasetteConfig &getConfig() const { return config; }
    void resetConfig() override;

    i64 getConfigItem(Option option) const;
    void setConfigItem(Option option, i64 value);

    
    //
    // Handling tapes
    //
    
public:
    
    // Returns true if a tape is inserted
    bool hasTape() const { return size != 0; }

    // Returns the duration from the tape start and the specified position
    util::Time tapeDuration(isize pos);

    // Returns the duration of the entire tape
    util::Time tapeDuration() { return tapeDuration(size); }

    // Returns the current tape counter in (truncated) seconds
    isize getCounter() { return (isize)counter.asSeconds(); }

    //Inserts a TAP archive as a virtual tape
    void insertTape(TAPFile &file);

    // Ejects the tape (if any)
    void ejectTape();

    // Returns the tape type (TAP format, 0 or 1)
    u8 getType() const { return type; }

    
    //
    // Operating the device
    //
    
public:
    
    // Returns true if the play key is pressed
    bool getPlayKey() const { return playKey; }

    // Presses the play key
    void pressPlay();

    // Presses the stop key
    void pressStop();

private:

    // Performs the pressPlay action
    void play();

    // Performs the pressStop action
    void stop();


    //
    // Emulating the device
    //

public:

    // Puts the read/write head at the beginning of the tape
    void rewind(isize seconds = 0);

    // Returns true if the datasette motor is switched on
    bool getMotor() const { return motor; }

    // Switches the motor on or off
    void setMotor(bool value);

private:

    // Advances the read/write head one pulse
    void advanceHead();



    //
    // Processing events
    //

public:

    void processMotEvent(EventID event);
    void processDatEvent(EventID event, i64 cycles);

private:

    // Updates the event in the DAT slot
    void updateDatEvent();

    // Schedules the next event in the DAT slot
    void scheduleNextDatEvent();

    // Schedules the rising and falling edge of the next pulse
    void schedulePulse(isize nr);
};

}
