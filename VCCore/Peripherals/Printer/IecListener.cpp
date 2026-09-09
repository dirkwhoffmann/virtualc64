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

#include "IecListener.h"

namespace vc64 {

void
IecListener::reset()
{
    if (pendingTimer != PendingTimer::None) host.iecCancel();

    state = State::Idle;
    clock = false;
    data = false;
    addressedToUs = false;
    bitCount = 0;
    shiftReg = 0;
    lastAtn = true;
    lastClk = true;
    awaitingRts = true;
    channelSelected = false;
    eoiPending = false;
    pendingTimer = PendingTimer::None;

    // clock/data may have been asserted (e.g. reset() called mid-hold), so
    // the host must always be told to re-run the wired-AND, not just when
    // we can prove something changed.
    host.iecLinesChanged();
}

void
IecListener::rearm(PendingTimer next, Cycle delay)
{
    if (pendingTimer != PendingTimer::None) host.iecCancel();
    pendingTimer = next;
    host.iecSchedule(delay);
}

void
IecListener::lineTransition(bool atn, bool clk, bool dat)
{
    // A release-to-assert edge on ATN (true -> false) starts a fresh
    // command byte, discarding whatever partial byte (if any) was in
    // flight, and cancels any handshake timer left over from the data
    // path. A stale EOI flag must not survive either: if ATN interrupts
    // before the byte it belongs to actually completes, the next byte
    // received after re-addressing is unrelated and must not inherit it.
    //
    // A DATA hold in effect is deliberately KEPT. The talker requires
    // DATA to be asserted before it starts each byte -- the KERNAL's send
    // routine reads the bus and bails out with DEVICE NOT PRESENT if DATA
    // is released (ISOUR, $ED44/$ED47) -- and that applies to the command
    // byte it is about to clock under this ATN just as much as to a data
    // byte. The hold cannot wedge the bus: the ready-to-send edge that
    // releases it always follows.
    if (!atn && lastAtn) {

        bitCount = 0;
        shiftReg = 0;
        eoiPending = false;
        awaitingRts = true;

        if (pendingTimer != PendingTimer::None) {

            host.iecCancel();
            pendingTimer = PendingTimer::None;
        }

        // Acknowledge the attention request by pulling DATA low, the way
        // every real device's ATN hardware does. This is what the
        // talker's presence check reads before it clocks the command byte
        // ($ED44/$ED47); without it, a bus with no drives on it reports
        // DEVICE NOT PRESENT even though the printer is right there. The
        // ready-to-send edge of the command byte releases it.
        if (!data) {

            data = true;
            host.iecLinesChanged();
        }
    }

    // The reverse edge ends the command frame. If this device was not
    // addressed, the conversation that follows belongs to somebody else,
    // so any acknowledge hold left over from the command bytes must be
    // released -- squatting on DATA would stall their transfer. If this
    // device IS the addressed listener, the hold stays: it is what tells
    // the talker a listener is present when it checks the line before
    // sending the first data byte.
    if (atn && !lastAtn && state != State::Listening && data) {

        data = false;
        host.iecLinesChanged();
    }

    if (!atn) {

        state = state == State::Listening ? State::Listening : State::UnderAttention;
    }

    // While Listening and ATN released, we're on the normal data path.
    bool listeningData = atn && state == State::Listening;

    // The moment ATN is released while we're already the addressed
    // listener marks the start of the data phase, with CLK already
    // observed released: this is the first point at which an extended
    // CLK-released gap could mean "the talker is about to signal EOI
    // before even the first byte". Arm the watch here, on this specific
    // edge, exactly once. If CLK is still ASSERTED at this exact moment —
    // a legal bus turnaround the live KERNAL path happens not to use, but
    // the protocol permits — this edge cannot arm anything yet: there is
    // no way to tell, from the ATN edge alone, whether CLK will release
    // again almost immediately (an ordinary first bit) or stay asserted
    // for a long quiet spell (an EOI announcement before the first byte).
    // The byte-boundary re-arm below resolves that ambiguity the only way
    // it can be resolved: by treating the CLK-release edge that carries a
    // byte's FIRST bit as both a real bit (DAT is valid at every such edge
    // per the protocol, whether or not the talker also pauses there) and
    // the start of a fresh EOI-detect watch, then letting the passage of
    // time (a genuine next bit's edge cancels it; wakeUp() firing confirms
    // it) decide which it was. Bits 1-7 never arm anything: a talker
    // announces EOI before a byte, never in the middle of one.
    //
    // This replaces what used to be a LEVEL check ("CLK is currently
    // released and was released last time too"), which re-armed on ANY
    // invocation of lineTransition() while CLK happened to be sitting
    // released, including calls caused by something that has nothing to
    // do with a fresh quiet gap (this device's own bus contribution
    // echoing back through the wired-AND, or any other external
    // transition that leaves CLK untouched). That both caused a
    // self-sustaining oscillation (assert -> echo -> re-arm -> assert...)
    // and could mis-flag an unrelated later byte as EOI. Arming only on
    // genuine edges — this ATN-release edge, the first-bit edge below, and
    // the ack-release point in wakeUp() — means arming is driven
    // exclusively by real protocol events, never by an incidental
    // re-evaluation of already-settled lines.
    if (atn && !lastAtn && clk && state == State::Listening && pendingTimer == PendingTimer::None) {

        rearm(PendingTimer::EoiDetect, eoiDetectCycles);
    }

    if (!atn || listeningData) {

        // A bit is valid on the edge where CLK is released (low -> high).
        if (clk && !lastClk) {

            // A new bit arrived, so whatever we were waiting on CLK for
            // (i.e. an EOI announcement) didn't happen — this is an
            // ordinary bit, not an EOI.
            if (pendingTimer == PendingTimer::EoiDetect) {

                host.iecCancel();
                pendingTimer = PendingTimer::None;
            }

            // The talker's "ready to send" announcement precedes each
            // byte and carries no data bit. Consume it as the handshake it
            // is, answering with "ready for data" by releasing DATA; the
            // eight bit-carrying edges start on the next release. This
            // edge still opens a byte boundary for EOI-detect purposes --
            // the EOI-announcing gap is precisely the pause between this
            // handshake and the talker starting bit 0 -- so it falls
            // through to the arm below rather than returning early.
            bool rtsEdge = awaitingRts;

            if (rtsEdge) {

                awaitingRts = false;

                if (data) {

                    data = false;
                    host.iecLinesChanged();
                }
            }

            // Whether THIS edge opens a byte: either the handshake that
            // precedes bit 0, or bit 0 itself. Captured before bitCount
            // moves on. See the arm below.
            bool firstBitOfByte = rtsEdge || bitCount == 0;

            if (!rtsEdge) {

                if (dat) shiftReg = u8(shiftReg | (1u << bitCount));
                bitCount++;

                if (bitCount == 8) {

                    if (!atn) {

                        processAtnByte(shiftReg);

                    } else {

                        // No secondary address arrived for this listen
                        // episode, so the default channel applies.
                        // Announce it now, once, before the first byte
                        // lands.
                        if (!channelSelected) {

                            channelSelected = true;
                            host.iecListen(0);
                        }

                        bool eoi = eoiPending;
                        eoiPending = false;
                        host.iecByte(shiftReg, eoi);
                    }

                    // Acknowledge the byte by asserting DATA -- and HOLD
                    // it. The talker waits up to a millisecond for this
                    // pull-down after the last bit (ISOUR, $ED9C-$EDA9),
                    // and the same hold is what its presence check reads
                    // at the start of the NEXT byte ($ED44/$ED47):
                    // acknowledge and not-ready-for-data are one and the
                    // same line level. The hold ends at the next
                    // ready-to-send edge, or when an ATN frame ends
                    // without this device being addressed. A timed
                    // release here (as this once did) re-runs the
                    // presence check against a floating line and loses
                    // the whole transfer to a race.
                    if (!data) {

                        data = true;
                        host.iecLinesChanged();
                    }

                    bitCount = 0;
                    shiftReg = 0;
                    awaitingRts = true;
                }
            }

            // This edge just released CLK while we're on the normal data
            // path. A talker only ever announces EOI in the gap that
            // precedes a byte, never between two bits of the same byte, so
            // the watch is armed at a BYTE BOUNDARY only -- i.e. on the
            // edge that carries bit 0 (firstBitOfByte), never on bits 1-7.
            // Arming on every bit, as this used to, would mis-flag a byte
            // as EOI whenever an inter-bit gap happened to exceed
            // eoiDetectCycles, and cost a pointless schedule/cancel pair
            // per bit.
            //
            // Why arm on bit 0's own edge rather than strictly before it:
            // the gap preceding bit 0 normally opens at the ATN-release
            // edge, which is handled above -- but only if CLK is already
            // released at that moment. In the legal turnaround where ATN
            // releases with CLK still ASSERTED, that edge cannot arm
            // anything, and this is then the earliest edge that can. DAT
            // is valid at every CLK-release edge per the protocol, so the
            // edge is taken as bit 0 and as the start of the watch, and
            // the passage of time decides which it was: a genuine next
            // bit's edge cancels it (the check at the top of this branch);
            // if none comes within eoiDetectCycles, wakeUp() treats the
            // gap since THIS edge as an EOI announcement.
            //
            // The pendingTimer guard keeps this arm from stealing a
            // timer some other path already has in flight.
            if (listeningData && firstBitOfByte && pendingTimer == PendingTimer::None) {

                rearm(PendingTimer::EoiDetect, eoiDetectCycles);
            }

        } else if (listeningData && !clk && lastClk && pendingTimer == PendingTimer::EoiDetect) {

            // The talker resumed clocking before the EOI window elapsed:
            // no EOI after all.
            host.iecCancel();
            pendingTimer = PendingTimer::None;
        }
    }

    lastAtn = atn;
    lastClk = clk;
}

void
IecListener::processAtnByte(u8 byte)
{
    if (byte == 0x3F) {

        // UNLISTEN
        if (state == State::Listening) host.iecUnlisten();
        state = State::Idle;
        addressedToUs = false;
        channelSelected = false;
        return;
    }

    if ((byte & 0xE0) == 0x20) {

        // LISTEN. A secondary address is OPTIONAL: `OPEN 4,4` sends this
        // command and nothing else, which is the ordinary way BASIC talks
        // to a printer. So being addressed is by itself enough to start
        // listening, on the default channel. A secondary address, if one
        // does follow, only refines that choice (SA 7 selects the business
        // character set) -- it is not what grants permission to listen.
        addressedToUs = (isize(byte & 0x1F) == deviceNum);

        if (addressedToUs) {

            state = State::Listening;
            channelSelected = false;
        }
        return;
    }

    if ((byte & 0xE0) == 0x40) {

        // TALK — the printer is never a talker. Recognise it so it doesn't
        // get mistaken for anything else, but there is nothing to do.
        return;
    }

    if ((byte & 0xE0) == 0x60) {

        // Secondary address
        if (addressedToUs) {

            state = State::Listening;
            channelSelected = true;
            host.iecListen(byte & 0x1F);
        }
        return;
    }

    // Anything else observed under ATN is not part of this device's
    // vocabulary; ignore it.
}

void
IecListener::wakeUp()
{
    switch (pendingTimer) {

        case PendingTimer::None:

            break;

        case PendingTimer::EoiDetect:

            // CLK stayed released past the EOI detection threshold: treat
            // this as an EOI announcement and acknowledge it by pulling
            // DATA low, then schedule its release. The timer that just
            // fired is already consumed, so clear it before rearm() so it
            // doesn't try to cancel an already-fired schedule.
            pendingTimer = PendingTimer::None;
            eoiPending = true;
            data = true;
            host.iecLinesChanged();
            rearm(PendingTimer::EoiAckRelease, eoiHoldCycles);
            break;

        case PendingTimer::EoiAckRelease:

            // Deliberately NOT re-armed from here: doing so would
            // perpetually alternate between "detect EOI" and "ack it"
            // with no external cause, recreating the same class of
            // self-sustaining oscillation that arming on a bus-line
            // LEVEL (rather than a genuine edge) caused in the first
            // place. The next genuine CLK edge (a real bit arriving) or
            // ATN transition re-establishes watching as needed.
            data = false;
            host.iecLinesChanged();
            pendingTimer = PendingTimer::None;
            break;

    }
}

}
