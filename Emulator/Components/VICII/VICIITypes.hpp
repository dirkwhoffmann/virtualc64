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
/// @file

#pragma once

#include "Aliases.h"
#include "Reflection.h"
#include "Serializable.h"
#include "BusTypes.h"
#include "DmaDebuggerTypes.hpp"

#include "VICIITypes.h"

struct VICIIRevisionEnum : util::Reflection<VICIIRevisionEnum, VICIIRevision> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = VICII_NTSC_8562;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "VICII"; }
    static const char *key(VICIIRevision value)
    {
        switch (value) {

            case VICII_PAL_6569_R1:    return "PAL_6569_R1";
            case VICII_PAL_6569_R3:    return "PAL_6569_R3";
            case VICII_PAL_8565:       return "PAL_8565";
            case VICII_NTSC_6567:      return "NTSC_6567";
            case VICII_NTSC_6567_R56A: return "NTSC_6567_R56A";
            case VICII_NTSC_8562:      return "NTSC_8562";
        }
        return "???";
    }
};

struct GlueLogicEnum : util::Reflection<GlueLogicEnum, GlueLogic> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = GLUE_LOGIC_IC;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "GLUE_LOGIC"; }
    static const char *key(GlueLogic value)
    {
        switch (value) {

            case GLUE_LOGIC_DISCRETE:  return "DISCRETE";
            case GLUE_LOGIC_IC:        return "IC";
        }
        return "???";
    }
};

struct PaletteEnum : util::Reflection<PaletteEnum, Palette> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = PALETTE_SEPIA;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "PALETTE"; }
    static const char *key(Palette value)
    {
        switch (value) {

            case PALETTE_COLOR:        return "COLOR";
            case PALETTE_BLACK_WHITE:  return "BLACK_WHITE";
            case PALETTE_PAPER_WHITE:  return "PAPER_WHITE";
            case PALETTE_GREEN:        return "GREEN";
            case PALETTE_AMBER:        return "AMBER";
            case PALETTE_SEPIA:        return "SEPIA";
        }
        return "???";
    }
};

struct ScreenGeometryEnum : util::Reflection<ScreenGeometryEnum, ScreenGeometry> {

    static constexpr long minVal = 1;
    static constexpr long maxVal = SCREEN_GEOMETRY_24_38;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "SCREEN_GEOMETRY"; }
    static const char *key(ScreenGeometry value)
    {
        switch (value) {

            case SCREEN_GEOMETRY_25_40:  return "25_40";
            case SCREEN_GEOMETRY_25_38:  return "25_38";
            case SCREEN_GEOMETRY_24_40:  return "24_40";
            case SCREEN_GEOMETRY_24_38:  return "24_38";
        }
        return "???";
    }
};

struct DisplayModeEnum : util::Reflection<DisplayModeEnum, DisplayMode> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DISPLAY_MODE_INV_MULTICOL_BITMAP;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "DISPLAY_MODE"; }
    static const char *key(DisplayMode value)
    {
        switch (value) {

            case DISPLAY_MODE_STANDARD_TEXT:        return "STANDARD_TEXT";
            case DISPLAY_MODE_MULTICOLOR_TEXT:      return "MULTICOLOR_TEXT";
            case DISPLAY_MODE_STANDARD_BITMAP:      return "STANDARD_BITMAP";
            case DISPLAY_MODE_MULTICOLOR_BITMAP:    return "MULTICOLOR_BITMAP";
            case DISPLAY_MODE_EXTENDED_BG_COLOR:    return "EXTENDED_BG_COLOR";
            case DISPLAY_MODE_INVALID_TEXT:         return "INVALID_TEXT";
            case DISPLAY_MODE_INV_STANDARD_BITMAP:  return "INV_STANDARD_BITMAP";
            case DISPLAY_MODE_INV_MULTICOL_BITMAP:  return "INV_MULTICOL_BITMAP";
        }
        return "???";
    }
};

struct ColorSourceEnum : util::Reflection<ColorSourceEnum, ColorSource> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = COLSRC_ZERO;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "COLSRC"; }
    static const char *key(Palette value)
    {
        switch (value) {

            case COLSRC_D021:     return "D021";
            case COLSRC_D022:     return "D022";
            case COLSRC_D023:     return "D023";
            case COLSRC_CHAR_LO:  return "CHAR_LO";
            case COLSRC_CHAR_HI:  return "CHAR_HI";
            case COLSRC_COLRAM3:  return "COLRAM3";
            case COLSRC_COLRAM4:  return "COLRAM4";
            case COLSRC_INDEXED:  return "INDEXED";
            case COLSRC_ZERO:     return "ZERO";
        }
        return "???";
    }
};

namespace vc64 {

struct VICIIRegisters : Serializable
{
    // Registers
    u16 sprX[8];     // D000, D002, ..., D00E, upper bits from D010
    u8  sprY[8];     // D001, D003, ..., D00F
    u8  ctrl1;       // D011
    u8  sprEnable;   // D015
    u8  ctrl2;       // D016
    u8  sprExpandY;  // D017
    u8  sprPriority; // D01B
    u8  sprMC;       // D01C
    u8  sprExpandX;  // D01D
    u8  colors[15];  // D020 - D02E

    // Derived values
    u8 xscroll;
    DisplayMode mode;


    template <class W>
    void serialize(W& worker)
    {
        worker

        << sprX
        << sprY
        << ctrl1
        << sprEnable
        << ctrl2
        << sprExpandY
        << sprPriority
        << sprMC
        << sprExpandX
        << colors
        << xscroll
        << mode;

    } SERIALIZERS(serialize);
};

struct SpriteSR : Serializable
{
    // Shift register data (24 bit)
    u32 data;

    // The shift register data is read in three chunks
    u8 chunk1, chunk2, chunk3;

    /* Multi-color synchronization flipflop
     * Whenever the shift register is loaded, the synchronization flipflop
     * is also set. It is toggled with each pixel and used to synchronize
     * the synthesis of multi-color pixels.
     */
    bool mcFlop;

    // X expansion synchronization flipflop
    bool expFlop;

    /* Color bits of the currently processed pixel
     * In single-color mode, these bits are updated every cycle. In
     * multi-color mode, these bits are updated every second cycle
     * (synchronized with mcFlop).
     */
    u8 colBits;

    template <class W>
    void serialize(W& worker)
    {
        worker

        << data
        << chunk1
        << chunk2
        << chunk3
        << mcFlop
        << expFlop
        << colBits;

    } SERIALIZERS(serialize);
};

}
