/*!
 * @header      VIC_types.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2015 - 2018 Dirk W. Hoffmann
 */
/*              This program is free software; you can redistribute it and/or modify
 *              it under the terms of the GNU General Public License as published by
 *              the Free Software Foundation; either version 2 of the License, or
 *              (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program; if not, write to the Free Software
 *              Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#ifndef _VIC_TYPES_INC
#define _VIC_TYPES_INC

//
// NTSC constants
//

//! @brief    NTSC clock frequency in Hz
static const uint32_t CLOCK_FREQUENCY_NTSC = 1022727;

//! @brief    Frames per second in NTSC mode
static const double NTSC_REFRESH_RATE = 59.826;

//! @brief    CPU cycles per rasterline in NTSC mode
static const uint16_t NTSC_CYCLES_PER_RASTERLINE = 65;

//! @brief    CPU cycles per frame in NTSC mode
static const unsigned NTSC_CYCLES_PER_FRAME = 17095;

//! @brief    CPU cycles per second in NTSC mode
static const unsigned NTSC_CYCLES_PER_SECOND = NTSC_REFRESH_RATE * NTSC_CYCLES_PER_FRAME;

//! @brief    Pixel aspect ratio in NTSC mode
static const double NTSC_PIXEL_ASPECT_RATIO = 0.75;


// Horizontal screen parameters

//! @brief    Width of left VBLANK area in NTSC mode
static const uint16_t NTSC_LEFT_VBLANK = 77;

//! @brief    Width of left border in NTSC mode
static const uint16_t NTSC_LEFT_BORDER_WIDTH = 55;

//! @brief    Width of canvas area in NTSC mode
static const uint16_t NTSC_CANVAS_WIDTH = 320;

//! @brief    Width of right border in NTSC mode
static const uint16_t NTSC_RIGHT_BORDER_WIDTH = 53;

//! @brief    Width of right VBLANK area in NTSC mode
static const uint16_t NTSC_RIGHT_VBLANK = 15;

//! @brief    Total width of a rasterline (including VBLANK) in NTSC mode
static const uint16_t NTSC_WIDTH = 520; // 77 + 55 + 320 + 53 + 15

//! @brief    Number of drawn pixels per rasterline in NTSC mode
static const uint16_t NTSC_PIXELS = 428; // 55 + 320 + 53

//! @brief    Number of viewable pixels per rasterline in NTSC mode
static const uint16_t NTSC_VISIBLE_PIXELS = 418;


// Vertical screen parameters

//! @brief    Number of VBLANK lines at top in NTSC mode
static const uint16_t NTSC_UPPER_VBLANK = 16;

//! @brief    Heigt of upper boder in NTSC mode
static const uint16_t NTSC_UPPER_BORDER_HEIGHT = 10;

//! @brief    Height of canvas area in NTSC mode
static const uint16_t NTSC_CANVAS_HEIGHT = 200;

//! @brief    Lower border height in NTSC mode
static const uint16_t NTSC_LOWER_BORDER_HEIGHT = 25;

//! @brief    Number of VBLANK lines at bottom in NTSC mode
static const uint16_t NTSC_LOWER_VBLANK = 12;

//! @brief    Total height of a frame (including VBLANK) in NTSC mode
static const uint16_t NTSC_HEIGHT = 263; // 16 + 10 + 200 + 25 + 12

//! @brief    Number of drawn rasterlines per frame in NTSC mode
static const uint16_t NTSC_RASTERLINES = 235; // 10 + 200 + 25

//! @brief    Number of viewable rasterlines per frame in NTSC mode
static const uint16_t NTSC_VISIBLE_RASTERLINES = 235;


//
// PAL constants
//

//! @brief    PAL clock frequency in Hz
static const uint32_t CLOCK_FREQUENCY_PAL = 985249;

//! @brief    Frames per second in PAL mode
static const double PAL_REFRESH_RATE = 50.125;

//! @brief    CPU cycles per rasterline in PAL mode
static const uint16_t PAL_CYCLES_PER_RASTERLINE = 63;

//! @brief    CPU cycles per frame in PAL mode
static const unsigned PAL_CYCLES_PER_FRAME = 19656;

//! @brief    CPU cycles per second in PAL mode
static const unsigned PAL_CYCLES_PER_SECOND = PAL_REFRESH_RATE * PAL_CYCLES_PER_FRAME;

//! @brief    Pixel aspect ratio in PAL mode
static const double PAL_PIXEL_ASPECT_RATIO = 0.9365;


// Horizontal screen parameters

//! @brief    Width of left VBLANK area in PAL mode
static const uint16_t PAL_LEFT_VBLANK = 76;

//! @brief    Width of left border in PAL mode
static const uint16_t PAL_LEFT_BORDER_WIDTH = 48;

//! @brief    Width of canvas area in PAL mode
static const uint16_t PAL_CANVAS_WIDTH = 320;

//! @brief    Width of right border in PAL mode
static const uint16_t PAL_RIGHT_BORDER_WIDTH = 37;

//! @brief    Width of right VBLANK area in PAL mode
static const uint16_t PAL_RIGHT_VBLANK = 23;

//! @brief    Total width of a rasterline (including VBLANK) in PAL mode
static const uint16_t PAL_WIDTH = 504; // 76 + 48 + 320 + 37 + 23

//! @brief    Number of drawn pixels per rasterline in PAL mode
static const uint16_t PAL_PIXELS = 405; // 48 + 320 + 37

//! @brief    Number of viewable pixels per rasterline in PAL mode
static const uint16_t PAL_VISIBLE_PIXELS = 403;


// Vertical screen parameters

//! @brief    Number of VBLANK lines at top in PAL mode
static const uint16_t PAL_UPPER_VBLANK = 16;

//! @brief    Heigt of upper boder in PAL mode
static const uint16_t PAL_UPPER_BORDER_HEIGHT = 35;

//! @brief    Height of canvas area in PAL mode
static const uint16_t PAL_CANVAS_HEIGHT = 200;

//! @brief    Lower border height in PAL mode
static const uint16_t PAL_LOWER_BORDER_HEIGHT = 49;

//! @brief    Number of VBLANK lines at bottom in PAL mode
static const uint16_t PAL_LOWER_VBLANK = 12;

//! @brief    Total height of a frame (including VBLANK) in PAL mode
static const uint16_t PAL_HEIGHT = 312; // 16 + 35 + 200 + 49 + 12

//! @brief    Number of drawn rasterlines per frame in PAL mode
static const uint16_t PAL_RASTERLINES = 284; // 35 + 200 + 49

//! @brief    Number of viewable rasterlines per frame in PAL mode
static const uint16_t PAL_VISIBLE_RASTERLINES = 284; // was 292


//
// Types
//

//! @brief    VIC II chip model
typedef enum {
    MOS6567_NTSC = 0,
    MOS6569_PAL = 1
} VICChipModel;

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
    uint16_t rasterline;
    uint8_t cycle;
    uint16_t xCounter;
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
    uint16_t memoryBankAddr;
    uint16_t screenMemoryAddr;
    uint16_t characterMemoryAddr;
    uint8_t imr;
    uint8_t irr;
    bool spriteCollisionIrqEnabled;
    bool backgroundCollisionIrqEnabled;
    bool rasterIrqEnabled;
    uint16_t irqRasterline;
    bool irqLine;
} VICInfo;

/*! @brief    Sprite info
 *  @details  Used by VIC::getSpriteInfo() to collect debug information
 */
typedef struct {
  
    bool enabled; 
    uint16_t x;
    uint8_t y;
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
