/*
 * (C) 2011 Dirk W. Hoffmann. All rights reserved.
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

// Forward declarations
@class C64Proxy;
@class MyDocument;

#define CPU_TABLE_VIEW_ITEMS 256

@interface CpuTableView : NSTableView <NSTableViewDataSource,NSTableViewDelegate>
{ 
	MyController *controller;
	C64Proxy *c64;

	NSMutableArray * items;
	
	// Address of the first disassembled instruction
	uint16_t disassembleStartAddr; 	
	
	// Mapping from table rows to displayed memory address
	uint16_t displayedAddress[CPU_TABLE_VIEW_ITEMS];	
}

//! Setter
- (void)setController:(MyController *)c;

//! Returns row in which addr is displays (-1 if addr is not displayed)
- (int)rowForAddress:(uint16_t)addr;

//! Returns which address is displayed in row
- (uint16_t)addressForRow:(unsigned)row;

//! Action method for double clicking a table item
- (void)doubleClickAction:(id)sender;

//! Updates the displayedAddress array 
- (void)updateDisplayedAddresses:(uint16_t)startAddr;

//! Refresh  data items to display
- (void)refresh;

@end
