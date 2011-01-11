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

#import "MyController.h"
#import "MyControllerMemoryPanel.h"

@implementation MyController(MemoryPanel) 

- (IBAction)searchAction:(id)sender
{
	uint16_t addr = [sender intValue];
	[memTableView scrollRowToVisible:addr/4];
	[self refresh];
}

- (Memory::MemoryType)currentMemSource
{
	if ([ramSource intValue]) return Memory::MEM_RAM;
	if ([romSource intValue]) return Memory::MEM_ROM;
	if ([ioSource intValue]) return Memory::MEM_IO;
	
	assert(false);
	return Memory::MEM_RAM;
}

- (void)setMemSource:(Memory::MemoryType)source
{
	[self refresh];
}

- (IBAction)setMemSourceToRAM:(id)sender
{
	[self setMemSource:Memory::MEM_RAM];
}

- (IBAction)setMemSourceToROM:(id)sender
{
	[self setMemSource:Memory::MEM_ROM];
}

- (IBAction)setMemSourceToIO:(id)sender
{
	[self setMemSource:Memory::MEM_IO];
}

- (id)objectValueForMemTableColumn:(NSTableColumn *)aTableColumn row:(int)row
{
	uint16_t addr = row * 4;
	NSString *id  = [aTableColumn identifier];
	
	// Address column...
	if ([[aTableColumn identifier] isEqual:@"addr"]) 
		return [NSNumber numberWithInt:4*row];
	
	// ASCII column...
	if ([[aTableColumn identifier] isEqual:@"ascii"]) {
		if (![[c64 mem] isValidAddr:addr memtype:[self currentMemSource]])
			return nil;
		else
			return [NSString stringWithFormat:@"%c%c%c%c", 
					toASCII([[c64 mem] peekFrom:(addr+0) memtype:[self currentMemSource]]),
					toASCII([[c64 mem] peekFrom:(addr+1) memtype:[self currentMemSource]]),
					toASCII([[c64 mem] peekFrom:(addr+2) memtype:[self currentMemSource]]),
					toASCII([[c64 mem] peekFrom:(addr+3) memtype:[self currentMemSource]])];
	}
	
	// One of the hexadecimal columns...
	if ([id isEqual:@"hex1"]) addr += 1;
	if ([id isEqual:@"hex2"]) addr += 2;
	if ([id isEqual:@"hex3"]) addr += 3;
	
	if (![[c64 mem] isValidAddr:addr memtype:[self currentMemSource]])
		return nil;
	
	return [NSNumber numberWithInt:[[c64 mem] peekFrom:addr memtype:[self currentMemSource]]];
	
	return nil;
}

- (void)changeMemValue:(uint16_t)addr value:(int16_t)v memtype:(Memory::MemoryType)t
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] changeMemValue:addr value:[[c64 mem] peekFrom:addr memtype:t] memtype:t];
	if (![undo isUndoing]) [undo setActionName:@"Memory contents"];
	
	[[c64 mem] pokeTo:addr value:v memtype:t];
	[self refresh];	
}

- (void)setMemObjectValue:(id)anObject forTableColumn:(NSTableColumn *)aTableColumn row:(int)row
{
	
	uint16_t addr = row * 4;
	int16_t value = [anObject intValue];
	NSString *id  = [aTableColumn identifier];
	
	// Compute exact address
	if ([id isEqual:@"hex1"]) addr += 1;
	if ([id isEqual:@"hex2"]) addr += 2;
	if ([id isEqual:@"hex3"]) addr += 3;
	
	uint8_t oldValue = [[c64 mem] peekFrom:addr memtype:[self currentMemSource]];
	if (oldValue == value)
		return; 
	
	[self changeMemValue:addr value:value memtype:[self currentMemSource]];
}

- (void)doubleClickInMemTable:(id)sender
{
	[self refresh];
}

- (void)refreshMemory
{
}

@end
