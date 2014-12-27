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
    // NSLog(@"Validating %d...",(int)[theItem tag]);
	
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

- (void)validateJoystickItems
{
    /* Jostick port 1 */
    {
        NSMenuItem *item1 = [[joystickPortA menu] itemWithTag:4];
        NSMenuItem *item2 = [[joystickPortA menu] itemWithTag:5];
        [item1 setEnabled:joystickManager->joystickIsPluggedIn(1)];
        [item2 setEnabled:joystickManager->joystickIsPluggedIn(2)];
//        [[portA itemWithTag:[c64 portAssignment:0]] setState:1];
        [joystickPortA selectItemAtIndex:[c64 portAssignment:0]];
    }
    
    /* Jostick port 2 */
    {
        NSMenuItem *item1 = [[joystickPortB menu] itemWithTag:4];
        NSMenuItem *item2 = [[joystickPortB menu] itemWithTag:5];
        [item1 setEnabled:joystickManager->joystickIsPluggedIn(1)];
        [item2 setEnabled:joystickManager->joystickIsPluggedIn(2)];
//        [[portB itemWithTag:[c64 portAssignment:1]] setState:1];
        [joystickPortB selectItemAtIndex:[c64 portAssignment:1]];
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
    int oldvalue = (int)[c64 portAssignment:0]; /* Old pop-up menu selection */
    int newvalue = (int)[[sender selectedItem] tag]; /* New pop-up menu selection */
    int othervalue = (int)[[joystickPortB selectedItem] tag]; /* Pop-up selection of other port */
    
    // Target joystick is second joystick (port A)
    Joystick* targetPort =[c64 c64]->joystick1;
    
    NSLog(@"portAAction (%d)", newvalue);
    
    // Remember old value
    [c64 setInputDevice:0 device:newvalue];
    
    // Unconnect old joystick
    if (oldvalue == IPD_JOYSTICK_1)
        joystickManager->bindJoystick(1,NULL);
    if (oldvalue == IPD_JOYSTICK_2)
        joystickManager->bindJoystick(2,NULL);
    
    switch (newvalue) {
        case IPD_UNCONNECTED:
        case IPD_KEYBOARD_1:
        case IPD_KEYBOARD_2:
        case IPD_KEYBOARD_3:
            /* Nothing to do */
            break;
            
        case IPD_JOYSTICK_1:
            
            if (othervalue == IPD_JOYSTICK_1) {
                // Disconnect from other port
                NSLog(@"First USB joystick has double mapping");
                [c64 setInputDevice:1 device:IPD_UNCONNECTED];
            }
            joystickManager->bindJoystick(1,targetPort);
            break;
            
        case IPD_JOYSTICK_2:
            
            if (othervalue == IPD_JOYSTICK_2) {
                // Disconnect from other port
                NSLog(@"Second USB joystick has double mapping");
                [c64 setInputDevice:1 device:IPD_UNCONNECTED];
            }
            joystickManager->bindJoystick(2,targetPort);
            break;
            
        default:
            assert(0);
    }
    [self validateJoystickItems];
}

- (IBAction)portBAction:(id)sender
{
    int oldvalue = (int)[c64 portAssignment:1]; /* Old pop-up menu selection */
    int newvalue = (int)[[sender selectedItem] tag]; /* New pop-up menu selection */
    int othervalue = (int)[[joystickPortA selectedItem] tag]; /* Pop-up selection of other port */
    
    // Target joystick is second joystick (port B)
    Joystick* targetPort =[c64 c64]->joystick2;
    
    NSLog(@"portBAction (%d)", newvalue);
    
    // Remember old value
    [c64 setInputDevice:1 device:newvalue];
    
    // Unconnect old joystick
    if (oldvalue == IPD_JOYSTICK_1)
        joystickManager->bindJoystick(1,NULL);
    if (oldvalue == IPD_JOYSTICK_2)
        joystickManager->bindJoystick(2,NULL);
    
    switch (newvalue) {
        case IPD_UNCONNECTED:
        case IPD_KEYBOARD_1:
        case IPD_KEYBOARD_2:
        case IPD_KEYBOARD_3:
            /* Nothing to do */
            break;
            
        case IPD_JOYSTICK_1:
            
            if (othervalue == IPD_JOYSTICK_1) {
                // Disconnect from other port
                NSLog(@"First USB joystick has double mapping");
                [c64 setInputDevice:0 device:IPD_UNCONNECTED];
            }
            joystickManager->bindJoystick(1,targetPort);
            break;
            
        case IPD_JOYSTICK_2:
            
            if (othervalue == IPD_JOYSTICK_2) {
                // Disconnect from other port
                NSLog(@"Second USB joystick has double mapping");
                [c64 setInputDevice:0 device:IPD_UNCONNECTED];
            }
            joystickManager->bindJoystick(2,targetPort);
            break;
            
        default:
            assert(0);
    }
    [self validateJoystickItems];
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
