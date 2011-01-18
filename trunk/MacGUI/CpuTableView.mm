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

#import "C64GUI.h"

@implementation CpuTableView 

- (void)setController:(MyController *)c
{
	controller = c;
	c64 = [c c64];
	[self updateDisplayedAddresses:[[c64 cpu] getPC]];
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

- (void)dealloc {
	[super dealloc];
}

#pragma mark NSTableViewDataSource

- (int)numberOfRowsInTableView:(NSTableView *)tableView
{
	return CPU_TABLE_VIEW_ITEMS;
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)row
{
	uint16_t addr = [self addressForRow:row];
	uint8_t length = [[c64 cpu] getLengthOfInstruction:[[c64 mem] peek:addr]];
	
	if ([[aTableColumn identifier] isEqual:@"addr"]) 
		return [NSNumber numberWithInt:addr];
	else if ([[aTableColumn identifier] isEqual:@"data01"]) 
		return (length > 0 ? [NSNumber numberWithInt:[[c64 mem] peek:addr]] : nil);
	else if ([[aTableColumn identifier] isEqual:@"data02"]) 
		return (length > 1 ? [NSNumber numberWithInt:[[c64 mem] peek:(addr+1)]] : nil);
	else if ([[aTableColumn identifier] isEqual:@"data03"]) 
		return (length > 2 ? [NSNumber numberWithInt:[[c64 mem] peek:(addr+2)]] : nil);
	else if ([[aTableColumn identifier] isEqual:@"ascii"]) 
		return [NSNumber numberWithInt:addr];
	
	return @"???";
}

- (void)tableView: (NSTableView *)aTableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn row:(int)row
{
	uint16_t addr = [self addressForRow:row];
	if ([[c64 cpu] getBreakpoint:addr] == CPU::HARD_BREAKPOINT) {
		[cell setTextColor:[NSColor redColor]];
	} else {
		[cell setTextColor:[NSColor blackColor]];
	}
}

#pragma mark NSTableViewDelegate

- (void)doubleClickAction:(id)sender
{
	NSLog(@"doubleClickAction (item %d)", [sender selectedRow]);
	
	uint16_t addr;
		
	addr = [self addressForRow:[sender selectedRow]]; 
	[controller setHardBreakpointAction:[NSNumber numberWithInt:addr]];
}


- (int)rowForAddress:(uint16_t)addr
{
	for (unsigned i = 0; i < CPU_TABLE_VIEW_ITEMS; i++) {
		if (displayedAddresses[i] == addr)
			return i;
	}
	return -1;
}

- (uint16_t)addressForRow:(unsigned)row;
{
	assert (row < CPU_TABLE_VIEW_ITEMS);
	return displayedAddresses[row];
}

- (void)updateDisplayedAddresses:(uint16_t)startAddr
{
	uint16_t address = startAddr;
	
	for (unsigned i = 0; i < CPU_TABLE_VIEW_ITEMS; i++) {
		displayedAddresses[i] = address;
		address += [[c64 cpu] getLengthOfInstructionAtAddress:address];
	}	
}

- (void)refresh {

	// Refreshing the cpu disassembler window works the following way:
	//
	// Case 1: PC points to an address which is already displayed.
	//         In this case, we simply select the correspondig row and don't modify anything else
	// Case 2: PC points to an address that is not yet displayed.
	//         In that case, we display PC in row 0.
	
	uint16_t address = [[c64 cpu] getPC];
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
