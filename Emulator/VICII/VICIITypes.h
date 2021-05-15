// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include "Reflection.h"
#include "BusTypes.h"

#include "DmaDebuggerTypes.h"


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

// Depth of different drawing layers
#define BORDER_LAYER_DEPTH         0x10 // In front of everything
#define SPRITE_LAYER_FG_DEPTH      0x20 // Behind border
#define FOREGROUND_LAYER_DEPTH     0x30 // Behind sprite 1 layer
#define SPRITE_LAYER_BG_DEPTH      0x40 // Behind foreground
#define BACKGROUD_LAYER_DEPTH      0x50 // Behind sprite 2 layer
#define BEHIND_BACKGROUND_DEPTH    0x60 // Behind background

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

enum_long(VICII_REV)
{
    VICII_PAL_6569_R1 = 1,
    VICII_PAL_6569_R3 = 2,
    VICII_PAL_8565 = 4,
    VICII_NTSC_6567_R56A = 8,
    VICII_NTSC_6567 = 16,
    VICII_NTSC_8562 = 32
};
typedef VICII_REV VICIIRevision;

#ifdef __cplusplus
struct VICIIRevisionEnum : util::Reflection<VICIIRevisionEnum, VICIIRevision> {
    
    static bool isValid(long value)
    {
        return
        (value == VICII_PAL_6569_R1) ||
        (value == VICII_PAL_6569_R3) ||
        (value == VICII_PAL_8565) ||
        (value == VICII_NTSC_6567) ||
        (value == VICII_NTSC_6567_R56A) ||
        (value == VICII_NTSC_8562);
    }

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
    
    static std::map <string, long> pairs() { return Reflection::pairs(VICII_NTSC_8562); }
};
#endif

enum_long(GLUE_LOGIC)
{
    GLUE_LOGIC_DISCRETE,
    GLUE_LOGIC_IC,
    GLUE_LOGIC_COUNT
};
typedef GLUE_LOGIC GlueLogic;

#ifdef __cplusplus
struct GlueLogicEnum : util::Reflection<GlueLogicEnum, GlueLogic> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < GLUE_LOGIC_COUNT;
    }

    static const char *prefix() { return "GLUE_LOGIC"; }
    static const char *key(GlueLogic value)
    {
        switch (value) {
                
            case GLUE_LOGIC_DISCRETE:  return "DISCRETE";
            case GLUE_LOGIC_IC:        return "IC";
            case GLUE_LOGIC_COUNT:     return "???";
        }
        return "???";
    }
};
#endif

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

#ifdef __cplusplus
struct PaletteEnum : util::Reflection<PaletteEnum, Palette> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < PALETTE_COUNT;
    }

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
            case PALETTE_COUNT:        return "???";
        }
        return "???";
    }
};
#endif

enum_long(SCREEN_GEOMETRY)
{
    SCREEN_GEOMETRY_25_40 = 1,
    SCREEN_GEOMETRY_25_38,
    SCREEN_GEOMETRY_24_40,
    SCREEN_GEOMETRY_24_38,
    SCREEN_GEOMETRY_COUNT
};
typedef SCREEN_GEOMETRY ScreenGeometry;

#ifdef __cplusplus
struct ScreenGeometryEnum : util::Reflection<ScreenGeometryEnum, ScreenGeometry> {
    
    static bool isValid(long value)
    {
        return value >= 1 && value < SCREEN_GEOMETRY_COUNT;
    }

    static const char *prefix() { return "SCREEN_GEOMETRY"; }
    static const char *key(ScreenGeometry value)
    {
        switch (value) {
                
            case SCREEN_GEOMETRY_25_40:  return "25_40";
            case SCREEN_GEOMETRY_25_38:  return "25_38";
            case SCREEN_GEOMETRY_24_40:  return "24_40";
            case SCREEN_GEOMETRY_24_38:  return "24_38";
            case SCREEN_GEOMETRY_COUNT:  return "???";
        }
        return "???";
    }
};
#endif

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

#ifdef __cplusplus
struct DisplayModeEnum : util::Reflection<DisplayModeEnum, DisplayMode> {
    
    static bool isValid(long value)
    {
        return
        (value == DISPLAY_MODE_STANDARD_TEXT) ||
        (value == DISPLAY_MODE_MULTICOLOR_TEXT) ||
        (value == DISPLAY_MODE_STANDARD_BITMAP) ||
        (value == DISPLAY_MODE_MULTICOLOR_BITMAP) ||
        (value == DISPLAY_MODE_EXTENDED_BG_COLOR) ||
        (value == DISPLAY_MODE_INVALID_TEXT) ||
        (value == DISPLAY_MODE_INV_STANDARD_BITMAP) ||
        (value == DISPLAY_MODE_INV_MULTICOL_BITMAP);
    }

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

    static std::map <string, long> pairs() { return Reflection::pairs(0x70); }
};
#endif

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

#ifdef __cplusplus
struct ColorSourceEnum : util::Reflection<ColorSourceEnum, ColorSource> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= COLSRC_ZERO;
    }

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
#endif

enum VICIIMode
{
    PAL_CYCLE         = 0x0,
    PAL_DEBUG_CYCLE   = 0x1,
    NTSC_CYCLE        = 0x2,
    NTSC_DEBUG_CYCLE  = 0x3
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
    // Silicon
    VICIIRevision revision;
    bool grayDotBug;
    GlueLogic glueLogic;
    
    // Colors
    Palette palette;
    isize brightness;
    isize contrast;
    isize saturation;

    // Debugger
    bool hideSprites;
    u16 cutLayers;
    u8 cutOpacity;
    
    // Cheating
    bool checkSSCollisions;
    bool checkSBCollisions;
}
VICIIConfig;

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
    
#ifdef __cplusplus
struct VICIIRegisters
{
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
    
    template <class W>
    void operator<<(W& worker)
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
        << colors;
    }
};

struct SpriteSR
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
    void operator<<(W& worker)
    {
        worker
        
        << data
        << chunk1
        << chunk2
        << chunk3
        << mcFlop
        << expFlop
        << colBits;
    }
};
#endif
