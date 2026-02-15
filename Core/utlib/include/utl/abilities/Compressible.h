// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/abilities/Reflectable.h"

namespace utl {

enum class Compressor : long
{
    NONE,
    GZIP,
    LZ4,
    RLE2,
    RLE3
};

struct CompressorEnum : Reflectable<CompressorEnum, Compressor>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(Compressor::RLE3);

    static const char *_key(Compressor value)
    {
        switch (value) {

            case Compressor::NONE:  return "NONE";
            case Compressor::GZIP:  return "GZIP";
            case Compressor::RLE2:  return "RLE2";
            case Compressor::RLE3:  return "RLE3";
            case Compressor::LZ4:   return "LZ4";
        }
        return "???";
    }
    static const char *help(Compressor value)
    {
        switch (value) {

            case Compressor::NONE:  return "No compression";
            case Compressor::GZIP:  return "Gzip compression";
            case Compressor::RLE2:  return "Run-length encoding (2)";
            case Compressor::RLE3:  return "Run-length encoding (3)";
            case Compressor::LZ4:   return "LZ4 compression";
        }
        return "???";
    }
};

class Compressible {

public:
    
    static void gzip(u8 *buffer, isize len, std::vector<u8> &result);
    static void gunzip(u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate = 0);

    static void lz4(u8 *buffer, isize len, std::vector<u8> &result);
    static void unlz4(u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate = 0);

    static void rle2(u8 *buffer, isize len, std::vector<u8> &result);
    static void unrle2(u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate = 0);

    static void rle3(u8 *buffer, isize len, std::vector<u8> &result);
    static void unrle3(u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate = 0);

    static void rle(isize n, u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate = 0);
    static void unrle(isize n, u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate = 0);
};

}
