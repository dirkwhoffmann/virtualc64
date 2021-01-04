// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "VICIIPublicTypes.h"
#include "Reflection.h"

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
// Reflection APIs
//

struct VICRevisionEnum : Reflection<VICRevisionEnum, VICRevision> {
    
    static bool isValid(long value)
    {
        return
        (value == VICREV_PAL_6569_R1) ||
        (value == VICREV_PAL_6569_R3) ||
        (value == VICREV_PAL_8565) ||
        (value == VICREV_NTSC_6567) ||
        (value == VICREV_NTSC_6567_R56A) ||
        (value == VICREV_NTSC_8562);
    }

    static const char *prefix() { return "VICREV"; }
    static const char *key(VICRevision value)
    {
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
    
    static bool verify(long nr) { return Reflection::verify(nr,VICREV_NTSC_8562); }
};

struct GlueLogicEnum : Reflection<GlueLogicEnum, GlueLogic> {
    
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

struct PaletteEnum : Reflection<PaletteEnum, Palette> {
    
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

struct ScreenGeometryEnum : Reflection<ScreenGeometryEnum, ScreenGeometry> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < SCREEN_GEOMETRY_COUNT;
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

struct DisplayModeEnum : Reflection<DisplayModeEnum, DisplayMode> {
    
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

    static bool verify(long nr) { return Reflection::verify(nr, 0x70); }
};

struct MemAccessEnum : Reflection<MemAccessEnum, MemAccess> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < MEMACCESS_COUNT;
    }
    
    static const char *prefix() { return "MEMACCESS"; }
    static const char *key(MemAccess value)
    {
        switch (value) {
                
            case MEMACCESS_R:      return "R";
            case MEMACCESS_I:      return "I";
            case MEMACCESS_C:      return "C";
            case MEMACCESS_G:      return "G";
            case MEMACCESS_P:      return "P";
            case MEMACCESS_S:      return "S";
            case MEMACCESS_COUNT:  return "???";
        }
        return "???";
    }
};

struct DmaDisplayModeEnum : Reflection<DmaDisplayModeEnum, DmaDisplayMode> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < DMA_DISPLAY_MODE_COUNT;
    }
    
    static const char *prefix() { return "DMA_DISPLAY_MODE"; }
    static const char *key(DmaDisplayMode value)
    {
        switch (value) {
                
            case DMA_DISPLAY_MODE_FG_LAYER:         return "FG_LAYER";
            case DMA_DISPLAY_MODE_BG_LAYER:         return "BG_LAYER";
            case DMA_DISPLAY_MODE_ODD_EVEN_LAYERS:  return "ODD_EVEN_LAYERS";
            case DMA_DISPLAY_MODE_COUNT:            return "???";
        }
        return "???";
    }
};

//
// Private types
//

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
    
    template <class T>
    void applyToItems(T& worker)
    {        
        worker
        
        & sprX
        & sprY
        & ctrl1
        & sprEnable
        & ctrl2
        & sprExpandY
        & sprPriority
        & sprMC
        & sprExpandX
        & colors;
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
    
    template <class T>
    void applyToItems(T& worker)
    {
        worker
        
        & data
        & chunk1
        & chunk2
        & chunk3
        & mcFlop
        & expFlop
        & colBits;
    }
};
