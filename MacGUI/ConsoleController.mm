/*
 * (C) 2006-2008 Dirk W. Hoffmann. All rights reserved.
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
	int i;
	
	self = [super initWithWindowNibName:@"Console"];

	pthread_mutex_init(&ringbuffer_lock, NULL);
	writePtr = 0;
	
	for (i = 0; i < MAX_CONSOLE_ENTRIES; i++)
		ringbuffer[i] = NULL;
		
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
	// [log setString:@"Welcome to Virtual C64\n"];
	// [log insertText:[NSString stringWithFormat:@"Build %d\n", [c64 buildNr]]];
}

- (IBAction)traceCPUAction:(id)sender
{	
	if ([c64 cpuTracingEnabled]) {
		[c64 cpuSetTraceMode:NO];
		[console reloadData];
		[console setNeedsDisplay:YES];
	} else {
		[c64 cpuSetTraceMode:YES];
	}
}

- (IBAction)traceIECAction:(id)sender
{	
	if ([c64 iecTracingEnabled]) {
		[c64 iecSetTraceMode:NO];
		[console reloadData];
		[console setNeedsDisplay:YES];
	} else {
		[c64 iecSetTraceMode:YES];
		NSLog(@"IEC tracing enabled");
	}
}

- (IBAction)traceDriveCPUAction:(id)sender
{	
	if ([c64 driveCPUTracingEnabled]) {
		[c64 driveSetCPUTraceMode:NO];
		[console reloadData];
		[console setNeedsDisplay:YES];
	} else {
		[c64 driveSetCPUTraceMode:YES];
		NSLog(@"Floppy CPU tracing enabled");
	}
}

- (IBAction)traceDriveDataAction:(id)sender
{	
	if ([c64 driveDataTracingEnabled]) {
		[c64 driveSetDataTraceMode:NO];
		[console reloadData];
		[console setNeedsDisplay:YES];
	} else {
		[c64 driveSetDataTraceMode:YES];
		NSLog(@"Floppy data tracing enabled");
	}
}

- (IBAction)dumpCPUAction:(id)sender
{
	NSLog(@"Dump CPU");
	[c64 dumpCPU];
	[console reloadData];
	[console setNeedsDisplay:YES];
}

- (IBAction)dumpCIAAction:(id)sender
{
	NSLog(@"Dump CIA");
	[c64 dumpCIA];
	[console reloadData];
	[console setNeedsDisplay:YES];
}

- (IBAction)dumpVICAction:(id)sender
{
	NSLog(@"Dump VIC");
	[c64 dumpVIC];
	[console reloadData];
	[console setNeedsDisplay:YES];
}

- (IBAction)dumpIECAction:(id)sender
{
	NSLog(@"Dump IEC");
	[c64 dumpIEC];
	[console reloadData];
	[console setNeedsDisplay:YES];
}

- (IBAction)dumpMemoryAction:(id)sender
{
	NSLog(@"Dump Memory");
	[c64 dumpMemory];
	[console reloadData];
	[console setNeedsDisplay:YES];
}

- (IBAction)dumpDriveAction:(id)sender
{
	NSLog(@"Dump Drive");
	[c64 dumpDrive];
	[console reloadData];
	[console setNeedsDisplay:YES];
}

- (IBAction)refreshAction:(id)sender		
{		
	[console reloadData];		
	[console setNeedsDisplay:YES];
	// debug("Last entry: %s\n", ringbuffer[writePtr-1]);		
}
	
- (IBAction)clearAction:(id)sender
{
	// [log setString:@""];
	int i;		
		
	pthread_mutex_lock(&ringbuffer_lock);		
	writePtr = 0;		
	for (i = 0; i < MAX_CONSOLE_ENTRIES; i++)		
		ringbuffer[i] = NULL;		
	pthread_mutex_unlock(&ringbuffer_lock);		
		
	[console reloadData];		
	[bgImage setNeedsDisplay:YES];
}

- (IBAction)rasterlineAction:(id)sender
{
	[c64 vicToggleMarkIRQLines];
}

- (IBAction)hideSpritesAction:(id)sender
{
	[c64 vicToggleDrawSprites];
}

- (int)numberOfRowsInTableView:(NSTableView *)aTableView		
{		
	int result;		
		
	pthread_mutex_lock(&ringbuffer_lock);		
	if (ringbuffer[writePtr] != NULL)		
		result = MAX_CONSOLE_ENTRIES; // buffer already filled		
	else		
		result = writePtr;		
	pthread_mutex_unlock(&ringbuffer_lock);		
		
	return result;		
}		
	
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)row		
{		
	int index;		
	NSString *result;		
		
	pthread_mutex_lock(&ringbuffer_lock);		
	if (ringbuffer[writePtr] == NULL)		
		index = row;		
	else		
		index = (writePtr + row) % MAX_CONSOLE_ENTRIES;		
	result = [NSString stringWithFormat:@"%s", ringbuffer[index]];		
		
	if (row == 999)		
		NSLog(result);		
		
	pthread_mutex_unlock(&ringbuffer_lock);		
		
	return result;		
}		
		
- (void)tableView: (NSTableView *)aTableView willDisplayCell: (id)aCell forTableColumn: (NSTableColumn *)aTableColumn row: (int)row		
{		
	[aCell setTextColor:[NSColor blackColor]];		
}		
		
- (void)_insertText:(char *)message		
{		
	if (ringbuffer[writePtr]) {		
		free(ringbuffer[writePtr]);		
	}		
	ringbuffer[writePtr] = message;		
	
	writePtr++;		
	if (writePtr == MAX_CONSOLE_ENTRIES)		
		writePtr = 0;		
}

- (void)_appendText:(char *)message	maxLength:(int)len
{		
	char *str;
	int ptr;
	
	if (writePtr == 0)
		ptr = MAX_CONSOLE_ENTRIES-1;
	else 
		ptr = writePtr - 1;

	if (!ringbuffer[ptr] || (strlen(ringbuffer[ptr]) + strlen(message) > len)) {		
		[self _insertText:message];
		return;
	}
	
	str = (char *)malloc(strlen(ringbuffer[ptr]) + strlen(message) + 1);
	strcpy(str, ringbuffer[ptr]);
	strcat(str, message);
	free(ringbuffer[ptr]);
	ringbuffer[ptr] = str;
}

- (void)insertText:(char *)message
{
	pthread_mutex_lock(&ringbuffer_lock);		
	[self _insertText:message];		
	pthread_mutex_unlock(&ringbuffer_lock);		
		
	[console reloadData];
	[console setNeedsDisplay:YES];
}

- (void)appendText:(char *)message maxLength:(int)len
{
	pthread_mutex_lock(&ringbuffer_lock);		
	[self _appendText:message maxLength:len];		
	pthread_mutex_unlock(&ringbuffer_lock);		
		
	[console reloadData];
	[console setNeedsDisplay:YES];
}

#if 0
- (void)_insertText:(NSString *)message
{
	[log insertText:message];	
}

- (void)insertText:(char *)message
{
	[self performSelectorOnMainThread:@selector(_insertText:) withObject:[NSString stringWithUTF8String:message] waitUntilDone:NO];
}
#endif

@end
