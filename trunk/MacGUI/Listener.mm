/*
 * (C) 2008 Dirk W. Hoffmann. All rights reserved.
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

#import "Listener.h"

@implementation MyDocument (Listener)


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

- (void) runAction
{
	NSAutoreleasePool* arp = [[NSAutoreleasePool alloc] init];

	NSLog(@"runAction");
	[info setStringValue:@""];
	[self enableUserEditing:NO];
	needsRefresh = true;
	
	[arp release];
}

- (void) haltAction
{
	NSAutoreleasePool* arp = [[NSAutoreleasePool alloc] init];
	
	NSLog(@"haltAction");
	[info setStringValue:@"Emulation halted."];
	[self enableUserEditing:YES];	
	needsRefresh = true;

	[arp release];
}

- (void) cpuAction:(CPU::ErrorState)state;
{
	NSAutoreleasePool* arp = [[NSAutoreleasePool alloc] init];

	NSLog(@"cpuAction");
	switch(state) {
		case CPU::OK: 
			[info setStringValue:@""];
			break;
		case CPU::BREAKPOINT_REACHED:
			[info setStringValue:@"Breakpoint reached."];
			[self enableUserEditing:YES];	
			break;
		case CPU::WATCHPOINT_REACHED:
			[info setStringValue:@"Watchpoint reached."];
			[self enableUserEditing:YES];	
			break;
		case CPU::ILLEGAL_INSTRUCTION:
			[info setStringValue:@"CPU halted due to an illegal instruction."];
			[self enableUserEditing:YES];	
			break;
		default:
			assert(0);
	}
	needsRefresh = true;

	[arp release];
}

- (void) driveAttachedAction:(BOOL)connected
{
	NSAutoreleasePool* arp = [[NSAutoreleasePool alloc] init];

	NSLog(@"driveAttachedAction");
	if (connected)
		[greenLED setImage:[NSImage imageNamed:@"LEDgreen"]];
	else
		[greenLED setImage:[NSImage imageNamed:@"LEDgray"]];

	[arp release];
}

- (void) driveDiscAction:(BOOL)inserted
{
	NSAutoreleasePool* arp = [[NSAutoreleasePool alloc] init];
	
	NSLog(@"driveDiscAction");
	[drive setHidden:!inserted];
	[eject setHidden:!inserted];

	[arp release];
}

- (void) driveLEDAction:(BOOL)on
{
	NSAutoreleasePool* arp = [[NSAutoreleasePool alloc] init];
	
	NSLog(@"driveLEDAction");
	if (on)
		[redLED setImage:[NSImage imageNamed:@"LEDred"]];
	else
		[redLED setImage:[NSImage imageNamed:@"LEDgray"]];
	
	[arp release];	
}

- (void) driveDataAction:(BOOL)transfering
{
	NSAutoreleasePool* arp = [[NSAutoreleasePool alloc] init];

	NSLog(@"haltAction");
	// MESSAGE IS NOT SEND YET 
	// don't now how to determine reliably if data is transferred

	[arp release];
}

- (void) driveMotorAction:(BOOL)rotating
{
	NSAutoreleasePool* arp = [[NSAutoreleasePool alloc] init];

	NSLog(@"driveMotorAction");
	debug("Warpload = %d\n", warpLoad);

	if (rotating) {
		[driveBusy setHidden:false];
		[driveBusy startAnimation:self];
	} else {
		[driveBusy stopAnimation:self];
		[driveBusy setHidden:true];
	}
	
	//if (warpLoad)
	//	[c64 cpuSetWarpMode:rotating];
	[arp release];
}

- (void) warpmodeAction:(BOOL)warping
{
	NSAutoreleasePool* arp = [[NSAutoreleasePool alloc] init];
	
	NSLog(@"warpmodeAction");
	if (warping)
		[warpMode setImage:[NSImage imageNamed:@"warpOff"]];
	else
		[warpMode setImage:[NSImage imageNamed:@"warpOn"]];
	
	[arp release];
}

- (void) logAction:(char *)message
{
	NSAutoreleasePool* arp = [[NSAutoreleasePool alloc] init];

	if (consoleController)
		[consoleController insertText:message];

	[arp release];
}

@end
