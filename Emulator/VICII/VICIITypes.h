// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _VICII_TYPES_H
#define _VICII_TYPES_H

//
// Enumerations
//

typedef enum : long
{
    PAL_6569_R1 = 1,
    PAL_6569_R3 = 2,
    PAL_8565 = 4,
    NTSC_6567_R56A = 8,
    NTSC_6567 = 16,
    NTSC_8562 = 32
}
VICRevision;

inline bool isVICRevision(long value)
{
    return
    (value == PAL_6569_R1) ||
    (value == PAL_6569_R3) ||
    (value == PAL_8565) ||
    (value == NTSC_6567) ||
    (value == NTSC_6567_R56A) ||
    (value == NTSC_8562);
}

inline const char *vicRevisionName(VICRevision revision)
{
    assert(isVICRevision(revision));
    
    switch (revision) {
        case PAL_6569_R1:    return "PAL_6569_R1";
        case PAL_6569_R3:    return "PAL_6569_R3";
        case PAL_8565:       return "PAL_8565";
        case NTSC_6567:      return "NTSC_6567";
        case NTSC_6567_R56A: return "NTSC_6567_R56A";
        case NTSC_8562:      return "NTSC_8562";
        default:             return "???";
    }
}

typedef enum : long
{
    GLUE_DISCRETE = 0,
    GLUE_CUSTOM_IC = 1
}
GlueLogic;

inline bool isGlueLogic(long value)
{
    return value == GLUE_DISCRETE || value == GLUE_CUSTOM_IC;
}

inline const char *glueLogicName(GlueLogic type)
{
    assert(isGlueLogic(type));
    
    switch (type) {
        case GLUE_DISCRETE:  return "Discrete";
        case GLUE_CUSTOM_IC: return "IC";
        default:             return "???";
    }
}

typedef enum : long
{
    COLOR_PALETTE = 0,
    BLACK_WHITE_PALETTE,
    PAPER_WHITE_PALETTE,
    GREEN_PALETTE,
    AMBER_PALETTE,
    SEPIA_PALETTE
}
Palette;

inline bool isPalette(long value) {
    return value >= COLOR_PALETTE && value <= SEPIA_PALETTE;
}

typedef enum
{
    COL_40_ROW_25 = 0x01,
    COL_38_ROW_25 = 0x02,
    COL_40_ROW_24 = 0x03,
    COL_38_ROW_24 = 0x04
}
ScreenGeometry;

typedef enum
{
    STANDARD_TEXT             = 0x00,
    MULTICOLOR_TEXT           = 0x10,
    STANDARD_BITMAP           = 0x20,
    MULTICOLOR_BITMAP         = 0x30,
    EXTENDED_BACKGROUND_COLOR = 0x40,
    INVALID_TEXT              = 0x50,
    INVALID_STANDARD_BITMAP   = 0x60,
    INVALID_MULTICOLOR_BITMAP = 0x70
}
DisplayMode;


//
// Structures
//

typedef struct
{
    VICRevision revision;
    bool grayDotBug;
    GlueLogic glueLogic;
    Palette palette;
    
    // Debugging
    bool markIrqLines;
    bool markDmaLines;
    bool hideSprites;
    u16  cutLayers;
    u8   cutOpacity;
    
    // Cheating
    bool checkSSCollisions;
    bool checkSBCollisions;
}
VICConfig;

typedef struct
{
    bool vertical;
    bool main;
}
FrameFlipflops;

typedef struct
{
    // Counters
    u16 rasterLine;
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
    u16 irqRasterline;
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
    
#endif
