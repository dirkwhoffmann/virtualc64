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

@implementation MyController(Toolbar) 

#pragma mark NSToolbarItemValidation 

- (BOOL)validateToolbarItem:(NSToolbarItem *)theItem
{
    NSLog(@"Validating %d...",[theItem tag]);
	/* */
	if ([c64 isRunning]) {
		[[self document] updateChangeCount:NSChangeDone];
	}
	
	/* Pause/Continue */
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
	
	/* Step into, Step out, Step over */
	if ([theItem tag] >= 2 && [theItem tag] <= 4) {
		return ![c64 isRunning] && [c64 isRunnable];
	}

    /* Jostick port 1 */
    /* Problem: Only image type items receive validation messages
       For now, we always validate */
    // if ([theItem tag] == 10)
    {
        NSMenuItem *item1 = [portA itemWithTag:4];
        NSMenuItem *item2 = [portA itemWithTag:5];
        [item1 setEnabled:[c64 joystickOneIsActive]];
        [item2 setEnabled:[c64 joystickTwoIsActive]];
        [[portA itemWithTag:[c64 portAssignment:0]] setState:1];
    }
    
    /* Jostick port 2 */
    /* Problem: Only image type items receive validation messages
     For now, we always validate */
    // if ([theItem tag] == 10)
    {
        NSMenuItem *item1 = [portB itemWithTag:4];
        NSMenuItem *item2 = [portB itemWithTag:5];
        [item1 setEnabled:[c64 joystickOneIsActive]];
        [item2 setEnabled:[c64 joystickTwoIsActive]];
        [[portB itemWithTag:[c64 portAssignment:1]] setState:1];
    }

    
#if 0
	/* Jostick port */
	if ([theItem tag] == 10 || [theItem tag] == 11) { 

		int port = ([theItem tag] == 10) ? [c64 portAssignment:0] : [c64 portAssignment:1];
		switch (port) {
			case IPD_KEYBOARD_1:
				[theItem setImage:[NSImage imageNamed:@"keyboard32"]];
				return YES;
			case IPD_JOYSTICK_1:
				[theItem setImage:[NSImage imageNamed:@"joystick1_32"]];
				return YES;
			case IPD_JOYSTICK_2:
				[theItem setImage:[NSImage imageNamed:@"joystick2_32"]];
				return YES;
			case IPD_UNCONNECTED:
				[theItem setImage:[NSImage imageNamed:@"none_32"]];
				return YES;
			default:
				assert(0);
		}
	}	
#endif 
    
#if 0
    /* Drive icon */
 	if ([theItem tag] == 88) 
    { 
        NSImage *background, *foreground;
        
        bool isConnected = [[c64 iec] isDriveConnected];
        bool showsRed = [[c64 vc1541] hasRedLED];
        bool hasDisk = [[c64 vc1541] hasDisk];

        NSLog(@"Update drive icon %d", showsRed);

        background = hasDisk ? [NSImage imageNamed:@"diskette"] : [NSImage imageNamed:@"drive48"];
        foreground = isConnected ? (showsRed ? [NSImage imageNamed:@"LEDgr"] : [NSImage imageNamed:@"LEDgb"]) : [NSImage imageNamed:@"LEDbb"];
        
        [background lockFocus];
        // PROBLEM: ICONS DO NOT UPDATE FAST ENOUGH, SO THIS APPROACH DOES NOT SEEM TO WORK
        // [foreground compositeToPoint:NSMakePoint(0,0) operation:NSCompositeSourceOver];
        [background unlockFocus];
        [theItem setImage:background];
        
        return YES;
    }
#endif
    
    /* All other items */
    return YES;
}

- (void) setupToolbarIcons
{
    /*
	NSImage *tmIcon = [[NSWorkspace sharedWorkspace] iconForFile:@"/Applications/Time Machine.app"];
    if (tmIcon)
        [cheatboxIcon setImage:tmIcon];

    NSImage *amIcon = [[NSWorkspace sharedWorkspace] iconForFile:@"/Applications/Utilities/Activity Monitor.app"];
    if (amIcon)
        [inspectIcon setImage:amIcon];

    NSImage *prIcon = [[NSWorkspace sharedWorkspace] iconForFile:@"/Applications/System Preferences.app"];
    if (amIcon)
        [preferencesIcon setImage:prIcon];
*/
}


- (void) printDocument:(id) sender
{
    NSLog(@"printDocument");

    // Set printing properties
	NSPrintInfo *myPrintInfo = [[self document] printInfo];
	[myPrintInfo setHorizontalPagination:NSFitPagination];
	[myPrintInfo setHorizontallyCentered:YES];
	[myPrintInfo setVerticalPagination:NSFitPagination];
	[myPrintInfo setVerticallyCentered:YES];
	[myPrintInfo setOrientation:NSLandscapeOrientation];
	[myPrintInfo setLeftMargin:0.0]; // 32.0
	[myPrintInfo setRightMargin:0.0]; // 32.0
	[myPrintInfo setTopMargin:0.0]; // 32.0
	[myPrintInfo setBottomMargin:0.0]; // 32.0
	
	// Capture image and create image view
    NSLog(@"screenshot");
    NSImage *image = [screen screenshot];
    NSLog(@"NSMakeRect");
	NSRect printRect = NSMakeRect(0.0, 0.0, [image size].width, [image size].height);
    NSLog(@"NSImageView");
	NSImageView *imageView = [[NSImageView alloc] initWithFrame:printRect];
    NSLog(@"setImage");
	[imageView setImage:image];
	[imageView setImageScaling:NSScaleToFit];

    NSLog(@"NSPrintOperation");
	// Print image
    NSPrintOperation *printOperation = [NSPrintOperation printOperationWithView:imageView  printInfo:myPrintInfo];
    [printOperation runOperationModalForWindow:[[self document] windowForSheet] delegate: nil didRunSelector: NULL contextInfo:NULL];
	
}


- (IBAction)joystick1Action:(id)sender
{
	[c64 switchInputDevice:0];
}

- (IBAction)joystick2Action:(id)sender
{
	[c64 switchInputDevice:1];
}

- (IBAction)switchJoysticksAction:(id)sender
{
	[c64 switchInputDevices];
}

- (IBAction)portAAction:(id)sender
{
    int value = (int)[[sender selectedItem] tag];

    NSLog(@"portAAction (%d)", value);
    
    switch (value) {
        case 0: [c64 setInputDevice:0 device:IPD_UNCONNECTED]; break;
        case 1: [c64 setInputDevice:0 device:IPD_KEYBOARD_1]; break;
        case 2: [c64 setInputDevice:0 device:IPD_KEYBOARD_2]; break;
        case 3: [c64 setInputDevice:0 device:IPD_KEYBOARD_3]; break;
        case 4: [c64 setInputDevice:0 device:IPD_JOYSTICK_1]; break;
        case 5: [c64 setInputDevice:0 device:IPD_JOYSTICK_2]; break;
    }
}

- (IBAction)portBAction:(id)sender
{
    int value = (int)[[sender selectedItem] tag];
    
    NSLog(@"portBAction (%d)", value);
    
    switch (value) {
        case 0: [c64 setInputDevice:1 device:IPD_UNCONNECTED]; break;
        case 1: [c64 setInputDevice:1 device:IPD_KEYBOARD_1]; break;
        case 2: [c64 setInputDevice:1 device:IPD_KEYBOARD_2]; break;
        case 3: [c64 setInputDevice:1 device:IPD_KEYBOARD_3]; break;
        case 4: [c64 setInputDevice:1 device:IPD_JOYSTICK_1]; break;
        case 5: [c64 setInputDevice:1 device:IPD_JOYSTICK_2]; break;
    }
}

- (IBAction)propertiesAction:(id)sender
{
    [propertiesDialog initialize:self];
    [NSApp beginSheet:propertiesDialog
       modalForWindow:[[self document] windowForSheet]
        modalDelegate:self
       didEndSelector:NULL
          contextInfo:NULL];	
}


- (IBAction)debugOpenAction:(id)sender
{
	NSLog(@"debugOpenAction");

	if ([debugPanel state] == NSDrawerClosedState || [debugPanel state] == NSDrawerClosingState) {
		[debugPanel open];
	}
}

- (IBAction)debugCloseAction:(id)sender
{
	NSLog(@"debugCloseAction");
	
	if ([debugPanel state] == NSDrawerOpenState || [debugPanel state] == NSDrawerOpeningState) {
		[debugPanel close];
	}
}

- (IBAction)debugAction:(id)sender
{	
	NSLog(@"debugAction");
	if ([debugPanel state] == NSDrawerClosedState || [debugPanel state] == NSDrawerClosingState) {
		[self cheatboxCloseAction:self];
		[self debugOpenAction:self];
	} else {
		[self debugCloseAction:self];
	}
	
	[self refresh];
}


- (IBAction)cheatboxOpenAction:(id)sender
{
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
		[self debugCloseAction:self];
		[self cheatboxOpenAction:self];
	} else {
		[self cheatboxCloseAction:self];
	}
}

- (IBAction)iPhoneAction:(id)sender
{	
	NSLog(@"iPhoneAction");
	// [self sendSnapshotToIPhone];
}

@end
