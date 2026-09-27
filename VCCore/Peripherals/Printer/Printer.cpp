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

#include "vcconfig.h"
#include "Printer.h"
#include "C64.h"
#include "MsgQueue.h"

namespace vc64 {

void
Printer::_didReset(bool hard)
{
    iec.reset();
    paper.clear();
    mps803.reset();
}

void
Printer::iecSchedule(Cycle delay)
{
    c64.scheduleRel<SLOT_PRT>(delay, PRT_WAKEUP);
}

void
Printer::iecCancel()
{
    c64.cancel<SLOT_PRT>();
}

void
Printer::iecLinesChanged()
{
    serialPort.setNeedsUpdate();
}

void
Printer::iecListen(u8 sa)
{
    mps803.setChannel(sa);
}

void
Printer::iecUnlisten()
{

}

void
Printer::iecByte(u8 value, bool eoi)
{
    // Coalesce PRT_ROWS / PRT_PAGE: rather than have Paper (or Mps803) post
    // a message per dot -- a bit-image graphics dump writes dots at
    // enormous rates, which would flood the message queue and stall the
    // emulator thread -- compare Paper's row/page counters before and after
    // the single write() call that renders this one byte. A single byte
    // touches at most a handful of rows (one glyph cell, one dot column, or
    // one CR/LF/FF advance), so this posts at most once per byte, i.e. at
    // most once per completed dot row (usually far less, since most bytes
    // in bit-image mode don't cross a row boundary at all).
    auto rowsBefore = paper.rows();
    auto pagesBefore = paper.pages();

    // Cap enforcement (Printer-level policy -- see the class comment on
    // maxPages): once the roll has grown past the cap, further bytes are
    // silently dropped instead of being rendered, exactly as a real printer
    // would refuse to print with no paper left.
    if (rowsBefore < maxPages * Paper::height) {
        mps803.write(value);
    }

    if (paper.rows() != rowsBefore) {
        msgQueue.put(Msg::PRT_ROWS, paper.rows());
    }
    if (paper.pages() != pagesBefore) {
        msgQueue.put(Msg::PRT_PAGE, paper.pages());
    }
}

void
Printer::processPrtEvent(EventID id)
{
    c64.cancel<SLOT_PRT>();
    iec.wakeUp();
}

bool
Printer::iecClock() const
{
    return isConnected() && iec.clock;
}

bool
Printer::iecData() const
{
    return isConnected() && iec.data;
}

}
