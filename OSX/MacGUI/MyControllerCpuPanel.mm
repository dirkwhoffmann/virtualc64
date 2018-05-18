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
#import "VirtualC64-Bridging-Header.h"

@implementation MyController(CpuPanel) 

- (IBAction)aAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] aAction:@((int)[[c64 cpu] A])];
	if (![undo isUndoing]) [undo setActionName:@"Set accumulator"];
	
	[[c64 cpu] setA:[sender intValue]];
	[self refresh];
}

- (IBAction)xAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] xAction:@((int)[[c64 cpu] X])];
	if (![undo isUndoing]) [undo setActionName:@"Set X register"];
	
	[[c64 cpu] setX:[sender intValue]];
	[self refresh];
}

- (IBAction)yAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] yAction:@((int)[[c64 cpu] Y])];
	if (![undo isUndoing]) [undo setActionName:@"Set Y register"];
	
	[[c64 cpu] setY:[sender intValue]];
	[self refresh];
}

- (IBAction)pcAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] pcAction:@((int)[[c64 cpu] PC])];
	if (![undo isUndoing]) [undo setActionName:@"Set program counter"];
	
	[[c64 cpu] setPC:[sender intValue]];
	[self refresh];
}

- (IBAction)spAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spAction:@((int)[[c64 cpu] SP])];
	if (![undo isUndoing]) [undo setActionName:@"Set stack pointer"];
	
	[[c64 cpu] setSP:[sender intValue]];
	[self refresh];
}

- (IBAction)NAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] NAction:@((int)[[c64 cpu] Nflag])];
	if (![undo isUndoing]) [undo setActionName:@"Negative Flag"];
	
	[[c64 cpu] setNflag:[sender intValue]];
	[self refresh];
}

- (IBAction)ZAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ZAction:@((int)[[c64 cpu] Zflag])];
	if (![undo isUndoing]) [undo setActionName:@"Zero Flag"];
	
	[[c64 cpu] setZflag:[sender intValue]];
	[self refresh];
}

- (IBAction)CAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] CAction:@((int)[[c64 cpu] Cflag])];
	if (![undo isUndoing]) [undo setActionName:@"Carry Flag"];
	
	[[c64 cpu] setCflag:[sender intValue]];
	[self refresh];
}

- (IBAction)IAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] IAction:@((int)[[c64 cpu] Iflag])];
	if (![undo isUndoing]) [undo setActionName:@"Interrupt Flag"];
	
	[[c64 cpu] setIflag:[sender intValue]];
	[self refresh];
}

- (IBAction)BAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] BAction:@((int)[[c64 cpu] Bflag])];
	if (![undo isUndoing]) [undo setActionName:@"Break Flag"];
	
	[[c64 cpu] setBflag:[sender intValue]];
	[self refresh];
}

- (IBAction)DAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] DAction:@((int)[[c64 cpu] Dflag])];
	if (![undo isUndoing]) [undo setActionName:@"Decimal Flag"];
	
	[[c64 cpu] setDflag:[sender intValue]];
	[self refresh];
}

- (IBAction)VAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] VAction:@((int)[[c64 cpu] Vflag])];
	if (![undo isUndoing]) [undo setActionName:@"Overflow Flag"];
	
	[[c64 cpu] setVflag:[sender intValue]];
	[self refresh];
}

- (void)refreshCPU
{
	[a setIntValue:[[c64 cpu] A]];
	[x setIntValue:[[c64 cpu] X]];
	[y setIntValue:[[c64 cpu] Y]];
	[pc setIntValue:[[c64 cpu] PC]];
	[sp setIntValue:[[c64 cpu] SP]];
	
	[Nflag setIntValue:[[c64 cpu] Nflag]];
	[Vflag setIntValue:[[c64 cpu] Vflag]];
	[Bflag setIntValue:[[c64 cpu] Bflag]];
	[Dflag setIntValue:[[c64 cpu] Dflag]];
	[Iflag setIntValue:[[c64 cpu] Iflag]];
	[Zflag setIntValue:[[c64 cpu] Zflag]];
	[Cflag setIntValue:[[c64 cpu] Cflag]];
}

@end
