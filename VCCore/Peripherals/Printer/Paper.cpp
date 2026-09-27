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

#include "Paper.h"

namespace vc64 {

void
Paper::clear()
{
    dots.clear();
}

void
Paper::setDot(isize x, isize y, bool value)
{
    if (x < 0 || x >= width || y < 0) return;

    auto neededRows = y + 1;
    if (neededRows > rows()) {
        dots.resize(usize(neededRows) * usize(bytesPerRow), 0);
    }

    auto byteIndex = usize(y) * usize(bytesPerRow) + usize(x / 8);
    auto bitMask = u8(0x80 >> (x % 8));

    if (value) {
        dots[byteIndex] |= bitMask;
    } else {
        dots[byteIndex] &= ~bitMask;
    }
}

bool
Paper::getDot(isize x, isize y) const
{
    if (x < 0 || x >= width || y < 0 || y >= rows()) return false;

    auto byteIndex = usize(y) * usize(bytesPerRow) + usize(x / 8);
    auto bitMask = u8(0x80 >> (x % 8));

    return (dots[byteIndex] & bitMask) != 0;
}

isize
Paper::rows() const
{
    return isize(dots.size()) / bytesPerRow;
}

isize
Paper::pages() const
{
    return rows() / height;
}

void
Paper::formFeed()
{
    auto remainder = rows() % height;
    if (remainder == 0) return;

    auto nextBoundary = rows() + (height - remainder);
    dots.resize(usize(nextBoundary) * usize(bytesPerRow), 0);
}

isize
Paper::pageStart(isize page) const
{
    return page * height;
}

isize
Paper::copyPage(isize page, u8 *dst, isize capacity) const
{
    if (page < 0 || dst == nullptr || capacity <= 0) return 0;
    if (pageStart(page) >= rows()) return 0;

    auto start = pageStart(page);
    isize written = 0;

    for (isize y = 0; y < height && written < capacity; y++) {

        for (isize x = 0; x < width && written < capacity; x++) {

            // Greyscale luminance, so ink is BLACK (0x00) and bare paper
            // is WHITE (0xFF). The buffer is handed straight to a
            // DeviceGray CGImage, where 0 is black and 255 is white, so
            // the opposite mapping renders a black page with white ink.
            dst[written++] = getDot(x, start + y) ? 0x00 : 0xFF;
        }
    }

    return written;
}

}
