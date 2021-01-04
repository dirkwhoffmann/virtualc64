// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

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

enum_long(GLUE_LOGIC)
{
    GLUE_LOGIC_DISCRETE,
    GLUE_LOGIC_IC,
    GLUE_LOGIC_COUNT
};
typedef GLUE_LOGIC GlueLogic;

enum_long(PALETTE)
{
    PALETTE_COLOR,
    PALETTE_BLACK_WHITE,
    PALETTE_PAPER_WHITE,
    PALETTE_GREEN,
    PALETTE_AMBER,
    PALETTE_SEPIA,
    PALETTE_COUNT
};
typedef PALETTE Palette;

enum_long(SCREEN_GEOMETRY)
{
    SCREEN_GEOMETRY_25_40 = 1,
    SCREEN_GEOMETRY_25_38,
    SCREEN_GEOMETRY_24_40,
    SCREEN_GEOMETRY_24_38,
    SCREEN_GEOMETRY_COUNT
};
typedef SCREEN_GEOMETRY ScreenGeometry;

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

enum_long(MEMACCESS)
{
    MEMACCESS_R,     // Memory Refresh
    MEMACCESS_I,     // Idle read
    MEMACCESS_C,     // Character access
    MEMACCESS_G,     // Graphics access
    MEMACCESS_P,     // Sprite pointer access
    MEMACCESS_S,     // Sprite data access
    MEMACCESS_COUNT
};
typedef MEMACCESS MemAccess;

enum_long(DMA_DISPLAY_MODE)
{
    DMA_DISPLAY_MODE_FG_LAYER,
    DMA_DISPLAY_MODE_BG_LAYER,
    DMA_DISPLAY_MODE_ODD_EVEN_LAYERS,
    DMA_DISPLAY_MODE_COUNT
};
typedef DMA_DISPLAY_MODE DmaDisplayMode;

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
    
