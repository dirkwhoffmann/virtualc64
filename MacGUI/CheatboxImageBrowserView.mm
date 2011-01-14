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

	//[self setIntercellSpacing:NSMakeSize(10.0,-40.0)];
	[self setIntercellSpacing:NSMakeSize(10.0,-20.0)];
	
	[self reloadData];
}

- (void)dealloc {
	[items release];
	[super dealloc];
}

#pragma mark Browser Data Source Methods

- (NSUInteger) numberOfItemsInImageBrowser:(IKImageBrowserView *)aBrowser
{	
	return [items count];
}

- (id) imageBrowser:(IKImageBrowserView *)aBrowser itemAtIndex:(NSUInteger)index
{
	return [items objectAtIndex:index];
}

-(void) imageBrowser:(IKImageBrowserView *)aBrowser cellWasDoubleClickedAtIndex:(NSUInteger)index
{
	NSLog(@"doubleClickAction (item %d)", index);
	[controller revertAction:index];
}

- (void)refresh {
	
	unsigned char *data;
	
	NSLog(@"CheatboxImageBrowserView::refresh");
	
	setupTime = time(NULL);
	[items removeAllObjects];
	
	NSImage *tmIcon = [[NSWorkspace sharedWorkspace] iconForFile:@"/Applications/Image Capture.app"];
	NSImage *glossy = [NSImage imageNamed:@"glossy.png"];
	// NSImage *pin = [NSImage imageNamed:@"pin.png"];

	for (int i = 0; (data = [[controller c64] historicSnapshotImageData:i]) != NULL; i++) {
				
		// Time information
		char buf[64]; // , subtitle[64];
		time_t stamp = [c64 historicSnapshotTimestamp:i];
		int diff = (int)difftime(setupTime, stamp);

		sprintf(buf, "%d %s ago", diff, diff == 1 ? "second" : "seconds");
		NSString *title = [NSString stringWithUTF8String:buf];

		strftime(buf, sizeof(buf)-1, "taken at %H:%M:%S", localtime(&stamp));
		NSString *subtitle = [NSString stringWithUTF8String:buf];
		
		// Determine texture bounds
		int width = VIC::TOTAL_SCREEN_WIDTH;
		int height = VIC::TOTAL_SCREEN_HEIGHT;
		
		// We skip a couple of rows (upper and lower border should be same size)
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
		NSImage *final = [[NSImage alloc] initWithSize:NSMakeSize(width, height+70)];
		[final lockFocus];
		[image drawInRect:NSMakeRect(0, 0, width, height) 
				 fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];		
		[glossy drawInRect:NSMakeRect(0, 0, width, height) 
				  fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
		[tmIcon drawInRect:NSMakeRect(0, height-30, 100, 100) 
				  fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
		[final unlockFocus];
		
		CheatboxItem *item = [[CheatboxItem alloc] initWithImage:final 
														imageUID:subtitle
													  imageTitle:title
												   imageSubtitle:subtitle];
		[items addObject:item];
	}
	
	[self reloadData];
}

- (IKImageBrowserCell *)newCellForRepresentedItem:(id)cell
{
	return [[CheatboxImageBrowserCell alloc] init];
}

#pragma mark Drag'n drop

- (unsigned int)dragginSourceOperationMaskForLocal:(BOOL)isLocal
{
	return NSDragOperationMove;
}

- (void)mouseDragged:(NSEvent *)event
{
	NSLog(@"mouseDragged");
	
	// Get location of drag event
	NSPoint imageLoc = [self convertPoint:[event locationInWindow] fromView:nil];

	// Get index of dragged item
	NSUInteger index = (NSUInteger)[self indexOfItemAtPoint:imageLoc];
	if (index >= [items count])
		return;
	
	NSLog(@"Dragging item %d", index);
	
	// Get image for item
	NSImage *anImage = [[items objectAtIndex:index] image];

	// Scale image to correct size and adjust drag position
	NSSize s = [[self cellForItemAtIndex:index] imageFrame].size;	
	[anImage setSize:s];
	imageLoc.x -= s.width / 2;			
	imageLoc.y -= s.height / 2;			
	
	// Get pasteboard
	NSPasteboard *pboard = [NSPasteboard pasteboardWithName:NSDragPboard];
	
	// Put number of selected snapshot in pasteboard
	[pboard declareTypes:[NSArray arrayWithObject:NSPasteboardTypeString] owner:self];
	[pboard setString:[NSString stringWithFormat:@"%d", index] forType:NSStringPboardType];

	// Start dragging
	[self dragImage:anImage 
				 at:imageLoc
			 offset:NSMakeSize(0.0,0.0)
			  event:event 
		 pasteboard:pboard 
			 source:self
		  slideBack:YES];
}
@end
