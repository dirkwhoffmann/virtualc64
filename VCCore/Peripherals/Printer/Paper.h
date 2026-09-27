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

#include "BasicTypes.h"
#include <vector>

namespace vc64 {

// Represents the dot bitmap of a continuous roll of fan-fold printer paper.
//
// The paper is modelled as a single, ever-growing sheet: the y coordinate is
// absolute across the whole roll, not relative to the current page. Pages
// have a fixed height, so a page is considered "complete" the moment the
// roll has been written past a multiple of that height, either because
// formFeed() was called or simply because a dot was set beyond the page
// boundary. This class has no knowledge of the IEC bus or of MPS-803
// semantics; it is pure data with no dependencies on the rest of the
// emulator.
class Paper {

public:

    // Width of the paper in dots
    static constexpr isize width = 480;

    // Height of a single page in dot rows (11in * 72dpi)
    static constexpr isize height = 792;

private:

    // Bytes per dot row (8 dots per byte)
    static constexpr isize bytesPerRow = width / 8;

    // Dot storage: one row is bytesPerRow bytes, appended as rows are touched.
    // The number of rows stored (dots.size() / bytesPerRow) is the absolute
    // row count reached so far on the continuous roll.
    std::vector<u8> dots;

public:

    Paper() = default;

    // Wipes the paper clean
    void clear();

    // Turns a single dot on or off. Coordinates outside the paper are
    // silently ignored (a real printer cannot print off the paper).
    void setDot(isize x, isize y, bool value);

    // Reads back the state of a single dot. Dots outside the paper, or that
    // have never been written to, read as false (off).
    bool getDot(isize x, isize y) const;

    // Number of dot rows used so far on the roll (absolute, not page-relative)
    isize rows() const;

    // Number of pages that have been completed so far, i.e.
    // rows() / height using integer division. A page completes the moment
    // the roll has advanced past a multiple of `height`, whether via an
    // explicit formFeed() or simply by writing a dot beyond the boundary --
    // both paths only ever grow `rows()`, so this single formula covers both.
    isize pages() const;

    // Pads the used row count up to the next page boundary. Does not move
    // an origin -- y coordinates remain absolute across the whole roll.
    void formFeed();

    // First dot row of page `page`. Pages have a fixed height, so this is
    // always page * height, regardless of how much has actually been
    // printed.
    isize pageStart(isize page) const;

    // Copies page `page` into `dst` as 8-bit greyscale, one byte per dot
    // (0x00 or 0xFF), row-major, so a caller (the GUI) can build a greyscale
    // bitmap without any bit twiddling. `page` need not be complete -- any
    // page that has been touched at all (i.e. pageStart(page) < rows()) is
    // copyable, so a live view can render a page while it is still being
    // printed; untouched rows within it simply come back blank. Returns 0 if
    // `page` has not been touched yet. Never writes past `capacity` bytes --
    // a `dst` buffer too small for the full page (width * height bytes)
    // gets a truncated copy and the actual number of bytes written is
    // returned, so a short buffer can never be overrun.
    isize copyPage(isize page, u8 *dst, isize capacity) const;

    template <class T> void serialize(T &worker)
    {
        worker << dots;
    }
};

}
