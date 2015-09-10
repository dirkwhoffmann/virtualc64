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
	[[undo prepareWithInvocationTarget:self] NAction:@((int)[[c64 cpu] N])];
	if (![undo isUndoing]) [undo setActionName:@"Negative Flag"];
	
	[[c64 cpu] setN:[sender intValue]];
	[self refresh];
}

- (IBAction)ZAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ZAction:@((int)[[c64 cpu] Z])];
	if (![undo isUndoing]) [undo setActionName:@"Zero Flag"];
	
	[[c64 cpu] setZ:[sender intValue]];
	[self refresh];
}

- (IBAction)CAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] CAction:@((int)[[c64 cpu] C])];
	if (![undo isUndoing]) [undo setActionName:@"Carry Flag"];
	
	[[c64 cpu] setC:[sender intValue]];
	[self refresh];
}

- (IBAction)IAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] IAction:@((int)[[c64 cpu] I])];
	if (![undo isUndoing]) [undo setActionName:@"Interrupt Flag"];
	
	[[c64 cpu] setI:[sender intValue]];
	[self refresh];
}

- (IBAction)BAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] BAction:@((int)[[c64 cpu] B])];
	if (![undo isUndoing]) [undo setActionName:@"Break Flag"];
	
	[[c64 cpu] setB:[sender intValue]];
	[self refresh];
}

- (IBAction)DAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] DAction:@((int)[[c64 cpu] D])];
	if (![undo isUndoing]) [undo setActionName:@"Decimal Flag"];
	
	[[c64 cpu] setD:[sender intValue]];
	[self refresh];
}

- (IBAction)VAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] VAction:@((int)[[c64 cpu] V])];
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
	[[undo prepareWithInvocationTarget:self] setHardBreakpointAction:@((int)[sender intValue])];
	if (![undo isUndoing]) [undo setActionName:@"Breakpoint"];
	
	[[c64 cpu] toggleHardBreakpoint:[sender intValue]];
	[cpuTableView reloadData]; // [self refresh];
}

- (void)refreshCPU
{
	[a setIntValue:[[c64 cpu] A]];
	[x setIntValue:[[c64 cpu] X]];
	[y setIntValue:[[c64 cpu] Y]];
	[pc setIntValue:[[c64 cpu] PC]];
	[sp setIntValue:[[c64 cpu] SP]];
	
	[N setIntValue:[[c64 cpu] N]];
	[V setIntValue:[[c64 cpu] V]];
	[B setIntValue:[[c64 cpu] B]];
	[D setIntValue:[[c64 cpu] D]];
	[I setIntValue:[[c64 cpu] I]];
	[Z setIntValue:[[c64 cpu] Z]];
	[C setIntValue:[[c64 cpu] C]];	
}

@end
