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
#include "Serializable.h"

namespace vc64 {

void 
Serializable::hardReset() {

    SerHardResetter resetter;
    *this << resetter;
}

void
Serializable::softReset() {

    SerSoftResetter resetter;
    *this << resetter;
}

isize
Serializable::size() {

    SerCounter counter;
    *this << counter;
    return counter.count;
}

u64 
Serializable::checksum() {

    SerChecker checker;
    *this << checker;
    return checker.hash;
}

isize 
Serializable::load(const u8 *buffer) {

    SerReader reader(buffer);
    *this << reader;
    return (isize)(reader.ptr - buffer);
}

isize 
Serializable::save(u8 *buffer) {

    SerWriter writer(buffer);
    *this << writer;
    return (isize)(writer.ptr - buffer);
}

}

