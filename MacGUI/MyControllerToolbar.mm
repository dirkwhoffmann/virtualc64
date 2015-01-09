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
    
    // VC1541
    if ([theItem tag] == 20) {
        if (![[c64 vc1541] hasDisk]) {
            return NO;
        }
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
        [item1 setEnabled:joystickManager->joystickIsPluggedIn(1)];
        [item2 setEnabled:joystickManager->joystickIsPluggedIn(2)];
        [joystickPortA selectItemAtIndex:[self inputDeviceA]];
    }
    
    /* Jostick port 2 */
    {
        NSMenuItem *item1 = [[joystickPortB menu] itemWithTag:IPD_JOYSTICK_1];
        NSMenuItem *item2 = [[joystickPortB menu] itemWithTag:IPD_JOYSTICK_2];
        [item1 setEnabled:joystickManager->joystickIsPluggedIn(1)];
        [item2 setEnabled:joystickManager->joystickIsPluggedIn(2)];
        [joystickPortB selectItemAtIndex:[self inputDeviceB]];
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
    NSImage *image = [screen screenshot];
	NSRect printRect = NSMakeRect(0.0, 0.0, [image size].width, [image size].height);
	NSImageView *imageView = [[NSImageView alloc] initWithFrame:printRect];
	[imageView setImage:image];
	[imageView setImageScaling:NSScaleToFit];

	// Print image
    NSPrintOperation *printOperation = [NSPrintOperation printOperationWithView:imageView  printInfo:myPrintInfo];
    [printOperation runOperationModalForWindow:[[self document] windowForSheet] delegate: nil didRunSelector: NULL contextInfo:NULL];
	
}

- (IBAction)portAAction:(id)sender
{
    {
        int newvalue = (int)[[sender selectedItem] tag]; /* New pop-up menu selection */
        int oldvalue = [self inputDeviceA]; /* Old pop-up menu selection */
        int othervalue = [self inputDeviceB]; /* Pop-up selection of other port */
        Joystick* target =[c64 c64]->joystick1; /* Target is joystick on port A */
        
        NSLog(@"portAAction (%d)", newvalue);
        
        // Update input device
        [self setInputDeviceA:newvalue];
        
        // Unconnect old binding of selected port
        if (oldvalue == IPD_JOYSTICK_1)
            joystickManager->bindJoystick(1,NULL);
        if (oldvalue == IPD_JOYSTICK_2)
            joystickManager->bindJoystick(2,NULL);
        
        // Unconnect binding of other port as well if a double mapping occurs
        if (newvalue == othervalue) {
            NSLog(@"Selected USB joystick is already assigned. Removing binding.");
            [self setInputDeviceB:IPD_UNCONNECTED];
        }
        
        // Establish new binding
        switch (newvalue) {
            case IPD_UNCONNECTED:
            case IPD_KEYBOARD_1:
            case IPD_KEYBOARD_2:
            case IPD_KEYBOARD_3:
                /* Nothing to do */
                break;
                
            case IPD_JOYSTICK_1:
                joystickManager->bindJoystick(1,target);
                break;
                
            case IPD_JOYSTICK_2:
                joystickManager->bindJoystick(2,target);
                break;
                
            default:
                assert(0);
        }
        
        [self validateJoystickItems];
    }
}

- (IBAction)portBAction:(id)sender
{
    int newvalue = (int)[[sender selectedItem] tag]; /* New pop-up menu selection */
    int oldvalue = [self inputDeviceB]; /* Old pop-up menu selection */
    int othervalue = [self inputDeviceA]; /* Pop-up selection of other port */
    Joystick* target =[c64 c64]->joystick2; /* Target is joystick on port B */
    
    NSLog(@"portBAction (%d)", newvalue);
    
    // Update input device
    [self setInputDeviceB:newvalue];
    
    // Unconnect old binding of selected port
    if (oldvalue == IPD_JOYSTICK_1)
        joystickManager->bindJoystick(1,NULL);
    if (oldvalue == IPD_JOYSTICK_2)
        joystickManager->bindJoystick(2,NULL);
    
    // Unconnect binding of other port as well if a double mapping occurs
    if (newvalue == othervalue) {
        NSLog(@"Selected USB joystick is already assigned. Removing binding.");
        [self setInputDeviceA:IPD_UNCONNECTED];
    }
    
    // Establish new binding
    switch (newvalue) {
        case IPD_UNCONNECTED:
        case IPD_KEYBOARD_1:
        case IPD_KEYBOARD_2:
        case IPD_KEYBOARD_3:
            /* Nothing to do */
            break;
            
        case IPD_JOYSTICK_1:
            joystickManager->bindJoystick(1,target);
            break;
            
        case IPD_JOYSTICK_2:
            joystickManager->bindJoystick(2,target);
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

- (IBAction)VC1541Action:(id)sender
{
    NSLog(@"VC1541 action");
    [self showMountDialog];
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
