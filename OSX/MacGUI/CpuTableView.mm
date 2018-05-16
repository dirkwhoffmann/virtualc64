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

#if 0

#import "C64GUI.h"

@implementation CpuTableView 

- (void)setController:(MyController *)ctrl
{
	c = ctrl;
	// c64 = [c c64];
	[self updateDisplayedAddresses:[[[c c64] cpu] PC]];
}

#pragma mark NSTableView

- (void)awakeFromNib {

	// we are our own data source
	[self setDelegate:self];
	[self setDataSource:self];

	// prepare to get click and double click messages
	[self setTarget:self];
	[self setDoubleAction:@selector(doubleClickAction:)];

	[self reloadData];
}


#pragma mark NSTableViewDataSource

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
	return CPU_TABLE_VIEW_ITEMS;
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)row
{
    C64Proxy *c64 = [c c64];
    if (c64 == nil)
        return nil;
    
    uint16_t addr = [self addressForRow:row];
	uint8_t length = [[c64 cpu] lengthOfInstruction:[[c64 mem] read:addr]];
	
    if ([[aTableColumn identifier] isEqual:@"addr"]) {
		return @((int)addr);
    } else if ([[aTableColumn identifier] isEqual:@"data01"]) {
		return (length > 0 ? @((int)[[c64 mem] read:addr]) : nil);
    } else if ([[aTableColumn identifier] isEqual:@"data02"]) {
		return (length > 1 ? @((int)[[c64 mem] read:(addr+1)]) : nil);
    } else if ([[aTableColumn identifier] isEqual:@"data03"]) {
		return (length > 2 ? @((int)[[c64 mem] read:(addr+2)]) : nil);
    } else if ([[aTableColumn identifier] isEqual:@"ascii"]) {
        DisassembledInstruction instr = [[c64 cpu] disassemble:addr hex:[c hex]];
        return [NSString stringWithUTF8String:instr.command];
    }
		// return @((int)addr);
	
	return @"???";
}

- (void)tableView: (NSTableView *)aTableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    C64Proxy *c64 = [c c64];
    if (c64 == nil)
        return;
    
    uint16_t addr = [self addressForRow:row];
	if ([[c64 cpu] breakpoint:addr] == HARD_BREAKPOINT) {
		[cell setTextColor:[NSColor redColor]];
	} else {
		[cell setTextColor:[NSColor blackColor]];
	}
}

#pragma mark NSTableViewDelegate

- (void)doubleClickAction:(id)sender
{
	NSLog(@"doubleClickAction (item %ld)", (long)[sender selectedRow]);
	
	uint16_t addr;
		
    addr = [self addressForRow:[sender selectedRow]];
	[c setHardBreakpointAction:@((int)addr)];
}


- (int)rowForAddress:(uint16_t)addr
{
	for (unsigned i = 0; i < CPU_TABLE_VIEW_ITEMS; i++) {
		if (displayedAddresses[i] == addr)
			return i;
	}
	return -1;
}

- (uint16_t)addressForRow:(long)row;
{
	assert (row < CPU_TABLE_VIEW_ITEMS);
	return displayedAddresses[row];
}

- (void)updateDisplayedAddresses:(uint16_t)startAddr
{
    C64Proxy *c64 = [c c64];
    if (c64 == nil)
        return;
    
	uint16_t address = startAddr;
	
	for (unsigned i = 0; i < CPU_TABLE_VIEW_ITEMS; i++) {
		displayedAddresses[i] = address;
		address += [[c64 cpu] lengthOfInstructionAtAddress:address];
	}	
}

- (void)refresh
{
	// Refreshing the cpu disassembler window works the following way:
	//
	// Case 1: PC points to an address which is already displayed.
	//         In this case, we simply select the correspondig row and don't modify anything else
	// Case 2: PC points to an address that is not yet displayed.
	//         In that case, we display PC in row 0.
	
    C64Proxy *c64 = [c c64];
    if (c64 == nil)
        return;
    
	uint16_t address = [[c64 cpu] PC];
	int row = [self rowForAddress:address];
	
	if (row != -1) { // Case 1

		[self scrollRowToVisible:row];
		[self selectRowIndexes:[NSIndexSet indexSetWithIndex:row] byExtendingSelection:NO];

	} else {

		[self updateDisplayedAddresses:address];
		[self scrollRowToVisible:0];
		[self selectRowIndexes:[NSIndexSet indexSetWithIndex:0] byExtendingSelection:NO];		
	}
	
	[self reloadData];
}

@end

#endif

