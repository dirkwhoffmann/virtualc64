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


@implementation TimeTravelTableView 

- (void)setController:(MyController *)c
{
	controller = c;
	c64 = [c c64];
}

#pragma mark NSTableView

- (void)awakeFromNib {
	
	NSLog(@"TimeTravelTableView::awakeFromNib");
	
	items = [NSMutableArray new];

	// we are our own data source
	[self setDelegate:self];
	[self setDataSource:self];

	// prepare to get click and double click messages
	[self setTarget:self];
	[self setAction:@selector(clickAction:)];
	[self setDoubleAction:@selector(doubleClickAction:)];

	[self reloadData];
}

- (void)dealloc {
	[items release];
	[super dealloc];
}

- (void)keyDown:(NSEvent *)theEvent
{
	if([theEvent keyCode] == 0x24 && [self numberOfSelectedRows] == 1) {
		[self revertAction:self];
	} else {
		[super keyDown:theEvent];		
	}
}

#pragma mark NSTableViewDataSource

- (int)numberOfRowsInTableView:(NSTableView *)tableView 
{	
	return [items count];
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(int)row 
{
	return (row >= 0)  ? [items objectAtIndex:row] : nil;
}

#pragma mark NSTableViewDelegate

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
	NSLog(@"tableViewSelectionDidChange (item %d)", [self selectedRow]);
	
	// Compute info strings
	char buf1[64], buf2[64];
	time_t stamp = [[controller c64] historicSnapshotTimestamp:[self selectedRow]];
	strftime(buf1, sizeof(buf1)-1, "Snapshot taken at %H:%M:%S", localtime(&stamp));
	sprintf(buf2, "%d seconds ago", (int)difftime(setupTime, stamp));
		
	// Display info strings
	[controller updateTimeTravelInfoText:(NSString *)[NSString stringWithUTF8String:buf1]
							  secondText:(NSString *)[NSString stringWithUTF8String:buf2]];
}

- (void)clickAction:(id)sender
{
	NSLog(@"clickAction (item %d)", [sender selectedRow]);
}

- (void)doubleClickAction:(id)sender
{
	NSLog(@"doubleClickAction (item %d)", [sender selectedRow]);
	
	[self revertAction:self];	
}

- (void)revertAction:(id)sender;
{
	NSLog(@"revertAction");

	[[controller c64] revertToHistoricSnapshot:[self selectedRow]];
	[controller timeTravelAction:self];
}

- (void)refresh {
	
	unsigned char *data;
	
	NSLog(@"TimeTravelTableView::setItems");
	
	setupTime = time(NULL);
	[items removeAllObjects];
	
	for (int i = 0; (data = [[controller c64] historicSnapshotImageData:i]) != NULL; i++) {
				
		// Convert data into an NSImage
		int width = VIC::TOTAL_SCREEN_WIDTH;
		int height = VIC::TOTAL_SCREEN_HEIGHT;
		
		// Skip first few rows (upper and lower border should be same size)
		height -= 38;
		data += 38 * 4 * width;
		
		// Create bitmap representation
		NSBitmapImageRep* bmp = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:	&data 
																		pixelsWide: width
																		pixelsHigh: height 
																	 bitsPerSample: 8
																   samplesPerPixel: 4 
																		  hasAlpha: YES
																		  isPlanar: NO
																	colorSpaceName: NSCalibratedRGBColorSpace
																	   bytesPerRow: 4*width
																	  bitsPerPixel: 32];	
		
		// Create NSImage from bitmap representation
		NSImage *image = [[NSImage alloc] initWithSize:[bmp size]];
		[image addRepresentation:bmp];
		[bmp release];
		
		[image setSize:NSMakeSize(120,80)];
		[items addObject:image];
	}
	
	[self reloadData];
}


@end
