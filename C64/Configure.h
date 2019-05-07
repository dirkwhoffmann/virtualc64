/*!
 * @header      Configure.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/*
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

#ifndef CONFIGURE_H
#define CONFIGURE_H

// Snapshot version number of this release
#define V_MAJOR 3
#define V_MINOR 3
#define V_SUBMINOR 0

// Disable assertion checking (Uncomment in release build)
#define NDEBUG

// Default debug level for all components (Set to 1 in release build)
#define DEBUG_LEVEL 1

#endif 

// RELEASE NOTES (Version 3.3.1)
//
// New code base is compatible with XCode 10.2.
// Fixed several bugs that caused random crashes when eclosing an emulator window.
// Gamepad events prevent the Mac from going into sleep mode now.
// The lists of recently inserted URLs are shared across multiple documents.
//
//
// TODOs for the next release:
//
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

