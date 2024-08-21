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
#include "Datasette.h"
#include "Emulator.h"

namespace vc64 {

util::Time
Pulse::delay() const
{
    return util::Time((i64)cycles * 1000000000 / PAL::CLOCK_FREQUENCY);
}

util::Time
Datasette::tapeDuration(isize pos)
{
    util::Time result;
    
    for (isize i = 0; i < pos && i < numPulses; i++) {
        result += pulses[i].delay();
    }
    
    return result;
}

void
Datasette::insertTape(MediaFile &file)
{
    try {

        TAPFile &tapFile = dynamic_cast<TAPFile &>(file);

        {   SUSPENDED

            // Allocate pulse buffer
            isize numPulses = tapFile.numPulses();
            alloc(numPulses);

            debug(TAP_DEBUG, "Inserting tape (%ld pulses)...\n", numPulses);

            // Read pulses
            tapFile.seek(0);
            for (isize i = 0; i < numPulses; i++) {

                pulses[i].cycles = (i32)tapFile.read();
                assert(pulses[i].cycles != -1);
            }

            // Rewind the tape
            rewind();

            // Update the execution event slot
            updateDatEvent();

            // Inform the GUI
            msgQueue.put(MSG_VC1530_TAPE, 1);
        }

    } catch (...) {

        throw Error(VC64ERROR_FILE_TYPE_MISMATCH);
    }
}

void
Datasette::ejectTape()
{
    // Only proceed if a tape is present
    if (!hasTape()) return;

    {   SUSPENDED
        
        debug(TAP_DEBUG, "Ejecting tape...\n");
        
        pressStop();
        rewind();
        dealloc();
        
        msgQueue.put(MSG_VC1530_TAPE, 0);
    }
}

void
Datasette::rewind(isize seconds)
{
    i64 old = (i64)counter.asSeconds();

    // Start at the beginning
    counter = util::Time(0);
    head = 0;
    
    // Fast forward to the requested position
    while (counter.asMilliseconds() < 1000 * seconds && head + 1 < numPulses) {
        advanceHead();
    }
    
    // Inform the GUI
    if (old != (i64)counter.asSeconds()) {
        msgQueue.put(MSG_VC1530_COUNTER, (i64)counter.asSeconds());
    }
}

void
Datasette::advanceHead()
{
    assert(head < numPulses);
    
    i64 old = (i64)counter.asSeconds();

    counter += pulses[head].delay();
    head++;
    
    // Inform the GUI
    if (old != (i64)counter.asSeconds()) {
        msgQueue.put(MSG_VC1530_COUNTER, (i64)counter.asSeconds());
    }
}

void
Datasette::pressPlay()
{
    debug(TAP_DEBUG, "pressPlay\n");

    // Only proceed if the device is connected
    if (!config.connected) return;

    // Only proceed if a tape is present
    if (!hasTape()) return;

    // Pause the emulator and press press
    { SUSPENDED play(); }
}

void
Datasette::play()
{
    if (!playKey) {

        playKey = true;

        // Schedule the first pulse
        schedulePulse(head);
        advanceHead();

        // Update the execution event slot
        updateDatEvent();

        // Inform the GUI
        msgQueue.put(MSG_VC1530_PLAY, 1);
    }
}

void
Datasette::pressStop()
{
    debug(TAP_DEBUG, "pressStop\n");

    // Only proceed if the device is connected
    if (!config.connected) return;

    // Pause the emulator and press press
    { SUSPENDED stop(); }
}

void
Datasette::stop()
{
    if (playKey) {
        
        playKey = false;
        motor = false;

        // Update the execution event slot
        scheduleNextDatEvent();

        msgQueue.put(MSG_VC1530_PLAY, 0);
    }
}

void
Datasette::setMotor(bool value)
{
    if (motor != value) {

        // Only proceed if the device is connected
        if (!config.connected) return;

        motor = value;

        // Update the execution event slot
        scheduleNextDatEvent();

        /* When the motor is switched on or off, a MSG_VC1530_MOTOR message is
         * sent to the GUI. However, if we sent the message immediately, we
         * would risk to flood the message queue, because some C64 switch the
         * motor state insanely often. To cope with this situation, we set a
         * counter and let the vsync handler send the message once the counter
         * has timed out.
         */
        c64.scheduleRel<SLOT_MOT>(C64::msec(200), motor ? MOT_START : MOT_STOP);
    }
}

void
Datasette::processCommand(const Cmd &cmd)
{
    switch (cmd.type) {

        case CMD_DATASETTE_PLAY:    pressPlay(); break;
        case CMD_DATASETTE_STOP:    pressStop(); break;
        case CMD_DATASETTE_REWIND:  rewind(); break;

        default:
            fatalError;
    }
}

void
Datasette::processMotEvent(EventID event)
{
    switch (event) {

        case MOT_START: msgQueue.put(MSG_VC1530_MOTOR, true);
        case MOT_STOP:  msgQueue.put(MSG_VC1530_MOTOR, false);

        default:
            break;
    }

    c64.cancel<SLOT_MOT>();
}

void
Datasette::processDatEvent(EventID event, i64 cycles)
{
    assert(event == DAT_EXECUTE);

    for (isize i = 0; i < cycles; i++) {

        if (--nextRisingEdge == 0) {

            cia1.triggerRisingEdgeOnFlagPin();
        }

        if (--nextFallingEdge == 0) {

            cia1.triggerFallingEdgeOnFlagPin();

            if (head < numPulses) {

                schedulePulse(head);
                advanceHead();

            } else {

                pressStop();
            }
        }
    }

    scheduleNextDatEvent();
}

void
Datasette::updateDatEvent()
{
    if (playKey && motor && hasTape() && config.connected) {

        scheduleNextDatEvent();

    } else {

        c64.cancel<SLOT_DAT>();
    }
}

void
Datasette::scheduleNextDatEvent()
{
    static constexpr Cycle period = 16;

    if (playKey && motor && hasTape() && config.connected) {

        // Call the execution handler periodically
        c64.scheduleRel<SLOT_DAT>(period, DAT_EXECUTE, period);

    } else {

        c64.cancel<SLOT_DAT>();
    }
}

void
Datasette::schedulePulse(isize nr)
{
    assert(nr < numPulses);
    
    // The VC1530 uses square waves with a 50% duty cycle
    nextRisingEdge = pulses[nr].cycles / 2;
    nextFallingEdge = pulses[nr].cycles;
}

}
