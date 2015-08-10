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

// TODO:
// 1. Introduce PixelEngine
//    Sub component to synthesize pixels
//    Will contain drawingContents stuff, pixelBuffers, renderRoutines etc.
//
// 3. Make sprite drawing cycle based.
// 4. Replace pixel buffers by 8 bit variables and implement mixer(). This makes z buffering obsolete.

#ifndef _PIXELENGINGE_INC
#define _PIXELENGINGE_INC

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
};

    
#endif