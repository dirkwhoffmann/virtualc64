// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef C64CONFIG_H
#define C64CONFIG_H

// Snapshot version number
#define V_MAJOR 3
#define V_MINOR 3
#define V_SUBMINOR 0

// Uncomment these settings in a release build
// #define RELEASEBUILD
// #define NDEBUG


// Default debug level for all components (Set to 1 in release build)
#define DEBUG_LEVEL 1

#endif 

// RELEASE NOTES (Version 3.4)
//
// Added support for the MikroAss CRT
// Added support for the ISEPIC CRT
// Added more Rom fingerprints
// Added support for iNNEXT gamepads
// Merged in reSID code from VICE 3.4
//
//
//
// CLEANUP:
//
// OPTIMIZATION:
//
// Check, how -Ofast compares to -O3
// Check how USE_OPTIMIZATION_PROFILE = true influences runtime
//
// 
// Update IEC bus inside CIA and VIA. Use delay flags if neccessary
// Use a simpler implementation for the raster irq trigger. Edge sensitive matching value
// Call CA1 action in VIA class only if the pin value really has changed.
//
// Add setter API for SID stuff
//

