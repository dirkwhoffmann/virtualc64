// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64Component.h"
#include "Constants.h"

class Datasette : public C64Component {
    
    //
    // Tape
    //
    
    // Data buffer (contains the raw data of the TAP archive)
    u8 *data = nullptr;
    
    // Size of the attached data buffer
    u64 size = 0;
    
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
    u64 head = 0;
    
    // Head position measured in cycles
    u64 headInCycles = 0;
    
    // Head position, measured in seconds
    u32 headInSeconds = 0;
    
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
    
    usize _size() override { COMPUTE_SNAPSHOT_SIZE }
    usize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    usize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    

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

    // Returns the tape length in cycles.
    u64 getDurationInCycles() const { return durationInCycles; }
    
    // Returns the tape length in seconds.
    u32 getDurationInSeconds() const { return (u32)(durationInCycles / (u64)PAL_CLOCK_FREQUENCY); }

    
    //
    // Operating the read/write head
    //

    // Puts the read/write head at the beginning of the tape
    void rewind() { head = headInSeconds = headInCycles = 0; }

    /* Advances the read/write head one pulse. This methods updates head,
     * headInCycles, and headInSeconds. If silent is set to false, a
     * MSG_VC1530_PROGRESS message is sent.
     */
    void advanceHead(bool silent = false);
    
    // Returns the head position
    u64 getHead() const { return head; }

    // Returns the head position in CPU cycles
    u64 getHeadInCycles() const { return headInCycles; }

    // Returns the head position in seconds
    u32 getHeadInSeconds() const { return headInSeconds; }
    
    // Sets the current head position in cycles.
    void setHeadInCycles(u64 value);
    
    // Returns the pulse length at the current head position
    int pulseLength(int *skip) const;
    int pulseLength() const { return pulseLength(nullptr); }

    
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
