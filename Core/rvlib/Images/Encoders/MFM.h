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

namespace retro::vault::MFM {

using namespace utl;

void encodeMFM(u8 *dst, const u8 *src, isize count);
void decodeMFM(u8 *dst, const u8 *src, isize count);

void encodeOddEven(u8 *dst, const u8 *src, isize count);
void decodeOddEven(u8 *dst, const u8 *src, isize count);

void addClockBits(u8 *dst, isize count);
u8 addClockBits(u8 value, u8 previous);

}
