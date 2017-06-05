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

@implementation MyController(MemoryPanel) 

- (IBAction)searchAction:(id)sender
{
	uint16_t addr = [sender intValue];
	[memTableView scrollRowToVisible:addr/4];
	[self refresh];
}

- (IBAction)setMemSourceToRAM:(id)sender
{
	[memTableView setSource:MEM_RAM];
	[memTableView refresh];
}

- (IBAction)setMemSourceToROM:(id)sender
{
	[memTableView setSource:MEM_ROM];
	[memTableView refresh];
}

- (IBAction)setMemSourceToIO:(id)sender
{
	[memTableView setSource:MEM_IO];
	[memTableView refresh];
}

- (void)refreshMemory
{
	[memTableView reloadData];
}

- (MemoryType)currentMemSource
{
    if ([ramSource intValue]) return MEM_RAM;
    if ([romSource intValue]) return MEM_ROM;
    if ([ioSource intValue]) return MEM_IO;
    
    assert(false);
    return MEM_RAM;
}

- (void)doubleClickInMemTable:(id)sender
{
    [self refresh];
}

- (void)changeMemValue:(uint16_t)addr value:(int16_t)v memtype:(MemoryType)t
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

@end
