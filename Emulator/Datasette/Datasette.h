// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64Component.h"
#include "Constants.h"
#include "Chrono.h"

class Pulse {
    
public:
    
    i32 cycles;

public:
        
    Pulse() : cycles(0) { };
    Pulse(i32 value) : cycles(value) { };
    
    util::Time time() const;
};

class Datasette : public C64Component {
    
    //
    // Tape
    //
    
    // Pulse buffer
    Pulse *pulses = nullptr;
    
    // Number of stored pulses
    isize size = 0;
    
    /* Data format (as specified in the TAP type).
     *
     * In TAP format 0, data byte 0 signals a long pulse without stating its
     * length precisely.
     *
     * In TAP format 1, each 0 is followed by three bytes stating the precise
     * length in LO_LO_HI_00 format.
     */
    u8 type = 0;
    
    /* Tape length in cycles. The value is set when insertTape() is called. It
     * is computed by iterating over all pulses in the data buffer.
     */
    u64 durationInCycles = 0;
    
    
    //
    // Tape drive
    //
    
    // The position of the read/write head inside the data buffer (0 ... size)
    isize head = 0;
    
    // Head position measured in cycles
    i64 headInCycles = 0;
    
    // Head position, measured in seconds
    isize headInSeconds = 0;
    
    // Next scheduled rising edge on data line
    i64 nextRisingEdge = 0;
    
    // Next scheduled falling edge on data line
    i64 nextFallingEdge = 0;
    
    // Indicates whether the play key is pressed
    bool playKey = false;
    
    // Indicates whether the motor is switched on
    bool motor = false;
    
    
    //
    // Initializing
    //
    
public:
 
    Datasette(C64 &ref) : C64Component(ref) { };
    ~Datasette();
    
    void alloc(isize capacity);
    void dealloc();

    const char *getDescription() const override { return "Datasette"; }
    
private:
    
    void _reset() override;
    
    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        << size
        << type
        << durationInCycles;
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
        worker
        
        << head
        << headInCycles
        << headInSeconds
        << nextRisingEdge
        << nextFallingEdge
        << playKey
        << motor;
    }
    
    isize _size() override;
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    isize didLoadFromBuffer(const u8 *buffer) override;
    isize didSaveToBuffer(u8 *buffer) override;


    //
    // Analyzing
    //
    
    // Returns the duration from the tape start and the specified position
    util::Time tapeDuration(isize pos);

    // Returns the duration of the entire tape
    util::Time tapeDuration() { return tapeDuration(size); }
    
    
    //
    // Handling tapes
    //
    
public:
    
    // Returns true if a tape is inserted
    bool hasTape() const { return size != 0; }
    
    //Inserts a TAP archive as a virtual tape
    bool insertTape(TAPFile *a);

    // Ejects the virtual tape (if any)
    void ejectTape();

    // Returns the tape type (TAP format, 0 or 1).
    u8 getType() const { return type; }

    
    //
    // Operating the read/write head
    //

    // Puts the read/write head at the beginning of the tape
    void rewind() { head = headInSeconds = headInCycles = 0; }

    // Advances the read/write head one pulse
    void advanceHead(bool silent = false);
    
    // Returns the head position
    isize getHead() const { return head; }

    // Returns the head position in CPU cycles
    i64 getHeadInCycles() const { return headInCycles; }

    // Returns the head position in seconds
    isize getHeadInSeconds() const { return headInSeconds; }
        
    
    //
    // Running the device
    //
    
    // Returns true if the play key is pressed
    bool getPlayKey() const { return playKey; }

    // Press play on tape
    void pressPlay(); 

    // Press stop key
    void pressStop();

    // Returns true if the datasette motor is switched on
    bool getMotor() const { return motor; }

    // Switches the motor on or off
    void setMotor(bool value) { motor = value; }

    // Emulates the datasette
    void execute() { if (playKey && motor) _execute(); }

private:

    // Internal execution function
    void _execute();
};
