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

enum_long(VICRev)
{
    VICRev_PAL_6569_R1 = 1,
    VICRev_PAL_6569_R3 = 2,
    VICRev_PAL_8565 = 4,
    VICRev_NTSC_6567_R56A = 8,
    VICRev_NTSC_6567 = 16,
    VICRev_NTSC_8562 = 32
};

inline bool isVICRev(long value)
{
    return
    (value == VICRev_PAL_6569_R1) ||
    (value == VICRev_PAL_6569_R3) ||
    (value == VICRev_PAL_8565) ||
    (value == VICRev_NTSC_6567) ||
    (value == VICRev_NTSC_6567_R56A) ||
    (value == VICRev_NTSC_8562);
}

inline const char *sVICRev(VICRev revision)
{
    assert(isVICRev(revision));
    
    switch (revision) {
            
        case VICRev_PAL_6569_R1:    return "PAL_6569_R1";
        case VICRev_PAL_6569_R3:    return "PAL_6569_R3";
        case VICRev_PAL_8565:       return "PAL_8565";
        case VICRev_NTSC_6567:      return "NTSC_6567";
        case VICRev_NTSC_6567_R56A: return "NTSC_6567_R56A";
        case VICRev_NTSC_8562:      return "NTSC_8562";
        default:                    return "???";
    }
}

enum_long(GlueLogic)
{
    GlueLogic_DISCRETE,
    GlueLogic_IC
};

inline bool isGlueLogic(long value)
{
    return (unsigned long)value <= GlueLogic_IC;
}

inline const char *GlueLogicName(GlueLogic value)
{
    assert(isGlueLogic(value));
    
    switch (value) {
            
        case GlueLogic_DISCRETE:   return "DISCRETE";
        case GlueLogic_IC:         return "IC";
        default:                   return "???";
    }
}

enum_long(Palette)
{
    Palette_COLOR,
    Palette_BLACK_WHITE,
    Palette_PAPER_WHITE,
    Palette_GREEN,
    Palette_AMBER,
    Palette_SEPIA
};

inline bool isPalette(long value) {
    return (unsigned long)value <= Palette_SEPIA;
}

inline const char *PaletteName(Palette value)
{
    assert(isGlueLogic(value));
    
    switch (value) {
            
        case Palette_COLOR:        return "COLOR";
        case Palette_BLACK_WHITE:  return "BLACK_WHITE";
        case Palette_PAPER_WHITE:  return "PAPER_WHITE";
        case Palette_GREEN:        return "GREEN";
        case Palette_AMBER:        return "AMBER";
        case Palette_SEPIA:        return "SEPIA";
        default:                   return "???";
    }
}

enum_long(ScreenGeometry)
{
    ScreenGeometry_25_40 = 1,
    ScreenGeometry_25_38,
    ScreenGeometry_24_40,
    ScreenGeometry_24_38
};

inline bool isScreenGeometry(long value) {
    return (unsigned long)value <= ScreenGeometry_24_38;
}

inline const char *ScreenGeometryName(ScreenGeometry value)
{
    assert(isScreenGeometry(value));
    
    switch (value) {
            
        case ScreenGeometry_25_40:  return "25_40";
        case ScreenGeometry_25_38:  return "25_38";
        case ScreenGeometry_24_40:  return "24_40";
        case ScreenGeometry_24_38:  return "24_38";
        default:                    return "???";
    }
}

enum_long(DisplayMode)
{
    DisplayMode_STANDARD_TEXT       = 0x00,
    DisplayMode_MULTICOLOR_TEXT     = 0x10,
    DisplayMode_STANDARD_BITMAP     = 0x20,
    DisplayMode_MULTICOLOR_BITMAP   = 0x30,
    DisplayMode_EXTENDED_BG_COLOR   = 0x40,
    DisplayMode_INVALID_TEXT        = 0x50,
    DisplayMode_INV_STANDARD_BITMAP = 0x60,
    DisplayMode_INV_MULTICOL_BITMAP = 0x70
};

inline bool isDisplayMode(long value) {
    return (unsigned long)value <= DisplayMode_INV_MULTICOL_BITMAP;
}

inline const char *DisplayModeName(DisplayMode value)
{
    assert(isDisplayMode(value));
    
    switch (value) {
            
        case DisplayMode_STANDARD_TEXT:        return "STANDARD_TEXT";
        case DisplayMode_MULTICOLOR_TEXT:      return "MULTICOLOR_TEXT";
        case DisplayMode_STANDARD_BITMAP:      return "STANDARD_BITMAP";
        case DisplayMode_MULTICOLOR_BITMAP:    return "MULTICOLOR_BITMAP";
        case DisplayMode_EXTENDED_BG_COLOR:    return "EXTENDED_BG_COLOR";
        case DisplayMode_INVALID_TEXT:         return "INVALID_TEXT";
        case DisplayMode_INV_STANDARD_BITMAP:  return "INV_STANDARD_BITMAP";
        case DisplayMode_INV_MULTICOL_BITMAP:  return "INV_MULTICOL_BITMAP";
        default:                               return "???";
    }
}

enum_long(MemAccess)
{
    MemAccess_R,                 // Memory Refresh
    MemAccess_I,                 // Idle read
    MemAccess_C,                 // Character access
    MemAccess_G,                 // Graphics access
    MemAccess_P,                 // Sprite pointer access
    MemAccess_S                  // Sprite data access
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
        default:           return "???";
    }
}

enum_long(DmaDisplayMode)
{
    MODULATE_FG_LAYER,
    MODULATE_BG_LAYER,
    MODULATE_ODD_EVEN_LAYERS
};


//
// Structures
//

typedef struct
{
    VICRev revision;
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
