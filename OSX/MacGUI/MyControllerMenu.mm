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
		
    NSImage *image = [metalScreen screenshot];
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
    
    NSImage *image = [metalScreen screenshot];
    NSData *data = [image TIFFRepresentation];
    [data writeToURL:url atomically:YES];
}

- (IBAction)exportDiskDialog:(id)sender
{
    (void)[self exportDiskDialogWorker:[sender tag]];
}

- (bool)exportDiskDialogWorker:(int)type
{
    VC1541 *floppy = [c64 c64]->floppy;
    D64Archive *diskContents;
    NSArray *fileTypes;
    Archive *target;
    
    // Create archive from drive
    if ((diskContents = D64Archive::archiveFromDrive(floppy)) == NULL) {
        NSLog(@"Cannot create D64 archive from drive");
        return false;
    }
    
    // Determine target format and convert archive
    switch (type) {
            
        case D64_CONTAINER:
            
            NSLog(@"Exporting to D64 format");
            fileTypes = @[@"D64"];
            target = diskContents;
            break;
            
        case T64_CONTAINER:
            
            NSLog(@"Exporting to T64 format");
            fileTypes = @[@"T64"];
            target = T64Archive::archiveFromArchive(diskContents);
            delete diskContents;
            break;
            
        case PRG_CONTAINER:
            
            NSLog(@"Exporting to PRG format");
            fileTypes = @[@"PRG"];
            target = PRGArchive::archiveFromArchive(diskContents);
            // delete diskContents;
            break;
            
        case P00_CONTAINER:
            
            NSLog(@"Exporting to P00 format");
            fileTypes = @[@"P00"];
            target = P00Archive::archiveFromArchive(diskContents);
            delete diskContents;
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
    if ([sPanel runModal] != NSModalResponseOK) {
        delete target;
        return false;
    }
    
    // Export
    NSURL *selectedURL = [sPanel URL];
    NSString *selectedFileURL = [selectedURL absoluteString];
    NSString *selectedFile = [selectedFileURL stringByReplacingOccurrencesOfString:@"file://" withString:@""];
    
    NSLog(@"Exporting to file %@", selectedFile);
    target->writeToFile([selectedFile UTF8String]);
    delete target;
    floppy->disk.setModified(false);
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
    [metalScreen rotateBack];
	[c64 reset];
	[self continueAction:self];
}

- (IBAction)toggleStatusBarAction:(id)sender
{
    NSLog(@"toggleStatusBarAction");
 
    NSUndoManager *undo = [self undoManager];
    [[undo prepareWithInvocationTarget:self] toggleStatusBarAction:sender];
    if (![undo isUndoing]) [undo setActionName:@"Show/Hide status bar"];

    if ([metalScreen drawInEntireWindow])
        [self showStatusBar];
    else
        [self hideStatusBar];
}

- (IBAction)showStatusBar
{
    NSLog(@"showStatusBarAction");
    
    [drive setHidden:![[c64 vc1541] hasDisk]];
    [eject setHidden:![[c64 vc1541] hasDisk]];
    [progress setHidden:NO];
    [cartridgeIcon setHidden:![[c64 expansionport] cartridgeAttached]];
    [cartridgeEject setHidden:![[c64 expansionport] cartridgeAttached]];
    [greenLED setHidden:NO];
    [redLED setHidden:NO];
    [info setHidden:NO];
    [clockSpeed setHidden:NO];
    [clockSpeedBar setHidden:NO];
    [warpMode setHidden:NO];
    
    [metalScreen setDrawInEntireWindow:NO];
}
    
- (IBAction)hideStatusBar
{
    NSLog(@"hideStatusBarAction");
    
    // Hide bottom bar
    [drive setHidden:YES];
    [eject setHidden:YES];
    [progress setHidden:YES];
    [cartridgeIcon setHidden:YES];
    [cartridgeEject setHidden:YES];
    [greenLED setHidden:YES];
    [redLED setHidden:YES];
    [info setHidden:YES];
    [clockSpeed setHidden:YES];
    [clockSpeedBar setHidden:YES];
    [warpMode setHidden:YES];
        
    [metalScreen setDrawInEntireWindow:YES];
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

- (IBAction)runstopRestoreAction:(id)sender
{
    NSLog(@"Rustop Restore combination pressed");
    [[self document] updateChangeCount:NSChangeDone];
    
    [c64 keyboardPressRunstopRestore];
    
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

- (IBAction)ClearKeyAction:(id)sender
{
    NSLog(@"Clear key pressed");

    [[c64 keyboard] pressClearKey];
    sleepMicrosec(100000);
    [[c64 keyboard] releaseClearKey];
}

- (IBAction)HomeKeyAction:(id)sender
{
    NSLog(@"Home key pressed");

    [[c64 keyboard] pressHomeKey];
    sleepMicrosec(100000);
    [[c64 keyboard] releaseHomeKey];
}

- (IBAction)InsertKeyAction:(id)sender
{
    NSLog(@"Insert key pressed");

    [[c64 keyboard] pressInsertKey];
    sleepMicrosec(100000);
    [[c64 keyboard] releaseInsertKey];
}

- (IBAction)LoadDirectoryAction:(id)sender
{
    NSLog(@"LoadDirectoryAction");
    [[self document] updateChangeCount:NSChangeDone];
    [[c64 keyboard] typeText:@"LOAD \"$\",8"];
    [self refresh];
}

- (IBAction)LoadFirstFileAction:(id)sender
{
    NSLog(@"LoadFirstFileAction");
    [[self document] updateChangeCount:NSChangeDone];
    [[c64 keyboard] typeText:@"LOAD \"*\",8,1"];
    [self refresh];
}

- (IBAction)FormatDiskAction:(id)sender
{
    NSLog(@"FormatDiskAction");
    [[self document] updateChangeCount:NSChangeDone];
    [[c64 keyboard] typeText:@"OPEN 1,8,15,\"N:TEST, ID\": CLOSE 1"];
    [self refresh];
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
- (IBAction)dumpC64Joystick1:(id)sender { [[c64 joystick1] dump]; }
- (IBAction)dumpC64Joystick2:(id)sender { [[c64 joystick2] dump]; }
- (IBAction)dumpIEC:(id)sender { [[c64 iec] dump]; }
- (IBAction)dumpC64ExpansionPort:(id)sender { [[c64 expansionport] dump]; }

@end
