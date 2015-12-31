/*
 * Written 2015 by Dirk W. Hoffmann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _VIC_CONSTANTS_INC
#define _VIC_CONSTANTS_INC

// -----------------------------------------------------------------------------------------------
//                                           General
// -----------------------------------------------------------------------------------------------

#define SPR0 0x01
#define SPR1 0x02
#define SPR2 0x04
#define SPR3 0x08
#define SPR4 0x10
#define SPR5 0x20
#define SPR6 0x40
#define SPR7 0x80

//! @brief Supported VIC II chip models
enum VICChipModel {
    MOS6567_NTSC = 0,
    MOS6569_PAL = 1
};

//! @brief Screen geometries
enum ScreenGeometry {
    COL_40_ROW_25 = 0x01,
    COL_38_ROW_25 = 0x02,
    COL_40_ROW_24 = 0x03,
    COL_38_ROW_24 = 0x04
};

//! @brief Start address of the VIC I/O space
static const uint16_t VIC_START_ADDR = 0xD000;

//! @brief End address of the VIC I/O space
static const uint16_t VIC_END_ADDR = 0xD3FF;


// -----------------------------------------------------------------------------------------------
//                                       NTSC constants
// -----------------------------------------------------------------------------------------------

//! Frames per second
static const double NTSC_REFRESH_RATE = 59.826;

//! CPU cycles per rasterline
static const uint16_t NTSC_CYCLES_PER_RASTERLINE = 65;

//! CPU cycles per frame
static const unsigned NTSC_CYCLES_PER_FRAME = 17095;

//! CPU cycles per second
static const unsigned NTSC_CYCLES_PER_SECOND = NTSC_REFRESH_RATE * NTSC_CYCLES_PER_FRAME;

//! Pixel aspect ratio
static const uint16_t NTSC_PIXEL_ASPECT_RATIO = 0.75;


// Horizontal screen parameters

//! Width of left VBLANK area
static const uint16_t NTSC_LEFT_VBLANK = 77;

//! Width of left border
static const uint16_t NTSC_LEFT_BORDER_WIDTH = 55; // was 49

//! Width of canvas area
static const uint16_t NTSC_CANVAS_WIDTH = 320;

//! Width of right border
static const uint16_t NTSC_RIGHT_BORDER_WIDTH = 53; // was 49

//! Width of right VBLANK area
static const uint16_t NTSC_RIGHT_VBLANK = 15;

//! Total width of a rasterline (including VBLANK)
static const uint16_t NTSC_WIDTH = 520; // 77 + 55 + 320 + 53 + 15

//! Number of drawn pixels per rasterline
static const uint16_t NTSC_PIXELS = 428; // 55 + 320 + 53

//! Number of viewable pixels per rasterline
static const uint16_t NTSC_VISIBLE_PIXELS = 418;


// Vertical screen parameters

//! Number of VBLANK lines at top
static const uint16_t NTSC_UPPER_VBLANK = 16;

//! Heigt of upper boder
static const uint16_t NTSC_UPPER_BORDER_HEIGHT = 10; // Was 23

//! Height of canvas area
static const uint16_t NTSC_CANVAS_HEIGHT = 200;

//! Lower border height
static const uint16_t NTSC_LOWER_BORDER_HEIGHT = 25; // Was 12

//! Number of VBLANK lines at bottom
static const uint16_t NTSC_LOWER_VBLANK = 12;

//! Total height of a frame (including VBLANK)
static const uint16_t NTSC_HEIGHT = 263; // 16 + 10 + 200 + 25 + 12

//! Number of drawn rasterlines per frame
static const uint16_t NTSC_RASTERLINES = 235; // 10 + 200 + 25

//! Number of viewable rasterlines per frame
static const uint16_t NTSC_VISIBLE_RASTERLINES = 235;


// -----------------------------------------------------------------------------------------------
//                                       PAL constants
// -----------------------------------------------------------------------------------------------

//! Frames per second
static const double PAL_REFRESH_RATE = 50.125;

//! CPU cycles per rasterline
static const uint16_t PAL_CYCLES_PER_RASTERLINE = 63;

//! CPU cycles per frame
static const unsigned PAL_CYCLES_PER_FRAME = 19656;

//! CPU cycles per second
static const unsigned PAL_CYCLES_PER_SECOND = PAL_REFRESH_RATE * PAL_CYCLES_PER_FRAME;

//! Pixel aspect ratio
static const uint16_t PAL_PIXEL_ASPECT_RATIO = 0.9365;


// Horizontal screen parameters

//! Width of left VBLANK area
static const uint16_t PAL_LEFT_VBLANK = 76;

//! Width of left border
static const uint16_t PAL_LEFT_BORDER_WIDTH = 48; // was 46; // Should be 48

//! Width of canvas area
static const uint16_t PAL_CANVAS_WIDTH = 320;

//! Width of right border
static const uint16_t PAL_RIGHT_BORDER_WIDTH = 37; // was 36; // Should be 37

//! Width of right VBLANK area
static const uint16_t PAL_RIGHT_VBLANK = 23;

//! Total width of a rasterline (including VBLANK)
static const uint16_t PAL_WIDTH = 504; // 76 + 48 + 320 + 37 + 23

//! Number of drawn pixels per rasterline
static const uint16_t PAL_PIXELS = 405; // 48 + 320 + 37

//! Number of viewable pixels per rasterline
static const uint16_t PAL_VISIBLE_PIXELS = 403; // was 402


// Vertical screen parameters

//! Number of VBLANK lines at top
static const uint16_t PAL_UPPER_VBLANK = 16;

//! Heigt of upper boder
static const uint16_t PAL_UPPER_BORDER_HEIGHT = 35; // was 43

//! Height of canvas area
static const uint16_t PAL_CANVAS_HEIGHT = 200;

//! Lower border height
static const uint16_t PAL_LOWER_BORDER_HEIGHT = 49;

//! Number of VBLANK lines at bottom
static const uint16_t PAL_LOWER_VBLANK = 12;

//! Total height of a frame (including VBLANK)
static const uint16_t PAL_HEIGHT = 312; // 16 + 35 + 200 + 49 + 12

//! Number of drawn rasterlines per frame
static const uint16_t PAL_RASTERLINES = 284; // 35 + 200 + 49

//! Number of viewable rasterlines per frame
static const uint16_t PAL_VISIBLE_RASTERLINES = 284; // was 292


// -----------------------------------------------------------------------------------------------
//                                     VIC state pipes
//
// Each pipe comprises a certain portion of the VICs internal state. I.e., they comprise those
// state variables that are accessed by the pixel engine and need to be delayed by a certain
// amount to get the timing right. Most state variables need to be delayed by one cycle.
// An exception are the color registers that usually exhibit a value change somewhere in the middle
// of an pixel chunk. To implement the delay, both VIC and PixelEngine hold a pipe variable of their
// own, and the contents of the VICs variable is copied over the contents of the PixelEngines
// variable at the right time. Putting the state variables in seperate structures allows the compiler
// to optize the copy process.
// -----------------------------------------------------------------------------------------------

/*! @brief    A certain portion of VICs internal state
 *  @details  This structure comprises all state variables that need to be delayed one cycle to get
 *            the timing right. 
 */
typedef struct {
    
    //! @brief    Internal x counter of the sequencer (sprite coordinate system)
    uint16_t xCounter;
    
    /*! @brief    Sprite X coordinates
     *  @details  The X coordinate is a 9 bit value. For each sprite, the lower 8 bits are stored in a
     *            seperate IO register, while the uppermost bits are packed in a single register (0xD010).
     *            The sprites X coordinate is updated whenever one the corresponding IO register changes
     *            its value. 
     */
    uint16_t spriteX[8];

    //! @brief    Sprite X expansion bits
    uint8_t spriteXexpand;

    //! @brief    Internal VIC-II register D011, control register 1
    uint8_t registerCTRL1;

    //! @brief    Internal VIC-II register D016, control register 2
    uint8_t registerCTRL2;

    //! @brief    Data value grabbed in gAccess()
    uint8_t g_data;
    
    //! @brief    Character value grabbed in gAccess()
    uint8_t g_character;
    
    //! @brief    Color value grabbed in gAccess()
    uint8_t g_color;
    
    //! @brief    Main frame flipflop
    uint8_t mainFrameFF;

    //! @brief    Vertical frame Flipflop
    uint8_t verticalFrameFF;
    
} PixelEnginePipe;


//! @brief    Color for drawing border pixels
typedef struct {
    
    uint8_t borderColor;
    
} BorderColorPipe;


//! @brief    Colors for drawing canvas pixels
typedef struct {
    
    uint8_t backgroundColor[4];
    
} CanvasColorPipe;

//! @brief    Colors for drawing sprites
typedef struct {
    
    uint8_t spriteColor[8];
    uint8_t spriteExtraColor1;
    uint8_t spriteExtraColor2;

} SpriteColorPipe;


#endif
