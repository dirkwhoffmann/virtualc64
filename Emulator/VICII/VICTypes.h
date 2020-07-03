// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _VIC_TYPES_INC
#define _VIC_TYPES_INC

//
// VICII colors
//

//! @brief    VIC colors
enum VICIIColors {
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

//! @brief    VIC color registers (D020 - D02E)
enum VICIIColorRegs {
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

//! @brief    Values of the two frame flipflops
typedef struct {
    
    bool vertical;
    bool main;
    
} FrameFlipflops;

//! @brief    Values of (piped) I/O registers
typedef struct {
    
    u16 sprX[8];     // D000, D002, ..., D00E, upper bits from D010
    uint8_t  sprY[8];     // D001, D003, ..., D00F
    uint8_t  ctrl1;       // D011
    uint8_t  sprEnable;   // D015
    uint8_t  ctrl2;       // D016
    uint8_t  sprExpandY;  // D017
    uint8_t  sprPriority; // D01B
    uint8_t  sprMC;       // D01C
    uint8_t  sprExpandX;  // D01D
    uint8_t  colors[15];  // D020 - D02E
    
} VICIIRegisters;


//
// NTSC constants
//

//! @brief    NTSC clock frequency in Hz
static const u32 NTSC_CLOCK_FREQUENCY = 1022727;

//! @brief    CPU cycles per second in NTSC mode
static const unsigned NTSC_CYCLES_PER_SECOND = NTSC_CLOCK_FREQUENCY;

//! @brief    Pixel aspect ratio in NTSC mode
static const double NTSC_PIXEL_ASPECT_RATIO = 0.75;


// Horizontal screen parameters

//! @brief    Width of left VBLANK area in NTSC mode
static const u16 NTSC_LEFT_VBLANK = 77;

//! @brief    Width of left border in NTSC mode
static const u16 NTSC_LEFT_BORDER_WIDTH = 55;

//! @brief    Width of canvas area in NTSC mode
static const u16 NTSC_CANVAS_WIDTH = 320;

//! @brief    Width of right border in NTSC mode
static const u16 NTSC_RIGHT_BORDER_WIDTH = 53;

//! @brief    Width of right VBLANK area in NTSC mode
static const u16 NTSC_RIGHT_VBLANK = 15;

//! @brief    Total width of a rasterline (including VBLANK) in NTSC mode
static const u16 NTSC_WIDTH = 520; // 77 + 55 + 320 + 53 + 15

//! @brief    Number of drawn pixels per rasterline in NTSC mode
static const u16 NTSC_PIXELS = 428; // 55 + 320 + 53

//! @brief    Number of viewable pixels per rasterline in NTSC mode
static const u16 NTSC_VISIBLE_PIXELS = 418;


// Vertical screen parameters

//! @brief    Number of VBLANK lines at top in NTSC mode
static const u16 NTSC_UPPER_VBLANK = 16;

//! @brief    Heigt of upper boder in NTSC mode
static const u16 NTSC_UPPER_BORDER_HEIGHT = 10;

//! @brief    Height of canvas area in NTSC mode
static const u16 NTSC_CANVAS_HEIGHT = 200;

//! @brief    Lower border height in NTSC mode
static const u16 NTSC_LOWER_BORDER_HEIGHT = 25;

//! @brief    Number of VBLANK lines at bottom in NTSC mode
static const u16 NTSC_LOWER_VBLANK = 12;

//! @brief    Total height of a frame (including VBLANK) in NTSC mode
static const u16 NTSC_HEIGHT = 263; // 16 + 10 + 200 + 25 + 12

//! @brief    Number of drawn rasterlines per frame in NTSC mode
//! @deprecated because the value differes between NTSC models
static const u16 NTSC_RASTERLINES = 235; // 10 + 200 + 25

//! @brief    Number of viewable rasterlines per frame in NTSC mode
//! @deprecated because the value differes between NTSC models
// static const u16 NTSC_VISIBLE_RASTERLINES = 235;


//
// PAL constants
//

//! @brief    PAL clock frequency in Hz
static const u32 PAL_CLOCK_FREQUENCY = 985249;

//! @brief    CPU cycles per second in PAL mode
static const unsigned PAL_CYCLES_PER_SECOND = PAL_CLOCK_FREQUENCY;

//! @brief    Pixel aspect ratio in PAL mode
static const double PAL_PIXEL_ASPECT_RATIO = 0.9365;


// Horizontal screen parameters

//! @brief    Width of left VBLANK area in PAL mode
static const u16 PAL_LEFT_VBLANK = 76;

//! @brief    Width of left border in PAL mode
static const u16 PAL_LEFT_BORDER_WIDTH = 48;

//! @brief    Width of canvas area in PAL mode
static const u16 PAL_CANVAS_WIDTH = 320;

//! @brief    Width of right border in PAL mode
static const u16 PAL_RIGHT_BORDER_WIDTH = 37;

//! @brief    Width of right VBLANK area in PAL mode
static const u16 PAL_RIGHT_VBLANK = 23;

//! @brief    Total width of a rasterline (including VBLANK) in PAL mode
static const u16 PAL_WIDTH = 504; // 76 + 48 + 320 + 37 + 23

//! @brief    Number of drawn pixels per rasterline in PAL mode
static const u16 PAL_PIXELS = 405; // 48 + 320 + 37

//! @brief    Number of viewable pixels per rasterline in PAL mode
// static const u16 PAL_VISIBLE_PIXELS = 403;


// Vertical screen parameters

//! @brief    Number of VBLANK lines at top in PAL mode
static const u16 PAL_UPPER_VBLANK = 16;

//! @brief    Heigt of upper boder in PAL mode
static const u16 PAL_UPPER_BORDER_HEIGHT = 35;

//! @brief    Height of canvas area in PAL mode
static const u16 PAL_CANVAS_HEIGHT = 200;

//! @brief    Lower border height in PAL mode
static const u16 PAL_LOWER_BORDER_HEIGHT = 49;

//! @brief    Number of VBLANK lines at bottom in PAL mode
static const u16 PAL_LOWER_VBLANK = 12;

//! @brief    Total height of a frame (including VBLANK) in PAL mode
static const u16 PAL_HEIGHT = 312; // 16 + 35 + 200 + 49 + 12

//! @brief    Number of drawn rasterlines per frame in PAL mode
static const u16 PAL_RASTERLINES = 284; // 35 + 200 + 49

//! @brief    Number of viewable rasterlines per frame in PAL mode
static const u16 PAL_VISIBLE_RASTERLINES = 284; // was 292


//
// Types
//

//! @brief    VICII chip model
typedef enum {
    PAL_6569_R1 = 1,
    PAL_6569_R3 = 2,
    PAL_8565 = 4,
    NTSC_6567_R56A = 8,
    NTSC_6567 = 16,
    NTSC_8562 = 32
} VICModel;

inline bool isVICChhipModel(VICModel model) {
    return
    (model == PAL_6569_R1) ||
    (model == PAL_6569_R3) ||
    (model == PAL_8565) ||
    (model == NTSC_6567) ||
    (model == NTSC_6567_R56A) ||
    (model == NTSC_8562);
}

//! @brief    Color palette type
/*! @details  Used to emulate monochrome displays
 */
typedef enum {
    COLOR_PALETTE = 0,
    BLACK_WHITE_PALETTE,
    PAPER_WHITE_PALETTE,
    GREEN_PALETTE,
    AMBER_PALETTE,
    SEPIA_PALETTE
} VICPalette;

inline bool isVICPalette(VICPalette model) {
    return model >= COLOR_PALETTE && model <= SEPIA_PALETTE;
}

//! @brief    Glue logic type
typedef enum {
    GLUE_DISCRETE = 0,
    GLUE_CUSTOM_IC = 1
} GlueLogic;

inline bool isGlueLogic(GlueLogic type) {
    return
    (type == GLUE_DISCRETE) ||
    (type == GLUE_CUSTOM_IC);
}

//! @brief    Screen geometries
typedef enum {
    COL_40_ROW_25 = 0x01,
    COL_38_ROW_25 = 0x02,
    COL_40_ROW_24 = 0x03,
    COL_38_ROW_24 = 0x04
} ScreenGeometry;

//! Display mode
typedef enum {
    STANDARD_TEXT             = 0x00,
    MULTICOLOR_TEXT           = 0x10,
    STANDARD_BITMAP           = 0x20,
    MULTICOLOR_BITMAP         = 0x30,
    EXTENDED_BACKGROUND_COLOR = 0x40,
    INVALID_TEXT              = 0x50,
    INVALID_STANDARD_BITMAP   = 0x60,
    INVALID_MULTICOLOR_BITMAP = 0x70
} DisplayMode;

/*! @brief    VIC info
 *  @details  Used by VIC::getInfo() to collect debug information
 */
typedef struct {
    u16 rasterline;
    uint8_t cycle;
    u16 xCounter;
    bool badLine;
    bool ba; 
    DisplayMode displayMode;
    uint8_t borderColor;
    uint8_t backgroundColor0;
    uint8_t backgroundColor1;
    uint8_t backgroundColor2;
    uint8_t backgroundColor3;
    ScreenGeometry screenGeometry;
    uint8_t dx;
    uint8_t dy;
    bool verticalFrameFlipflop;
    bool horizontalFrameFlipflop;
    u16 memoryBankAddr;
    u16 screenMemoryAddr;
    u16 characterMemoryAddr;
    uint8_t imr;
    uint8_t irr;
    bool spriteCollisionIrqEnabled;
    bool backgroundCollisionIrqEnabled;
    bool rasterIrqEnabled;
    u16 irqRasterline;
    bool irqLine;
} VICInfo;

/*! @brief    Sprite info
 *  @details  Used by VIC::getSpriteInfo() to collect debug information
 */
typedef struct {
  
    bool enabled; 
    u16 x;
    uint8_t y;
    uint8_t ptr; 
    uint8_t color;
    bool multicolor;
    uint8_t extraColor1;
    uint8_t extraColor2;
    bool expandX;
    bool expandY;
    bool priority;
    bool collidesWithSprite;
    bool collidesWithBackground;
} SpriteInfo;
    
#endif
