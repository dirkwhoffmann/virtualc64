// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/common.h"
#include "utl/primitives/BitView.h"

namespace retro::vault::GCR {

using namespace utl;

constexpr isize bitsPerByte = 10;

using namespace utl;

// GCR encoding table. Maps 4 data bits to 5 GCR bits.
static constexpr u8 gcr[16] = {

    0x0a, 0x0b, 0x12, 0x13, /*  0 -  3 */
    0x0e, 0x0f, 0x16, 0x17, /*  4 -  7 */
    0x09, 0x19, 0x1a, 0x1b, /*  8 - 11 */
    0x0d, 0x1d, 0x1e, 0x15  /* 12 - 15 */
};

// Inverse GCR encoding table. Maps 5 GCR bits to 4 data bits.
static constexpr u8 invgcr[32] = {

    255, 255, 255, 255, /* 0x00 - 0x03 */
    255, 255, 255, 255, /* 0x04 - 0x07 */
    255,   8,   0,   1, /* 0x08 - 0x0B */
    255,  12,   4,   5, /* 0x0C - 0x0F */
    255, 255,   2,   3, /* 0x10 - 0x13 */
    255,  15,   6,   7, /* 0x14 - 0x17 */
    255,   9,  10,  11, /* 0x18 - 0x1B */
    255,  13,  14, 255  /* 0x1C - 0x1F */
};

// Converts a data nibble to a 5 bit GCR codeword or vice versa
static inline u8 bin2gcr(u8 value) { assert(value < 16); return gcr[value]; }
static inline u8 gcr2bin(u8 value) { assert(value < 32); return invgcr[value]; }

// Returns true if the provided 5 bit codeword is a valid GCR codeword
static inline bool isGcr(u8 value) { assert(value < 32); return invgcr[value] != 0xFF; }

// Encodes a byte as a GCR bit stream
void encodeGcr(MutableBitView &view, isize bitPos, u8 value);

// Decodes 5 GCR bits back into a data nibble
u8 decodeGcr4(BitView &view, isize offset);

// Decodes 10 GCR bits back into a data byte
u8 decodeGcr(BitView &view, isize offset);

}
