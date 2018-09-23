/*!
 * @header      C64.h
 * @brief       This file is part of VirtualC64.
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2018 Dirk W. Hoffmann
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

#ifndef CONFIGURE_H
#define CONFIGURE_H

// Snapshot version number of this release
#define V_MAJOR 2
#define V_MINOR 6
#define V_SUBMINOR 0

// Disable assertion checking (Uncomment in release build)
// #define NDEBUG

// Default debug level for all components (Set to 1 in release build)
#define DEBUG_LEVEL 1

#endif 



// RELEASE NOTES (Version 2.6)
//
// VirtualC64 uses Pepto's Colodore color palette from now on.
// Three new sliders have been added to the emulator preferences. They allow the
// user to adjust brightness, contrast, and saturation of the used color palette.
// The VICII implementation has been refined. Now, the user can choose between six
// different chip models. The PAL models 6569_R1, 6569_R3,8565, and the NTSC models
// 6567, 6567_R56A, 8562 are supported.
// VirtualC64 now passes VICE tests sprite-enable2.prg, spritey.prg, and sprite-sprite.prg
//
// TODOs for the next release:
//
//
// Can sub components initialized without registerSubcomponents function?
// Debug lightpen stuff
// Map right Mac shift key to right Commodore key
//
//
// CLEANUP:
// Store sprite number in firstDMACycle, secondDMACycle int's. Use -1 if no DMA happens
// Don't use mount() for inserting disks. Use insertDisk instead
//
// OPTIMIZATION:
// Update IEC bus inside CIA and VIA. Use delay flags if neccessary
// Use a simpler implementation for the raster irq trigger. Edge sensitive matching value
// Call CA1 action in VIA class only if the pin value really has changed.
// Get rid of timer in DriveInspector. Do it message based

// Loading and saving:
//
// New object structure:
// AnyC64File : Base class for all supported file types
// AnyDisk : Interface for all files representing a floppy disk
//           Gives access to tracks and sectors
// AnyArchive : Interface for all files representing a file archive
//           Gives access to files and their data
//
// Snapshot : AnyC64File
// RomFile : AnyC64File
// TapFile : AnyC64File
// CrtFile : AnyC64File
// G64File : AnyC64File, AnyDisk
// D64File : AnyC64File, AnyDisk, AnyArchive
// T64File : AnyC64File, AnyArchive
// PRGFile : AnyC64File, AnyArchive
// P00File : AnyC64File, AnyArchive
//
// Add setter API for SID stuff
//

