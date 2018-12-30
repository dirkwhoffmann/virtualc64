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
#define V_MAJOR 3
#define V_MINOR 3
#define V_SUBMINOR 0

// Disable assertion checking (Uncomment in release build)
// #define NDEBUG

// Default debug level for all components (Set to 1 in release build)
#define DEBUG_LEVEL 2

#endif 

// RELEASE NOTES (Version 3.3)
//
// All emulator preferences have been merged into a unified preferences pane.
// New options in the emulator settings allow the user to customize the actions that are taken when opening a media file.
// The input device selection is now retained in the user default storage.
// The user can now choose whether to save the emulator texture or the upscaled texture as a screenshot image.
// If menu item "Save Snapshot..." is selected, emulation is paused until the screenshot has been saved.
// Added support for the Expert cartridge.
// Cardridge buttons are now displayed in the bottom bar as small (clickable) icons.
// Upgraded ReSID to the version used in VICE V3.3.
// Added code to dynamically adjust the sampling rate to reduce buffer underflows and overflows.
// Fixed a bug that made FastSID unusable on audio hardware with a sample rate other than 44.1khz.
// The assignment of keystrokes to joystick events has been improved when multiple keys are pressed at the same time.
// JiffyDOS Rom images have been added to the list of known images.

//
// TODOs for the next release:
//
//
//
//
// CLEANUP:
// Don't use mount() for inserting disks. Use insertDisk instead
// Replace Mouse* C64::mouse by (enum) MouseModel C64::mouse (use bridge pattern)
// Remove prefix.pch from project
//
// OPTIMIZATION:
//
// Check, how -Ofast compares to -O3
// Check how USE_OPTIMIZATION_PROFILE = true influences runtime
// Try enable warning option "Pedantic Warning"
// Try enable warning option "Implicit float conversions"
//
// 
// Update IEC bus inside CIA and VIA. Use delay flags if neccessary
// Use a simpler implementation for the raster irq trigger. Edge sensitive matching value
// Call CA1 action in VIA class only if the pin value really has changed.
//
// Add setter API for SID stuff
//

