/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/glu.h>
#import "C64.h"
#import "Formatter.h"
#import "Disassembler.h"
#import "VICScreen.h"
#import "MountDialog.h"
#import "RomDialog.h"
#import "MyController.h"
#import "PreferenceController.h"
#import "AudioDevice.h"
#import "C64Proxy.h"
#import "JoystickManager.h"
#import "TimeTravelTableView.h"
#import "CpuTableView.h"
#import "MemTableView.h"

@interface MyDocument : NSDocument
{
	// ObjC/C++ bridge
	C64Proxy *c64;

	//! Reference to an attached D64 archive
	D64Archive *archive;
	
	//! Reference to an attached cartridge 
	Cartridge *cartridge;
	
	//! Initial snapshot
	/*! If unequal NULL, the newly created document will be initialized with the provided snapshot data */
	Snapshot *snapshot;			
}

@property C64Proxy *c64;
@property D64Archive *archive;
@property Cartridge *cartridge;
@property Snapshot *snapshot;

- (BOOL)setArchiveWithName:(NSString *)path;
- (BOOL)setCartridgeWithName:(NSString *)path;
- (BOOL)detachCartridge;

// Loading and saving
- (BOOL)loadRom:(NSString *)filename;

@end

