// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _DATASETTE_H
#define _DATASETTE_H

#include "C64Component.h"

class TAPFile;

class Datasette : public C64Component {
    
    //
    // Tape
    //
    
    // Data buffer (contains the raw data of the TAP archive)
    u8 *data = NULL;
    
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
 
    Datasette(C64 &ref);    
    ~Datasette();
    
    void _reset() override;
    
    
    //
    // Serialization
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
    }
    
    size_t _size() override { COMPUTE_SNAPSHOT_SIZE }
    size_t _load(u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    size_t _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
private:
    
    void _ping() override;
    size_t oldStateSize() override;
    void oldDidLoadFromBuffer(u8 **buffer) override;
    void oldDidSaveToBuffer(u8 **buffer) override;


    //
    // Handling tapes
    //
    
public:
    
    // Returns true if a tape is inserted
    bool hasTape() { return size != 0; }
    
    //Inserts a TAP archive as a virtual tape
    bool insertTape(TAPFile *a);

    // Ejects the virtual tape (if any)
    void ejectTape();

    // Returns the tape type (TAP format, 0 or 1).
    u8 getType() { return type; }

    // Returns the tape length in cycles.
    u64 getDurationInCycles() { return durationInCycles; }
    
    // Returns the tape length in seconds.
    u32 getDurationInSeconds() { return (u32)(durationInCycles / (u64)PAL_CLOCK_FREQUENCY); }

    
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
    u64 getHead() { return head; }

    // Returns the head position in CPU cycles
    u64 getHeadInCycles() { return headInCycles; }

    // Returns the head position in seconds
    u32 getHeadInSeconds() { return headInSeconds; }
    
    // Sets the current head position in cycles.
    void setHeadInCycles(u64 value);
    
    // Returns the pulse length at the current head position
    int pulseLength(int *skip);
    int pulseLength() { return pulseLength(NULL); }

    
    //
    // Running the device
    //
    
    // Returns true if the play key is pressed
    bool getPlayKey() { return playKey; }

    // Press play on tape
    void pressPlay(); 

    // Press stop key
    void pressStop();

    // Returns true if the datasette motor is switched on
    bool getMotor() { return motor; }

    // Switches the motor on or off
    void setMotor(bool value) { motor = value; }

    // Emulates the datasette
    void execute() { if (playKey && motor) _execute(); }

private:

    // Internal execution function
    void _execute();
};

#endif
