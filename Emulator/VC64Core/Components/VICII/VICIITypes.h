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

#include "Reflection.h"
#include "BusTypes.h"
#include "DmaDebuggerTypes.h"

namespace vc64 {

//
// Constants
//

// Sprite bit masks
#define SPR0 0x01
#define SPR1 0x02
#define SPR2 0x04
#define SPR3 0x08
#define SPR4 0x10
#define SPR5 0x20
#define SPR6 0x40
#define SPR7 0x80

// Cycle flags
static const u16 PAL_CYCLE      = 0b0001;
static const u16 DEBUG_CYCLE    = 0b0010;
static const u16 HEADLESS_CYCLE = 0b0100;

/* Depths of different drawing layers
 *
 * Format: <src><src><src><col><spr><spr><spr><spr>
 *
 *         <src> : Indicates where the displayed pixel comes from
 *         <col> : Indicator bit for sb-collision detection
 *         <spr> : Sprite number
 *
 * Examples:
 *
 *       001 0 0000 : Border pixel
 *       011 1 0000 : Foreground pixel
 *       010 0 0000 : Sprite 0 (without sb-collision)
 *       010 1 0101 : Sprite 6 (with sb-collision)
 */
#define DEPTH_BORDER        0b00100000 // In front of everything
#define DEPTH_SPRITE_FG     0b01000000 // Behind border
#define DEPTH_FG            0b01110000 // Behind sprite 1 layer
#define DEPTH_SPRITE_BG     0b10000000 // Behind foreground
#define DEPTH_BG            0b10100000 // Behind sprite 2 layer

// Event flags
#define VICUpdateIrqLine    (1ULL << 0) // Sets or releases the IRQ line
#define VICLpTransition     (1ULL << 1) // Triggers a lightpen event
#define VICUpdateFlipflops  (1ULL << 2) // Updates the flipflop value pipeline
#define VICUpdateRegisters  (1ULL << 3) // Updates the register value pipeline
#define VICUpdateBankAddr   (1ULL << 4) // Updates the bank address
#define VICSetDisplayState  (1ULL << 5) // Flagged when control reg 1 changes
#define VICClrSprSprCollReg (1ULL << 6) // Resets the sprite-sprite coll reg
#define VICClrSprBgCollReg  (1ULL << 7) // Resets the sprite-background coll reg

#define VICClearanceMask \
~((1ULL << 8) | \
VICUpdateIrqLine | \
VICLpTransition | \
VICUpdateFlipflops | \
VICUpdateRegisters | \
VICUpdateBankAddr | \
VICSetDisplayState | \
VICClrSprSprCollReg | \
VICClrSprBgCollReg);


//
// Enumerations
//

/// VICII chip revision
enum class VICIIRev : long
{
    PAL_6569_R1,                  ///< Early C64 PAL models
    PAL_6569_R3,                  ///< Later C64 PAL models
    PAL_8565,                     ///< C64 II PAL models
    NTSC_6567_R56A,               ///< Early C64 NTSC models
    NTSC_6567,                    ///< Later C64 NTSC models
    NTSC_8562                     ///< C64 II NTSC models
};

struct VICIIRevEnum : util::Reflection<VICIIRevEnum, VICIIRev> {
    
	static constexpr long minVal = 0;
    static constexpr long maxVal = long(VICIIRev::NTSC_8562);

    static const char *_key(VICIIRev value)
    {
        switch (value) {
                
            case VICIIRev::PAL_6569_R1:    return "PAL_6569_R1";
            case VICIIRev::PAL_6569_R3:    return "PAL_6569_R3";
            case VICIIRev::PAL_8565:       return "PAL_8565";
            case VICIIRev::NTSC_6567:      return "NTSC_6567";
            case VICIIRev::NTSC_6567_R56A: return "NTSC_6567_R56A";
            case VICIIRev::NTSC_8562:      return "NTSC_8562";
        }
        return "???";
    }
    
    static const char *help(VICIIRev value)
    {
        return "";
    }    
};

/// Glue logic implementation
enum class GlueLogic : long
{
    DISCRETE,                ///< Discrete logic
    IC                       ///< Integrated Circuit (PLA)
};

struct GlueLogicEnum : util::Reflection<GlueLogicEnum, GlueLogic> {
    
	static constexpr long minVal = 0;
    static constexpr long maxVal = long(GlueLogic::IC);

    static const char *_key(GlueLogic value)
    {
        switch (value) {
                
            case GlueLogic::DISCRETE:  return "DISCRETE";
            case GlueLogic::IC:        return "IC";
        }
        return "???";
    }
    
    static const char *help(GlueLogic value)
    {
        return "";
    }
};

/// C64 canvas size
enum class ScreenGeometry : long
{
    GEOMETRY_25_40,              ///< 25 rows x 40 columns
    GEOMETRY_25_38,              ///< 25 rows x 38 columns
    GEOMETRY_24_40,              ///< 24 rows x 40 columns
    GEOMETRY_24_38               ///< 24 rows x 38 columns
};

struct ScreenGeometryEnum : util::Reflection<ScreenGeometryEnum, ScreenGeometry> {
    
	static constexpr long minVal = 0;
    static constexpr long maxVal = long(ScreenGeometry::GEOMETRY_24_38);

    static const char *_key(ScreenGeometry value)
    {
        switch (value) {
                
            case ScreenGeometry::GEOMETRY_25_40:  return "25_40";
            case ScreenGeometry::GEOMETRY_25_38:  return "25_38";
            case ScreenGeometry::GEOMETRY_24_40:  return "24_40";
            case ScreenGeometry::GEOMETRY_24_38:  return "24_38";
        }
        return "???";
    }
    
    static const char *help(ScreenGeometry value)
    {
        return "";
    }
};

/// C64 display mode
enum class DisplayMode : long
{
    STANDARD_TEXT,         ///< Standard Character Mode
    MULTICOLOR_TEXT,       ///< Multicolor Character Mode
    STANDARD_BITMAP,       ///< Standard Bitmap Mode
    MULTICOLOR_BITMAP,     ///< Multicolor Bitmap Mode
    EXTENDED_BG_COLOR,     ///< Extended Background Color Mode
    INVALID_TEXT,          ///< Extended Background Color Multicolor Character Mode
    INV_STANDARD_BITMAP,   ///< Extended Background Color Standard Bitmap Mode
    INV_MULTICOL_BITMAP    ///< Extended Background Color Multicolor Bitmap Mode
};

struct DisplayModeEnum : util::Reflection<DisplayModeEnum, DisplayMode> {
    
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(DisplayMode::INV_MULTICOL_BITMAP);

    static const char *_key(DisplayMode value)
    {
        switch (value) {
                
            case DisplayMode::STANDARD_TEXT:        return "STANDARD_TEXT";
            case DisplayMode::MULTICOLOR_TEXT:      return "MULTICOLOR_TEXT";
            case DisplayMode::STANDARD_BITMAP:      return "STANDARD_BITMAP";
            case DisplayMode::MULTICOLOR_BITMAP:    return "MULTICOLOR_BITMAP";
            case DisplayMode::EXTENDED_BG_COLOR:    return "EXTENDED_BG_COLOR";
            case DisplayMode::INVALID_TEXT:         return "INVALID_TEXT";
            case DisplayMode::INV_STANDARD_BITMAP:  return "INV_STANDARD_BITMAP";
            case DisplayMode::INV_MULTICOL_BITMAP:  return "INV_MULTICOL_BITMAP";
        }
        return "???";
    }
    
    static const char *help(DisplayMode value)
    {
        return "";
    }
};

enum class ColorSource : long
{
    D021,     // Color comes from background color register
    D022,     // Color comes from first extended color register
    D023,     // Color comes from second extended color register
    CHAR_LO,  // Color comes from the low byte of the fetched character
    CHAR_HI,  // Color comes from the high byte of the fetched character
    COLRAM3,  // Color comes from the color RAM (lower three 3 bits)
    COLRAM4,  // Color comes from the color RAM (all 4 bits)
    INDEXED,  // Color comes from a color register
    ZERO      // Invalid display modes
};

struct ColorSourceEnum : util::Reflection<ColorSourceEnum, ColorSource> {
    
	static constexpr long minVal = 0;
    static constexpr long maxVal = long(ColorSource::ZERO);

    static const char *_key(ColorSource value)
    {
        switch (value) {
                
            case ColorSource::D021:     return "D021";
            case ColorSource::D022:     return "D022";
            case ColorSource::D023:     return "D023";
            case ColorSource::CHAR_LO:  return "CHAR_LO";
            case ColorSource::CHAR_HI:  return "CHAR_HI";
            case ColorSource::COLRAM3:  return "COLRAM3";
            case ColorSource::COLRAM4:  return "COLRAM4";
            case ColorSource::INDEXED:  return "INDEXED";
            case ColorSource::ZERO:     return "ZERO";
        }
        return "???";
    }
    
    static const char *help(ColorSource value)
    {
        return "";
    }
};

enum class VICIIColor : long
{
    BLACK       = 0x0,
    WHITE       = 0x1,
    RED         = 0x2,
    CYAN        = 0x3,
    PURPLE      = 0x4,
    GREEN       = 0x5,
    BLUE        = 0x6,
    YELLOW      = 0x7,
    ORANGE      = 0x8,
    BROWN       = 0x9,
    LIGHT_RED   = 0xA,
    DARK_GREY   = 0xB,
    GREY        = 0xC,
    LIGHT_GREEN = 0xD,
    LIGHT_BLUE  = 0xE,
    LIGHT_GREY  = 0xF
};

namespace VICIIColorReg
{
const isize BORDER     = 0x0;
const isize BG_0       = 0x1;
const isize BG_1       = 0x2;
const isize BG_2       = 0x3;
const isize BG_3       = 0x4;
const isize SPR_EX1    = 0x5;
const isize SPR_EX2    = 0x6;
const isize SPR_0      = 0x7;
const isize SPR_1      = 0x8;
const isize SPR_2      = 0x9;
const isize SPR_3      = 0xA;
const isize SPR_4      = 0xB;
const isize SPR_5      = 0xC;
const isize SPR_6      = 0xD;
const isize SPR_7      = 0xE;
}


//
// Structures
//

typedef struct
{
    // Video standard
    bool pal;
    isize frequency;
    double fps;

    // Picture layout
    isize linesPerFrame;
    isize cyclesPerLine;
    isize cyclesPerFrame;
    isize visibleLines;

    // Chip properties
    bool grayCodeBug;
    bool delayedLpIrqs;
}
VICIITraits;

typedef struct
{
    // Silicon
    VICIIRev revision;
    VICIIRev awaiting;
    bool powerSave;
    bool grayDotBug;
    GlueLogic glueLogic;

    // Sprites
    bool hideSprites;
    
    // Cheating
    bool checkSSCollisions;
    bool checkSBCollisions;
}
VICIIConfig;

typedef struct
{
    // Graphics synthesizer (speed optimizations)
    isize canvasFastPath;
    isize canvasSlowPath;
    isize spriteFastPath;
    isize spriteSlowPath;
    isize quickExitHit;
    isize quickExitMiss;
}
VICIIStats;

typedef struct
{
    bool vertical;
    bool main;
}
FrameFlipflops;

typedef struct
{
    // Counters
    u16 scanline;
    u8 rasterCycle;
    u32 yCounter;
    u16 xCounter;
    u16 vc;
    u16 vcBase;
    u8 rc;
    u8 vmli;

    // Display
    u8 ctrl1;
    u8 ctrl2;
    u8 dy;
    u8 dx;
    bool denBit;
    bool badLine;
    bool displayState;
    bool vblank;
    ScreenGeometry screenGeometry;
    FrameFlipflops frameFF;
    DisplayMode displayMode;
    u8 borderColor;
    u8 bgColor0;
    u8 bgColor1;
    u8 bgColor2;
    u8 bgColor3;
    
    // Memory
    u8 memSelect;
    bool ultimax;
    u16 memoryBankAddr;
    u16 screenMemoryAddr;
    u16 charMemoryAddr;

    // Interrupts
    u16 irqLine;
    u8 irr;
    u8 imr;

    // Lightpen
    u8 latchedLPX;
    u8 latchedLPY;
    bool lpLine;
    bool lpIrqHasOccurred;
}
VICIIInfo;

typedef struct
{
    u16 x;
    u8 y;
    bool enabled;
    bool expandX;
    bool expandY;
    bool priority;
    bool multicolor;
    bool ssCollision;
    bool sbCollision;
    u8 color;
    u8 extraColor1;
    u8 extraColor2;
}
SpriteInfo;

}
