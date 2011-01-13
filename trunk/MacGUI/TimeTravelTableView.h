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

@interface TimeTravelTableView : NSTableView <NSTableViewDataSource,NSTableViewDelegate>
{ 
	MyController *controller;
	C64Proxy *c64;

	NSMutableArray *items;
	time_t setupTime;
}

//! Setter
- (void)setController:(MyController *)c;

//! Action method for single clicking a table item
- (void)clickAction:(id)sender;

//! Action method for double clicking a table item
- (void)doubleClickAction:(id)sender;

//! Revert to selected snapshot
- (void)revertAction:(id)sender;

//! Refresh  data items to display
- (void)refresh;
	
@end
