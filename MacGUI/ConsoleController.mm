/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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

#import "ConsoleController.h"


@implementation ConsoleController


- (id)init
{
//	int i;
	
	self = [super initWithWindowNibName:@"Console"];

#if 0	
	pthread_mutex_init(&ringbuffer_lock, NULL); 
	writePtr = 0;
		
	for (i = 0; i < MAX_CONSOLE_ENTRIES; i++) 
		ringbuffer[i] = NULL;
#endif

	return self;
}
	
- (void)setC64:(C64Proxy *)proxy
{
	c64 = proxy;
}

- (void)setDoc:(MyDocument *)doc
{
	mydoc = doc;
}

- (void)awakeFromNib 
{
	[log setString:@"Welcome to Virtual C64\n"];
	[log insertText:[NSString stringWithFormat:@"Build %d\n", [c64 buildNr]]];
}

- (IBAction)traceCPUAction:(id)sender
{	
	if ([c64 cpuTracingEnabled]) {
		[c64 cpuSetTraceMode:NO];
	} else {
		[c64 cpuSetTraceMode:YES];
	}
}

- (IBAction)traceIECAction:(id)sender
{	
	if ([c64 iecTracingEnabled]) {
		[c64 iecSetTraceMode:NO];
	} else {
		[c64 iecSetTraceMode:YES];
		NSLog(@"IEC tracing enabled");
	}
}

- (IBAction)dumpCPUAction:(id)sender
{
	NSLog(@"Dump CPU");
	[c64 dumpCPU];
}

- (IBAction)dumpCIAAction:(id)sender
{
	NSLog(@"Dump CIA");
	[c64 dumpCIA];
}

- (IBAction)dumpVICAction:(id)sender
{
	NSLog(@"Dump VIC");
	[c64 dumpVIC];
}

- (IBAction)dumpIECAction:(id)sender
{
	NSLog(@"Dump IEC");
	[c64 dumpIEC];
}

- (IBAction)dumpMemoryAction:(id)sender
{
	NSLog(@"Dump Memory");
	[c64 dumpMemory];
}

- (IBAction)dumpDriveAction:(id)sender
{
	NSLog(@"Dump Drive");
	[c64 dumpDrive];
}

	
- (IBAction)clearAction:(id)sender
{
	[log setString:@""];
}

- (IBAction)rasterlineAction:(id)sender
{
	[c64 vicToggleMarkIRQLines];
}

- (IBAction)hideSpritesAction:(id)sender
{
	[c64 vicToggleDrawSprites];
}

- (void)insertText:(char *)message
{
	[log insertText:[NSString stringWithUTF8String:message]];
}

@end
