/*!
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
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
#ifndef _VIC_INC
#define _VIC_INC

#include "VirtualComponent.h"
#include "C64_types.h"
#include "TimeDelayed.h"
#include "PixelEngine.h"

#define SPR0 0x01
#define SPR1 0x02
#define SPR2 0x04
#define SPR3 0x08
#define SPR4 0x10
#define SPR5 0x20
#define SPR6 0x40
#define SPR7 0x80

#define VICTriggerIrq0     (1ULL << 0) // Sets the IRQ line
#define VICTriggerIrq1     (1ULL << 1)
#define VICReleaseIrq0     (1ULL << 2) // Clears the IRQ line
#define VICReleaseIrq1     (1ULL << 3)
#define VICLpTransition0   (1ULL << 4) // Triggers a lightpen event

#define VICClearanceMask ~((1ULL << 5) | VICTriggerIrq0 | VICReleaseIrq0 | VICLpTransition0)

// Forward declarations
class C64Memory;
class PixelEngine; 

/*! @brief    Virtual Video Controller (VICII)
 *  @details  VICII is the video controller chip of the Commodore 64.
 *            It occupies the memory mapped I/O space from address 0xD000 to 0xD02E. 
 */
class VIC : public VirtualComponent {

    friend PixelEngine;
    friend C64Memory;
    
private:
    
    //! @brief    Selected chip model
    VICChipModel chipModel;
    
    /*! @brief    Reference to the attached pixel engine (PE). 
     *  @details  The PE encapsulates all drawing related routines.
     */
    PixelEngine pixelEngine;

    /*! @brief    Event pipeline
     *  @details  If a time delayed event needs to be performed, a flag is set
     *            inside this variable and executed at the beginning of the next
     *            cycle.
     *  @see      processDelayedActions()
     */
    uint64_t delay;
    
    //! @brief    Main pixel engine pipe
    PixelEnginePipe p;
    
 

public:
    
    //! @brief    Indicates if the gray dot bug should be emulated
    /*! @note     The gray mode bug only affects the newer VICII models 856x
     */
    bool emulateGrayDotBug;
    
    /*! @brief    Current value of the LP pin
     *  @details  A negative transition on this pin triggers a lightpen interrupt.
     */
     bool lp;
     
    /*! @brief    Address bus
     *  @details  Whenever VIC performs a memory read, the generated memory
     *            address is stored in this variable.
     */
    uint16_t addrBus;
    
    /*! @brief    Data bus
     *  @details  Whenever VIC performs a memory read, the result is stored
     *            in this variable.
     */
    uint8_t dataBus;
    
    //
    // Registers
    //
    
    //! @brief    Control register 1 (D011)
    TimeDelayed<uint8_t> control1 = TimeDelayed<uint8_t>(2);
    
    //! @brief    Control register 2 (D016)
    TimeDelayed<uint8_t> control2 = TimeDelayed<uint8_t>(2);
    
    //! @brief    Border color register (D020)
    TimeDelayed<uint8_t> borderColor = TimeDelayed<uint8_t>(2);
    
    //! @brief    Background color registers (D021 - D024)
    TimeDelayed<uint8_t> bgColor[4] = {
        TimeDelayed<uint8_t>(2),
        TimeDelayed<uint8_t>(2),
        TimeDelayed<uint8_t>(2),
        TimeDelayed<uint8_t>(2)
    };
    
    //! @brief    Sprite extra color 1 ($D025)
    TimeDelayed<uint8_t> sprExtraColor1 = TimeDelayed<uint8_t>(2);
    
    //! @brief    Sprite extra color 2 ($D026)
    TimeDelayed<uint8_t> sprExtraColor2 = TimeDelayed<uint8_t>(2);
    
    //! @brief    Sprite colors  color registers ($D027 - $D02E)
    TimeDelayed<uint8_t> sprColor[8] = {
        TimeDelayed<uint8_t>(2),
        TimeDelayed<uint8_t>(2),
        TimeDelayed<uint8_t>(2),
        TimeDelayed<uint8_t>(2),
        TimeDelayed<uint8_t>(2),
        TimeDelayed<uint8_t>(2),
        TimeDelayed<uint8_t>(2),
        TimeDelayed<uint8_t>(2)
    };
    
    
    
    
    //! @brief    Interrupt Request Register ($D019)
    uint8_t irr;

    //! @brief    Interrupt Mask Register ($D01A)
    uint8_t imr;

    //! @brief    True if the current rasterline belongs to the VBLANK area.
    bool vblank;
    
	//! @brief    Internal VIC register, 10 bit video counter
	uint16_t registerVC;
	
	//! @brief    Internal VIC-II register, 10 bit video counter base
	uint16_t registerVCBASE; 
	
	//! @brief    Internal VIC-II register, 3 bit row counter
	uint8_t registerRC;
	
	//! @brief    Internal VIC-II register, 6 bit video matrix line index
	uint8_t registerVMLI; 

    //! @brief    Internal x counter of the sequencer (sprite coordinate system)
    uint16_t xCounter;
    
    /*! @brief    Rasterline counter
     *  @details  The rasterline counter is usually incremented in cycle 1. The
     *            only exception is the overflow condition which is handled in
     *            cycle 2.
     */
    uint32_t yCounter;
    
    /*! @brief    Vertical frame flipflop set condition
     *  @details  Indicates whether the vertical frame flipflop needs to be set
     *            in the current rasterline.
     */
    bool verticalFrameFFsetCond;
    
    /*! @brief    Vertical frame flipflop clear condition
     *  @details  Indicates whether the vertical frame ff needs to be cleared in
     *            the current rasterline.
     */
    bool verticalFrameFFclearCond;

    /*! @brief    DRAM refresh counter
     *  @details  "In jeder Rasterzeile führt der VIC fünf Lesezugriffe zum Refresh des
     *             dynamischen RAM durch. Es wird ein 8-Bit Refreshzähler (REF) zur Erzeugung
     *             von 256 DRAM-Zeilenadressen benutzt." [C.B.] 
     */
    uint8_t refreshCounter;
    
    //! @brief    Display mode in latest gAccess
    uint8_t gAccessDisplayMode;
    
    //! @brief    Foreground color fetched in latest gAccess
    uint8_t gAccessfgColor;

    //! @brief    Background color fetched in latest gAccess
    uint8_t gAccessbgColor;

	//! @brief    True if we are currently processing a DMA line (bad line)
	bool badLineCondition;
	
	/*! @brief    True, if DMA lines can occurr within the current frame.
     *  @details  Bad lines can occur only if the DEN bit was set during an
     *            arbitary cycle in rasterline 30. The DEN bit is located in
     *            control register 1 (0x11).
     */
    bool DENwasSetInRasterline30;

	/*! @brief    Display State
	 *  @details  The VIC is either in idle or display state
     */
	bool displayState;

	/*! @brief    Current value of the BA line
	 *  @details  Remember: Each CPU cycle is split into two phases:
     *            First phase (LOW):   VIC gets access to the bus
     *            Second phase (HIGH): CPU gets access to the bus
     *            In rare cases, VIC needs access in the HIGH phase, too.
     *            To block the CPU, the BA line is pulled down.
     *  @note     BA can be pulled down by multiple sources (wired AND).
     */
    uint16_t BAlow;
	
    //! @brief    Remember at which cycle BA line has been pulled down
    uint64_t BAwentLowAtCycle;
    
    //! @brief    Increases the X counter by 8
    void countX() { xCounter += 8; }
    
    //! @brief    True if yCounter needs to be reset to 0 in this rasterline.
    bool yCounterOverflow();

    /*! @brief    True if a cAccess can occur.
     *  @note     A cAccess can only be performed is BA line is down for more
     *            than 2 cycles.
     */
    bool BApulledDownForAtLeastThreeCycles();
    
    /* "Der VIC benutzt zwei Flipflops, um den Rahmen um das Anzeigefenster
        herum zu erzeugen: Ein Haupt-Rahmenflipflop und ein vertikales
        Rahmenflipflop. [...]

        Die Flipflops werden nach den folgenden Regeln geschaltet:
     
        1. Erreicht die X-Koordinate den rechten Vergleichswert, wird das
           Haupt-Rahmenflipflop gesetzt.
        2. Erreicht die Y-Koordinate den unteren Vergleichswert in Zyklus 63, wird
           das vertikale Rahmenflipflop gesetzt.
        3. Erreicht die Y-Koordinate den oberern Vergleichswert in Zyklus 63 und
           ist das DEN-Bit in Register $d011 gesetzt, wird das vertikale
           Rahmenflipflop gelöscht.
        4. Erreicht die X-Koordinate den linken Vergleichswert und die Y-Koordinate
           den unteren, wird das vertikale Rahmenflipflop gesetzt.
        5. Erreicht die X-Koordinate den linken Vergleichswert und die Y-Koordinate
           den oberen und ist das DEN-Bit in Register $d011 gesetzt, wird das
           vertikale Rahmenflipflop gelöscht.
        6. Erreicht die X-Koordinate den linken Vergleichswert und ist das
           vertikale Rahmenflipflop gelöscht, wird das Haupt-Flipflop gelöscht." [C.B.]
     */

    /*! @brief    Takes care of the vertical frame flipflop value.
     *  @details  Invoked in each VIC II cycle 
     */
    void checkVerticalFrameFF();
    
    //! @brief    Checks frame fliplops at left border
    void checkFrameFlipflopsLeft(uint16_t comparisonValue);

    //! @brief    Checks frame fliplops at right border
    void checkFrameFlipflopsRight(uint16_t comparisonValue);

    //! @brief    Returns in which cycle the frame flipflop is checked for the left border
    uint16_t leftComparisonValue() { return isCSEL() ? 24 : 31; }

    //! @brief    Returns in which cycle the frame flipflop is checked for the right border
    uint16_t rightComparisonValue() { return isCSEL() ? 344 : 335; }

    //! @brief    Returns in which cycle the frame flipflop is checked for the upper border
    uint16_t upperComparisonValue() { return isRSEL() ? 51 : 55; }
    
    //! @brief    Returns in which cycle the frame flipflop is checked for the lower border
    uint16_t lowerComparisonValue() { return isRSEL() ? 251 : 247; }
    
	/*! @brief    Clear main frame flipflop
     *  @details  "Das vertikale Rahmenflipflop dient zur Unterstützung bei der Darstellung
     *             des oberen/unteren Rahmens. Ist es gesetzt, kann das Haupt-Rahmenflipflop
     *             nicht gelöscht werden." [C.B.] 
     */
    void clearMainFrameFF() { if (!p.verticalFrameFF && !verticalFrameFFsetCond) p.mainFrameFF = false; }
     
    
	//
	// I/O memory handling and RAM access
	//

public:
	
	/*! @brief    I/O Memory
	 *  @details  This array is used to store most of the register values that are poked into
     *            the VIC address space. Note that this does not hold for all register values.
     *            Some of them are directly stored inside the state pipe for speedup purposes.
     *  @deprecated
     */
	uint8_t iomem[64]; 



private:

    /*! @brief    Start address of the currently selected memory bank
     *  @details  There are four banks in total since the VIC chip can only "see" 16 KB
     *            of memory at the same time. Two bank select bits in the CIA I/O space
     *            determine which quarter of memory is currently seen.
     *
     */
    /*
     *            +-------+------+-------+----------+-------------------------------------+
     *            | VALUE | BITS |  BANK | STARTING |  VIC-II CHIP RANGE                  |
     *            |  OF A |      |       | LOCATION |                                     |
     *            +-------+------+-------+----------+-------------------------------------+
     *            |   0   |  00  |   3   |   49152  | ($C000-$FFFF)                       |
     *            |   1   |  01  |   2   |   32768  | ($8000-$BFFF)                       |
     *            |   2   |  10  |   1   |   16384  | ($4000-$7FFF)                       |
     *            |   3   |  11  |   0   |       0  | ($0000-$3FFF) (DEFAULT VALUE)       |
     *            +-------+------+-------+----------+-------------------------------------+
     */
    uint16_t bankAddr;
    
    /*! @brief    cAcess character storage
     *  @details  Every 8th rasterline, the VIC chips performs a DMA access and fills this
     *            array with character information.
     */
    uint8_t characterSpace[40];
    
    /*! @brief    cAcess color storage
     *  @details  Every 8th rasterline, the VIC chips performs a DMA access and fills
     *            the array with color information.
     */
    uint8_t colorSpace[40];
    
    //! @brief    Bit i is set to 1 iff sprite i performs its first DMA in the current cycle
    uint8_t isFirstDMAcycle;

    //! @brief    Bit i is set to 1 iff sprite i performs its second and third DMA in the current cycle
    uint8_t isSecondDMAcycle;

    
    //
    // Memory refresh accesses (rAccess)
    //
    
    //! @brief    Performs a DRAM refresh
    void rAccess() { (void)memAccess(0x3F00 | refreshCounter--); }
    
    //! @brief    Performs a DRAM idle access
    void rIdleAccess() { (void)memIdleAccess(); }
    

	//
	// Sprites
	//

	/*! @brief    MOB data counter.
	 *  @details  A 6 bit counter, one for each sprite.
     */
	uint8_t mc[8];
	
	/*! @brief    MCBASE register.
	 *  @details  A 6 bit counter, one register for each sprite.
     */
	uint8_t mcbase[8];
		
	/*! @brief    Sprite pointer fetched during a pAccess.
	 *  @details  Determines where the sprite data comes from.
     */
	uint16_t spritePtr[8];

	/*! @brief    Sprite on off register
	 *  @details  Determines if a sprite needs to be drawn in the current rasterline. 
     *            Each bit represents a single sprite.
     *  @deprecated Use TimeDelayed register instead
     */
    TimeDelayed<uint8_t> spriteOnOff = TimeDelayed<uint8_t>(3);
    
	/*! @brief    Sprite DMA on off register
	 *  @details  Determines  if sprite dma access is enabled or disabled. 
     *            Each bit represents a single sprite. 
     */
	uint8_t spriteDmaOnOff;
    
	/*! @brief    Expansion flipflop
	 *  @details  Used to handle Y sprite stretching, one bit for each sprite 
     */
	uint8_t expansionFF;

    /*! @brief    Remembers which bits the CPU has cleared in the expansion Y register (D017)
     *  @details  This value is set in pokeIO and cycle 15 and read in cycle 16 
     */
    uint8_t cleared_bits_in_d017;
    
    //! Sprite colors (same for all sprites)
    //! @deprecated
    // uint8_t spriteColor[8];

    //! Sprite extra color 1 (same for all sprites)
    //! @deprecated
    // uint8_t spriteExtraColor1;

    //! Sprite extra color 2 (same for all sprites)
    //! @deprecated
    // uint8_t spriteExtraColor2;
    
    
	//
	// Lightpen
	//
	
	/*! @brief    Indicates whether the lightpen has triggered
	 *  @details  This variable indicates whether a lightpen interrupt has
     *            occurred within the current frame. The variable is needed,
     *            because a lightpen interrupt can only occur once per frame.
     */
	bool lightpenIRQhasOccured;
	
    
    //
    // Color management
    //
    
    //! @brief    User adjustable brightness value used in palette computation
    /*! @details  Value may range from 0.0 to 100.0
     */
    double brightness = 50.0;

    //! @brief    User adjustable contrast value used in palette computation
    /*! @details  Value may range from 0.0 to 100.0
     */
    double contrast = 100.0;

    //! @brief    User adjustable saturation value used in palette computation
    /*! @details  Value may range from 0.0 to 100.0
     */
    double saturation = 50.0;

    
	//
	// Debugging
	//
	
	/*! @brief    Determines whether sprites are drawn or not
	 *  @details  During normal emulation, the value is always true. For
     *            debugging purposes, the value can be set to false. In this
     *            case, sprites are no longer drawn.
	 */
	bool drawSprites;
	
	/*! @brief    Enables sprite-sprite collision
	 *  @details  If set to true, the virtual VIC chips checks for sprite-sprite
     *            collision as the original C64 does. For debugging purposes and
     *            cheating, collision detection can be disabled by setting the
     *            variabel to false. Collision detection can be enabled or
     *            disabled for each sprite seperately. Each bit is dedicated to
     *            a single sprite.
     */
	uint8_t spriteSpriteCollisionEnabled;
	
	/*! @brief    Enable sprite-background collision
	 *  @details  If set to true, the virtual VIC chips checks for sprite-
     *            background collision as the original C64 does. For debugging
     *            purposes and cheating, collision detection can be disabled by
     *            setting the variabel to false. Collision detection can be
     *            enabled or disabled for each sprite seperately. Each bit is
     *            dedicated to a single sprite.
     */
	uint8_t spriteBackgroundCollisionEnabled;
	
	/*! @brief    Determines whether IRQ lines will be made visible.
	 *  @details  Each rasterline that will potentially trigger a raster IRQ is
     *            highlighted. This feature is useful for debugging purposes as
     *            it visualizes how the screen is divided into multiple parts.
     */
	bool markIRQLines;
	
	/*! @brief    Determines whether DMA lines will be made visible.
	 *  @details  Each rasterline in which the vic will read additional data
     *            from the memory and stun the CPU is made visible. Note that
     *            partial DMA lines may not appear. 
     */
	bool markDMALines;

	
	//
	// Methods
	//

public:
	
	//! @brief    Constructor
	VIC();
	
	//! @brief    Destructor
	~VIC();
	
	//! @brief    Methods from VirtualComponent
	void reset();
    void setC64(C64 *c64);
    void ping();
	void dumpState();
    size_t stateSize();
    void loadFromBuffer(uint8_t **buffer);
    void saveToBuffer(uint8_t **buffer);
    
    //! @brief    Gathers debug information.
    VICInfo getInfo();

    //! @brief    Gathers debug information about a certain sprite.
    SpriteInfo getSpriteInfo(unsigned i);

    
    //
    //! @functiongroup Accessing chip model related properties
    //
    
    //! @brief    Returns the currently plugged in chip model.
    VICChipModel getChipModel() { return chipModel; }
    
    //! @brief    Sets the chip model.
    void setChipModel(VICChipModel model);
    
    //! @brief    Returns true if a PAL chip is plugged in.
    bool isPAL() { return chipModel & (PAL_6569_R1 | PAL_6569_R3 | PAL_8565); }
    
    //! @brief    Returns true if a NTSC chip is plugged in.
    bool isNTSC() { return chipModel & (NTSC_6567 | NTSC_6567_R56A | NTSC_8562); }

    //! @brief    Returns true if a newer MOS 856x chip is plugged in.
    bool is856x() { return chipModel & (PAL_8565 | NTSC_8562); }
    
    //! @brief    Returns true if an older MOS 656x chip is plugged in.
    bool is656x() { return chipModel & ~(PAL_8565 | NTSC_8562); }

    //! @brief    Returns true if the emulated chip model has the gray dot bug.
    bool hasGrayDotBug() { return is856x(); }

    //! @brief    Returns true if light pen interrupts are triggered with a delay.
    bool delayedLightPenIrqs() { return chipModel & (PAL_6569_R1 | NTSC_6567_R56A); }

    //! @brief    Returns the clock frequencay of the selected VICII model.
    unsigned getClockFrequency();
    
    //! @brief    Returns the number of CPU cycles performed per rasterline.
    unsigned getCyclesPerRasterline();
    
    //! @brief    Returns true if the end of the rasterline has been reached.
    bool isLastCycleInRasterline(unsigned cycle);
    
    //! @brief    Returns the number of rasterlines drawn per frame.
    unsigned getRasterlinesPerFrame();
    
    //! @brief    Returns true if rasterline belongs to the VBLANK area.
    bool isVBlankLine(unsigned rasterline);
    
    //! @brief    Returns the number of CPU cycles executed in one frame.
    unsigned getCyclesPerFrame() {
        return getRasterlinesPerFrame() * getCyclesPerRasterline(); }
    
    /*! @brief    Returns the number of frames drawn per second.
     *  @note     The result is returned as a floating point value, because
     *            Commodore did not manage to match the expected values exactly
     *            (50 Hz for PAL and 60 Hz for NTSC). E.g., a PAL C64 outputs
     *            50.125 Hz.
     */
    double getFramesPerSecond() {
        return (double)getClockFrequency() / (double)getCyclesPerFrame();
    }
    
    //! @brief    Returns the time interval between two frames in nanoseconds.
    uint64_t getFrameDelay() {
        return 1000000000.0 / (uint64_t)getFramesPerSecond();
    }
    
    
    //
    //! @functiongroup Accessing the screen buffer and display properties
    //
    
    //! @brief    Returns the currently stabel screen buffer.
    void *screenBuffer() { return pixelEngine.screenBuffer(); }

    /*! @brief    Returns one of the sixteen C64 colors in RGBA format.
     *  @seealso  updateColors
     */
    uint32_t getColor(unsigned nr);
    
    //! @brief    Returns the brightness monitor parameter
    double getBrightness() { return brightness; }
    
    //! @brief    Sets the brightness monitor parameter
    void setBrightness(double value);
    
    //! @brief    Returns the contrast monitor parameter
    double getContrast() { return contrast; }
    
    //! @brief    Sets the contrast monitor parameter
    void setContrast(double value);
    
    //! @brief    Returns the saturation monitor parameter
    double getSaturation() { return saturation; }
    
    //! @brief    Sets the saturation monitor parameter
    void setSaturation(double value);
    
private:
    
    /*! @brief    Updates the RGBA values for all sixteen C64 colors.
     *! @details  The base palette is determined by the selected VICII model.
     */
    void updatePalette();
    

	//
	//! @functiongroup Getter and setter
	//

public:
    
    

    
	

			
	/*! @brief    Returns the screen memory address
     *  @deprecated Use getInfo() instead
     */
	uint16_t getScreenMemoryAddr();
	
    /*! @brief    Sets the screen memory address
     *  @note     This function is not needed internally and only invoked by the GUI debug panel
     */
	void setScreenMemoryAddr(uint16_t addr);
		
    /*! @brief    Returns the character memory address
     *  @deprecated Use getInfo() instead
     */
	uint16_t getCharacterMemoryAddr();
	
    /*! @brief    Sets the character memory address
     *  @note     This function is not needed internally and only invoked by the GUI debug panel
     */
	void setCharacterMemoryAddr(uint16_t addr);
    
    
    //
    //! @functiongroup Accessing memory
    //

public:
    
    //! @brief    Peeks a value from a VIC register without causing side effects.
    uint8_t spypeek(uint16_t addr);
    
private:

    //! @brief    Peeks a value from a VIC register.
	uint8_t peek(uint16_t addr);
    
    //! @brief    Pokes a value into a VIC register.
	void poke(uint16_t addr, uint8_t value);
 
    //! @brief    POkes a value into one of the 15 color registers.
    void pokeColorReg(uint16_t addr, uint8_t value);
    
    //! @brief    Simulates a memory access via the address and data bus.
    uint8_t memAccess(uint16_t addr);
    
    //! @brief    Simulates an idle memory access.
    uint8_t memIdleAccess();
    
    /*! @brief    Performs a character access (cAccess).
     *  @details  During a cAccess, the video matrix is read.
     */
    void cAccess();
    
    /*! @brief    Performs a graphics access (gAccess).
     *  @details  During a gAccess, graphics data (character or bitmap patterns)
     *            is reads. The result of the gAccess is stored in variables
     *            prefixed with 'g_', i.e., g_data, g_character, g_color, and
     *            g_mode.
     */
    void gAccess();
    
    //! @brief    Performs a sprite pointer access (pAccess).
    void pAccess(unsigned sprite);
    
    //! @brief    Performs the first sprite data access.
    void sFirstAccess(unsigned sprite);
    
    //! @brief    Performs the second sprite data access.
    void sSecondAccess(unsigned sprite);
    
    //! @brief    Performs the third sprite data access.
    void sThirdAccess(unsigned sprite);
    
    /*! @brief    Finalizes the sprite data access
     *  @details  This method is invoked one cycle after the second and third
     *            sprite DMA has occured.
     */
    void sFinalize(unsigned sprite);
    
    
	//
	// Properties
	//
	
public:
		
    //! @brief    Returns the current value of the DEN bit (Display Enabled).
    bool DENbit() { return GET_BIT(control1.current(), 4); }

    //! @brief    Returns the current value of the BMM bit (Bit Map Mode).
    bool BMMbit() {
        return is856x() ?
        GET_BIT(control1.delayed(), 5) :
        GET_BIT(control1.delayed(), 5) | GET_BIT(control1.current(), 5);
    }
    
    //! @brief    Returns the value of the BMM bit in the previous cycle.
    bool BMMbitInPreviousCycle() { return GET_BIT(control1.delayed(), 5); }

    //! @brief    Returns the current value of the ECM bit (Extended Character Mode).
    bool ECMbit() {
        return is856x() ?
        GET_BIT(control1.delayed(), 6) :
        GET_BIT(control1.current(), 6);
    }

    //! @brief    Returns the masked CB13 bit (controls memory access).
    uint8_t CB13() { return iomem[0x18] & 0x08; }

    //! @brief    Returns the masked CB13/CB12/CB11 bits (controls memory access).
    uint8_t CB13CB12CB11() { return iomem[0x18] & 0x0E; }

    //! @brief    Returns the masked VM13/VM12/VM11/VM10 bits (controls memory access).
    uint8_t VM13VM12VM11VM10() { return iomem[0x18] & 0xF0; }

	//! @brief    Returns the state of the CSEL bit.
	bool isCSEL() { return GET_BIT(control2.current(), 3); }
    
	//! @brief    Returns the state of the RSEL bit.
    bool isRSEL() { return GET_BIT(control1.current(), 3); }

	/*! @brief    Returns the current display mode.
	 *  @details  The display mode is determined by bits 5 and 6 of control
     *            register 1 and bit 4 of control register 2.
     *  @todo     Move to debug section
     */
    DisplayMode getDisplayMode() {
        return (DisplayMode)((control1.current() & 0x60) | (control2.current() & 0x10));
    }
    
	//! @brief    Sets the display mode.
    void setDisplayMode(DisplayMode m);
	
	//! @brief    Returns the current screen geometry.
	ScreenGeometry getScreenGeometry(void);
	
	//! @brief    Sets the current screen geometry.
	void setScreenGeometry(ScreenGeometry mode);
	
	//! @brief    Returns the number of rows to be drawn (24 or 25).
    int numberOfRows() { return GET_BIT(control1.current(), 3) ? 25 : 24; }
	
	//! @brief    Sets the number of rows to be drawn (24 or 25).
    void setNumberOfRows(int rs) {
        assert(rs == 24 || rs == 25);
        uint8_t value = control1.current() & 0xFF;
        WRITE_BIT(value, 3, rs == 25);
        control1.write(value);
    }
	
	//! @brief    Returns the number of columns to be drawn (38 or 40).
    int numberOfColumns() { return GET_BIT(control2.current(), 3) ? 40 : 38; }

	//! @brief    Sets the number of columns to be drawn (38 or 40).
	void setNumberOfColumns(int cs) {
        assert(cs == 38 || cs == 40);
        uint8_t value = control2.current() & 0xFF;
        WRITE_BIT(value, 3, cs == 40);
        control2.write(value);
    }
    
	/*! @brief    Returns the vertical raster scroll offset (0 to 7).
	 *  @details  The vertical raster offset is usally used by games for
     *            smoothly scrolling the screen.
     */
    uint8_t getVerticalRasterScroll() { return control1.current() & 0x07; }
	
	//! @brief    Sets the vertical raster scroll offset (0 to 7).
    void setVerticalRasterScroll(uint8_t offset) {
        assert(offset < 8);
        uint8_t value = (control1.current() & 0xF8) | (offset & 0x07);
        control1.write(value);
    }
    
	/*! @brief    Returns the horizontal raster scroll offset (0 to 7).
	 *  @details  The vertical raster offset is usally used by games for
     *            smoothly scrolling the screen.
     */
    uint8_t getHorizontalRasterScroll() { return control2.current() & 0x07; }
	
	//! @brief    Sets the horizontan raster scroll offset (0 to 7).
    void setHorizontalRasterScroll(uint8_t offset) {
        assert(offset < 8);
        uint8_t value = (control2.current() & 0xF8) | (offset & 0x07);
        control2.write(value);
   }
    


	//
	// DMA lines, BA signal and IRQs
	//

private:
    
    /*! @brief    Set to true in cycle 1, cycle 63 (65) iff yCounter matches D012
     *  @details  Variable is needed to determine if a rasterline should be
     *            issued in cycle 1 or 2.
     *  @deprecates Will be replaced by rasterlineMatchesIrqLine
     */
    bool yCounterEqualsIrqRasterline;
    
    /*! @brief    Update bad line condition
     *  @details  "Ein Bad-Line-Zustand liegt in einem beliebigen Taktzyklus vor, wenn an der
     *             negativen Flanke von ¯0 zu Beginn des
     *             [1] Zyklus RASTER >= $30 und RASTER <= $f7 und
     *             [2] die unteren drei Bits von RASTER mit YSCROLL übereinstimmen
     *             [3] und in einem beliebigen Zyklus von Rasterzeile $30 das DEN-Bit gesetzt war." [C.B.] 
     */
    void updateBadLineCondition() {
        badLineCondition =
            yCounter >= 0x30 && yCounter <= 0xf7 /* [1] */ &&
            (yCounter & 0x07) == getVerticalRasterScroll() /* [2] */ &&
            DENwasSetInRasterline30 /* [3] */;
    }
    
    /*! @brief    Update display state
     *  @details  Invoked at the end of each VIC cycle
     */
    void updateDisplayState() { if (badLineCondition) displayState = true; }
    
    //! @brief    Set BA line
    void setBAlow(uint8_t value);
	
	/*! @brief    Triggers a VIC interrupt
	 *  @param    source Interrupt source (1, 2, or 4)
     *            1 : Rasterline interrupt
     *            2 : Collision of a sprite with background pixels
     *            4 : Collision between two sprites.
     *            8 : Lightpen interrupt
     *            cycleDelay lets you postpone the interrupt by up to 1 cycle.
     */
	void triggerIRQ(uint8_t source, unsigned cycleDelay = 0);
	
    //! @brief    Triggers a VIC interrupt delayed by one cycle
    void triggerDelayedIRQ(uint8_t source) { triggerIRQ(source, 1); }
    
public: 
	
	/*! @brief    Returns next interrupt rasterline
     *  @details  In line 0, the interrupt is triggered in cycle 2. In all other lines,
     *            it is triggered in cycle 1.
     */
    uint16_t rasterInterruptLine() { return ((control1.current() & 0x80) << 1) | iomem[0x12]; }
    
	//! @brief    Set interrupt rasterline
    void setRasterInterruptLine(uint16_t line) {
        iomem[0x12] = line & 0xFF;
        if (line > 0xFF) {
            control1.write(control1.current() | 0x80);
        } else {
            control1.write(control1.current() & 0x7F);
        }
    }

	//! @brief    Returns true, iff rasterline interrupts are enabled
    bool rasterInterruptEnabled() { return GET_BIT(imr, 1); }

	//! @brief    Enable or disable rasterline interrupts
    void setRasterInterruptEnable(bool b) { WRITE_BIT(imr, 1, b); }
	
	//! @brief    Enable or disable rasterline interrupts
    void toggleRasterInterruptFlag() { TOGGLE_BIT(imr, 1); }
	
    //! @brief    Method from Hoxs64
    //! @details  Used to determine X coordinate when a lightpen interrupt takes place
    //! @deprecated Use lightpenX(), lightpenY()
    uint16_t vicXPosFromCycle(uint8_t cycle, uint16_t offset);
    
    //! @brief    Returns the X coordinate of a light pen event.
    /*! @details  The coordinate depends on the current rasterline cycle and
     *            differes slightly between the supported VICII chip models.
     */
    uint16_t lightpenX();
    
    //! @brief    Returns the Y coordinate of a light pen event.
    uint16_t lightpenY();
    
	/*! @brief    Sets the value of the LP pin
	 *  @details  The LP pin is connected to bit 4 of control port A.
     *  @seealso  triggerLightpenInterrupt()
     */
	void setLP(bool value);
    
    /*! @brief    Trigger lightpen interrupt if conditions are met.
     *  @details  This function is called on each negative transition of the
     *            LP pin. It latches the x and y coordinates and immediately
     *            triggers an interrupt if a newer VICII model is emulated.
     *            Older models trigger the interrupt later, at the beginning of
     *            a new frame.
     *  @seealso  retriggerLightpenInterrupt()
     */
    void triggerLightpenInterrupt();

    /*! @brief    Retriggers a lightpen interrupt if conditions are met.
     *  @details  This function is called at the beginning of each frame.
     *            If the lp line is still low at this point of time, a lightpen
     *            interrupt is retriggered. Note that older VICII models trigger
     *            interrupts only at this point in time.
     */
    void retriggerLightpenInterrupt();
    
    
	//
	// Sprites
	//

private:

    /*! @brief    Turns off sprite dma if conditions are met.
     *  @details  In cycle 16, the mcbase pointer is advanced three bytes for
     *            all dma enabled sprites. Advancing three bytes means that
     *            mcbase will then point to the next sprite line. When mcbase
     *            reached 63, all 21 sprite lines have been drawn and sprite dma
     *            is switched off. The whole operation is skipped when the y
     *            expansion flipflop is 0. This never happens for normal sprites
     *            (there is no skipping then), but happens every other cycle for
     *            vertically expanded sprites. Thus, mcbase advances for those
     *            sprites at half speed which actually causes the expansion.
     */
    void turnSpriteDmaOff();

    /*! @brief    Turns on sprite dma accesses if drawing conditions are met.
     *  @details  Sprite dma is turned on either in cycle 55 or cycle 56.
     *            Dma is turned on iff it's currently turned off and the
     *            sprite y positon equals the lower 8 bits of yCounter.
     */
    void turnSpriteDmaOn();

    /*! @brief    Toggles expansion flipflop for vertically stretched sprites.
     *  @details  In cycle 56, register D017 is read and the flipflop gets
     *            inverted for all sprites with vertical stretching enabled.
     *            When the flipflop goes down, advanceMCBase() will have no
     *            effect in the next rasterline. This causes each sprite line
     *            to be drawn twice.
     */
    void toggleExpansionFlipflop();
    
	/*! @brief    Gets depth of a sprite.
	 *  @details  The value is written to the z buffer to resolve overlapping
     *            pixels.
     */
    uint8_t spriteDepth(uint8_t nr) {
        return spritePriority(nr) ? (SPRITE_LAYER_BG_DEPTH | nr) : (SPRITE_LAYER_FG_DEPTH | nr); }
	
public: 
	
	//! @brief    Sets the color of a sprite.
    void setSpriteColor(uint8_t nr, uint8_t color);
		
	//! @brief    Returns the X coordinate of a sprite.
    uint16_t getSpriteX(uint8_t nr) { assert(nr < 8); return p.spriteX[nr]; }

	//! @brief    Set the X coordinate of a sprite.
    void setSpriteX(uint8_t nr, uint16_t x) {
        if (x < 512) {
            p.spriteX[nr] = x;
            iomem[2*nr] = x & 0xFF;
            if (x & 0x100) SET_BIT(iomem[0x10],nr); else CLR_BIT(iomem[0x10],nr);
        }
    }
    
	//! @brief    Returns the Y coordinate of a sprite.
    uint8_t getSpriteY(uint8_t nr) { assert(nr < 8); return iomem[1+2*nr]; }

	//! @brief    Sets the Y coordinate of sprite.
    void setSpriteY(uint8_t nr, uint8_t y) { iomem[1+2*nr] = y; }
	
    //! @brief    Compares the Y coordinates of all sprites with an eight bit value.
    uint8_t compareSpriteY(uint8_t y) {
        return
        (iomem[1] == y) |
        ((iomem[3] == y) << 1) |
        ((iomem[5] == y) << 2) |
        ((iomem[7] == y) << 3) |
        ((iomem[9] == y) << 4) |
        ((iomem[11] == y) << 5) |
        ((iomem[13] == y) << 6) |
        ((iomem[15] == y) << 7);
    }
    
	//! @brief    Enables or disables a sprite.
    void setSpriteEnabled(uint8_t nr, bool b) { WRITE_BIT(iomem[0x15], nr, b); }

	//! @brief    Enables or disables a sprite.
    void toggleSpriteEnabled(uint8_t nr) { TOGGLE_BIT(iomem[0x15], nr); }
	    
	/*! @brief    Returns true, iff an interrupt will be triggered when a
     *            sprite/background collision occurs.
     */
    bool irqOnSpriteBackgroundCollision() { return GET_BIT(imr, 1); }

    //! @brief    Enables or disables IRQs on sprite/background collision
    void setIrqOnSpriteBackgroundCollision(bool b) { WRITE_BIT(imr, 1, b); }

    //! @brief    Enables or disables IRQs on sprite/background collision
    void toggleIrqOnSpriteBackgroundCollision() { TOGGLE_BIT(imr, 1); }
    
	/*! @brief    Returns true, iff an interrupt will be triggered when a
     *            sprite/sprite collision occurs.
     */
    bool irqOnSpriteSpriteCollision() { return GET_BIT(imr, 2); }

    //! @brief    Enables or disables IRQs on sprite/sprite collision
    void setIrqOnSpriteSpriteCollision(bool b) { WRITE_BIT(imr, 2, b); }
    
    //! @brief    Enables or disables IRQs on sprite/sprite collision
    void toggleIrqOnSpriteSpriteCollision() { TOGGLE_BIT(imr, 2); }
    
	//! @brief    Returns true, iff a rasterline interrupt has occurred.
    bool rasterInterruptOccurred() { return GET_BIT(irr, 0); }

	//! @brief    Returns true, iff a sprite/background interrupt has occurred.
    bool spriteBackgroundInterruptOccurred() { return GET_BIT(irr, 1); }

	//! @brief    Returns true, iff a sprite/sprite interrupt has occurred.
    bool spriteSpriteInterruptOccurred() { return GET_BIT(irr, 2); }

	//! @brief    Returns true, iff sprites are drawn behind the scenary.
    bool spritePriority(unsigned nr) { assert(nr < 8); return GET_BIT(iomem[0x1B], nr); }

	//! @brief    Determines whether a sprite is drawn before or behind the scenary.
    void setSpritePriority(unsigned nr, bool b) { assert(nr < 8); WRITE_BIT(iomem[0x1B], nr, b); }

	//! @brief    Determines whether a sprite is drawn before or behind the scenary.
    void toggleSpritePriority(unsigned nr) { assert(nr < 8); TOGGLE_BIT(iomem[0x1B], nr); }
	
	//! @brief    Returns true, iff sprite is a multicolor sprite.
    bool spriteIsMulticolor(unsigned nr) { assert(nr < 8); return GET_BIT(iomem[0x1C], nr); }

	//! @brief    Sets single color or multi color mode for sprite.
    void setSpriteMulticolor(unsigned nr, bool b) { assert(nr < 8); WRITE_BIT(iomem[0x1C], nr, b); }

	//! @brief    Switches between single color or multi color mode.
    void toggleMulticolorFlag(unsigned nr) { assert(nr < 8); TOGGLE_BIT(iomem[0x1C], nr); }
		
	//! @brief    Returns true, iff the sprite is vertically stretched.
    bool spriteHeightIsDoubled(unsigned nr) { assert(nr < 8); return GET_BIT(iomem[0x17], nr); }

	//! @brief    Stretches or shrinks a sprite vertically.
    void setSpriteStretchY(unsigned nr, bool b) { assert(nr < 8); WRITE_BIT(iomem[0x17], nr, b); }

	//! @brief    Stretches or shrinks a sprite vertically.
    void spriteToggleStretchYFlag(unsigned nr) { assert(nr < 8); TOGGLE_BIT(iomem[0x17], nr); }

	//! @brief    Returns true, iff the sprite is horizontally stretched.
    bool spriteWidthIsDoubled(unsigned nr) { assert(nr < 8); return GET_BIT(p.spriteXexpand, nr); }

	//! @brief    Stretches or shrinks sprite horizontally.
    void setSpriteStretchX(unsigned nr, bool b) { assert(nr < 8); WRITE_BIT(p.spriteXexpand, nr, b); }

	//! @brief    Stretches or shrinks sprite horizontally.
    void spriteToggleStretchXFlag(unsigned nr) { assert(nr < 8); TOGGLE_BIT(p.spriteXexpand, nr); }

	//! @brief    Returns true, iff sprite collides with another sprite.
    bool spriteCollidesWithSprite(unsigned nr) { assert(nr < 8); return GET_BIT(iomem[0x1E], nr); }

	//! @brief    Returns true, iff sprite collides with background.
    bool spriteCollidesWithBackground(unsigned nr) { assert(nr < 8); return GET_BIT(iomem[0x1F], nr); }

	
	//
	//  Execution functions
	//

public:
	
	/*! @brief    Prepares VIC for drawing a new frame.
	 *  @details  This function is called prior to cycle 1 of rasterline 0.
     */
	void beginFrame();
	
	/*! @brief    Prepares VIC for drawing a new rasterline.
	 *  @details  This function is called prior to cycle 1 at the beginning of each rasterline.
     */
	void beginRasterline(uint16_t rasterline);

	/*! @brief    Finishes up a rasterline.
	 *  @details  This function is called after the last cycle of each rasterline.
     */
	void endRasterline();
	
	/*! @brief    Finishes up a frame.
	 *  @details  This function is called after the last cycle of the last rasterline .
     */
	void endFrame();
	
    //! @brief    Pushes portions of the VIC state into the pixel engine.
    void preparePixelEngine();
    
    //! @brief    Processes all time delayed actions.
    /*! @details  This function is called at the beginning of each VIC cycle.
     */
    void processDelayedActions();
    
	/*! @brief    Executes a specific rasterline cycle
     *  @note     The cycle specific actions differ depending on the selected
     *            chip model.
     */
    void cycle1pal();   void cycle1ntsc();
    void cycle2pal();   void cycle2ntsc();
    void cycle3pal();   void cycle3ntsc();
    void cycle4pal();   void cycle4ntsc();
    void cycle5pal();   void cycle5ntsc();
    void cycle6pal();   void cycle6ntsc();
    void cycle7pal();   void cycle7ntsc();
    void cycle8pal();   void cycle8ntsc();
    void cycle9pal();   void cycle9ntsc();
    void cycle10pal();  void cycle10ntsc();
    void cycle11pal();  void cycle11ntsc();
    void cycle12();
    void cycle13();
    void cycle14();
    void cycle15();
    void cycle16();
    void cycle17();
    void cycle18();
    void cycle19to54();
    void cycle55pal();  void cycle55ntsc();
    void cycle56();
    void cycle57pal();  void cycle57ntsc();
    void cycle58pal();  void cycle58ntsc();
    void cycle59pal();  void cycle59ntsc();
    void cycle60pal();  void cycle60ntsc();
    void cycle61pal();  void cycle61ntsc();
    void cycle62pal();  void cycle62ntsc();
    void cycle63pal();  void cycle63ntsc();
    void cycle64ntsc();
    void cycle65ntsc();
	
    
    
	//
	//! @functiongroup Debugging VICII (VIC_debug.cpp)
	//

public: 

    //
    //! @functiongroup Modifying VICII internals
    //

    //! @brief    Sets the memory bank start address
    void setMemoryBankAddr(uint16_t addr);
    
    
    
    //
    //! @functiongroup Enabling and disabling some VICII features
    //

    //! @brief    Returns true if IRQ lines are colorized.
	bool showIrqLines() { return markIRQLines; }

	//! @brief    Shows or hides IRQ lines.
	void setShowIrqLines(bool show) { markIRQLines = show; }

	//! @brief    Returns true if DMA lines are colorized.
	bool showDmaLines() { return markDMALines; }
	
	//! @brief    Shows or hides DMA lines.
	void setShowDmaLines(bool show) { markDMALines = show; }

	//! @brief    Returns true if sprites are hidden.
	bool hideSprites() { return !drawSprites; }

	//! @brief    Hides or shows sprites.
	void setHideSprites(bool hide) { drawSprites = !hide; }
	
	//! @brief    Returns true if sprite-sprite collision detection is enabled.
	bool getSpriteSpriteCollisionFlag() { return spriteSpriteCollisionEnabled; }

	//! @brief    Enables or disables sprite-sprite collision detection.
    void setSpriteSpriteCollisionFlag(bool b) { spriteSpriteCollisionEnabled = b; };

	//! @brief    Toggles sprite-sprite collision detection.
    void toggleSpriteSpriteCollisionFlag() { spriteSpriteCollisionEnabled = !spriteSpriteCollisionEnabled; }
	
	//! @brief    Returns true if sprite-background collision detection is enabled.
	bool getSpriteBackgroundCollisionFlag() { return spriteBackgroundCollisionEnabled; }

	//! @brief    Enables or disable sprite-background collision detection.
    void setSpriteBackgroundCollisionFlag(bool b) { spriteBackgroundCollisionEnabled = b; }

	//! @brief    Toggles sprite-background collision detection.
    void toggleSpriteBackgroundCollisionFlag() { spriteBackgroundCollisionEnabled = !spriteBackgroundCollisionEnabled; }
};

#endif

