/*!
 * @header      PixelEngine.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
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


public:
    
    //! @brief    Method from VirtualComponent
    void resetPixelEngine();

    //! @brief    Initializes both screenBuffers
    /*! @details  This function is needed for debugging, only. It write some
     *            recognizable pattern into both buffers.
     */
    void resetScreenBuffers();

    

    
    
    //
    // Rastercycle information
    //

private:
    

    
    
    //
    // Execution functions
    //

public:
    
    //! @brief    Prepares for a new frame.
    void beginFramePixelEngine();
    
    //! @brief    Prepares for a new rasterline.
    void beginRasterlinePixelEngine();
    
    //! @brief    Finishes a rasterline.
    void endRasterlinePixelEngine();
    
    //! @brief    Finishes a frame.
    void endFramePixelEngine();

   


    //! Sprite extra color 1 (same for all sprites)
    uint8_t spriteExtraColor1;
    
    //! Sprite extra color 2 (same for all sprites)
    uint8_t spriteExtraColor2;

    /*! @brief    Loads the sprite shift register.
     *  @details  The shift register is loaded with the three data bytes fetched
     *            in the previous sAccesses.
     */
    void loadShiftRegister(unsigned nr) {
        sprite_sr[nr].data =
        (sprite_sr[nr].chunk1 << 16) |
        (sprite_sr[nr].chunk2 << 8) |
        (sprite_sr[nr].chunk3);
    }
    
    
    //
    // Accessing colors
    //
    
    
    //
    // External drawing routines (called inside the VICII::cycle functions)
    //

public:
  
    /*! @brief    Synthesize 8 pixels according the the current drawing context.
     *  @details  This is the main entry point and is invoked in each VIC
     *            drawing cycle, except cycle 17 and cycle 55 which are handles
     *            seperately for speedup purposes. To get the correct output,
     *            preparePixelEngine() must be called one cycle before.
     */
    void draw();

    //! @brief    Special draw routine for cycle 17
    void draw17();

    //! @brief    Special draw routine for cycle 55
    void draw55();

    /*! @brief    Draw routine for cycles outside the visible screen region.
     *  @details  The sprite sequencer needs to be run outside the visible area,
     *            although no pixels will be drawn (drawing is omitted by having
     *            visibleColumn set to false.
     */
    void drawOutsideBorder();
    
    
    //
    // Internal drawing routines (called by the external ones)
    //
    
private:
    
    /*! @brief    Draws a part of the border
     *  @details  Invoked inside draw()
     */
    void drawBorder();
    
    /*! @brief    Draws a part of the border
     *  @details  Invoked inside draw17() 
     */
    void drawBorder17();
    
    /*! @brief    Draws a part of the border
     *  @details  Invoked inside draw55()
     */
    void drawBorder55();

    /*! @brief    Draws 8 canvas pixels
     *  @details  Invoked inside draw()
     */
    void drawCanvas();
    
    /*! @brief    Draws a single canvas pixel
     *  @param    pixelnr is the pixel number and must be in the range 0 to 7
     *  @param    loadShiftReg is set to true if the shift register needs to be
     *            reloaded
     *  @param    updateColors is set to true if the four selectable colors
     *            might have changed.
     */
    void drawCanvasPixel(uint8_t pixelnr,
                         uint8_t mode,
                         uint8_t d016,
                         bool loadShiftReg,
                         bool updateColors);
    
    /*! @brief    Draws 8 sprite pixels
     *  @details  Invoked inside draw() 
     */
    void drawSprites();

    /*! @brief    Draws a single sprite pixel for all sprites
     *  @param    pixelnr  Pixel number (0 to 7)
     *  @param    freeze   If the i-th bit is set to 1, the i-th shift register
     *                     will freeze temporarily
     *  @param    halt     If the i-th bit is set to 1, the i-th shift register
     *                     will be deactivated
     *  @param    load     If the i-th bit is set to 1, the i-th shift register
     *                     will grab new data bits
     */
    /*
    void drawSpritePixel(unsigned pixelnr,
                         uint8_t freeze,
                         uint8_t halt,
                         uint8_t load);
     */
    
    /*! @brief    Draws a single sprite pixel for a single sprite
     *  @param    spritenr Sprite number (0 to 7)
     *  @param    pixelnr  Pixel number (0 to 7)
     *  @param    freeze   If set to true, the sprites shift register will
     *                     freeze temporarily
     *  @param    halt     If set to true, the sprites shift shift register will
     *                     be deactivated
     *  @param    load     If set to true, the sprites shift shift register will
     *                     grab new data bits
     */
    void drawSpritePixel(unsigned spritenr,
                         unsigned pixelnr,
                         bool freeze,
                         bool halt,
                         bool load);

    /*! @brief    Draws all sprites into the pixelbuffer
     *  @details  A sprite is only drawn if it's enabled and if sprite drawing
     *            is not switched off for debugging
     */
    void drawAllSprites();
    
    /*! @brief    Draw single sprite into pixel buffer
     *  @details  Helper function for drawSprites 
     */
    void drawSprite(uint8_t nr);
    
    
    //
    // Mid level drawing (semantic pixel rendering)
    //

private:
    
    /*! @brief    This is where loadColors() stores all retrieved colors
     *  @details  [0] : color for '0'  pixels in single color mode
     *                         or '00' pixels in multicolor mode
     *            [1] : color for '1'  pixels in single color mode
     *                         or '01' pixels in multicolor mode
     *            [2] : color for '10' pixels in multicolor mode
     *            [3] : color for '11' pixels in multicolor mode 
     */
    uint8_t col[4];

    //! @brief    Sprite colors
    uint64_t sprExtraCol1;
    uint64_t sprExtraCol2;
    uint64_t sprCol[8];

public:
    
    //! @brief    Determines pixel colors accordig to the provided display mode
    void loadColors(uint8_t pixelNr, uint8_t mode,
                    uint8_t characterSpace, uint8_t colorSpace);

    /*! @brief    Draws single canvas pixel in single-color mode
     *  @details  1s are drawn with drawForegroundPixel, 0s are drawn with
     *            drawBackgroundPixel. Uses the drawing colors that are setup by
     *            loadColors().
     */
    void setSingleColorPixel(unsigned pixelnr, uint8_t bit);
    
    /*! @brief    Draws single canvas pixel in multi-color mode
     *  @details  The left of the two color bits determines whether
     *            drawForegroundPixel or drawBackgroundPixel is used.
     *            Uses the drawing colors that are setup by loadColors(). 
     */
    void setMultiColorPixel(unsigned pixelnr, uint8_t two_bits);
    
    /*! @brief    Draws single sprite pixel in single-color mode
     *  @details  Uses the drawing colors that are setup by updateSpriteColors 
     */
    void setSingleColorSpritePixel(unsigned spritenr, unsigned pixelnr, uint8_t bit);
    
    /*! @brief    Draws single sprite pixel in multi-color mode
     *  @details  Uses the drawing colors that are setup by updateSpriteColors 
     */
    void setMultiColorSpritePixel(unsigned spritenr, unsigned pixelnr, uint8_t two_bits);

    /*! @brief    Draws a single sprite pixel
     *  @details  This function is invoked by setSingleColorSpritePixel() and
     *            setMultiColorSpritePixel(). It takes care of collison and invokes
     *            setSpritePixel(4) to actually render the pixel.
     */
    void drawSpritePixel(unsigned pixelnr, uint8_t color, int nr);
    
    
    //
    // Low level drawing (pixel buffer access)
    //
    
public:

    //! @brief    Draws the frame pixels for a certain range
    void drawFramePixels(unsigned first, unsigned last, uint8_t color);

    //! @brief    Draws a single frame pixel
    void drawFramePixel(unsigned nr, uint8_t color) { drawFramePixels(nr, nr, color); }

    //! @brief    Draws all eight frame pixels of a single cycle
    void drawFramePixels(uint8_t color) { drawFramePixels(0, 7, color); }

    //! @brief    Draw a single foreground pixel
    void drawForegroundPixel(unsigned pixelnr, uint8_t color);
    
    //! @brief    Draw a single foreground pixel
    //! @deprecated
    // void setForegroundPixel(unsigned pixelnr, int rgba);

    //! @brief    Draw a single background pixel
    void drawBackgroundPixel(unsigned pixelNr, uint8_t color);

    //! @brief    Draw a single background pixel
    //! @deprecated
    // void setBackgroundPixel(unsigned pixelnr, int rgba);

    //! @brief    Draw eight background pixels in a row
    void drawEightBackgroundPixels(uint8_t color) {
        for (unsigned i = 0; i < 8; i++) drawBackgroundPixel(i, color); }

    //! @brief    Draw eight background pixels in a row
    //! @deprecated
    /*
    void setEightBackgroundPixels(int rgba) {
        for (unsigned i = 0; i < 8; i++) setBackgroundPixel(i, rgba); }
    */
    
    //! @brief    Draw a single sprite pixel
    void putSpritePixel(unsigned pixelnr, uint8_t color, int depth, int source);
    
    //! @brief    Draw a single sprite pixel
    //! @deprecated
    // void setSpritePixel(unsigned pixelnr, int rgba, int depth, int source);

    /*! @brief    Copies eight synthesized pixels into to the pixel buffer.
     *  @details  Each pixel is first translated to the corresponding RGBA value
     *            and then copied over.
     */
    void copyPixels();
    
    /*! @brief    Extend border to the left and right to look nice.
     *  @details  This functions replicates the color of the leftmost and
     *            rightmost pixel
     */
    void expandBorders();

    /*! @brief    Draw a horizontal colored line into the screen buffer
     *  @details  This method is utilized for debugging purposes, only.
     */
    void markLine(uint8_t color, unsigned start = 0, unsigned end = NTSC_PIXELS);

