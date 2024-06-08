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
enum_long(VICII_REV)
{
    VICII_PAL_6569_R1,                  ///< Early C64 PAL models
    VICII_PAL_6569_R3,                  ///< Later C64 PAL models
    VICII_PAL_8565,                     ///< C64 II PAL models
    VICII_NTSC_6567_R56A,               ///< Early C64 NTSC models
    VICII_NTSC_6567,                    ///< Later C64 NTSC models
    VICII_NTSC_8562                     ///< C64 II NTSC models
};
typedef VICII_REV VICIIRevision;

struct VICIIRevisionEnum : util::Reflection<VICIIRevisionEnum, VICIIRevision> {
    
	static constexpr long minVal = 0;
    static constexpr long maxVal = VICII_NTSC_8562;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "VICII"; }
    static const char *key(long value)
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

/// Glue logic implementation
enum_long(GLUE_LOGIC)
{
    GLUE_LOGIC_DISCRETE,                ///< Discrete logic
    GLUE_LOGIC_IC                       ///< Integrated Circuit (PLA)
};
typedef GLUE_LOGIC GlueLogic;

struct GlueLogicEnum : util::Reflection<GlueLogicEnum, GlueLogic> {
    
	static constexpr long minVal = 0;
    static constexpr long maxVal = GLUE_LOGIC_IC;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "GLUE_LOGIC"; }
    static const char *key(long value)
    {
        switch (value) {
                
            case GLUE_LOGIC_DISCRETE:  return "DISCRETE";
            case GLUE_LOGIC_IC:        return "IC";
        }
        return "???";
    }
};

/// C64 canvas size
enum_long(SCREEN_GEOMETRY)
{
    SCREEN_GEOMETRY_25_40 = 1,          ///< 25 rows x 40 columns
    SCREEN_GEOMETRY_25_38,              ///< 25 rows x 38 columns
    SCREEN_GEOMETRY_24_40,              ///< 24 rows x 40 columns
    SCREEN_GEOMETRY_24_38               ///< 24 rows x 38 columns
};
/// Type alias
typedef SCREEN_GEOMETRY ScreenGeometry;

struct ScreenGeometryEnum : util::Reflection<ScreenGeometryEnum, ScreenGeometry> {
    
	static constexpr long minVal = 1;
    static constexpr long maxVal = SCREEN_GEOMETRY_24_38;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "SCREEN_GEOMETRY"; }
    static const char *key(long value)
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

/// C64 display mode
enum_long(DISPLAY_MODE)
{
    DISPLAY_MODE_STANDARD_TEXT,         ///< Standard Character Mode
    DISPLAY_MODE_MULTICOLOR_TEXT,       ///< Multicolor Character Mode
    DISPLAY_MODE_STANDARD_BITMAP,       ///< Standard Bitmap Mode
    DISPLAY_MODE_MULTICOLOR_BITMAP,     ///< Multicolor Bitmap Mode
    DISPLAY_MODE_EXTENDED_BG_COLOR,     ///< Extended Background Color Mode
    DISPLAY_MODE_INVALID_TEXT,          ///< Extended Background Color Multicolor Character Mode
    DISPLAY_MODE_INV_STANDARD_BITMAP,   ///< Extended Background Color Standard Bitmap Mode
    DISPLAY_MODE_INV_MULTICOL_BITMAP    ///< Extended Background Color Multicolor Bitmap Mode
};
typedef DISPLAY_MODE DisplayMode;

struct DisplayModeEnum : util::Reflection<DisplayModeEnum, DisplayMode> {
    
    static constexpr long minVal = 0;
    static constexpr long maxVal = DISPLAY_MODE_INV_MULTICOL_BITMAP;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "DISPLAY_MODE"; }
    static const char *key(long value)
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

enum_long(COLSRC)
{
    COLSRC_D021,     // Color comes from background color register
    COLSRC_D022,     // Color comes from first extended color register
    COLSRC_D023,     // Color comes from second extended color register
    COLSRC_CHAR_LO,  // Color comes from the low byte of the fetched character
    COLSRC_CHAR_HI,  // Color comes from the high byte of the fetched character
    COLSRC_COLRAM3,  // Color comes from the color RAM (lower three 3 bits)
    COLSRC_COLRAM4,  // Color comes from the color RAM (all 4 bits)
    COLSRC_INDEXED,  // Color comes from a color register
    COLSRC_ZERO      // Invalid display modes
};
typedef COLSRC ColorSource;

struct ColorSourceEnum : util::Reflection<ColorSourceEnum, ColorSource> {
    
	static constexpr long minVal = 0;
    static constexpr long maxVal = COLSRC_ZERO;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "COLSRC"; }
    static const char *key(long value)
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

enum VICIIColors
{
    VICII_BLACK       = 0x0,
    VICII_WHITE       = 0x1,
    VICII_RED         = 0x2,
    VICII_CYAN        = 0x3,
    VICII_PURPLE      = 0x4,
    VICII_GREEN       = 0x5,
    VICII_BLUE        = 0x6,
    VICII_YELLOW      = 0x7,
    VICII_ORANGE      = 0x8,
    VICII_BROWN       = 0x9,
    VICII_LIGHT_RED   = 0xA,
    VICII_DARK_GREY   = 0xB,
    VICII_GREY        = 0xC,
    VICII_LIGHT_GREEN = 0xD,
    VICII_LIGHT_BLUE  = 0xE,
    VICII_LIGHT_GREY  = 0xF
};

enum VICIIColorRegs
{
    COLREG_BORDER     = 0x0,
    COLREG_BG0        = 0x1,
    COLREG_BG1        = 0x2,
    COLREG_BG2        = 0x3,
    COLREG_BG3        = 0x4,
    COLREG_SPR_EX1    = 0x5,
    COLREG_SPR_EX2    = 0x6,
    COLREG_SPR0       = 0x7,
    COLREG_SPR1       = 0x8,
    COLREG_SPR2       = 0x9,
    COLREG_SPR3       = 0xA,
    COLREG_SPR4       = 0xB,
    COLREG_SPR5       = 0xC,
    COLREG_SPR6       = 0xD,
    COLREG_SPR7       = 0xE
};


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
    VICIIRevision revision;
    VICIIRevision awaiting;
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
