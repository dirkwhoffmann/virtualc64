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

/*
- (IBAction)exportDiskDialog:(id)sender
{
    (void)[self exportDiskDialogWorker:[sender tag]];
}

- (bool)exportDiskDialogWorker:(long)type
{
    D64ArchiveProxy *newDiskContents;
    NSArray *fileTypes;
    ArchiveProxy *target;

    // Create archive from drive
    // if ((newDiskContents = [[c64 vc1541] convertToD64]) == nil) {
    if ((newDiskContents = [D64ArchiveProxy archiveFromVC1541:[c64 vc1541]]) == nil) {
        NSLog(@"Cannot create D64 archive from drive");
        return false;
    }
    
    // Determine target format and convert archive
    switch (type) {
            
        case D64_CONTAINER:
            
            NSLog(@"Exporting to D64 format");
            fileTypes = @[@"D64"];
            target = newDiskContents;
            break;
            
        case T64_CONTAINER:
            
            NSLog(@"Exporting to T64 format");
            fileTypes = @[@"T64"];
            target = [T64ArchiveProxy archiveFromArchive:newDiskContents];
            break;
            
        case PRG_CONTAINER:
            
            NSLog(@"Exporting to PRG format");
            fileTypes = @[@"PRG"];
            target = [PRGArchiveProxy archiveFromArchive:newDiskContents];
            break;
            
        case P00_CONTAINER:
            
            NSLog(@"Exporting to P00 format");
            fileTypes = @[@"P00"];
            target = [P00ArchiveProxy archiveFromArchive:newDiskContents];
            break;
            
        default:
            assert(0);
            return false;
    }
    
    // Create panel
    NSSavePanel* sPanel = [NSSavePanel savePanel];
    [sPanel setCanSelectHiddenExtension:YES];
    [sPanel setAllowedFileTypes:fileTypes];
    
    // Show panel
    if ([sPanel runModal] != NSModalResponseOK)
        return false;
    
    // Export
    NSURL *selectedURL = [sPanel URL];
    NSString *selectedFileURL = [selectedURL absoluteString];
    NSString *selectedFile = [selectedFileURL stringByReplacingOccurrencesOfString:@"file://" withString:@""];
    
    NSLog(@"Exporting to file %@", selectedFile);
    [target writeToFile:selectedFile];
    [[[c64 vc1541] disk] setModified:NO];
    return true;
}

- (IBAction)exportFileFromDiskDialog:(id)sender
{
    [self exportDiskDialog: sender];
}
*/
 
// --------------------------------------------------------------------------------
//                                  Edit menu
// --------------------------------------------------------------------------------

#pragma mark edit menu

- (IBAction)resetAction:(id)sender
{
    [[self document] updateChangeCount:NSChangeDone];
    
    [self rotateBack];
    [c64 reset];
    [self refresh];
}

- (IBAction)toggleStatusBarAction:(id)sender
{
    NSLog(@"toggleStatusBarAction");
 
    NSUndoManager *undo = [self undoManager];
    [[undo prepareWithInvocationTarget:self] toggleStatusBarAction:sender];
    if (![undo isUndoing]) [undo setActionName:@"Show/Hide status bar"];

    if (statusBar)
        [self hideStatusBar];
    else
        [self showStatusBar];
}

- (IBAction)showStatusBar
{
    NSLog(@"showStatusBarAction");
 
    if (statusBar)
        return
        
    [greenLED setHidden:NO];
    [redLED setHidden:NO];
    [progress setHidden:NO];
    [tapeProgress setHidden:NO];
    [driveIcon setHidden:![[c64 vc1541] hasDisk]];
    [driveEject setHidden:![[c64 vc1541] hasDisk]];
    [tapeIcon setHidden:![[c64 datasette] hasTape]];
    [tapeEject setHidden:![[c64 datasette] hasTape]];
    [cartridgeIcon setHidden:![[c64 expansionport] cartridgeAttached]];
    [cartridgeEject setHidden:![[c64 expansionport] cartridgeAttached]];
    [info setHidden:NO];
    [clockSpeed setHidden:NO];
    [clockSpeedBar setHidden:NO];
    [warpIcon setHidden:NO];
    
    [self shrink];
    [[self window] setContentBorderThickness:24 forEdge: NSMinYEdge];
    [self adjustWindowSize];
    statusBar = YES;
}
    
- (IBAction)hideStatusBar
{
    NSLog(@"hideStatusBarAction");
    
    if (!statusBar)
        return
        
    // Hide bottom bar
    [greenLED setHidden:YES];
    [redLED setHidden:YES];
    [progress setHidden:YES];
    [tapeProgress setHidden:YES];
    [driveIcon setHidden:YES];
    [driveEject setHidden:YES];
    [tapeIcon setHidden:YES];
    [tapeEject setHidden:YES];
    [cartridgeIcon setHidden:YES];
    [cartridgeEject setHidden:YES];
    [info setHidden:YES];
    [clockSpeed setHidden:YES];
    [clockSpeedBar setHidden:YES];
    [warpIcon setHidden:YES];
    
    [self expand];
    [[self window] setContentBorderThickness:0 forEdge: NSMinYEdge];
    [self adjustWindowSize];
    statusBar = NO;
}





- (IBAction)datasetteEjectAction:(id)sender
{
    NSLog(@"datasetteEjectAction");
    [[self document] updateChangeCount:NSChangeDone];
    [[c64 datasette] ejectTape];
}

- (IBAction)datasettePressPlayAction:(id)sender
{
    NSLog(@"datasettePressPlayAction");
    [[self document] updateChangeCount:NSChangeDone];
    [[c64 datasette] pressPlay];
}

- (IBAction)datasettePressStopAction:(id)sender
{
    NSLog(@"datasettePressStopAction");
    [[self document] updateChangeCount:NSChangeDone];
    [[c64 datasette] pressStop];
}

@end
