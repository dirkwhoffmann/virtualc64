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

enum_long(VICREV)
{
    VICREV_PAL_6569_R1 = 1,
    VICREV_PAL_6569_R3 = 2,
    VICREV_PAL_8565 = 4,
    VICREV_NTSC_6567_R56A = 8,
    VICREV_NTSC_6567 = 16,
    VICREV_NTSC_8562 = 32
};
typedef VICREV VICRevision;

inline bool isVICRevision(long value)
{
    return
    (value == VICREV_PAL_6569_R1) ||
    (value == VICREV_PAL_6569_R3) ||
    (value == VICREV_PAL_8565) ||
    (value == VICREV_NTSC_6567) ||
    (value == VICREV_NTSC_6567_R56A) ||
    (value == VICREV_NTSC_8562);
}

inline const char *VICRevisionName(VICRevision value)
{
    assert(isVICRevision(value));
    
    switch (value) {
            
        case VICREV_PAL_6569_R1:    return "PAL_6569_R1";
        case VICREV_PAL_6569_R3:    return "PAL_6569_R3";
        case VICREV_PAL_8565:       return "PAL_8565";
        case VICREV_NTSC_6567:      return "NTSC_6567";
        case VICREV_NTSC_6567_R56A: return "NTSC_6567_R56A";
        case VICREV_NTSC_8562:      return "NTSC_8562";
    }
    return "???";
}

enum_long(GLUE_LOGIC)
{
    GLUE_LOGIC_DISCRETE,
    GLUE_LOGIC_IC
};
typedef GLUE_LOGIC GlueLogic;

inline bool isGlueLogic(long value)
{
    return (unsigned long)value <= GLUE_LOGIC_IC;
}

inline const char *GlueLogicName(GlueLogic value)
{
    assert(isGlueLogic(value));
    
    switch (value) {
            
        case GLUE_LOGIC_DISCRETE:  return "DISCRETE";
        case GLUE_LOGIC_IC:        return "IC";
    }
    return "???";
}

enum_long(PALETTE)
{
    PALETTE_COLOR,
    PALETTE_BLACK_WHITE,
    PALETTE_PAPER_WHITE,
    PALETTE_GREEN,
    PALETTE_AMBER,
    PALETTE_SEPIA
};
typedef PALETTE Palette;

inline bool isPalette(long value) {
    return (unsigned long)value <= PALETTE_SEPIA;
}

inline const char *PaletteName(Palette value)
{
    assert(isGlueLogic(value));
    
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

enum_long(SCREEN_GEOMETRY)
{
    SCREEN_GEOMETRY_25_40 = 1,
    SCREEN_GEOMETRY_25_38,
    SCREEN_GEOMETRY_24_40,
    SCREEN_GEOMETRY_24_38
};
typedef SCREEN_GEOMETRY ScreenGeometry;

inline bool isScreenGeometry(long value) {
    return (unsigned long)value <= SCREEN_GEOMETRY_24_38;
}

inline const char *ScreenGeometryName(ScreenGeometry value)
{
    assert(isScreenGeometry(value));
    
    switch (value) {
            
        case SCREEN_GEOMETRY_25_40:  return "25_40";
        case SCREEN_GEOMETRY_25_38:  return "25_38";
        case SCREEN_GEOMETRY_24_40:  return "24_40";
        case SCREEN_GEOMETRY_24_38:  return "24_38";
    }
    return "???";
}

enum_long(DISPLAY_MODE)
{
    DISPLAY_MODE_STANDARD_TEXT       = 0x00,
    DISPLAY_MODE_MULTICOLOR_TEXT     = 0x10,
    DISPLAY_MODE_STANDARD_BITMAP     = 0x20,
    DISPLAY_MODE_MULTICOLOR_BITMAP   = 0x30,
    DISPLAY_MODE_EXTENDED_BG_COLOR   = 0x40,
    DISPLAY_MODE_INVALID_TEXT        = 0x50,
    DISPLAY_MODE_INV_STANDARD_BITMAP = 0x60,
    DISPLAY_MODE_INV_MULTICOL_BITMAP = 0x70
};
typedef DISPLAY_MODE DisplayMode;

inline bool isDisplayMode(long value) {
    return (unsigned long)value <= DISPLAY_MODE_INV_MULTICOL_BITMAP;
}

inline const char *DisplayModeName(DisplayMode value)
{
    assert(isDisplayMode(value));
    
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

enum_long(MemAccess)
{
    MemAccess_R, // Memory Refresh
    MemAccess_I, // Idle read
    MemAccess_C, // Character access
    MemAccess_G, // Graphics access
    MemAccess_P, // Sprite pointer access
    MemAccess_S  // Sprite data access
};

static inline bool isMemAccess(long value)
{
    return (unsigned long)value <= MemAccess_S;
}

inline const char *MemAccessName(MemAccess value)
{
    assert(isMemAccess(value));
    
    switch (value) {
            
        case MemAccess_R:  return "R";
        case MemAccess_I:  return "I";
        case MemAccess_C:  return "C";
        case MemAccess_G:  return "G";
        case MemAccess_P:  return "P";
        case MemAccess_S:  return "S";
    }
    return "???";
}

enum_long(DMA_DISPLAY_MODE)
{
    DMA_DISPLAY_MODE_FG_LAYER,
    DMA_DISPLAY_MODE_BG_LAYER,
    DMA_DISPLAY_MODE_ODD_EVEN_LAYERS
};
typedef DMA_DISPLAY_MODE DmaDisplayMode;

static inline bool isDmaDisplayMode(long value)
{
    return (unsigned long)value <= DMA_DISPLAY_MODE_ODD_EVEN_LAYERS;
}

inline const char *DmaDisplayModeName(DmaDisplayMode value)
{
    assert(isDmaDisplayMode(value));
    
    switch (value) {
            
        case DMA_DISPLAY_MODE_FG_LAYER:         return "FG_LAYER";
        case DMA_DISPLAY_MODE_BG_LAYER:         return "BG_LAYER";
        case DMA_DISPLAY_MODE_ODD_EVEN_LAYERS:  return "ODD_EVEN_LAYERS";
    }
    return "???";
}

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
    bool hideSprites;
    bool dmaDebug;
    bool dmaChannel[6];
    u32 dmaColor[6];
    DmaDisplayMode dmaDisplayMode;
    u8 dmaOpacity;
    u16 cutLayers;
    u8 cutOpacity;
    
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
    
    // Debugging
    
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
