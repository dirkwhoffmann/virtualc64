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

namespace vc64 {

class IEC : public SubComponent {

public:
    
    // Current values of the IEC bus lines
    bool atnLine;
    bool clockLine;
    bool dataLine;

    /* Indicates if the bus lines variables need an undate, because the values
     * coming from the C64 side have changed.
     * DEPRECATED
     */
    bool isDirtyC64Side;

    /* Indicates if the bus lines variables need an undate, because the values
     * coming from the drive side have changed.
     * DEPRECATED
     */
    bool isDirtyDriveSide;

    // Bus driving values from drive 1
    bool device1Atn;
    bool device1Clock;
    bool device1Data;
    
    // Bus driving values from drive 2
    bool device2Atn;
    bool device2Clock;
    bool device2Data;
    
    // Bus driving values from the CIA
    bool ciaAtn;
    bool ciaClock;
    bool ciaData;

    // Bus idle time measured in frames
    i64 idle = 0;
    
private:

    // Indicates whether data is being transferred from or to a drive
    bool transferring = false;

    
    //
    // Initializing
    //
    
public:

    IEC(C64 &ref) : SubComponent(ref) { };


    //
    // Methods from CoreObject
    //

private:

    const char *getDescription() const override { return "IEC"; }
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
        
        << atnLine
        << clockLine
        << dataLine
        << isDirtyC64Side
        << isDirtyDriveSide
        << device1Atn
        << device1Clock
        << device1Data
        << device2Atn
        << device2Clock
        << device2Data
        << ciaAtn
        << ciaClock
        << ciaData
        << idle;
    }
    
    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    u64 _checksum() override { COMPUTE_SNAPSHOT_CHECKSUM }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
    //
    // Accessing
    //
    
public:
    
    // Requensts an update of the bus lines from the C64 side
    // DEPRECATED
    void setNeedsUpdateC64Side() { isDirtyC64Side = true; }

    // Requensts an update of the bus lines from the drive side
    // DEPRECATED
    void setNeedsUpdateDriveSide() { isDirtyDriveSide = true; }

    /* Updates all three bus lines. The new values are determined by VIA1
     * (drive side) and CIA2 (C64 side).
     */
    void updateIecLinesC64Side();
    void updateIecLinesDriveSide();

    /* Execution function for observing the bus activity. This method is
     * invoked periodically. It's purpose is to determines if data is
     * transmitted on the bus.
     */
    void execute();
    
    // Returns true if data is currently transferred over the bus
    bool isTransferring() { return transferring; }
    
    // Updates variable transferring
    void updateTransferStatus();
    
private:
    
    void updateIecLines();
    
    /* Work horse for method updateIecLines. It returns true if at least one
     * line changed it's value.
     */
    bool _updateIecLines();
};

}
