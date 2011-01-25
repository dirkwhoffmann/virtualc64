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
	
	[self setDelegate:self];
	[self setDataSource:self];
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
	
	//[controller revertToSnapshotWithNumber:index];
	[c64 revertToHistoricSnapshot:index];
	[controller cheatboxAction:self];
}

- (void)refresh {
	
	unsigned char *data;
	
	NSLog(@"CheatboxImageBrowserView::refresh");
	
	setupTime = time(NULL);
	[items removeAllObjects];
	
	NSImage *camera = [[NSWorkspace sharedWorkspace] iconForFile:@"/Applications/Image Capture.app"];
	NSImage *glossy = [NSImage imageNamed:@"glossy.png"];

	for (int i = 0; (data = [[controller c64] historicSnapshotImageData:i]) != NULL; i++) {
				
		// Setup time information
		char buf[64];
		time_t stamp = [c64 historicSnapshotTimestamp:i];
		int diff = (int)difftime(setupTime, stamp);

		sprintf(buf, "%d %s ago", diff, diff == 1 ? "second" : "seconds");
		NSString *title = [NSString stringWithUTF8String:buf];

		strftime(buf, sizeof(buf)-1, "taken at %H:%M:%S", localtime(&stamp));
		NSString *subtitle = [NSString stringWithUTF8String:buf];
		
		// Determine texture bounds
		int width, height;
		if ([c64 historicSnapshotIsPAL:i]) {
			width = VIC::PAL_VIEWABLE_PIXELS;
			height = VIC::PAL_VIEWABLE_RASTERLINES;
			// Skip invisible lines
			// TODO: Why do we store invisible lines in screen texture???
			data += VIC::PAL_UPPER_INVISIBLE * 4 * width;
		} else {
			width = VIC::NTSC_VIEWABLE_PIXELS;
			height = VIC::NTSC_VIEWABLE_RASTERLINES;	
			// Skip invisible lines
			// TODO: Why do we store invisible lines in screen texture???
			data += VIC::NTSC_UPPER_INVISIBLE * 4 * width;
		}
					
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
		[camera drawInRect:NSMakeRect(0, height-30, 100, 100) 
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

- (NSUInteger) imageBrowser:(IKImageBrowserView *)browser writeItemsAtIndexes:(NSIndexSet *)itemIndexes toPasteboard:(NSPasteboard *)pboard
{
	// Put number of selected snapshot in pasteboard
	NSUInteger index = [itemIndexes firstIndex];
	
	if (index == NSNotFound) {
		NSLog(@"imageBrowser:writeItemsAtIndexes:NSNotFound (%d)", index);
		return 0;
	}
	//NSLog(@"imageBrowser:writeItemsAtIndexes:%d", index);

	[pboard declareTypes:[NSArray arrayWithObject:NSFileContentsPboardType] owner:self];
	const void *fileContents = [c64 historicSnapshotFileContents:index];
	unsigned fileContentsSize = [c64 historicSnapshotFileContentsSize:index];
	NSData *fileData = [NSData dataWithBytes:fileContents length:fileContentsSize];
	NSFileWrapper *fileWrapper = [[NSFileWrapper alloc] initRegularFileWithContents:fileData];
	[fileWrapper setPreferredFilename:@"Snapshot.VC64"];
	[pboard writeFileWrapper:fileWrapper];
	[fileWrapper release];
	return 1;
}

- (void)draggedImage:(NSImage *)image endedAt:(NSPoint)screenPoint operation:(NSDragOperation)operation
{
	if (operation == NSDragOperationCopy) {
		// Drag n drop operation was successful. Close cheatbox panel
		//NSLog(@"draggedImage:endedAt:operation:NSDragOperationCopy");		
		[controller cheatboxAction:self];
	}
}

@end
