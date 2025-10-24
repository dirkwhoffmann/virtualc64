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

#include "config.h"
#include "BasicTypes.h"

namespace vc64::util {

void gzip(u8 *buffer, isize len, std::vector<u8> &result);
void gunzip(u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate = 0);

void lz4(u8 *buffer, isize len, std::vector<u8> &result);
void unlz4(u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate = 0);

void rle2(u8 *buffer, isize len, std::vector<u8> &result);
void unrle2(u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate = 0);

void rle3(u8 *buffer, isize len, std::vector<u8> &result);
void unrle3(u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate = 0);

void rle(isize n, u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate = 0);
void unrle(isize n, u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate = 0);

}
