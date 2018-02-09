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
#import <VirtualC64-Swift.h>

@implementation MyController(Menu) 


// --------------------------------------------------------------------------------
//                                 File menu
// --------------------------------------------------------------------------------

#pragma mark file menu

#if 0
- (IBAction)saveScreenshotDialog:(id)sender
{
    NSArray *fileTypes = @[@"tif", @"jpg", @"gif", @"png", @"psd", @"tga"];
	
	// Create panel
	NSSavePanel* sPanel = [NSSavePanel savePanel];
	[sPanel setCanSelectHiddenExtension:YES];
	[sPanel setAllowedFileTypes:fileTypes];
    
    // Show panel
	if ([sPanel runModal] != NSModalResponseOK)
        return;
    
    // Export
    NSURL *url = [sPanel URL];
    NSLog(@"Saving screenshot to file %@", url);
		
    NSImage *image = [self screenshot];
    NSData *data = [image TIFFRepresentation];
    [data writeToURL:url atomically:YES];
}

- (IBAction)quicksaveScreenshot:(id)sender
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains (NSDesktopDirectory, NSUserDomainMask, YES);
    NSString *desktopPath = [paths objectAtIndex:0];
    NSString *filePath = [desktopPath stringByAppendingPathComponent:@"Untitled.png"];
    NSURL *url = [NSURL fileURLWithPath:filePath];
    
    NSLog(@"Quicksaving screenshot to file %@", url);
    
    NSImage *image = [self screenshot];
    NSData *data = [image TIFFRepresentation];
    [data writeToURL:url atomically:YES];
}

#endif

@end
