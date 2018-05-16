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

@implementation MemTableView 

@synthesize source;

- (void)setController:(MyController *)ctrl
{
	c = ctrl;
}
#pragma mark NSTableView

- (void)awakeFromNib {
		
	// we are our own data source
	[self setDelegate:self];
	[self setDataSource:self];

	// prepare to get mouse click events
	[self setTarget:self];
	// [self setDoubleAction:@selector(doubleClickAction:)];

	[self reloadData];
}


#pragma mark NSTableViewDataSource

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{	
	return 65536 / 4;	
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row 
{
    C64Proxy *c64 = [c c64];
    if (c64 == nil)
        return nil;
    
	uint16_t addr = row * 4;
	NSString *id  = [tableColumn identifier];
	
	// Address column...
	if ([[tableColumn identifier] isEqual:@"addr"]) 
		return @((int)(4*row));
	
	// ASCII column...
	if ([[tableColumn identifier] isEqual:@"ascii"]) {
        
		if (![[c64 mem] isValidAddr:addr memtype:source])
			return nil;
        
        uint8_t c1 = [[c64 mem] readFrom:(addr+0) memtype:source];
        uint8_t c2 = [[c64 mem] readFrom:(addr+1) memtype:source];
        uint8_t c3 = [[c64 mem] readFrom:(addr+2) memtype:source];
        uint8_t c4 = [[c64 mem] readFrom:(addr+3) memtype:source];
        
        return [NSString stringWithFormat:@"%c%c%c%c",
                petscii2printable(c1, '.'),
                petscii2printable(c2, '.'),
                petscii2printable(c3, '.'),
                petscii2printable(c4, '.')];
	}
	
	// One of the hexadecimal columns...
	if ([id isEqual:@"hex1"]) addr += 1;
	if ([id isEqual:@"hex2"]) addr += 2;
	if ([id isEqual:@"hex3"]) addr += 3;
	
	if (![[c64 mem] isValidAddr:addr memtype:source])
		return nil;
	
	return @((int)[[c64 mem] readFrom:addr memtype:source]);
	
	return nil;
}

/*
- (NSCell *)tableView:(NSTableView *)tableView dataCellForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    NSTextFieldCell *cell = [tableColumn dataCell];
    
    if ([[tableColumn identifier] isEqual:@"ascii"]) {
        NSFont *cbmfont = [NSFont fontWithName:@"C64ProMono" size: 9];
        [cell setFont:cbmfont];
    }

    return cell;
}
*/

- (void)changeMemValue:(uint16_t)addr value:(int16_t)v memtype:(MemoryType)t
{
    C64Proxy *c64 = [c c64];
    if (c64 == nil)
        return;
    
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] changeMemValue:addr value:[[c64 mem] readFrom:addr memtype:t] memtype:t];
	if (![undo isUndoing]) [undo setActionName:@"Memory contents"];
	
	[[c64 mem] pokeTo:addr value:v memtype:t];
	[self refresh];	
}

- (void)setMemObjectValue:(id)anObject forTableColumn:(NSTableColumn *)aTableColumn row:(int)row
{
    C64Proxy *c64 = [c c64];
    if (c64 == nil)
        return;
    
	uint16_t addr = row * 4;
	int16_t value = [anObject intValue];
	NSString *id  = [aTableColumn identifier];
	
	// Compute exact address
	if ([id isEqual:@"hex1"]) addr += 1;
	if ([id isEqual:@"hex2"]) addr += 2;
	if ([id isEqual:@"hex3"]) addr += 3;
	
	uint8_t oldValue = [[c64 mem] readFrom:addr memtype:source];
	if (oldValue == value)
		return; 
	
	[self changeMemValue:addr value:value memtype:source];
}

- (void)refresh 
{
	[self setNeedsDisplay];
}

@end
