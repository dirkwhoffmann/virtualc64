// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
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
