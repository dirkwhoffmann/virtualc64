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
#import "VirtualC64-Swift.h"

@implementation MyController(Toolbar) 

#pragma mark NSToolbarItemValidation 

- (BOOL)validateToolbarItem:(NSToolbarItem *)theItem
{
    // NSLog(@"Validating %d...",(int)[theItem tag]);
	
	if ([c64 isRunning]) {
		[[self document] updateChangeCount:NSChangeDone];
	}
	
	// Pause/Continue
	if ([theItem tag] == 1) { 
		if ([c64 isRunning]) {
			[theItem setImage:[NSImage imageNamed:@"pause32"]];
			[theItem setLabel:@"Pause"];
		} else {
			[theItem setImage:[NSImage imageNamed:@"play32"]];
			[theItem setLabel:@"Run"];
		}
		return YES;
	}
	
	// Step into, Step out, Step over
	if ([theItem tag] >= 2 && [theItem tag] <= 4) {
		return ![c64 isRunning] && [c64 isRunnable];
	}
    
    // All other items
    return YES;
}

- (void)validateJoystickItems
{
    /* Jostick port 1 */
    {
        NSMenuItem *item1 = [[joystickPortA menu] itemWithTag:IPD_JOYSTICK_1];
        NSMenuItem *item2 = [[joystickPortA menu] itemWithTag:IPD_JOYSTICK_2];
        [item1 setEnabled:[gamePadManager gamePadSlotIsEmpty:2]];
        [item2 setEnabled:[gamePadManager gamePadSlotIsEmpty:3]];

        NSInteger slotNr = [gamePadManager slotOfGamePadAttachedToPort:[c64 joystickA]];
        [joystickPortA selectItemAtIndex:(slotNr == -1) ? 0 : slotNr + 1];
        
    }
    
    /* Jostick port 2 */
    {
        NSMenuItem *item1 = [[joystickPortB menu] itemWithTag:IPD_JOYSTICK_1];
        NSMenuItem *item2 = [[joystickPortB menu] itemWithTag:IPD_JOYSTICK_2];
        [item1 setEnabled:[gamePadManager gamePadSlotIsEmpty:2]];
        [item2 setEnabled:[gamePadManager gamePadSlotIsEmpty:3]];

        NSInteger slotNr = [gamePadManager slotOfGamePadAttachedToPort:[c64 joystickB]];
        [joystickPortB selectItemAtIndex:(slotNr == -1) ? 0 : slotNr + 1];
     }
}

- (void) setupToolbarIcons
{
    // Joystick selectors
    [joystickPortA selectItemAtIndex:0];
    [joystickPortB selectItemAtIndex:0];
    [self validateJoystickItems];
}


- (void) printDocument:(id) sender
{
    NSLog(@"printDocument");

    // Printing properties
	NSPrintInfo *myPrintInfo = [[self document] printInfo];
    [myPrintInfo setHorizontalPagination:NSFitPagination];
	[myPrintInfo setHorizontallyCentered:YES];
	[myPrintInfo setVerticalPagination:NSFitPagination];
	[myPrintInfo setVerticallyCentered:YES];
	[myPrintInfo setOrientation:NSPaperOrientationLandscape];
	[myPrintInfo setLeftMargin:32.0]; // 32.0
	[myPrintInfo setRightMargin:32.0]; // 32.0
	[myPrintInfo setTopMargin:32.0]; // 32.0
	[myPrintInfo setBottomMargin:32.0]; // 32.0
	
	// Image view
    NSSize paperSize = [myPrintInfo paperSize];
    NSImage *image = [metalScreen screenshot];
	// NSRect printRect = NSMakeRect(0.0, 0.0, [image size].width, [image size].height);
    NSRect printRect = NSMakeRect(0.0, 0.0, paperSize.width, paperSize.height);
    NSImageView *imageView = [[NSImageView alloc] initWithFrame:printRect];
	[imageView setImage:image];
    [imageView setImageScaling:NSImageScaleAxesIndependently];

	// Print image
    NSPrintOperation *printOperation = [NSPrintOperation printOperationWithView:imageView printInfo:myPrintInfo];
    [printOperation runOperationModalForWindow:[[self document] windowForSheet] delegate: nil didRunSelector: NULL contextInfo:NULL];
}

- (IBAction)portAction:(id)sender port:(JoystickProxy *)port
{
    int value = (int)[[sender selectedItem] tag];
    
    switch (value) {
        case IPD_UNCONNECTED:
            [gamePadManager detachGamePadFromPort:port];
            break;
        case IPD_KEYSET_1:
            [gamePadManager attachGamePad:0 toPort:port];
            break;
        case IPD_KEYSET_2:
            [gamePadManager attachGamePad:1 toPort:port];
            break;
        case IPD_JOYSTICK_1:
            [gamePadManager attachGamePad:2 toPort:port];
            break;
        case IPD_JOYSTICK_2:
            [gamePadManager attachGamePad:3 toPort:port];
            break;
            
        default:
            assert(0);
    }
    
    [self validateJoystickItems];
}

- (IBAction)portAAction:(id)sender
{
    [self portAction:sender port:[c64 joystickA]];
}

- (IBAction)portBAction:(id)sender
{
     [self portAction:sender port:[c64 joystickB]];
}

- (IBAction)propertiesAction:(id)sender
{
    [self showPropertiesDialog];
}

- (IBAction)hardwareAction:(id)sender
{
    [self showHardwareDialog];
}

- (IBAction)mediaAction:(id)sender
{
    [self showMediaDialog];
}

- (IBAction)debugOpenAction:(id)sender
{
    [self cheatboxCloseAction:self];
	if ([debugPanel state] == NSDrawerClosedState || [debugPanel state] == NSDrawerClosingState) {
		[debugPanel open];
	}
}

- (IBAction)debugCloseAction:(id)sender
{
	if ([debugPanel state] == NSDrawerOpenState || [debugPanel state] == NSDrawerOpeningState) {
		[debugPanel close];
	}
}

- (IBAction)debugAction:(id)sender
{	
	if ([debugPanel state] == NSDrawerClosedState || [debugPanel state] == NSDrawerClosingState) {
		[self debugOpenAction:self];
	} else {
		[self debugCloseAction:self];
	}
	
	[self refresh];
}

- (IBAction)cheatboxOpenAction:(id)sender
{
    [self debugCloseAction:self];
	if ([cheatboxPanel state] == NSDrawerClosedState || [cheatboxPanel state] == NSDrawerClosingState) {
		[c64 suspend];
		[cheatboxImageBrowserView refresh];
		[cheatboxPanel open];
	}
}

- (IBAction)cheatboxCloseAction:(id)sender
{
	if ([cheatboxPanel state] == NSDrawerOpenState || [cheatboxPanel state] == NSDrawerOpeningState) {
		[c64 resume];
		[cheatboxPanel close];
	}
}

- (IBAction)cheatboxAction:(id)sender
{	
	if ([cheatboxPanel state] == NSDrawerClosedState || [cheatboxPanel state] == NSDrawerClosingState) {
		[self cheatboxOpenAction:self];
	} else {
		[self cheatboxCloseAction:self];
	}
}

// UNUSED
- (IBAction)iPhoneAction:(id)sender
{
    NSLog(@"iPhoneAction");
}

@end
