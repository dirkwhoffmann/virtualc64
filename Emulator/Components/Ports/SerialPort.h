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

#include "SerialPortTypes.h"
#include "SubComponent.h"

namespace vc64 {

class SerialPort final : public SubComponent, public Inspectable<Void, SerialPortStats> {

    Descriptions descriptions = {{

        .name           = "Serial",
        .description    = "Serial Port (IEC Bus)"
    }};

public:
    
    // Current values of the IEC bus lines
    bool atnLine;
    bool clockLine;
    bool dataLine;

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
    
private:

    // Indicates whether data is being transferred from or to a drive
    bool transferring = false;

    
    //
    // Methods
    //
    
public:

    SerialPort(C64 &ref) : SubComponent(ref) { };

public:

    SerialPort& operator= (const SerialPort& other) {

        CLONE(atnLine)
        CLONE(clockLine)
        CLONE(dataLine)
        CLONE(device1Atn)
        CLONE(device1Clock)
        CLONE(device1Data)
        CLONE(device2Atn)
        CLONE(device2Clock)
        CLONE(device2Data)
        CLONE(ciaAtn)
        CLONE(ciaClock)
        CLONE(ciaData)
        CLONE(stats)

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

        << atnLine
        << clockLine
        << dataLine
        << device1Atn
        << device1Clock
        << device1Data
        << device2Atn
        << device2Clock
        << device2Data
        << ciaAtn
        << ciaClock
        << ciaData
        << stats.idle;

    } SERIALIZERS(serialize);


    //
    // Methods from CoreComponent
    //

public:
    
    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream& os) const override;
    void _reset(bool hard) override;


    //
    // Accessing
    //
    
public:
    
    // Schedules an update event for the serial port (IEC bus)
    void setNeedsUpdate();

    /* Updates all three bus lines. The new values are determined by VIA1
     * (drive side) and CIA2 (C64 side).
     */
    void update();

    /* Execution function for observing the bus activity. This method is
     * invoked periodically. It's purpose is to determines if data is
     * transmitted on the bus.
     */
    void execute();
    
    // Returns true if data is currently transferred over the bus
    bool isTransferring() const { return transferring; }
    
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
