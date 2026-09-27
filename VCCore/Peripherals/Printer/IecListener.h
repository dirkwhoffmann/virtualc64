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

#include "C64Types.h"

namespace vc64 {

// Callbacks IecListener needs from whatever owns it. Printer implements this
// against the real emulator; unit tests implement a fake in-memory
// stand-in that just records what was called.
class IecHost {

public:

    virtual ~IecHost() = default;

    // Ask to be woken up via wakeUp() after `delay` cycles have elapsed.
    virtual void iecSchedule(Cycle delay) = 0;

    // Cancel a previously requested wakeUp() call, if still pending.
    virtual void iecCancel() = 0;

    // Called whenever the listener's own clock/data bus contribution changed,
    // so the host can recompute the shared bus lines.
    virtual void iecLinesChanged() = 0;

    // Called once the listener has been addressed to listen, with the
    // secondary address (lower 5 bits of the 0x60|sa command byte).
    virtual void iecListen(u8 sa) = 0;

    // Called when an UNLISTEN command ends an active listen session.
    virtual void iecUnlisten() = 0;

    // Called for every data byte received while listening.
    virtual void iecByte(u8 value, bool eoi) = 0;
};

// Implements the Commodore IEC serial bus protocol as a pure state machine,
// with no knowledge of the C64, of SerialPort, or of what a "printer" is.
// It is driven entirely by line-transition calls and reports what happened
// through the IecHost callbacks.
//
// LINE POLARITY — the single most important thing to get right here:
//
//   - `clock` and `data` (below) are this device's OWN bus contribution, in
//     PIN-value form, matching the convention SerialPort uses for its device
//     pins (SerialPort.cpp: `clockLine = !device1Clock && !device2Clock &&
//     !ciaClock`): `true` means THIS DEVICE IS PULLING THE LINE LOW, `false`
//     means released. After reset() both are false (nothing pulled).
//
//   - `atn`, `clk` and `dat`, the incoming arguments of lineTransition(),
//     are ALL bus-line values, exactly as computed by SerialPort's
//     wired-AND (`atnLine`, `clockLine`, `dataLine`): `true` means the line
//     is currently HIGH/released, `false` means it is LOW/asserted. This is
//     the OPPOSITE sense from `clock`/`data` above, which is exactly why
//     they are named differently rather than reusing `clk`/`dat` as member
//     names. `atn == false` means ATN IS ASSERTED (SerialPort.cpp:47 —
//     `atnLine = !ciaAtn`, same polarity as `clockLine`/`dataLine`).
//     Printer wires this up with a plain, unmodified
//     `lineTransition(atnLine, clockLine, dataLine)` — no negation at
//     the call site.
class IecListener {

public:

    IecListener(IecHost &host) : host(host) { }

    // Copies the state machine's own data, but not the `host` reference,
    // which stays bound to whichever object constructed this listener.
    // Needed because `host` is a reference member, which would otherwise
    // make the implicitly-generated copy assignment operator deleted
    // (required for run-ahead cloning via the owning Printer's CLONE macro).
    IecListener &operator=(const IecListener &other)
    {
        deviceNum = other.deviceNum;
        clock = other.clock;
        data = other.data;
        state = other.state;
        addressedToUs = other.addressedToUs;
        bitCount = other.bitCount;
        shiftReg = other.shiftReg;
        lastAtn = other.lastAtn;
        lastClk = other.lastClk;
        awaitingRts = other.awaitingRts;
        channelSelected = other.channelSelected;
        eoiPending = other.eoiPending;
        pendingTimer = other.pendingTimer;
        return *this;
    }

    // Bus timing constants, taken from Commodore's own IEC serial bus
    // timing table (the named-parameter min/typ/max table reproduced in
    // Commodore's C64/Plus4 serial bus documentation). The C64 runs at
    // ~1 cycle per microsecond, so for our purposes microseconds map 1:1 to
    // emulator cycles (that document's own caveat: do a more precise
    // PAL-exact conversion if ever needed).

    // Tat: maximum time a listener may take to respond to ATN before the
    // talker treats it as "device not present".
    static constexpr Cycle atnResponseCycles = 1000;

    // Tye: how long the talker holds CLK released, past the normal
    // handshake window, before a listener is expected to recognise this as
    // an EOI signal. Documented window is 200-250us; we detect at the start
    // of that window.
    static constexpr Cycle eoiDetectCycles = 200;

    // Tei (with the "external listener" footnote): how long the listener
    // must hold DATA low to acknowledge it has recognised the EOI
    // condition. Documented minimum for an external device acting as
    // listener is 80us.
    static constexpr Cycle eoiHoldCycles = 80;

    // Device number this listener answers to (4 or 5 for a real MPS-803).
    isize deviceNum = 4;

    // This listener's own contribution to the CLOCK/DATA bus lines, in
    // pin-value form (true = pulling the line low). See the class comment.
    bool clock = false;
    bool data = false;

    // Resets the state machine to its power-on/idle condition: not
    // addressed, not listening, both bus lines released.
    void reset();

    // Feed a new set of bus line values into the state machine. Called
    // whenever ATN, CLK or DATA actually changes.
    void lineTransition(bool atn, bool clk, bool dat);

    // Fired by the host after a previously requested iecSchedule() delay
    // has elapsed. Used by the timed steps of the byte handshake.
    void wakeUp();

    // True once this device has been addressed with LISTEN + a secondary
    // address and has not since been UNLISTENed.
    bool isListening() const { return state == State::Listening; }

    template <class T>
    void serialize(T &worker)
    {
        worker

        << state
        << deviceNum
        << clock
        << data
        << addressedToUs
        << bitCount
        << shiftReg
        << lastAtn
        << lastClk
        << awaitingRts
        << channelSelected
        << eoiPending
        << pendingTimer;
    }

private:

    // Parsing/session phase. Idle: never addressed (or just UNLISTENed).
    // UnderAttention: ATN currently asserted; not (yet) the addressed
    // listener for the command byte in progress. Listening: addressed via
    // LISTEN + a matching secondary address, and not since UNLISTENed —
    // this is what byte reception gates on together with ATN being
    // released.
    enum class State { Idle, UnderAttention, Listening };

    // Which timed handshake step (if any) is currently pending a wakeUp()
    // call. Only one timer can be outstanding at a time; scheduling a new
    // one always cancels whatever was previously pending first.
    enum class PendingTimer {

        None,           // Nothing scheduled.
        EoiDetect,      // Waiting to see if CLK stays released long enough
                        // to count as an EOI announcement.
        EoiAckRelease   // Holding DATA low to acknowledge EOI; waiting to
                        // release it again. (The byte acknowledge, by
                        // contrast, is not timed at all: it holds until
                        // the talker's next ready-to-send edge.)
    };

    IecHost &host;

    State state = State::Idle;

    // True while the most recently decoded LISTEN command byte named our
    // own device number, i.e. we're waiting for its secondary address.
    bool addressedToUs = false;

    // Bit accumulator, shared between command bytes clocked in under ATN
    // and data bytes clocked in while Listening with ATN released — the two
    // never happen at the same time.
    isize bitCount = 0;
    u8 shiftReg = 0;

    // Previous line values, used to detect the edges that matter: ATN's
    // release-to-assert edge (`true` -> `false`, starts a fresh command
    // byte) and CLK's assert-to-release edge (`false` -> `true`, the bit is
    // valid there). Both default to `true` (released), matching the idle
    // bus.
    bool lastAtn = true;
    bool lastClk = true;

    /* True while the next CLK-release edge is the talker's "ready to send"
     * announcement rather than the first data bit.
     *
     * The talker does not begin clocking bits the instant it has something
     * to say. Each byte is preceded by a handshake: the talker holds CLK
     * asserted, the listeners hold DATA asserted, and the talker then
     * RELEASES CLK to announce "ready to send". Only after the listener
     * answers by releasing DATA ("ready for data") does the talker clock
     * the eight data bits, each valid on its own CLK-release edge.
     *
     * That announcement is electrically indistinguishable from a bit's
     * CLK-release edge, so it has to be accounted for explicitly: counting
     * it as bit 0 shifts the whole byte left by one and drops bit 7, which
     * turns LISTEN 4 (0x24) into 0x48 -- a TALK command for device 8 --
     * and the printer is never addressed at all.
     */
    bool awaitingRts = true;

    /* True once the channel for the current listen episode has been handed
     * to the host, either by an explicit secondary address or by falling
     * back to the default. Deferring the fallback until the first data byte
     * keeps the host seeing exactly one channel selection per episode: an
     * SA, when one follows LISTEN, must be able to win without the default
     * having already been announced.
     */
    bool channelSelected = false;

    // True once an EOI condition has been detected for the byte currently
    // being clocked in; consumed (and cleared) when that byte completes.
    bool eoiPending = false;

    // Which timed handshake step, if any, is waiting on a wakeUp() call.
    PendingTimer pendingTimer = PendingTimer::None;

    // Decodes one fully-clocked-in command byte received while ATN was
    // asserted: LISTEN, TALK, UNLISTEN or a secondary address.
    void processAtnByte(u8 byte);

    // Cancels whatever timer is currently pending (if any) and schedules a
    // new one, keeping host.iecSchedule()/iecCancel() paired 1:1.
    void rearm(PendingTimer next, Cycle delay);
};

}
