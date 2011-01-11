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
#import "MyControllerDebugging.h"

@implementation MyController(Debugging) 

- (IBAction)hideSpritesAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] hideSpritesAction:sender];
	if (![undo isUndoing]) [undo setActionName:@"Hide sprites"];
	
	if ([[c64 vic] hideSprites]) {
		[sender setState:NSOffState];
		[[c64 vic] setHideSprites:NO];
	} else {
		[sender setState:NSOnState];
		[[c64 vic] setHideSprites:YES];
	}
}

- (IBAction)markIRQLinesAction:(id)sender
{
	// NSUndoManager *undo = [self undoManager];
	// [[undo prepareWithInvocationTarget:self] markIRQLinesAction:sender];
	// if (![undo isUndoing]) [undo setActionName:@"Mark IRQ lines"];
	
	if ([[c64 vic] showIrqLines]) {
		[sender setState:NSOffState];
		[[c64 vic] setShowIrqLines:NO];
	} else {
		[sender setState:NSOnState];
		[[c64 vic] setShowIrqLines:YES];
	}
}

- (IBAction)markDMALinesAction:(id)sender
{
	// NSUndoManager *undo = [self undoManager];
	// [[undo prepareWithInvocationTarget:self] markDMALinesAction:sender];
	// if (![undo isUndoing]) [undo setActionName:@"Mark DMA lines"];
	
	if ([[c64 vic] showDmaLines]) {
		[sender setState:NSOffState];
		[[c64 vic] setShowDmaLines:NO];
	} else {
		[sender setState:NSOnState];
		[[c64 vic] setShowDmaLines:YES];
	}	
}

- (IBAction)traceC64CpuAction:(id)sender 
{ 
	if ([[c64 cpu] tracingEnabled]) {
		[sender setState:NSOffState];
		[[c64 cpu] setTraceMode:NO];
	} else {
		[sender setState:NSOnState];
		[[c64 cpu] setTraceMode:YES];
	}
}

- (IBAction)traceIecAction:(id)sender
{
	if ([[c64 iec] tracingEnabled]) {
		[sender setState:NSOffState];
		[[c64 iec] setTraceMode:NO];
	} else {
		[sender setState:NSOnState];
		[[c64 iec] setTraceMode:YES];
	}	
}

- (IBAction)traceVC1541CpuAction:(id)sender
{
	if ([[[c64 vc1541] cpu] tracingEnabled]) {
		[sender setState:NSOffState];
		[[[c64 vc1541] cpu] setTraceMode:NO];
	} else {
		[sender setState:NSOnState];
		[[[c64 vc1541] cpu] setTraceMode:YES];
	}	
}

- (IBAction)traceViaAction:(id)sender 
{
	if ([[[c64 vc1541] via:1] tracingEnabled]) {
		[sender setState:NSOffState];
		[[[c64 vc1541] via:1] setTraceMode:NO];
		[[[c64 vc1541] via:2] setTraceMode:NO];		
	} else {
		[sender setState:NSOnState];
		[[[c64 vc1541] via:1] setTraceMode:YES];
		[[[c64 vc1541] via:2] setTraceMode:YES];
	}	
}

- (IBAction)dumpC64:(id)sender
{
	[c64 dump];
}

- (IBAction)dumpC64CPU:(id)sender
{
	[[c64 cpu] dump];
}

- (IBAction)dumpC64CIA1:(id)sender
{
	[[c64 cia:1] dump];
}

- (IBAction)dumpC64CIA2:(id)sender
{
	[[c64 cia:2] dump];
}

- (IBAction)dumpC64VIC:(id)sender
{
	[[c64 vic] dump];
}

- (IBAction)dumpC64SID:(id)sender
{
	[[c64 sid] dump];
}

- (IBAction)dumpC64Memory:(id)sender
{
	[[c64 mem] dump];
}

- (IBAction)dumpVC1541:(id)sender
{
	[[c64 vc1541] dump];
}

- (IBAction)dumpVC1541CPU:(id)sender
{
	[[[c64 vc1541] cpu] dump];
}

- (IBAction)dumpVC1541VIA1:(id)sender
{
	[[[c64 vc1541] via:1] dump];
}

- (IBAction)dumpVC1541VIA2:(id)sender
{
	[[[c64 vc1541] via:2] dump];
}

- (IBAction)dumpVC1541Memory:(id)sender
{
	[[[c64 vc1541] mem] dump];
}

- (IBAction)dumpKeyboard:(id)sender
{
	[[c64 keyboard] dump];
}

- (IBAction)dumpIEC:(id)sender
{
	[[c64 iec] dump];
}

@end
