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

#import "MyDocument.h"


@implementation TimeTravelTableView 

- (void)awakeFromNib {
	
	NSLog(@"TimeTravelTableView::awakeFromNib");
	
	items = [NSMutableArray new];
	[self setDelegate:self];
	[self setDataSource:self];

	[self setTarget:self];
	[self setDoubleAction:@selector(doubleClickAction:)];
	[self setAction:@selector(clickAction:)];

	[self reloadData];
}

- (void)dealloc {
	[items release];
	[super dealloc];
}

- (NSMutableArray *)items {
	return items;
}

- (id)selectedRowItemforColumnIdentifier:(NSString *)anIdentifier {
	if ([self selectedRow] != -1)
		return [[items objectAtIndex:[self selectedRow]] objectForKey:anIdentifier];
	
	return nil;
}

- (void)setItems:(MyDocument *)doc {
	
	V64Snapshot *s;
	
	NSLog(@"TimeTravelTableView::setItems");

	mydoc = doc;
	setupTime = time(NULL);
	[items removeAllObjects];
	
	for (int i = 0; (s = [[mydoc c64] historicSnapshot:i]) != nil; i++) {
				
		// Get pointer to raw image data
		unsigned char *data = [s imageData];
		assert(data != NULL);
		
		// Convert data into an NSImage
		int width = VIC::TOTAL_SCREEN_WIDTH;
		int height = VIC::TOTAL_SCREEN_HEIGHT;
		
		// Skip first few rows (upper and lower border should be same size)
		height -= 38;
		data += 38 * 4 * width;
		
		// Create bitmap representation
		NSBitmapImageRep* bmp = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:&data 
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
		[image addRepresentation: bmp];
		[bmp release];

		[image setSize:NSMakeSize(120,80)];
		[items addObject:image];
	}

	[self reloadData];
}

- (void)addRow:(NSDictionary *)item {
	[items insertObject:item atIndex:[items count]];
	[self reloadData];
}

- (void)removeRow:(unsigned)row {
	[items removeObjectAtIndex:row];
	[self reloadData];
}

- (int)numberOfRowsInTableView:(NSTableView *)tableView {
	
	NSLog(@"TimeTravelTableView::numberOfRowsInTableView");

	return [items count];
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(int)row {
	
	// NSLog(@"TimeTravelTableView::objectValueForTableColumn:%d", row);

	if (row != -1)
		return [items objectAtIndex:row];
	
	return nil;
}

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
	NSLog(@"tableViewSelectionDidChange (item %d)", [self selectedRow]);
	
	V64Snapshot *s = [[mydoc c64] historicSnapshot:[self selectedRow]];
	assert(s != NULL);

	// Compute info strings
	char buf1[64], buf2[64];
	time_t stamp = [s timeStamp];
	strftime(buf1, sizeof(buf1)-1, "Snapshot taken at %H:%M:%S", localtime(&stamp));
	sprintf(buf2, "%d seconds ago", (int)difftime(setupTime, stamp));
		
	// Display info strings
	[mydoc updateTimeTravelInfoText:(NSString *)[NSString stringWithUTF8String:buf1]
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

	V64Snapshot *s = [[mydoc c64] historicSnapshot:[self selectedRow]];

	if (s) {
		[s writeDataToC64:[mydoc c64]];
		[mydoc timeTravelAction:self];
	}
}

- (void)keyDown:(NSEvent *)theEvent
{
	if([theEvent keyCode] == 0x24 && [self numberOfSelectedRows] == 1) {
		[self revertAction:self];
	} else {
		[super keyDown:theEvent];		
	}
}

@end
