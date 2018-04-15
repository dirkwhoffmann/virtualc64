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

@implementation MyController(DebugPanel) 

- (IBAction)stepIntoAction:(id)sender
{
	[[self document] updateChangeCount:NSChangeDone];
	[c64 step];
	[self refresh];
}

- (IBAction)stepOutAction:(id)sender
{	
	[[self document] updateChangeCount:NSChangeDone];
	
	// Get return address from callstack
	int addr = [[c64 cpu] topOfCallStack];
	if (addr < 0)
		return;
	
	// Set soft breakpoint at next command and run
	[[c64 cpu] setSoftBreakpoint:(addr+1)];	
	[c64 run];
}


- (IBAction)stepOverAction:(id)sender
{	
	[[self document] updateChangeCount:NSChangeDone];
	// Is the next instruction a JSR instruction?
	uint8_t opcode = [[c64 cpu] readPC];
	if (opcode == 0x20) {
		// set soft breakpoint at next command
		[[c64 cpu] setSoftBreakpoint:[[c64 cpu] addressOfNextInstruction]];	
		[c64 run];
	} else {
		// same as step
		[self stepIntoAction:self];
	}
}

- (IBAction)stopAndGoAction:(id)sender
{	
	[[self document] updateChangeCount:NSChangeDone];
	
	NSLog(@"Stop and go action");
	if ([c64 isHalted]) {
		[c64 run];
	} else {
		[c64 halt];
		[debugPanel open];
	}
	
	[self refresh];
}

- (IBAction)pauseAction:(id)sender
{	
	if ([c64 isRunning]) {
		[c64 halt];
		[debugPanel open];
	}
	
	[self refresh];
}

- (IBAction)continueAction:(id)sender
{	
	[[self document] updateChangeCount:NSChangeDone];
	
	if ([c64 isHalted])
		[c64 run];
	
	[self refresh];
}

- (IBAction)setDecimalAction:(id)sender
{
    hex = NO;
    
	Formatter *bF = [[Formatter alloc] init:DECIMAL_FORMATTER inFormat:@"[0-9]{0,3}" outFormat:@"%03d"];
	Formatter *sF = [[Formatter alloc] init:DECIMAL_FORMATTER inFormat:@"[0-9]{0,3}" outFormat:@"%03d"];
	Formatter *wF = [[Formatter alloc] init:DECIMAL_FORMATTER inFormat:@"[0-9]{0,5}" outFormat:@"%05d"];
	
	[self refresh:bF word:wF threedigit:sF];
	
}

- (IBAction)setHexadecimalAction:(id)sender
{
    hex = YES;
    
	Formatter *bF  = [[Formatter alloc] init:HEXADECIMAL_FORMATTER inFormat:@"[0-9,a-f,A-F]{0,2}" outFormat:@"%02X"];
	Formatter *sF  = [[Formatter alloc] init:HEXADECIMAL_FORMATTER inFormat:@"[0-9,a-f,A-F]{0,3}" outFormat:@"%03X"];
	Formatter *wF  = [[Formatter alloc] init:HEXADECIMAL_FORMATTER inFormat:@"[0-9,a-f,A-F]{0,4}" outFormat:@"%04X"];
	
	[self refresh:bF word:wF threedigit:sF];
	
}

@end
