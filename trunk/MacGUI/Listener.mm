//
//  Listener.mm
//  V64
//
//  Created by Dirk Hoffmann on 07.04.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "Listener.h"

@implementation MyDocument (Listener)

- (void) runAction
{
	NSLog(@"%@ runAction", self);
	
	// Update info message
	infoString = @""; // @"Emulation in progress...";
	
	// Disable editing of all text fields
	[self enableUserEditing:NO];

	// Disable debug buttons
	
	// Refresh display
	needsRefresh = true;
}

- (void) haltAction
{
	infoString = @"Emulation halted.";
	[self enableUserEditing:YES];	
	needsRefresh = true;
}

- (void) okAction
{
	infoString = @"";
	needsRefresh = true;
}

- (void) breakpointAction
{
	infoString = @"Breakpoint reached. Virtual CPU halted.";
	[self enableUserEditing:YES];	
	needsRefresh = true;
}

- (void) watchpointAction
{
	infoString = @"Watchpoint reached. Virtual CPU halted.";
	[self enableUserEditing:YES];	
	needsRefresh = true;
}

- (void) illegalInstructionAction
{
	infoString = @"CPU halted due to an illegal instruction.";
	[self enableUserEditing:YES];	
	needsRefresh = true;
}

- (void) missingRomAction
{
	NSBeginAlertSheet(@"Warning:\nROMs are not part of this distribution", // Title
					  @"OK", // Default button
					  nil, // Alternate button
					  nil, // Other button
					  theWindow, // parent window of the sheet
					  self, // Modal delegate
					  nil, // willEndSelector
					  nil, // didEndSelector
					  nil, // contextInfo
					  @"Before you run the emulator, a Character, Basic, and Kernel ROM image needs to be added. If you are a legal owner of C64 ROM image files, you can add them by dragging and dropping them into the main window.");
}

- (void) connectDriveAction
{
	NSLog(@"connectDriveAction");
	[greenLED setImage:[NSImage imageNamed:@"LEDgreen"]];
	// greenLight = true; updateLight = true;
}

- (void) disconnectDriveAction
{
	NSLog(@"disconnectDriveAction");
	[greenLED setImage:[NSImage imageNamed:@"LEDgray"]];
	// greenLight = redLight = false; updateLight = true;
}

- (void) insertDiskAction
{
	NSLog(@"insertDiskAction");
	[drive setHidden:false];
	[eject setHidden:false];
}

- (void) ejectDiskAction
{
	NSLog(@"ejectDiskAction");
	[drive setHidden:true];
	[eject setHidden:true];
}

- (void) startDiskAction
{
	// NSLog(@"startDiskAction");
	if ([c64 isDriveConnected]) {
		redLight = true; updateLight = true;
		// [redLED setImage:[NSImage imageNamed:@"LEDred"]];
	}
	// [c64 cpuSetNativeSpeedFlag:!warpLoad];
	[c64 cpuSetNativeSpeedFlag:false];
}

- (void) stopDiskAction
{
	// NSLog(@"stopDiskAction");
	//[redLED setImage:[NSImage imageNamed:@"LEDgray"]];
	redLight = false; updateLight = true;
	// [c64 cpuSetNativeSpeedFlag:warpLoad];
	infoString = @"";
	needsRefresh = true;	
	[c64 cpuSetNativeSpeedFlag:true];
}

- (void) startWarpAction
{
	NSLog(@"startWarpAction");
	NSAutoreleasePool* arp = [[NSAutoreleasePool alloc] init];
	[warpMode setImage:[NSImage imageNamed:@"warpOff"]];
	[warpMode setNeedsDisplay:YES];
	[arp release];
}

- (void) stopWarpAction
{
	NSLog(@"stopWarpAction");
	NSAutoreleasePool* arp = [[NSAutoreleasePool alloc] init];
	[warpMode setImage:[NSImage imageNamed:@"warpOn"]];
	[warpMode setNeedsDisplay:YES];
	[arp release];
}

- (void) logAction:(char *)message
{
	if (consoleController)
		[consoleController insertText:message];
}

@end
