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

#include "TestSupport.h"
#include "IecListener.h"

#include <utility>
#include <vector>

using namespace vc64;

namespace {

// Bus-line convention, matching SerialPort: false = asserted (low), true = released (high)
constexpr bool atnAsserted = false;
constexpr bool atnReleased = true;

struct FakeHost : public IecHost {

    isize scheduled = 0;
    isize cancelled = 0;
    isize lineChanges = 0;
    std::vector<u8> listened;
    isize unlistened = 0;
    std::vector<std::pair<u8, bool>> bytes;

    void iecSchedule(Cycle delay) override { scheduled++; }
    void iecCancel() override { cancelled++; }
    void iecLinesChanged() override { lineChanges++; }
    void iecListen(u8 sa) override { listened.push_back(sa); }
    void iecUnlisten() override { unlistened++; }
    void iecByte(u8 value, bool eoi) override { bytes.push_back({ value, eoi }); }
};

// Clocks one byte to the listener the way the real bus does it.
//
// A byte is NOT just eight bit-clocks. Each one is preceded by a handshake:
// the talker holds CLK asserted while the listeners hold DATA asserted, then
// the talker RELEASES CLK to announce "ready to send", and the listener
// answers by releasing DATA ("ready for data"). Only then do the eight
// bit-carrying CLK-release edges follow, LSB first, each bit valid on its own
// release edge. That makes nine CLK-release edges per byte, not eight.
//
// Modelling only the eight is what let a real bug through: the listener
// counted the "ready to send" edge as bit 0, shifting every byte left one
// place and dropping bit 7, so LISTEN 4 (0x24) arrived as 0x48 -- read as a
// TALK for device 8 -- and the printer was never addressed.
// The "ready to send" / "ready for data" exchange that precedes every byte.
// Tests that clock bits by hand still have to run this first, or their first
// bit-clock is swallowed as the handshake.
void sendHandshake(IecListener &listener, bool atn)
{
    listener.lineTransition(atn, false, true);      // CLK asserted
    listener.lineTransition(atn, true, true);       // CLK released -- ready to send
}

void sendByte(IecListener &listener, bool atn, u8 value)
{
    // Handshake: "ready to send", answered by "ready for data"
    listener.lineTransition(atn, false, false);     // CLK asserted, DATA asserted
    listener.lineTransition(atn, true, false);      // CLK released -- ready to send
    listener.lineTransition(atn, true, true);       // DATA released -- ready for data

    for (isize i = 0; i < 8; i++) {

        bool bit = (value >> i) & 1;

        listener.lineTransition(atn, false, bit);   // CLK low, DATA carries the bit
        listener.lineTransition(atn, true, bit);    // CLK high, bit is valid
    }
}

}

TEST(listener_survives_the_ready_to_send_handshake)
{
    // Regression: the "ready to send" CLK release that precedes every byte
    // must not be counted as a data bit. When it was, LISTEN 4 (0x24) came
    // through as 0x48 and the printer never recognised its own address.
    FakeHost host;
    IecListener listener(host);
    listener.reset();
    listener.deviceNum = 4;

    // Bus idle, then the talker asserts ATN
    listener.lineTransition(atnReleased, true, true);
    listener.lineTransition(atnAsserted, false, true);

    sendByte(listener, atnAsserted, 0x24);      // LISTEN 4
    sendByte(listener, atnAsserted, 0x60);      // secondary address 0

    CHECK(listener.isListening());
    CHECK_EQ(host.listened.size(), (size_t)1);
}

TEST(listener_holds_data_through_the_turnaround_and_between_bytes)
{
    // The KERNAL's byte-send routine reads the DATA line before every
    // byte and aborts the whole transfer with DEVICE NOT PRESENT if it is
    // released (ISOUR, $ED44/$ED47). So the listener's acknowledge is not
    // a pulse -- it is a HOLD: asserted at the ATN turnaround and after
    // every completed byte, released only by the talker's next
    // ready-to-send edge. Without the hold, whether any byte survives
    // that check comes down to a race, which is exactly how this
    // presented: identical programs delivering one garbled byte on one
    // run and nothing at all on the next.
    FakeHost host;
    IecListener listener(host);
    listener.reset();
    listener.deviceNum = 4;

    // ATN asserted: the listener acknowledges attention by pulling DATA.
    listener.lineTransition(atnAsserted, false, true);
    CHECK(listener.data);

    sendByte(listener, atnAsserted, 0x24);      // LISTEN 4
    CHECK(listener.data);                       // command byte acknowledged

    // Turnaround: ATN releases with CLK asserted (the KERNAL's ordering).
    // Being the addressed listener, the hold must survive -- it is what
    // the talker's presence check is about to read.
    listener.lineTransition(atnReleased, false, true);
    CHECK(listener.data);

    // First data byte: its ready-to-send edge releases the hold ...
    sendByte(listener, atnReleased, 0x48);      // 'H'
    CHECK_EQ(host.bytes.size(), (size_t)1);
    CHECK_EQ(host.bytes[0].first, (u8)0x48);

    // ... and completing it re-asserts the hold for the next check.
    CHECK(listener.data);

    // A second byte flows the same way: no pulses, no races.
    sendByte(listener, atnReleased, 0x45);      // 'E'
    CHECK_EQ(host.bytes.size(), (size_t)2);
    CHECK(listener.data);
}

TEST(listener_releases_data_when_the_attention_frame_was_not_for_it)
{
    // The same hold must NOT survive an ATN frame that addressed some
    // other device: squatting on DATA would stall their transfer.
    FakeHost host;
    IecListener listener(host);
    listener.reset();
    listener.deviceNum = 4;

    listener.lineTransition(atnAsserted, false, true);
    sendByte(listener, atnAsserted, 0x28);      // LISTEN 8 -- not us
    CHECK(listener.data);                       // ack under ATN, like any device

    listener.lineTransition(atnReleased, false, true);
    CHECK(!listener.data);                      // their conversation now
}

TEST(listener_listens_without_a_secondary_address)
{
    // A secondary address is optional. `OPEN 4,4` -- the ordinary way to
    // reach a printer from BASIC -- sends LISTEN 4 and nothing else, so a
    // listener that only starts on a secondary-address byte never receives
    // a thing. The SA, when present, selects the character set (0 =
    // graphic, 7 = business); when absent the default applies.
    FakeHost host;
    IecListener listener(host);
    listener.reset();
    listener.deviceNum = 4;

    listener.lineTransition(atnAsserted, true, true);
    sendByte(listener, atnAsserted, 0x24);      // LISTEN 4, no SA follows

    CHECK(listener.isListening());

    // Data sent after ATN releases must reach the host, and the default
    // channel is announced once, just before the first byte lands.
    listener.lineTransition(atnReleased, true, true);
    sendByte(listener, atnReleased, 0x41);

    CHECK_EQ(host.listened.size(), (size_t)1);
    CHECK_EQ(host.listened[0], (u8)0);          // default channel
    CHECK_EQ(host.bytes.size(), (size_t)1);
    CHECK_EQ(host.bytes[0].first, (u8)0x41);
}

TEST(listener_ignores_traffic_for_other_devices)
{
    FakeHost host;
    IecListener listener(host);
    listener.reset();

    // 0x28 = LISTEN device 8
    listener.lineTransition(atnAsserted, true, true);
    sendByte(listener, atnAsserted, 0x28);

    CHECK(host.listened.empty());
    CHECK(!listener.isListening());
}

TEST(listener_accepts_its_own_listen_address)
{
    FakeHost host;
    IecListener listener(host);
    listener.reset();
    listener.deviceNum = 4;

    // 0x24 = LISTEN device 4, then 0x60 = secondary address 0 (OPEN channel)
    listener.lineTransition(atnAsserted, true, true);
    sendByte(listener, atnAsserted, 0x24);
    sendByte(listener, atnAsserted, 0x60);

    CHECK(listener.isListening());
    CHECK_EQ(host.listened.size(), (size_t)1);
    CHECK_EQ(host.listened[0], (u8)0);
}

TEST(listener_accepts_device_five_when_configured)
{
    FakeHost host;
    IecListener listener(host);
    listener.reset();
    listener.deviceNum = 5;

    listener.lineTransition(atnAsserted, true, true);
    sendByte(listener, atnAsserted, 0x25);
    sendByte(listener, atnAsserted, 0x67);

    CHECK(listener.isListening());
    CHECK_EQ(host.listened[0], (u8)7);
}

TEST(listener_unlistens)
{
    FakeHost host;
    IecListener listener(host);
    listener.reset();

    listener.lineTransition(atnAsserted, true, true);
    sendByte(listener, atnAsserted, 0x24);
    sendByte(listener, atnAsserted, 0x60);
    sendByte(listener, atnAsserted, 0x3F);      // UNLISTEN

    CHECK(!listener.isListening());
    CHECK_EQ(host.unlistened, (isize)1);
}

TEST(listener_receives_a_data_byte)
{
    FakeHost host;
    IecListener listener(host);
    listener.reset();

    listener.lineTransition(atnAsserted, true, true);
    sendByte(listener, atnAsserted, 0x24);
    sendByte(listener, atnAsserted, 0x60);
    sendByte(listener, atnReleased, 0x41);      // 'A'

    CHECK_EQ(host.bytes.size(), (size_t)1);
    CHECK_EQ(host.bytes[0].first, (u8)0x41);
    CHECK(!host.bytes[0].second);
}

TEST(listener_reports_eoi_on_the_last_byte)
{
    FakeHost host;
    IecListener listener(host);
    listener.reset();

    listener.lineTransition(atnAsserted, true, true);
    sendByte(listener, atnAsserted, 0x24);
    sendByte(listener, atnAsserted, 0x60);

    // The talker signals EOI by holding CLK released past the timeout
    // before clocking the byte out. The listener acknowledges by pulling
    // DATA low.
    listener.lineTransition(atnReleased, true, true);
    listener.wakeUp();                          // EOI timeout expires
    CHECK(listener.data);                       // acknowledged

    sendByte(listener, atnReleased, 0x0D);

    CHECK_EQ(host.bytes.size(), (size_t)1);
    CHECK(host.bytes[0].second);                // EOI flagged
}

TEST(listener_drops_bytes_when_not_addressed)
{
    FakeHost host;
    IecListener listener(host);
    listener.reset();

    sendByte(listener, atnReleased, 0x41);

    CHECK(host.bytes.empty());
}

TEST(listener_reset_releases_the_bus)
{
    FakeHost host;
    IecListener listener(host);

    listener.lineTransition(atnAsserted, true, true);
    sendByte(listener, atnAsserted, 0x24);
    listener.reset();

    CHECK(!listener.clock);
    CHECK(!listener.data);
    CHECK(!listener.isListening());
}

TEST(listener_atn_interrupt_keeps_the_data_hold_as_its_acknowledge)
{
    FakeHost host;
    IecListener listener(host);
    listener.reset();

    listener.lineTransition(atnAsserted, true, true);
    sendByte(listener, atnAsserted, 0x24);
    sendByte(listener, atnAsserted, 0x60);
    sendByte(listener, atnReleased, 0x41);      // 'A' — completes the byte,
                                                 // leaving the byte-ack
                                                 // DATA hold in effect.
    CHECK(listener.data);                       // hold in effect

    // ATN reasserted mid-hold. The hold does not end here: the same line
    // level that acknowledged the byte is what the talker's presence
    // check reads before clocking the command byte ($ED44/$ED47), so
    // releasing it would race that check. It changes meaning, from byte
    // acknowledge to attention acknowledge.
    listener.lineTransition(atnAsserted, true, true);
    CHECK(listener.data);

    // The command byte's own ready-to-send edge is what releases it, so
    // nothing can wedge.
    listener.lineTransition(atnAsserted, false, false);
    listener.lineTransition(atnAsserted, true, false);
    CHECK(!listener.data);
}

TEST(listener_atn_interrupt_clears_a_pending_eoi_flag)
{
    FakeHost host;
    IecListener listener(host);
    listener.reset();

    listener.lineTransition(atnAsserted, true, true);
    sendByte(listener, atnAsserted, 0x24);
    sendByte(listener, atnAsserted, 0x60);

    // EOI is detected, but ATN interrupts before the byte it belongs to is
    // actually clocked in.
    listener.lineTransition(atnReleased, true, true);
    listener.wakeUp();
    CHECK(listener.data);

    listener.lineTransition(atnAsserted, true, true);   // ATN interrupt

    // Re-address and send an ordinary byte — unrelated to the aborted EOI.
    sendByte(listener, atnAsserted, 0x24);
    sendByte(listener, atnAsserted, 0x60);
    sendByte(listener, atnReleased, 0x41);      // 'A'

    CHECK_EQ(host.bytes.size(), (size_t)1);
    CHECK(!host.bytes[0].second);                // must NOT be flagged EOI
}

TEST(listener_reset_notifies_host_when_a_hold_was_pending)
{
    FakeHost host;
    IecListener listener(host);
    listener.reset();

    listener.lineTransition(atnAsserted, true, true);
    sendByte(listener, atnAsserted, 0x24);
    sendByte(listener, atnAsserted, 0x60);
    sendByte(listener, atnReleased, 0x41);      // completes, DATA hold pending
    CHECK(listener.data);

    isize before = host.lineChanges;
    listener.reset();

    CHECK(!listener.data);
    CHECK(host.lineChanges > before);
}

TEST(listener_settles_after_a_byte_even_when_fed_its_own_echo)
{
    FakeHost host;
    IecListener listener(host);
    listener.reset();

    listener.lineTransition(atnAsserted, true, true);
    sendByte(listener, atnAsserted, 0x24);
    sendByte(listener, atnAsserted, 0x60);
    sendByte(listener, atnReleased, 0x41);      // completes; ack hold pending
    CHECK(listener.data);

    isize lineChangesBefore = host.lineChanges;

    // A naive bus wiring (the shape of the original bug: SerialPort feeding
    // its own composite wired-AND straight back to this device, including
    // this device's own contribution) re-invokes lineTransition() with
    // ATN/CLK unchanged every time this device's own DATA output changes.
    // Drive every timer that gets armed to completion under that echo and
    // confirm the whole thing settles quickly rather than re-arming
    // forever: a level-triggered EOI arm re-enters this loop indefinitely
    // (a real instance of exactly this produced 200,000+ calls in a
    // 53-second run before it was fixed); an edge-triggered one settles
    // after the byte's own ack and, at most, one EOI-detect examination of
    // the quiet gap that follows it.
    isize iterations;
    for (iterations = 0; iterations < 1000; iterations++) {

        isize before = host.lineChanges;
        listener.wakeUp();
        listener.lineTransition(atnReleased, true, !listener.data);
        if (host.lineChanges == before) break;
    }

    CHECK(iterations < 10);
    CHECK(host.lineChanges - lineChangesBefore < 10);

    // The ack hold itself persists through the quiet gap -- it is what the
    // talker's next presence check reads -- so settling means the line
    // stops CHANGING, not that it gets released.
    CHECK(listener.data);
}

TEST(listener_external_data_only_transition_never_arms_or_cancels_eoi_detect)
{
    FakeHost host;
    IecListener listener(host);
    listener.reset();

    listener.lineTransition(atnAsserted, true, true);
    sendByte(listener, atnAsserted, 0x24);
    sendByte(listener, atnAsserted, 0x60);
    sendByte(listener, atnReleased, 0x41);      // an ordinary byte; ack pending

    // Send the first 3 bits of a second byte. Since each bit's own
    // trailing CLK-release edge legitimately (re-)arms EOI-detect (a real
    // quiet gap could begin after any bit, not just at a byte boundary --
    // that is the fix for the "EOI on CLK-still-asserted turnaround" bug),
    // pendingTimer may well already be EoiDetect by the third bit. That is
    // expected and is not what this test is about.
    u8 second = 0x0D;
    for (isize i = 0; i < 3; i++) {

        bool bit = (second >> i) & 1;
        listener.lineTransition(atnReleased, false, bit);
        listener.lineTransition(atnReleased, true, bit);
    }

    // A transition that touches neither ATN nor CLK -- only DATA -- must
    // never itself schedule or cancel anything, regardless of whatever
    // timer is (legitimately) already pending. Arming or cancelling on
    // such a transition is exactly the mistake that caused the original
    // self-sustaining oscillation (this device's own bus contribution
    // echoing straight back) and could mis-flag an unrelated byte's EOI
    // status for a reason that has nothing to do with CLK timing. It is
    // indistinguishable, from this listener's point of view, from its own
    // bus contribution echoing back.
    isize scheduledBefore = host.scheduled;
    isize cancelledBefore = host.cancelled;

    listener.lineTransition(atnReleased, true, false);  // unrelated external DATA wiggle

    CHECK_EQ(host.scheduled, scheduledBefore);
    CHECK_EQ(host.cancelled, cancelledBefore);
}

TEST(listener_detects_eoi_after_atn_release_with_clk_still_asserted)
{
    FakeHost host;
    IecListener listener(host);
    listener.reset();

    listener.lineTransition(atnAsserted, true, true);
    sendByte(listener, atnAsserted, 0x24);
    sendByte(listener, atnAsserted, 0x60);

    // ATN releases into the data phase, but CLK is still ASSERTED at that
    // exact moment -- a legal bus turnaround. The live KERNAL path always
    // releases ATN with CLK already released, so this specific ordering is
    // latent rather than exercised in practice, but the protocol permits
    // it and a byte-level listener must handle it correctly. This edge
    // alone cannot arm anything: CLK isn't released yet.
    listener.lineTransition(atnReleased, false, true);

    // CLK now releases. This is the talker's "ready to send" for the byte
    // that follows -- it carries no data bit -- and it opens the gap in
    // which an EOI announcement lives, so it arms the watch. It is the
    // earliest edge that can arm at all, since the ATN-release edge above
    // found CLK still asserted.
    listener.lineTransition(atnReleased, true, true);

    listener.wakeUp();                          // EOI timeout expires
    CHECK(listener.data);                       // acknowledged

    // Now clock the byte itself, all eight bits.
    u8 value = 0x0D;
    for (isize i = 0; i < 8; i++) {

        bool bit = (value >> i) & 1;
        listener.lineTransition(atnReleased, false, bit);
        listener.lineTransition(atnReleased, true, bit);
    }

    CHECK_EQ(host.bytes.size(), (size_t)1);
    CHECK_EQ(host.bytes[0].first, value);
    CHECK(host.bytes[0].second);                 // EOI flagged
}

TEST(listener_external_transition_mid_byte_does_not_mis_flag_next_byte_as_eoi)
{
    FakeHost host;
    IecListener listener(host);
    listener.reset();

    listener.lineTransition(atnAsserted, true, true);
    sendByte(listener, atnAsserted, 0x24);
    sendByte(listener, atnAsserted, 0x60);
    sendByte(listener, atnReleased, 0x41);      // an ordinary byte
    CHECK(listener.data);                       // ack hold in effect

    // Send the first 3 bits of a second byte. Its ready-to-send edge
    // releases the previous byte's ack hold and arms EOI-detect (a byte
    // boundary); bit 0's edge cancels it again, and bits 1-2 never
    // re-arm. So mid-byte there is nothing pending.
    u8 second = 0x0D;
    sendHandshake(listener, atnReleased);
    CHECK(!listener.data);                      // hold released at RTS
    for (isize i = 0; i < 3; i++) {

        bool bit = (second >> i) & 1;
        listener.lineTransition(atnReleased, false, bit);
        listener.lineTransition(atnReleased, true, bit);
    }

    // An external transition unrelated to this device or this byte, plus a
    // timer expiry firing on top of it. Neither may invent an EOI: the
    // byte still being clocked in is an ordinary one.
    listener.lineTransition(atnReleased, true, false);
    listener.wakeUp();
    CHECK(!listener.data);

    for (isize i = 3; i < 8; i++) {

        bool bit = (second >> i) & 1;
        listener.lineTransition(atnReleased, false, bit);
        listener.lineTransition(atnReleased, true, bit);
    }

    CHECK_EQ(host.bytes.size(), (size_t)2);
    CHECK_EQ(host.bytes[1].first, second);
    CHECK(!host.bytes[1].second);                // must NOT be flagged EOI
}

TEST(listener_does_not_flag_eoi_for_a_long_gap_in_the_middle_of_a_byte)
{
    FakeHost host;
    IecListener listener(host);
    listener.reset();

    listener.lineTransition(atnAsserted, true, true);
    sendByte(listener, atnAsserted, 0x24);
    sendByte(listener, atnAsserted, 0x60);

    // The EOI window belongs to the gap BEFORE a byte. A talker that
    // dawdles between two bits of the same byte -- longer than
    // eoiDetectCycles, so any watch armed at that point would fire -- is
    // still just clocking out an ordinary byte, and must not have it
    // flagged as the last one.
    u8 value = 0x5A;
    isize scheduledBefore = host.scheduled;
    isize cancelledBefore = host.cancelled;

    sendHandshake(listener, atnReleased);
    for (isize i = 0; i < 4; i++) {

        bool bit = (value >> i) & 1;
        listener.lineTransition(atnReleased, false, bit);
        listener.lineTransition(atnReleased, true, bit);
    }

    // The long inter-bit gap: nothing may be pending here, so a wakeUp()
    // arriving now (the worst case: a stale timer that should never have
    // been armed) must not assert DATA or raise the EOI flag.
    listener.wakeUp();
    CHECK(!listener.data);

    for (isize i = 4; i < 8; i++) {

        bool bit = (value >> i) & 1;
        listener.lineTransition(atnReleased, false, bit);
        listener.lineTransition(atnReleased, true, bit);
    }

    CHECK_EQ(host.bytes.size(), (size_t)1);
    CHECK_EQ(host.bytes[0].first, value);
    CHECK(!host.bytes[0].second);                // must NOT be flagged EOI

    // The watch is armed at the byte boundary only: one arm on the
    // handshake edge that opens the byte, one cancel on bit 0, and the
    // byte's own ack hold -- not one schedule/cancel pair per bit. The
    // point of this bound is that it does not scale with the eight bits.
    CHECK(host.scheduled - scheduledBefore <= 3);
    CHECK(host.cancelled - cancelledBefore <= 3);
}
