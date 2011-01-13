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
}

#pragma mark NSTableView

- (void)awakeFromNib {
		
	// items = [NSMutableArray new];

	// we are our own data source
	[self setDelegate:self];
	[self setDataSource:self];

	// prepare to get click and double click messages
	[self setTarget:self];
	// [self setAction:@selector(clickAction:)];
	[self setDoubleAction:@selector(doubleClickAction:)];

	[self reloadData];
}

- (void)dealloc {
	// [items release];
	[super dealloc];
}

#pragma mark NSTableViewDataSource

- (int)numberOfRowsInTableView:(NSTableView *)tableView
{
	return 128;
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)row
{
	uint16_t addr = [[c64 cpu] getAddressOfNextIthInstruction:row from:disassembleStartAddr];
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
	uint16_t addr = [[c64 cpu] getAddressOfNextIthInstruction:row from:disassembleStartAddr];
	if ([[c64 cpu] getBreakpoint:addr] == CPU::HARD_BREAKPOINT) {
		[cell setTextColor:[NSColor redColor]];
	} else {
		[cell setTextColor:[NSColor blackColor]];
	}
}

#pragma mark NSTableViewDelegate

- (void)clickAction:(id)sender
{
	NSLog(@"clickAction (item %d)", [sender selectedRow]);
}

- (void)doubleClickAction:(id)sender
{
	NSLog(@"doubleClickAction (item %d)", [sender selectedRow]);
	
	uint16_t addr;
		
	addr = [[c64 cpu] getAddressOfNextIthInstruction:[sender selectedRow] from:disassembleStartAddr];
	[controller setHardBreakpointAction:[NSNumber numberWithInt:addr]];
}

- (BOOL)computeRowForAddr:(uint16_t)addr maxRows:(uint16_t)maxRows row:(uint16_t *)row
{
	uint16_t currentRow  = 0;
	uint16_t currentAddr = disassembleStartAddr;
	while (currentAddr <= addr && currentRow < maxRows) {
		if (currentAddr == addr) {
			*row = currentRow;
			return YES;
		}
		currentRow++;
		currentAddr += [[c64 cpu] getLengthOfInstruction:[[c64 mem] peek:currentAddr]];
	}
	return NO;
}

- (void)refresh {
	
	// Prepare Disassembler window.
	// The implementation is a little tricky. We distinguish two cases:
	// 1. The PC points to an address that is already visible in some row
	//    In this case, we simply select the row and don't modify anything else
	// 2. The PC points to an address that is not yet displayed
	//    In that case, we display the PC address in row 0
	uint16_t rowIndex = 0xffff;
	uint16_t address = [[c64 cpu] getPC];
	NSIndexSet *indexSet;
	if ([self computeRowForAddr:(uint16_t)address maxRows:[self numberOfRowsInTableView:self] row:(uint16_t *)&rowIndex]) {
		indexSet = [NSIndexSet indexSetWithIndex:rowIndex];		
		[self scrollRowToVisible:rowIndex];
	} else {
		disassembleStartAddr = address;
		indexSet = [NSIndexSet indexSetWithIndex:0];	
		[self scrollRowToVisible:0];
	}
	[self selectRowIndexes:indexSet byExtendingSelection:NO];

	[self reloadData];
}


@end
