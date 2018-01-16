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
//                                 General
// --------------------------------------------------------------------------------

- (BOOL)validateMenuItem:(NSMenuItem *)item
{
    if ([item action] == @selector(exportDiskDialog:)) {
        return [[c64 vc1541] hasDisk];
    }

    if ([item action] == @selector(exportFileFromDiskDialog:)) {
        // Possibiliy: Check how many files are present.
        //             Only enable items when a single file is present
        return [[c64 vc1541] hasDisk];
    }

    if ([item action] == @selector(pauseAction:)) {
        return [c64 isRunning];
    }

    if ([item action] == @selector(continueAction:) ||
        [item action] == @selector(stepIntoAction:) ||
        [item action] == @selector(stepOutAction:)  ||
        [item action] == @selector(stepOverAction:) ||
        [item action] == @selector(stopAndGoAction:)) {
        return ![c64 isRunning];
    }

    // View menu
    if ([item action] == @selector(toggleStatusBarAction:)) {
        if (statusBar)
            [item setTitle:@"Hide Status Bar"];
        else
            [item setTitle:@"Show Status Bar"];
        return true;
    }
    
    return YES;
}

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

// --------------------------------------------------------------------------------
//                                  Edit menu
// --------------------------------------------------------------------------------

#pragma mark edit menu

- (IBAction)resetAction:(id)sender
{
	[[self document] updateChangeCount:NSChangeDone];
    [self rotateBack];
	[c64 reset];
	[self continueAction:self];
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

// --------------------------------------------------------------------------------
//                               Keyboard menu
// --------------------------------------------------------------------------------

- (IBAction)runstopAction:(id)sender
{
    NSLog(@"Rustop key pressed");
    [[self document] updateChangeCount:NSChangeDone];
    [[c64 keyboard] pressRunstopKey];
    sleepMicrosec(100000);
    [[c64 keyboard] releaseRunstopKey];
    [self refresh];
}

- (IBAction)shiftRunstopAction:(id)sender
{
    NSLog(@"Shift rustop combination pressed");
    [[self document] updateChangeCount:NSChangeDone];
    [[c64 keyboard] pressShiftRunstopKey];
    sleepMicrosec(100000);
    [[c64 keyboard] releaseShiftRunstopKey];
    [self refresh];
}

- (IBAction)restoreAction:(id)sender
{
    NSLog(@"Restore key pressed");
    [[self document] updateChangeCount:NSChangeDone];
    [[c64 keyboard] pressRestoreKey];
    sleepMicrosec(100000);
    [[c64 keyboard] releaseRestoreKey];
    [self refresh];
}

- (IBAction)runstopRestoreAction:(id)sender
{
    NSLog(@"Rustop Restore combination pressed");
    [[self document] updateChangeCount:NSChangeDone];
    [[c64 keyboard] pressRunstopKey];
    [self restoreAction:sender];
    [[c64 keyboard] releaseRunstopKey];
    [self refresh];
}

- (IBAction)commodoreKeyAction:(id)sender
{
    NSLog(@"Commodore key pressed");
    [[self document] updateChangeCount:NSChangeDone];
    [[c64 keyboard] pressCommodoreKey];
    sleepMicrosec(100000);
    [[c64 keyboard] releaseCommodoreKey];
    [self refresh];
}

- (IBAction)clearKeyAction:(id)sender
{
    // NSLog(@"Clear key pressed");

    [[c64 keyboard] pressClearKey];
    sleepMicrosec(100000);
    [[c64 keyboard] releaseClearKey];
}

- (IBAction)homeKeyAction:(id)sender
{
    // NSLog(@"Home key pressed");

    [[c64 keyboard] pressHomeKey];
    sleepMicrosec(100000);
    [[c64 keyboard] releaseHomeKey];
}

- (IBAction)insertKeyAction:(id)sender
{
    // NSLog(@"Insert key pressed");

    [[c64 keyboard] pressInsertKey];
    sleepMicrosec(100000);
    [[c64 keyboard] releaseInsertKey];
}

- (IBAction)deleteKeyAction:(id)sender
{
    // NSLog(@"Delete key pressed");
    
    [[c64 keyboard] pressDeleteKey];
    sleepMicrosec(100000);
    [[c64 keyboard] releaseDeleteKey];
}

- (IBAction)loadDirectoryAction:(id)sender
{
    NSLog(@"LoadDirectoryAction");
    [[self document] updateChangeCount:NSChangeDone];
    [[c64 keyboard] typeText:@"LOAD \"$\",8"];
    [self refresh];
}

- (IBAction)loadFirstFileAction:(id)sender
{
    NSLog(@"LoadFirstFileAction");
    [[self document] updateChangeCount:NSChangeDone];
    [[c64 keyboard] typeText:@"LOAD \"*\",8,1"];
    [self refresh];
}

- (IBAction)formatDiskAction:(id)sender
{
    NSLog(@"FormatDiskAction");
    [[self document] updateChangeCount:NSChangeDone];
    [[c64 keyboard] typeText:@"OPEN 1,8,15,\"N:TEST, ID\": CLOSE 1"];
    [self refresh];
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

// --------------------------------------------------------------------------------
//                                 Debug menu
// --------------------------------------------------------------------------------

#pragma mark debug menu

- (IBAction)hideSpritesAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] hideSpritesAction:sender];
	if (![undo isUndoing]) [undo setActionName:@"Hide sprites"];
	
	if ([[c64 vic] hideSprites]) {
		[sender setState:NSOffState];
		[[c64 vic] setHideSprites:NO];
	} else {
		[sender setState:NSOnState];
		[[c64 vic] setHideSprites:YES];
	}
}

- (IBAction)markIRQLinesAction:(id)sender
{
	// NSUndoManager *undo = [self undoManager];
	// [[undo prepareWithInvocationTarget:self] markIRQLinesAction:sender];
	// if (![undo isUndoing]) [undo setActionName:@"Mark IRQ lines"];
	
	if ([[c64 vic] showIrqLines]) {
		[sender setState:NSOffState];
		[[c64 vic] setShowIrqLines:NO];
	} else {
		[sender setState:NSOnState];
		[[c64 vic] setShowIrqLines:YES];
	}
}

- (IBAction)markDMALinesAction:(id)sender
{
	// NSUndoManager *undo = [self undoManager];
	// [[undo prepareWithInvocationTarget:self] markDMALinesAction:sender];
	// if (![undo isUndoing]) [undo setActionName:@"Mark DMA lines"];
	
	if ([[c64 vic] showDmaLines]) {
		[sender setState:NSOffState];
		[[c64 vic] setShowDmaLines:NO];
	} else {
		[sender setState:NSOnState];
		[[c64 vic] setShowDmaLines:YES];
	}	
}

- (IBAction)traceC64CpuAction:(id)sender 
{ 
	if ([[c64 cpu] tracingEnabled]) {
		[sender setState:NSOffState];
		[[c64 cpu] setTraceMode:NO];
	} else {
		[sender setState:NSOnState];
		[[c64 cpu] setTraceMode:YES];
	}
}

- (IBAction)traceIecAction:(id)sender
{
	if ([[c64 iec] tracingEnabled]) {
		[sender setState:NSOffState];
		[[c64 iec] setTraceMode:NO];
	} else {
		[sender setState:NSOnState];
		[[c64 iec] setTraceMode:YES];
	}	
}

- (IBAction)traceVC1541CpuAction:(id)sender
{
	if ([[[c64 vc1541] cpu] tracingEnabled]) {
		[sender setState:NSOffState];
		[[[c64 vc1541] cpu] setTraceMode:NO];
	} else {
		[sender setState:NSOnState];
		[[[c64 vc1541] cpu] setTraceMode:YES];
	}	
}

- (IBAction)traceViaAction:(id)sender 
{
	if ([[[c64 vc1541] via:1] tracingEnabled]) {
		[sender setState:NSOffState];
		[[[c64 vc1541] via:1] setTraceMode:NO];
		[[[c64 vc1541] via:2] setTraceMode:NO];		
	} else {
		[sender setState:NSOnState];
		[[[c64 vc1541] via:1] setTraceMode:YES];
		[[[c64 vc1541] via:2] setTraceMode:YES];
	}	
}

- (IBAction)dumpC64:(id)sender { [c64 dump]; }
- (IBAction)dumpC64CPU:(id)sender {	[[c64 cpu] dump]; }
- (IBAction)dumpC64CIA1:(id)sender { [[c64 cia:1] dump]; }
- (IBAction)dumpC64CIA2:(id)sender { [[c64 cia:2] dump]; }
- (IBAction)dumpC64VIC:(id)sender { [[c64 vic] dump]; }
- (IBAction)dumpC64SID:(id)sender { [[c64 sid] dump]; }
- (IBAction)dumpC64Memory:(id)sender { [[c64 mem] dump]; }
- (IBAction)dumpVC1541:(id)sender {	[[c64 vc1541] dump]; }
- (IBAction)dumpVC1541CPU:(id)sender { [[[c64 vc1541] cpu] dump]; }
- (IBAction)dumpVC1541VIA1:(id)sender {	[[[c64 vc1541] via:1] dump]; }
- (IBAction)dumpVC1541VIA2:(id)sender { [[[c64 vc1541] via:2] dump]; }
- (IBAction)dumpVC1541Memory:(id)sender { [[[c64 vc1541] mem] dump]; }
- (IBAction)dumpKeyboard:(id)sender { [[c64 keyboard] dump]; }
- (IBAction)dumpC64JoystickA:(id)sender { [[c64 joystickA] dump]; }
- (IBAction)dumpC64JoystickB:(id)sender { [[c64 joystickB] dump]; }
- (IBAction)dumpIEC:(id)sender { [[c64 iec] dump]; }
- (IBAction)dumpC64ExpansionPort:(id)sender { [[c64 expansionport] dump]; }

@end
