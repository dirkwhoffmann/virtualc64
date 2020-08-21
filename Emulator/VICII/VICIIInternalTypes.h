// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _VICII_INTERNAL_TYPES_H
#define _VICII_INTERNAL_TYPES_H

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

#endif
