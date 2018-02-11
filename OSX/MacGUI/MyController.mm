/*
 * Author: Dirk W. Hoffmann, 2011 - 2015
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


// TODO
// Register user defaults is class method. Don't access keymap there
// Initialize keymap (where? in which init?) We can also create it on the fly in loadUserDefaults
// Replace Character by String in KeyMap class

@implementation MyController {
    
    // IBOutlet MetalView *metalScreen;
}

@synthesize c64;

@synthesize propertiesDialog;
@synthesize hardwareDialog;
@synthesize mediaDialog;
@synthesize mountDialog;
@synthesize tapeDialog;
@synthesize romDialog;

// Toolbar
@synthesize joystickPortA;
@synthesize joystickPortB;

// Main screen
@synthesize debugPanel;
@synthesize cheatboxPanel;

// Bottom bar
@synthesize greenLED;
@synthesize redLED;
@synthesize progress;
@synthesize driveIcon;
@synthesize driveEject;
@synthesize cartridgeIcon;
@synthesize cartridgeEject;
@synthesize tapeIcon;
@synthesize tapeEject;
@synthesize tapeProgress;
@synthesize info;
@synthesize clockSpeed;
@synthesize clockSpeedBar;
@synthesize warpIcon;

@synthesize cheatboxImageBrowserView;
@synthesize menuItemFinalIII;
@synthesize gamePadManager;
@synthesize modifierFlags;
@synthesize statusBar;
@synthesize gamepadSlotA;
@synthesize gamepadSlotB;

@synthesize keyboardcontroller;
@synthesize metalScreen;
@synthesize cpuTableView;
@synthesize memTableView;
@synthesize speedometer;
@synthesize animationCounter;

@synthesize timer;
@synthesize timerLock;

// --------------------------------------------------------------------------------
//       Metal screen API (will be removed when controller is Swift only)
// --------------------------------------------------------------------------------

- (BOOL)fullscreen { return [metalScreen fullscreen]; }
- (NSImage *)screenshot { return [metalScreen screenshot]; }
- (void)rotateBack { [metalScreen rotateBack]; }
 - (void)shrink { [metalScreen shrink]; }
- (void)expand { [metalScreen expand]; }
- (float)eyeX { return [metalScreen eyeX]; }
- (void)setEyeX:(float)x { [metalScreen setEyeX:x]; }
- (float)eyeY { return [metalScreen eyeY]; }
- (void)setEyeY:(float)y { [metalScreen setEyeY:y]; }
- (float)eyeZ { return [metalScreen eyeZ]; }
- (void)setEyeZ:(float)z { [metalScreen setEyeZ:z]; }
- (long)videoUpscaler { return [metalScreen videoUpscaler]; }
- (void)setVideoUpscaler:(long)val { [metalScreen setVideoUpscaler:val]; }
- (long)videoFilter { return [metalScreen videoFilter]; }
- (void)setVideoFilter:(long)val { [metalScreen setVideoFilter:val]; }
- (BOOL)fullscreenKeepAspectRatio { return [metalScreen fullscreenKeepAspectRatio]; }
- (void)setFullscreenKeepAspectRatio:(BOOL)val { [metalScreen setFullscreenKeepAspectRatio:val]; }

// --------------------------------------------------------------------------------
//     KeyboardController API (will be removed when controller is Swift only)
// --------------------------------------------------------------------------------

- (void)simulateUserPressingKey:(C64KeyFingerprint)key {
    [keyboardcontroller simulateUserPressingKey:key];
}
- (void)simulateUserPressingKeyWithShift:(C64KeyFingerprint)key {
    [keyboardcontroller simulateUserPressingKeyWithShift:key];
}
- (void)simulateUserPressingKeyWithRunstop:(C64KeyFingerprint)key {
    [keyboardcontroller simulateUserPressingKeyWithRunstop:key];
}
- (void)simulateUserTypingText:(NSString *)text {
    [keyboardcontroller simulateUserTypingWithText:text initialDelay:0 completion:nil];
}
- (void)simulateUserTypingText:(NSString *)text withInitialDelay:(useconds_t)delay {
    [keyboardcontroller simulateUserTypingWithText:text initialDelay:delay completion:nil];
}
- (void)simulateUserTypingTextAndPressPlay:(NSString *)text {
    [keyboardcontroller simulateUserTypingAndPressPlayWithText:text];
}

- (BOOL)getDisconnectEmulationKeys { return [keyboardcontroller getDisconnectEmulationKeys]; }
- (void)setDisconnectEmulationKeys:(BOOL)b { [keyboardcontroller setDisconnectEmulationKeys:b]; }

- (void)keyDown:(NSEvent *)event
{
    [keyboardcontroller keyDownWith:event];
}

- (void)keyUp:(NSEvent *)event
{
    [keyboardcontroller keyUpWith:event];
}

- (void)flagsChanged:(NSEvent *)event
{
    NSEventModifierFlags flags = [event modifierFlags];
    
    // Save modifier flags. They are needed in TouchBar code
    [self setModifierFlags:flags];
     
    [keyboardcontroller flagsChangedWith:event];
     
}

/*
- (void)processMessage:(Message *)msg
{
    // NSLog(@"Message %d",msg->id);

    switch (msg->id) {
			
        case MSG_READY_TO_RUN:
            
            // Close ROM dialog if open
            // TODO: MAKE SURE THAT DIALOG IS ALREADY CLOSED
            if (romDialog) {
                [romDialog orderOut:nil];
                [[self window] endSheet:romDialog returnCode:NSModalResponseCancel];
                romDialog = NULL;
            }
            
            // Start emulator
            [c64 run];
            [metalScreen blendIn];
            [metalScreen setDrawC64texture:true];
            
            // Show mount dialog if an attachment is present
            [self showMountDialog];
            break;
            
        case MSG_RUN:
            [info setStringValue:@""];
            [self enableUserEditing:NO];
            [self refresh];
            [cheatboxPanel close];
            
            // Disable undo because the internal state changes permanently
            [[self document] updateChangeCount:NSChangeDone];
            [[self undoManager] removeAllActions];
            break;
            
        case MSG_HALT:
            [self enableUserEditing:YES];
            [self refresh];
            break;
            
		case MSG_ROM_LOADED:
			// Update ROM dialog
			if (romDialog != NULL) {
				[romDialog update:[c64 missingRoms]];
			}
			break;
        
        case MSG_ROM_MISSING:
            NSLog(@"MSG_ROM_MISSING");
            assert(msg->i != 0);
            [self enableUserEditing:YES];
            [self refresh];
            [self showRomDialog:msg];
            break;
            
        case MSG_SNAPSHOT:
            // Update TouchBar with new snapshpot image
            [self rebuildTouchBar];
            break;

		case MSG_CPU:
			switch(msg->i) {
				case CPU_OK:
				case SOFT_BREAKPOINT_REACHED:
					[info setStringValue:@""];
					break;
				case HARD_BREAKPOINT_REACHED:
                    [self debugOpenAction:self];
					break;
				case ILLEGAL_INSTRUCTION:
                    [self debugOpenAction:self];
					break;
				default:
					assert(0);
			}
			[self refresh];			
			break;

        case MSG_WARP:
        case MSG_ALWAYS_WARP:
            if ([c64 alwaysWarp]) {
                [warpIcon setImage:[NSImage imageNamed:@"pin_red"]];
            } else if ([c64 warp]) {
                [warpIcon setImage:[NSImage imageNamed:@"clock_red"]];
            } else {
                [warpIcon setImage:[NSImage imageNamed:@"clock_green"]];
            }
            break;
            
        case MSG_PAL:
        case MSG_NTSC:
            [metalScreen updateScreenGeometry];
            break;
            
		case MSG_VC1541_ATTACHED:
            if (msg->i)
				[greenLED setImage:[NSImage imageNamed:@"LEDgreen"]];
            else
				[greenLED setImage:[NSImage imageNamed:@"LEDgray"]];
			break;
			
        case MSG_VC1541_ATTACHED_SOUND:
            if (msg->i) {
                // Not sure about the copyright of the following sound:
                // [[c64 vc1541] playSound:@"1541_power_on_0" volume:0.2];
                // Sound from Commodore 64 (C64) Preservation Project (c64preservation.com):
                [[c64 vc1541] playSound:@"drive_click" volume:1.0];
            } else {
                // Not sure about the copyright of the following sound:
                // [[c64 vc1541] playSound:@"1541_track_change_0" volume:0.6];
                // Sound from Commodore 64 (C64) Preservation Project (c64preservation.com):
                [[c64 vc1541] playSound:@"drive_click" volume:1.0];
            }
            break;

		case MSG_VC1541_DISK:
			[driveIcon setHidden:!msg->i];
			[driveEject setHidden:!msg->i];
            break;
			
        case MSG_VC1541_DISK_SOUND:
            if (msg->i) {
                // [[c64 vc1541] playSound:@"1541_door_closed_2" volume:0.2];
                [[c64 vc1541] playSound:@"drive_snatch_uae" volume:0.1];
            } else {
                // [[c64 vc1541] playSound:@"1541_door_open_1" volume:0.2];
                [[c64 vc1541] playSound:@"drive_snatch_uae" volume:0.1];
            }
            break;

        case MSG_VC1541_LED:
            if (msg->i) {
				[redLED setImage:[NSImage imageNamed:@"LEDred"]];
            } else {
				[redLED setImage:[NSImage imageNamed:@"LEDgray"]];
            }
            [redLED setNeedsDisplay];
			break;
			
		case MSG_VC1541_DATA:
			if (msg->i)
				[c64 setIecBusIsBusy:true];
			else
				[c64 setIecBusIsBusy:false];
			break;
			
		case MSG_VC1541_MOTOR:
        case MSG_VC1541_HEAD:
            break;
            
        case MSG_VC1541_HEAD_SOUND:
            if (msg->i) {
                // Not sure about the copyright of the following sound:
                // [[c64 vc1541] playSound:@"1541_track_change_0" volume:0.6];
                // Sound from Commodore 64 (C64) Preservation Project (c64preservation.com):
                [[c64 vc1541] playSound:@"drive_click" volume:1.0];
            } else {
                // Not sure about the copyright of the following sound:
                // [[c64 vc1541] playSound:@"1541_track_change_2" volume:1.0];
                // Sound from Commodore 64 (C64) Preservation Project (c64preservation.com):
                [[c64 vc1541] playSound:@"drive_click" volume:1.0];
            }
            break;
            
        case MSG_VC1530_TAPE:
            [tapeIcon setHidden:!msg->i];
            [tapeEject setHidden:!msg->i];
            break;

        case MSG_VC1530_PLAY:
             break;

        case MSG_VC1530_PROGRESS:
            [mediaDialog update];
            break;
        
        case MSG_CARTRIDGE:
            [cartridgeIcon setHidden:!msg->i];
            [cartridgeEject setHidden:!msg->i];
            break;
            
		default:
			assert(0);
	}
}
*/

// --------------------------------------------------------------------------------
//          Refresh methods: Force all GUI items to refresh their value
// --------------------------------------------------------------------------------

- (void)refresh
{		
	[self refreshCPU];
	[self refreshMemory];
	[self refreshCIA];
	[self refreshVIC];
	[cpuTableView refresh];
	[memTableView refresh];
}

- (void)refresh:(NSFormatter *)byteFormatter word:(NSFormatter *)wordFormatter threedigit:(NSFormatter *)threeDigitFormatter disassembler:(NSFormatter *)disassembler
{		
	NSControl *ByteFormatterControls[] = { 
		// CPU panel
		sp, a, x, y,
		// CIA panel
		cia1DataPortA, cia1DataPortDirectionA, cia1DataPortB, cia1DataPortDirectionB,
		tod1Hours, tod1Minutes, tod1Seconds, tod1Tenth, alarm1Hours, alarm1Minutes, alarm1Seconds, alarm1Tenth,
		cia2DataPortA, cia2DataPortDirectionA, cia2DataPortB, cia2DataPortDirectionB,
		tod2Hours, tod2Minutes, tod2Seconds, tod2Tenth, alarm2Hours, alarm2Minutes, alarm2Seconds, alarm2Tenth,
		// VIC panel
		VicSpriteY1, VicSpriteY2, VicSpriteY3, VicSpriteY4, VicSpriteY5, VicSpriteY6, VicSpriteY7, VicSpriteY8,
 		NULL };
	
	NSControl *WordFormatterControls[] = { 
		// CPU panel
		pc, breakpoint,
		// Memory panel
		addr_search,
		// CIA panel
		cia1TimerA, cia1LatchedTimerA, cia1TimerB, cia1LatchedTimerB,
		cia2TimerA, cia2LatchedTimerA, cia2TimerB, cia2LatchedTimerB,
		// VIC panel
		VicRasterline, VicRasterInterrupt,
		NULL };

    NSControl *threeDigitFormatterControls[] = { 
		// VIC panel
		VicSpriteX1, VicSpriteX2, VicSpriteX3, VicSpriteX4, VicSpriteX5, VicSpriteX6, VicSpriteX7, VicSpriteX8,
		NULL };

	// Bind formatters
	for (int i = 0; ByteFormatterControls[i] != NULL; i++) {
		[ByteFormatterControls[i] abortEditing];
		[ByteFormatterControls[i] setFormatter:byteFormatter];
		[ByteFormatterControls[i] setNeedsDisplay];
	}
	
	for (int i = 0; WordFormatterControls[i] != NULL; i++) {
		[WordFormatterControls[i] abortEditing];
		[WordFormatterControls[i] setFormatter:wordFormatter];
		[WordFormatterControls[i] setNeedsDisplay];
	}

    for (int i = 0; threeDigitFormatterControls[i] != NULL; i++) {
		[threeDigitFormatterControls[i] abortEditing];
		[threeDigitFormatterControls[i] setFormatter:threeDigitFormatter];
		[threeDigitFormatterControls[i] setNeedsDisplay];
	}

	// Assign formatters to all table view cells
	[[[cpuTableView tableColumnWithIdentifier:@"addr"] dataCell] setFormatter:wordFormatter];
	[[[cpuTableView tableColumnWithIdentifier:@"data01"] dataCell] setFormatter:byteFormatter];
	[[[cpuTableView tableColumnWithIdentifier:@"data02"] dataCell] setFormatter:byteFormatter];
	[[[cpuTableView tableColumnWithIdentifier:@"data03"] dataCell] setFormatter:byteFormatter];
	[[[cpuTableView tableColumnWithIdentifier:@"ascii"] dataCell] setFormatter:disassembler];
	
	[[[memTableView tableColumnWithIdentifier:@"addr"] dataCell] setFormatter:wordFormatter];
	[[[memTableView tableColumnWithIdentifier:@"hex0"] dataCell] setFormatter:byteFormatter];
	[[[memTableView tableColumnWithIdentifier:@"hex1"] dataCell] setFormatter:byteFormatter];
	[[[memTableView tableColumnWithIdentifier:@"hex2"] dataCell] setFormatter:byteFormatter];
	[[[memTableView tableColumnWithIdentifier:@"hex3"] dataCell] setFormatter:byteFormatter];	
	
	[self refresh];
}

- (void)enableUserEditing:(BOOL)enabled
{
	NSControl *controls[] = { 
		// CPU panel
		pc, sp, a, x, y, 
		Nflag, Zflag, Cflag, Iflag, Bflag, Dflag, Vflag,
		// CIA panel
		cia1DataPortA, cia1DataPortDirectionA, cia1TimerA, cia1LatchedTimerA, 
		//cia1RunningA, cia1OneShotA, cia1CountUnderflowsA, cia1SignalPendingA, cia1InterruptEnableA,
		cia1DataPortB, cia1DataPortDirectionB, cia1TimerB, cia1LatchedTimerB, 
		//cia1RunningB, cia1OneShotB, cia1CountUnderflowsB, cia1SignalPendingB, cia1InterruptEnableB,
		tod1Hours, tod1Minutes, tod1Seconds, tod1Tenth,
		alarm1Hours, alarm1Minutes, alarm1Seconds, alarm1Tenth,
        // tod1InterruptEnabled,
		cia2DataPortA, cia2DataPortDirectionA, cia2TimerA, cia2LatchedTimerA, 
		// cia2RunningA, cia2OneShotA, cia2CountUnderflowsA, cia2SignalPendingA, cia2InterruptEnableA,
		cia2DataPortB, cia2DataPortDirectionB, cia2TimerB, cia2LatchedTimerB, 
		//cia2RunningB, cia2OneShotB, cia2CountUnderflowsB, cia2SignalPendingB, cia2InterruptEnableB,
		tod2Hours, tod2Minutes, tod2Seconds, tod2Tenth,
		alarm2Hours, alarm2Minutes, alarm2Seconds, alarm2Tenth,
        // tod2InterruptEnabled,
		// VIC panel
		VicSpriteX1, VicSpriteX2, VicSpriteX3, VicSpriteX4, VicSpriteX5, VicSpriteX6, VicSpriteX7, VicSpriteX8,
        VicSpriteY1, VicSpriteY2, VicSpriteY3, VicSpriteY4, VicSpriteY5, VicSpriteY6, VicSpriteY7, VicSpriteY8,
        
        VicRasterline, VicRasterInterrupt, VicDX, VicDY,
		NULL };
	
	// Enable / disable controls
	for (int i = 0;; i++) {
		if (controls[i] == NULL) break;
		[controls[i] setEnabled:enabled];
	}
	
	// Enable / disable table columns
	[[memTableView tableColumnWithIdentifier:@"hex0"] setEditable:enabled];
	[[memTableView tableColumnWithIdentifier:@"hex1"] setEditable:enabled];
	[[memTableView tableColumnWithIdentifier:@"hex2"] setEditable:enabled];
	[[memTableView tableColumnWithIdentifier:@"hex3"] setEditable:enabled];
	
	// Change image and state of debugger control buttons
	if (![c64 isRunnable]) {
		[stopAndGoButton setImage:[NSImage imageNamed:@"play32"]];		
		[stopAndGoButton setEnabled:false];
		[stepIntoButton setEnabled:false];
		[stepOverButton setEnabled:false];
		[stepOutButton setEnabled:false];		
		
	} else if ([c64 isHalted]) {
		[stopAndGoButton setImage:[NSImage imageNamed:@"play32"]];		
		[stopAndGoButton setEnabled:true];
		[stepIntoButton setEnabled:true];
		[stepOverButton setEnabled:true];
		[stepOutButton setEnabled:true];		
	} else {
		[stopAndGoButton setImage:[NSImage imageNamed:@"pause32"]];
		[stopAndGoButton setEnabled:true];
		[stepIntoButton setEnabled:false];
		[stepOverButton setEnabled:false];
		[stepOutButton setEnabled:false];		
	}		
}

- (IBAction)alwaysWarpAction:(id)sender
{
    NSLog(@"alwaysWarpAction");
    
    NSUndoManager *undo = [self undoManager];
    [[undo prepareWithInvocationTarget:self] alwaysWarpAction:@((int)![c64 warp])];
    if (![undo isUndoing]) [undo setActionName:@"Native speed"];
    
    [c64 setAlwaysWarp:![c64 alwaysWarp]];
    [self refresh];
}


// --------------------------------------------------------------------------------
//                                     Dialogs
// --------------------------------------------------------------------------------

- (bool)showPropertiesDialog
{
    [propertiesDialog initialize:self];
    [[self window] beginSheet:propertiesDialog completionHandler:nil];
    
    return YES;
}

- (IBAction)cancelPropertiesDialog:(id)sender
{
	[propertiesDialog orderOut:sender]; // Hide sheet
    [[self window] endSheet:propertiesDialog returnCode:NSModalResponseCancel];
}

- (bool)showHardwareDialog
{
    [hardwareDialog initialize:self];
    [[self window] beginSheet:hardwareDialog completionHandler:nil];

    return YES;
}

- (IBAction)cancelHardwareDialog:(id)sender
{
    [hardwareDialog orderOut:sender];
    [[self window] endSheet:hardwareDialog returnCode:NSModalResponseCancel];
}

- (bool)showMediaDialog
{
    [mediaDialog initialize:self];
    [[self window] beginSheet:mediaDialog completionHandler:nil];
    
    return YES;
}

- (IBAction)cancelMediaDialog:(id)sender
{
    [mediaDialog orderOut:sender]; // Hide sheet
    [[self window] endSheet:mediaDialog returnCode:NSModalResponseCancel];
}

- (bool)showRomDialog:(NSInteger)i
{
    [romDialog initialize:(int)i];
    [[self window] beginSheet:romDialog completionHandler:nil];

    return YES;
}

- (IBAction)cancelRomDialog:(id)sender
{
    [romDialog orderOut:sender]; // Hide sheet
    [[self window] endSheet:romDialog returnCode:NSModalResponseCancel];
	[NSApp terminate: nil]; // Exit
}



#if 0
- (bool)showTapeDialog
{
    // Only proceed if a a tape image is present
    if (![[self document] attachedTape])
        return NO;
    
    // [tapeDialog initialize:[[self document] attachedTape] c64proxy:c64];
    [[self window] beginSheet:tapeDialog completionHandler:nil];
    
    return YES;
}

- (IBAction)cancelTapeDialog:(id)sender
{
    [tapeDialog orderOut:sender]; // Hide sheet
    [[self window] endSheet:tapeDialog returnCode:NSModalResponseCancel];
}

- (IBAction)endTapeDialog:(id)sender
{
    NSString *textToType = @"LOAD\n";
    bool doAutoType = [tapeDialog doAutoType];
    bool doPressPlay = [tapeDialog doPressPlay];
    
    NSLog(@"Should type:  %ld (%@)", (long)doAutoType, textToType);
    NSLog(@"Should press play: %ld", (long)doPressPlay);
    
    // Rotate C64 screen
    [metalScreen rotate];
    
    // Hide sheet
    [tapeDialog orderOut:sender];
    [[self window] endSheet:tapeDialog returnCode:NSModalResponseCancel];
    
    // Insert tape into datasette
    [c64 insertTape:[[self document] attachedTape]];
    
    // Type command if requested
    if (doAutoType) {
        // [[c64 keyboard] typeText:textToType withDelay:500000];

    }
    
    if (doAutoType && doPressPlay) {
        dispatch_async(dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
                       ^{ usleep(400000); [[c64 datasette] pressPlay]; });
    }
}
#endif

#if 0
- (BOOL)exportToD64:(NSString *)path
{
    NSLog(@"Writing drive contents to D64 archive in %@...",path);

    // Determine full destination path
    NSString *archivePath = [[[self document] attachedArchive] getPath];
    NSString *archiveName = [[archivePath lastPathComponent] stringByDeletingPathExtension];
    NSString *proposedName = [path stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.D64", archiveName]];
    
    for (unsigned i = 2; i < 256; i++) {
        
        // Hopefully, the file does not yet exist...
        if (![[NSFileManager defaultManager] fileExistsAtPath:proposedName]) {
            
            NSLog(@"Using file name %@...", proposedName);
            return [[c64 vc1541] exportToD64:proposedName];
        }

        // Try a different name
        proposedName = [path stringByAppendingPathComponent:[NSString stringWithFormat:@"%@-%d.D64", archiveName, i]];
    }
    
    // Sorry, too many failures
    return NO;
}
#endif

// --------------------------------------------------------------------------------
//                                  Keyboard events 
// --------------------------------------------------------------------------------

- (BOOL)acceptsFirstResponder
{
	return YES;
}

- (BOOL)resignFirstResponder
{
	return YES;
}

- (BOOL)becomeFirstResonder 
{
	return YES;
}


// --------------------------------------------------------------------------------
//                                      Paste
// --------------------------------------------------------------------------------


- (void)paste:(id)sender
{
    NSPasteboard *gpBoard;
    NSString *text;
    
    
    gpBoard = [NSPasteboard generalPasteboard];
    if (!(text = [gpBoard stringForType:NSStringPboardType])) {
        NSLog(@"Paste failed");
        return;
    }
    
    [self simulateUserTypingText:text];
    // [[c64 keyboard] typeText:text];
}


@end
