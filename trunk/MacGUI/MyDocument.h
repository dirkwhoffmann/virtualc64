/*
 * (C) 2006 - 2011 Dirk W. Hoffmann. All rights reserved.
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

@class C64Proxy;
class D64Archive;
class Cartridge;
class Snapshot;

// TODO
// 1. Intantiate C64Proxy in XIB file
//    No more custom creation of proxy needed
// 2. Use C64Proxy as data source for ObjectController (key value coding)
// 3. C64Proxy will look at received messages (before passing them to the controller) and update internal values accordingly
// 4. Sending "refresh" to C64Proxy object will update all internal values (frequently called when debug pabel is open or on startup)
// 5. Eliminate outlets and custom code for key/value controlled objects

@interface MyDocument : NSDocument
{
	// ObjC/C++ bridge
	C64Proxy *c64;
	
	//! Reference to an attached D64 archive
	/*! When a new documents opens and this variable is not NULL, the archive is automatically mounted */
	D64Archive *archive;
	
	//! Reference to an attached cartridge 
	Cartridge *cartridge;	
}

@property C64Proxy *c64;
@property D64Archive *archive;
@property Cartridge *cartridge;

- (BOOL)setArchiveWithName:(NSString *)path;
- (BOOL)setCartridgeWithName:(NSString *)path;
- (BOOL)detachCartridge;

// Loading and saving
- (BOOL)loadRom:(NSString *)filename;

@end

