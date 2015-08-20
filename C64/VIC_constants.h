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
//                                                NTSC
// -----------------------------------------------------------------------------------------------

//! Frames per second
static const uint16_t NTSC_REFRESH_RATE = 60;

//! CPU cycles per rasterline
static const uint16_t NTSC_CYCLES_PER_RASTERLINE = 65;


// Horizontal screen parameters

//! Width of left VBLANK area
static const uint16_t NTSC_LEFT_VBLANK = 77;

//! Width of left border
static const uint16_t NTSC_LEFT_BORDER_WIDTH = 49; // Should be 55

//! Width of canvas area
static const uint16_t NTSC_CANVAS_WIDTH = 320;

//! Width of right border
static const uint16_t NTSC_RIGHT_BORDER_WIDTH = 49; // Should be 53

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
static const uint16_t NTSC_UPPER_BORDER_HEIGHT = 23; // Should be 10

//! Height of canvas area
static const uint16_t NTSC_CANVAS_HEIGHT = 200;

//! Lower border height
static const uint16_t NTSC_LOWER_BORDER_HEIGHT = 12; // Should be 25

//! Number of VBLANK lines at bottom
static const uint16_t NTSC_LOWER_VBLANK = 12;

//! Total height of a frame (including VBLANK)
static const uint16_t NTSC_HEIGHT = 263; // 16 + 10 + 200 + 25 + 12

//! Number of drawn rasterlines per frame
static const uint16_t NTSC_RASTERLINES = 235; // 10 + 200 + 25

//! Number of viewable rasterlines per frame
static const uint16_t NTSC_VISIBLE_RASTERLINES = 235;



//! Number of invisible lines above upper border (first visible line is ???) // Needs verification
//  DEPRECATED
static const uint16_t NTSC_UPPER_INVISIBLE = 28;

//! Number of invisible lines below lower border (last visible line is ???) // Needs verification
//  DEPRECATED
static const uint16_t NTSC_LOWER_INVISIBLE = 0;



// -----------------------------------------------------------------------------------------------
//                                              PAL
// -----------------------------------------------------------------------------------------------

//! Frames per second
static const uint16_t PAL_REFRESH_RATE = 50;

//! CPU cycles per rasterline
static const uint16_t PAL_CYCLES_PER_RASTERLINE = 63;


// Horizontal screen parameters

//! Width of left VBLANK area
static const uint16_t PAL_LEFT_VBLANK = 76;

//! Width of left border
static const uint16_t PAL_LEFT_BORDER_WIDTH = 46; // Should be 48

//! Width of canvas area
static const uint16_t PAL_CANVAS_WIDTH = 320;

//! Width of right border
static const uint16_t PAL_RIGHT_BORDER_WIDTH = 36; // Should be 37

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
static const uint16_t PAL_UPPER_BORDER_HEIGHT = 43; // Should be 35

//! Height of canvas area
static const uint16_t PAL_CANVAS_HEIGHT = 200;

//! Lower border height
static const uint16_t PAL_LOWER_BORDER_HEIGHT = 49;

//! Number of VBLANK lines at bottom
static const uint16_t PAL_LOWER_VBLANK = 12;

//! Total height of a frame (including VBLANK)
static const uint16_t PAL_HEIGHT = 312; // 16 + 35 + 200 + 49 + 12

//! Number of drawn rasterlines per frame
static const uint16_t PAL_RASTERLINES = 284; // 43 + 200 + 49

//! Number of viewable rasterlines per frame
static const uint16_t PAL_VISIBLE_RASTERLINES = 284; // was 292




//! First visible rasterline (after VBLANK)
//  DEPRECATED
static const uint16_t PAL_UPPER_INVISIBLE = 8;  // Should be 16

//! Number of invisible lines below lower border (last visible line is 0x12B)
//  DEPRECATED
static const uint16_t PAL_LOWER_INVISIBLE = 12;


#endif
