/*
 * (C) 2015 Dirk W. Hoffmann. All rights reserved.
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

// CLEANUP:
// pixelBufferTmp.
//      this is no longer needed(?) as we draw pixels one by one
//      get rid of it

// TODO:
// 1. Introduce PixelEngine
//    Sub component to synthesize pixels
//    Will contain drawingContents stuff, pixelBuffers, renderRoutines etc.
//
// 3. Make sprite drawing cycle based.
// 4. Replace pixel buffers by 8 bit variables and implement mixer(). This makes z buffering obsolete.

#ifndef _PIXELENGINGE_INC
#define _PIXELENGINGE_INC

#define DIRK_DEBUG_LINE 77 // REMOVE ASAP

#include "VirtualComponent.h"


// Forward declarations
class VIC;
class C64;

//! PixelEngine
/*! This component is part of the virtual VICII chip and encapulates all functionality that is related to the
    synthesis of pixels. Its main entry point are prepareForCycle() and draw() which are called in every 
    VIC cycle inside the viewable range.
*/
class PixelEngine : public VirtualComponent {
    
    friend class VIC;
    
public:

    //! Reference to the connected video interface controller (VIC)
    VIC *vic;
    
    //! Constructor
    PixelEngine(C64 *c64);
    
    //! Destructor
    ~PixelEngine();
    
    //! Restore initial state
    void reset();
    
    //! Size of internal state
    uint32_t stateSize() { return 0; }
    
    //! Load state
    void loadFromBuffer(uint8_t **buffer) { }
    
    //! Save state
    void saveToBuffer(uint8_t **buffer) { }

    
    // -----------------------------------------------------------------------------------------------
    //                                     Constant definitions
    // -----------------------------------------------------------------------------------------------

    //! Predefined color schemes
    enum ColorScheme {
        CCS64           = 0x00,
        VICE            = 0x01,
        FRODO           = 0x02,
        PC64            = 0x03,
        C64S            = 0x04,
        ALEC64          = 0x05,
        WIN64           = 0x06,
        C64ALIVE_0_9    = 0x07,
        GODOT           = 0x08,
        C64SALLY        = 0x09,
        PEPTO           = 0x0A,
        GRAYSCALE       = 0x0B
    };
    
    //! VIC colors
    enum Color {
        BLACK   = 0x00,
        WHITE   = 0x01,
        RED     = 0x02,
        CYAN    = 0x03,
        PURPLE  = 0x04,
        GREEN   = 0x05,
        BLUE    = 0x06,
        YELLOW  = 0x07,
        LTBROWN = 0x08,
        BROWN   = 0x09,
        LTRED   = 0x0A,
        GREY1   = 0x0B,
        GREY2   = 0x0C,
        LTGREEN = 0x0D,
        LTBLUE  = 0x0E,
        GREY3   = 0x0F
    };
    
    //! Maximum number of viewable pixels per rasterline
    /*! TODO: 418 MIGHT BE TOO LARGE. CHECK HOW MUCH THE BORDER EXTENDS TO THE LEFT AND RIGHT
        IN BOTH PAL AND NTSC MODE. */
    static const uint16_t MAX_VIEWABLE_PIXELS = 418;

    
    // -----------------------------------------------------------------------------------------------
    //                                    Pixel buffers and colors
    // -----------------------------------------------------------------------------------------------
    
private:
    
    //! Currently used color scheme
    /*! Each color scheme uses slightly different RGB values to mimic the colors used in different emulators.
        Changing the color scheme modifies the colors[] array which stores the actual color codes in RGBA format. */
    /*! TODO: MOVE COLORSCHEME TO GUI CODE. IT'S A "LIFESTYLE" PROPERTY AND NOT A MACHINE FEATURE */
    ColorScheme colorScheme;
    
    //! All 16 C64 colors in RGBA format
    uint32_t colors[16];
    
    //! First screen buffer
    /*! The VIC chip writes it output into this buffer. The contents of the array is later copied into to
        texture RAM of your graphic card by the drawRect method in the OpenGL related code. */
    int screenBuffer1[512 * 512];
    
    //! Second screen buffer
    /*! The VIC chip uses double buffering. Once a frame is drawn, the VIC chip writes the next frame to the 
        second buffer. */
    int screenBuffer2[512 * 512];
    
    //! Target screen buffer for all rendering methods
    /*! The variable points either to screenBuffer1 or screenBuffer2 */
    int *currentScreenBuffer;
    
    //! Pointer to the current rasterline
    /*! This pointer is used by all rendering methods to write pixels. It always points to the beginning of a
        rasterline, either in screenBuffer1 or screenBuffer2. It is reset at the beginning of each frame and 
        incremented at the beginning of each rasterline. */
    int *pixelBuffer;
    
    //! Temporary pixel source
    /*! Data is first created here and later copied to pixelBuffer */
    int pixelBufferTmp[2];
    
    //! Z buffer
    /*! Virtual VICII uses depth buffering to determine pixel priority. In the various render routines, a pixel is 
        only written to the screen buffer, if it is closer to the view point. The depth of the closest pixel is kept 
        in the z buffer. The lower the value of the z buffer, the closer it is to the viewer.
        The z buffer is cleared before a new rasterline is drawn.
     */
    int zBuffer[MAX_VIEWABLE_PIXELS];
    
    //! Temporary Z buffer
    /*! Data is first created here and later copied to zBuffer */
    int zBufferTmp[2];
    
    //! Indicates the source of a drawn pixel
    /*! Whenever a foreground pixel or sprite pixel is drawn, a distinct bit in the pixelSource array is set.
     The information is utilized to detect sprite-sprite and sprite-background collisions.
     */
    int pixelSource[MAX_VIEWABLE_PIXELS];
    
    //! Temporary pixel source
    /*! Data is first created here and later copied to pixelSource */
    int pixelSourceTmp[2];

public:
    
    //! Set color scheme
    void setColorScheme(ColorScheme scheme);

    //! Get screen buffer that is currently stable
    /*! This method is called by the OpenGL code at the beginning of each frame. */
    inline void *screenBuffer() { return (currentScreenBuffer == screenBuffer1) ? screenBuffer2 : screenBuffer1; }

    
    // -----------------------------------------------------------------------------------------------
    //                                    Execution functions
    // -----------------------------------------------------------------------------------------------

    //! Prepare for new frame
    void beginFrame();
    
    //! Prepare for new rasterline
    void beginRasterline();
    
    //! Finish up rasterline
    void endRasterline();
    
    //! Finish up frame
    void endFrame();

    // -----------------------------------------------------------------------------------------------
    //                                  Higher level drawing routines
    // -----------------------------------------------------------------------------------------------

public:

    // -----------------------------------------------------------------------------------------------
    //                                      Character rendering
    // -----------------------------------------------------------------------------------------------

public:
    
    // -----------------------------------------------------------------------------------------------
    //                                        Pixel rendering
    // -----------------------------------------------------------------------------------------------

public:
    
    //! Draw a single frame pixel
    // DEPRECATED
    void setFramePixel(unsigned offset, int rgba);
    
    //! Render a singel frame pixel into temporary buffer
    // void renderFramePixel(unsigned offset, int rgba);
    
    //! Draw a single foreground pixel
    // DEPRECATED
    void setForegroundPixel(unsigned offset, int rgba);
    
    //! Render a singel foreground pixel into temporary buffer
    void renderForegroundPixel(unsigned offset, int rgba);
    
    //! Draw a single background pixel
    // DEPRECATED
    void setBackgroundPixel(unsigned offset, int rgba);
    
    //! Render a singel background pixel into temporary buffer
    void renderBackgroundPixel(unsigned offset, int rgba);
    
    //! Draw a single pixel behind background layer
    // DEPRECATED
    void setBehindBackgroundPixel(unsigned offset, int rgba);
    
    //! Draw a single sprite pixel
    void setSpritePixel(unsigned offset, int rgba, int depth, int source);
    
    
    //! Draw background pixels
    /*! This method is invoked when the sequencer is outside the main drawing area or the upper and lower border
     \param offset X coordinate of the first pixel to draw */
    void drawEightBehindBackgroudPixels(unsigned offset);
    
    //! Draw frame pixels
    inline void drawSevenFramePixels(unsigned offset, int rgba_color) {
        for (unsigned i = 0; i < 7; i++) setFramePixel(offset++, rgba_color); }
    
    inline void drawEightFramePixels(unsigned offset, int rgba_color) {
        for (unsigned i = 0; i < 8; i++) setFramePixel(offset++, rgba_color); }
    
    //! Render canvas pixel in single-color mode
    void renderSingleColorPixel(uint8_t bit);
    
    //! Render canvas pixel in single-color mode
    void renderMultiColorPixel(uint8_t color_bits);
    
    //! Render 2 pixels in single-color mode
    void renderTwoSingleColorPixels(uint8_t bits);
    
    //! Draw 2 pixels in single-color mode
    // DEPRECATED
    void drawTwoSingleColorPixels(unsigned offset, uint8_t bits);
    
    //! Draw a single character line (8 pixels) in single-color mode
    // DEPRECATED
    void drawSingleColorCharacter(unsigned offset);
    
    //! Render 2 pixels in multi-color mode
    // DEPRECATED
    void renderTwoMultiColorPixels(uint8_t bits);
    
    //! Draw 2 pixels in multi-color mode
    // DEPRECATED
    void drawTwoMultiColorPixels(unsigned offset, uint8_t bits);
    
    //! Draw a single character line (8 pixels) in multi-color mode
    // DEPRECATED
    void drawMultiColorCharacter(unsigned offset);
    
    //! Draw 2 single color pixels in invalid text mode
    // DEPRECATED
    void drawTwoInvalidSingleColorPixels(unsigned offset, uint8_t bits);
    
    //! Draw a single color character in invalid text mode
    // DEPRECATED
    void drawInvalidSingleColorCharacter(unsigned offset);
    
    //! Draw 2 multicolor pixels in invalid text mode
    // DEPRECATED
    void drawTwoInvalidMultiColorPixels(unsigned offset, uint8_t bits);
    
    //! Draw a multi color character in invalid text mode
    // DEPRECATED
    void drawInvalidMultiColorCharacter(unsigned offset);
    
    //! Draw a single foreground pixel
    /*! \param offset X coordinate of the pixel to draw
	    \param color Pixel color in RGBA format
	    \param nr Number of sprite (0 to 7)
     \note The function may trigger an interrupt, if a sprite/sprite or sprite/background collision is detected
     */
    void setSpritePixel(unsigned offset, int color, int nr);
    

    //! Draw a colored line into the screen buffer
    /*! This method is utilized for debugging purposes, only. */
    void markLine(uint8_t color, unsigned start = 0, unsigned end = 512);
};

    
#endif