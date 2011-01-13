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

@implementation MyController(CpuPanel) 

- (IBAction)aAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] aAction:[NSNumber numberWithInt:[[c64 cpu] getA]]];
	if (![undo isUndoing]) [undo setActionName:@"Set accumulator"];
	
	[[c64 cpu] setA:[sender intValue]];
	[self refresh];
}

- (IBAction)xAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] xAction:[NSNumber numberWithInt:[[c64 cpu] getX]]];
	if (![undo isUndoing]) [undo setActionName:@"Set X register"];
	
	[[c64 cpu] setX:[sender intValue]];
	[self refresh];
}

- (IBAction)yAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] yAction:[NSNumber numberWithInt:[[c64 cpu] getY]]];
	if (![undo isUndoing]) [undo setActionName:@"Set Y register"];
	
	[[c64 cpu] setY:[sender intValue]];
	[self refresh];
}

- (IBAction)pcAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] pcAction:[NSNumber numberWithInt:[[c64 cpu] getPC]]];
	if (![undo isUndoing]) [undo setActionName:@"Set program counter"];
	
	[[c64 cpu] setPC:[sender intValue]];
	[self refresh];
}

- (IBAction)spAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spAction:[NSNumber numberWithInt:[[c64 cpu] getSP]]];
	if (![undo isUndoing]) [undo setActionName:@"Set stack pointer"];
	
	[[c64 cpu] setSP:[sender intValue]];
	[self refresh];
}

- (IBAction)NAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] NAction:[NSNumber numberWithInt:[[c64 cpu] getN]]];
	if (![undo isUndoing]) [undo setActionName:@"Negative Flag"];
	
	[[c64 cpu] setN:[sender intValue]];
	[self refresh];
}

- (IBAction)ZAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ZAction:[NSNumber numberWithInt:[[c64 cpu] getZ]]];
	if (![undo isUndoing]) [undo setActionName:@"Zero Flag"];
	
	[[c64 cpu] setZ:[sender intValue]];
	[self refresh];
}

- (IBAction)CAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] CAction:[NSNumber numberWithInt:[[c64 cpu] getC]]];
	if (![undo isUndoing]) [undo setActionName:@"Carry Flag"];
	
	[[c64 cpu] setC:[sender intValue]];
	[self refresh];
}

- (IBAction)IAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] IAction:[NSNumber numberWithInt:[[c64 cpu] getI]]];
	if (![undo isUndoing]) [undo setActionName:@"Interrupt Flag"];
	
	[[c64 cpu] setI:[sender intValue]];
	[self refresh];
}

- (IBAction)BAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] BAction:[NSNumber numberWithInt:[[c64 cpu] getB]]];
	if (![undo isUndoing]) [undo setActionName:@"Break Flag"];
	
	[[c64 cpu] setB:[sender intValue]];
	[self refresh];
}

- (IBAction)DAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] DAction:[NSNumber numberWithInt:[[c64 cpu] getD]]];
	if (![undo isUndoing]) [undo setActionName:@"Decimal Flag"];
	
	[[c64 cpu] setD:[sender intValue]];
	[self refresh];
}

- (IBAction)VAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] VAction:[NSNumber numberWithInt:[[c64 cpu] getV]]];
	if (![undo isUndoing]) [undo setActionName:@"Overflow Flag"];
	
	[[c64 cpu] setV:[sender intValue]];
	[self refresh];
}

- (IBAction)mhzAction:(id)sender
{
#if 0
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] mhzAction:[c64 cpuGetMHz]];
	if (![undo isUndoing]) [undo setActionName:@"Clock frequency"];
	
	// float value = clip([sender floatValue], 0.1f, 10.0f);
	[c64 cpuSetMHz:sender];
	[self refresh];
#endif
}

- (IBAction)setHardBreakpointAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] setHardBreakpointAction:[NSNumber numberWithInt:[sender intValue]]];
	if (![undo isUndoing]) [undo setActionName:@"Breakpoint"];
	
	[[c64 cpu] toggleHardBreakpoint:[sender intValue]];
	[self refresh];
}

- (void)refreshCPU
{
	[a setIntValue:[[c64 cpu] getA]];
	[x setIntValue:[[c64 cpu] getX]];
	[y setIntValue:[[c64 cpu] getY]];
	[pc setIntValue:[[c64 cpu] getPC]];
	[sp setIntValue:[[c64 cpu] getSP]];
	
	[N setIntValue:[[c64 cpu] getN]];
	[V setIntValue:[[c64 cpu] getV]];
	[B setIntValue:[[c64 cpu] getB]];
	[D setIntValue:[[c64 cpu] getD]];
	[I setIntValue:[[c64 cpu] getI]];
	[Z setIntValue:[[c64 cpu] getZ]];
	[C setIntValue:[[c64 cpu] getC]];	
}

@end
