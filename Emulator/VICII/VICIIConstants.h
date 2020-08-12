// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _VIC_CONSTANTS_H
#define _VIC_CONSTANTS_H

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
// NTSC constants
//

// NTSC clock frequency in Hz
static const u32 NTSC_CLOCK_FREQUENCY = 1022727;

// CPU cycles per second in NTSC mode
static const unsigned NTSC_CYCLES_PER_SECOND = NTSC_CLOCK_FREQUENCY;

// Pixel aspect ratio in NTSC mode
static const double NTSC_PIXEL_ASPECT_RATIO = 0.75;


// Horizontal screen parameters

// Width of left VBLANK area in NTSC mode
static const u16 NTSC_LEFT_VBLANK = 77;

// Width of left border in NTSC mode
static const u16 NTSC_LEFT_BORDER_WIDTH = 55;

// Width of canvas area in NTSC mode
static const u16 NTSC_CANVAS_WIDTH = 320;

// Width of right border in NTSC mode
static const u16 NTSC_RIGHT_BORDER_WIDTH = 53;

// Width of right VBLANK area in NTSC mode
static const u16 NTSC_RIGHT_VBLANK = 15;

// Total width of a rasterline (including VBLANK) in NTSC mode
static const u16 NTSC_WIDTH = 520; // 77 + 55 + 320 + 53 + 15

// Number of drawn pixels per rasterline in NTSC mode
static const u16 NTSC_PIXELS = 428; // 55 + 320 + 53

// Number of viewable pixels per rasterline in NTSC mode
static const u16 NTSC_VISIBLE_PIXELS = 418;


// Vertical screen parameters

// Number of VBLANK lines at top in NTSC mode
static const u16 NTSC_UPPER_VBLANK = 16;

// Heigt of upper boder in NTSC mode
static const u16 NTSC_UPPER_BORDER_HEIGHT = 10;

// Height of canvas area in NTSC mode
static const u16 NTSC_CANVAS_HEIGHT = 200;

// Lower border height in NTSC mode
static const u16 NTSC_LOWER_BORDER_HEIGHT = 25;

// Number of VBLANK lines at bottom in NTSC mode
static const u16 NTSC_LOWER_VBLANK = 12;

// Total height of a frame (including VBLANK) in NTSC mode
static const u16 NTSC_HEIGHT = 263; // 16 + 10 + 200 + 25 + 12


//
// PAL constants
//

// PAL clock frequency in Hz
static const u32 PAL_CLOCK_FREQUENCY = 985249;

// CPU cycles per second in PAL mode
static const unsigned PAL_CYCLES_PER_SECOND = PAL_CLOCK_FREQUENCY;

// Pixel aspect ratio in PAL mode
static const double PAL_PIXEL_ASPECT_RATIO = 0.9365;


// Horizontal screen parameters

// Width of left VBLANK area in PAL mode
static const u16 PAL_LEFT_VBLANK = 76;

// Width of left border in PAL mode
static const u16 PAL_LEFT_BORDER_WIDTH = 48;

// Width of canvas area in PAL mode
static const u16 PAL_CANVAS_WIDTH = 320;

// Width of right border in PAL mode
static const u16 PAL_RIGHT_BORDER_WIDTH = 37;

// Width of right VBLANK area in PAL mode
static const u16 PAL_RIGHT_VBLANK = 23;

// Total width of a rasterline (including VBLANK) in PAL mode
static const u16 PAL_WIDTH = 504; // 76 + 48 + 320 + 37 + 23

// Number of drawn pixels per rasterline in PAL mode
static const u16 PAL_PIXELS = 405; // 48 + 320 + 37


// Vertical screen parameters

// Number of VBLANK lines at top in PAL mode
static const u16 PAL_UPPER_VBLANK = 16;

// Heigt of upper boder in PAL mode
static const u16 PAL_UPPER_BORDER_HEIGHT = 35;

// Height of canvas area in PAL mode
static const u16 PAL_CANVAS_HEIGHT = 200;

// Lower border height in PAL mode
static const u16 PAL_LOWER_BORDER_HEIGHT = 49;

// Number of VBLANK lines at bottom in PAL mode
static const u16 PAL_LOWER_VBLANK = 12;

// Total height of a frame (including VBLANK) in PAL mode
static const u16 PAL_HEIGHT = 312; // 16 + 35 + 200 + 49 + 12

// Number of drawn rasterlines per frame in PAL mode
static const u16 PAL_RASTERLINES = 284; // 35 + 200 + 49

// Number of viewable rasterlines per frame in PAL mode
static const u16 PAL_VISIBLE_RASTERLINES = 284; // was 292


#endif
