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


@implementation CheatboxImageBrowserView 

- (void)setController:(MyController *)c
{
	controller = c;
	c64 = [c c64];	
}

#pragma mark NSTableView

- (void)awakeFromNib {
	
	NSLog(@"CheatboxImageBrowserView::awakeFromNib");
	
	items = [NSMutableArray new];

	// we are our own data source
	[self setDelegate:self];
	[self setDataSource:self];

	// prepare to get click and double click messages
	//[self setTarget:self];
	//[self setAction:@selector(clickAction:)];
	//[self setDoubleAction:@selector(doubleClickAction:)];

	[self setIntercellSpacing:NSMakeSize(10.0,-40.0)];
	
	[self reloadData];
}

- (void)dealloc {
	[items release];
	[super dealloc];
}

#if 0
- (void)keyDown:(NSEvent *)theEvent
{
	if([theEvent keyCode] == 0x24 && [self numberOfSelectedRows] == 1) {
		[self revertAction:self];
	} else {
		[super keyDown:theEvent];		
	}
}
#endif

#pragma mark Browser Data Source Methods

- (NSUInteger) numberOfItemsInImageBrowser:(IKImageBrowserView *)browser
{	
	return [items count];
}

- (id) imageBrowser:(IKImageBrowserView *) aBrowser itemAtIndex:(NSUInteger)index
{
	return [items objectAtIndex:index];
}


#pragma mark NSTableViewDelegate

#if 0
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
#endif

- (void)clickAction:(id)sender
{
	NSLog(@"clickAction (item %d)", [sender selectedRow]);
}

- (void)doubleClickAction:(id)sender
{
	NSLog(@"doubleClickAction (item %d)", [sender selectedRow]);
	
	// [self revertAction:self];	
}

#if 0
- (void)revertAction:(id)sender;
{
	NSLog(@"revertAction");

	[[controller c64] revertToHistoricSnapshot:[self selectedRow]];
	[controller timeTravelAction:self];
}
#endif

- (void)refresh {
	
	unsigned char *data;
	
	NSLog(@"CheatboxImageBrowserView::refresh");
	
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
		
	    // Enhance image with some overlays 
		NSImage *final = [[NSImage alloc] initWithSize:NSMakeSize(width, height+30)];
		[final lockFocus];

		[image drawInRect:NSMakeRect(0, 0, width, height) 
				  fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];

		NSImage *glossy = [NSImage imageNamed:@"glossy.png"];
		[glossy drawInRect:NSMakeRect(0, 0, width, height) 
				  fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];

		NSImage *pin = [NSImage imageNamed:@"pin.tiff"];
		[pin drawInRect:NSMakeRect(width/2, height-30, 48, 60) 
			   fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];

		[final unlockFocus];

		CheatboxItem *item = [[CheatboxItem alloc] initWithImage:final imageID:@"imageID" imageTitle:@"title" imageSubtitle:@"subtitle"];
		[items addObject:item];
	}
	
	[self reloadData];
}

- (IKImageBrowserCell *)newCellForRepresentedItem:(id)cell
{
	return [[CheatboxImageBrowserCell alloc] init];
}


@end
