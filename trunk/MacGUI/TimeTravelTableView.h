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

#ifndef __TIME_TRAVEL_TABLE_VIEW_H__
#define __TIME_TRAVEL_TABLE_VIEW_H__

#import <Cocoa/Cocoa.h>

// Forward declarations
@class C64Proxy;
@class MyDocument;

@interface TimeTravelTableView : NSTableView <NSTableViewDataSource,NSTableViewDelegate>
{ 
	MyController *mydoc;
	NSMutableArray *items;
	time_t setupTime;
}
	
// Action methods

- (void)clickAction:(id)sender;
- (void)doubleClickAction:(id)sender;
- (void)revertAction:(id)sender;

// Accessors

//! Returns a mutable array with dictionaries of all the items
- (NSMutableArray *)items;

- (id)selectedRowItemforColumnIdentifier:(NSString *)anIdentifier;
	
// Mutators

//! Initialize all items
- (void)setItems:(MyController *)doc;
	
@end

#endif
