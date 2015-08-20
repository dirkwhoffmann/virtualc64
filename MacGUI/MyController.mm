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

@implementation MyController

@synthesize c64, screen;

// --------------------------------------------------------------------------------
//                          Construction and Destruction
// --------------------------------------------------------------------------------

+ (void)initialize {
	
	NSLog(@"MyController::initialize");
	[self registerStandardDefaults];	
}

- (void)dealloc
{	
	NSLog(@"MyController::dealloc");
}

- (void)windowWillClose:(NSNotification *)aNotification
{
	// NSLog(@"windowWillClose");
	
	// stop timer
	[timer invalidate];
	timer = nil;
	
	// stop OpenGL view
	[screen cleanUp];
	
	// release C64
	NSLog(@"Killing timer");
	[timerLock lock];
	NSLog(@"Killing C64");
	[c64 kill];
	c64 = nil;
	[timerLock unlock];
	
	delete joystickManager;
}

- (void)awakeFromNib
{	
	NSLog(@"MyController::awakeFromNib");
	
	// Change working directory to the main bundle ressource path. We may find some ROMs there...
	NSBundle* mainBundle = [NSBundle mainBundle];
	NSString *path = [mainBundle resourcePath];
	if (chdir([path UTF8String]) != 0)
		NSLog(@"WARNING: Could not change working directory.");

	// Bind virtual C64 to other object
	[[self document] setC64:c64];
	
	// Joystick handling
	joystickManager = new JoystickManager(c64);
	joystickManager->Initialize();
	
	// Update some toolbar icons
	[self setupToolbarIcons];
		
	// Create and bind number formatters
	[self setHexadecimalAction:self];
	
	// Setup table views
	[cpuTableView setController:self];
	[memTableView setController:self];
	[cheatboxImageBrowserView setController:self];
	
	// Create timer and speedometer
	timerLock = [[NSLock alloc] init];
	timer = [NSTimer scheduledTimerWithTimeInterval:(1.0f/6.0f) 
											 target:self 
										   selector:@selector(timerFunc) 
										   userInfo:nil repeats:YES];
	speedometer = [[Speedometer alloc] init];
		
	NSLog(@"GUI is initialized, timer is running");	
}

- (void)windowDidLoad
{
    NSLog(@"MyController::windowDidLoad");

    NSWindow *window = [self window];

    // Enable auto-save for window coordinates
    [[[self window] windowController] setShouldCascadeWindows:NO];
    [[self window] setFrameAutosaveName:@"dirkwhoffmann.de.virtualC64.window"];
    
	// Load user defaults
	[self loadUserDefaults];
    [self loadVirtualMachineUserDefaults];
    
    // Enable fullscreen mode
    [window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
    
	// Launch emulator
	[c64 run];
}

// --------------------------------------------------------------------------------
//                                   Full screen
// --------------------------------------------------------------------------------

- (void)windowWillEnterFullScreen:(NSNotification *)notification
{
    NSLog(@"windowWillEnterFullScreen");
    
    [screen setDrawIn3D:false];
}

- (void)windowDidEnterFullScreen:(NSNotification *)notification
{
    NSLog(@"windowDidEnterFullScreen");
}

- (void)windowWillExitFullScreen:(NSNotification *)notification
{
    NSLog(@"windowWillExitFullScreen");
    
    [screen setDrawIn3D:true];
}

- (void)windowDidExitFullScreen:(NSNotification *)notification
{
    NSLog(@"windowDidExitFullScreen");
}

- (NSApplicationPresentationOptions)window:(NSWindow *)window
      willUseFullScreenPresentationOptions:(NSApplicationPresentationOptions)proposedOptions
{
    NSLog(@"window:willUseFullScreenPresentationOptions");
    proposedOptions |= NSApplicationPresentationAutoHideToolbar;
    return proposedOptions;
}

- (NSSize)window:(NSWindow *)window willUseFullScreenContentSize:(NSSize)proposedSize
{
    NSLog(@"Proposed full screen size: %f x %f", proposedSize.width, proposedSize.height);
    
    NSRect myRect = [screen bounds];
    myRect.size = proposedSize;
    // [screen setFrame:myRect];
    // [window setFrame:myRect display:YES];
    return proposedSize;
}

- (NSUndoManager *)undoManager
{
    return [[self document] undoManager];
}

// --------------------------------------------------------------------------------
//                                   User defaults
// --------------------------------------------------------------------------------

+ (void)registerStandardDefaults
{
	// NSLog(@"MyController::Registering standard user defaults");
	
	NSMutableDictionary *defaultValues = [NSMutableDictionary dictionary];
	
	// System 
	[defaultValues setObject:@0 forKey:VC64PALorNTSCKey]; /*PAL*/
	[defaultValues setObject:@"" forKey:VC64BasicRomFileKey];
	[defaultValues setObject:@"" forKey:VC64CharRomFileKey];
	[defaultValues setObject:@"" forKey:VC64KernelRomFileKey];
	[defaultValues setObject:@"" forKey:VC64VC1541RomFileKey];
	
	// VC1541
	[defaultValues setObject:@YES forKey:VC64WarpLoadKey];

    // Joysticks
    [defaultValues setObject:@123 forKey:VC64Left1keycodeKey];
    [defaultValues setObject:@' ' forKey:VC64Left1charKey];
    [defaultValues setObject:@124 forKey:VC64Right1keycodeKey];
    [defaultValues setObject:@' ' forKey:VC64Right1charKey];
    [defaultValues setObject:@125 forKey:VC64Down1keycodeKey];
    [defaultValues setObject:@' ' forKey:VC64Down1charKey];
    [defaultValues setObject:@126 forKey:VC64Up1keycodeKey];
    [defaultValues setObject:@' ' forKey:VC64Up1charKey];
    [defaultValues setObject:@49  forKey:VC64Fire1keycodeKey];
    [defaultValues setObject:@' ' forKey:VC64Fire1charKey];

    [defaultValues setObject:@0 forKey:VC64Left2keycodeKey];
    [defaultValues setObject:@'a' forKey:VC64Left2charKey];
    [defaultValues setObject:@1 forKey:VC64Right2keycodeKey];
    [defaultValues setObject:@'s' forKey:VC64Right2charKey];
    [defaultValues setObject:@6 forKey:VC64Down2keycodeKey];
    [defaultValues setObject:@'y' forKey:VC64Down2charKey];
    [defaultValues setObject:@13 forKey:VC64Up2keycodeKey];
    [defaultValues setObject:@'w' forKey:VC64Up2charKey];
    [defaultValues setObject:@7 forKey:VC64Fire2keycodeKey];
    [defaultValues setObject:@'x' forKey:VC64Fire2charKey];

	// Audio
	[defaultValues setObject:@YES forKey:VC64SIDReSIDKey];
	[defaultValues setObject:@NO forKey:VC64SIDFilterKey];
	[defaultValues setObject:@1 forKey:VC64SIDChipModelKey];
	[defaultValues setObject:@0 forKey:VC64SIDSamplingMethodKey];
	
	// Video
	[defaultValues setObject:@((float)0.0) forKey:VC64EyeX];
	[defaultValues setObject:@((float)0.0) forKey:VC64EyeY];
	[defaultValues setObject:@((float)0.0) forKey:VC64EyeZ];

	[defaultValues setObject:@((int)CCS64) forKey:VC64ColorSchemeKey];
	[defaultValues setObject:@1 forKey:VC64VideoFilterKey];
		
	// Register dictionary
	[[NSUserDefaults standardUserDefaults] registerDefaults:defaultValues];
}

- (void)loadUserDefaults
{
	NSLog(@"MyController::Loading emulator user defaults");
	
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
			
    // Joysticks
    [screen setJoyKeycode:[defaults integerForKey:VC64Left1keycodeKey] keymap:1 direction:JOYSTICK_LEFT];
    [screen setJoyChar:[defaults integerForKey:VC64Left1charKey] keymap:1 direction:JOYSTICK_LEFT];
    [screen setJoyKeycode:[defaults integerForKey:VC64Right1keycodeKey] keymap:1 direction:JOYSTICK_RIGHT];
    [screen setJoyChar:[defaults integerForKey:VC64Right1charKey] keymap:1 direction:JOYSTICK_RIGHT];
    [screen setJoyKeycode:[defaults integerForKey:VC64Down1keycodeKey] keymap:1 direction:JOYSTICK_DOWN];
    [screen setJoyChar:[defaults integerForKey:VC64Down1charKey] keymap:1 direction:JOYSTICK_DOWN];
    [screen setJoyKeycode:[defaults integerForKey:VC64Up1keycodeKey] keymap:1 direction:JOYSTICK_UP];
    [screen setJoyChar:[defaults integerForKey:VC64Up1charKey] keymap:1 direction:JOYSTICK_UP];
    [screen setJoyKeycode:[defaults integerForKey:VC64Fire1keycodeKey] keymap:1 direction:JOYSTICK_FIRE];
    [screen setJoyChar:[defaults integerForKey:VC64Fire1charKey] keymap:1 direction:JOYSTICK_FIRE];

    [screen setJoyKeycode:[defaults integerForKey:VC64Left2keycodeKey] keymap:2 direction:JOYSTICK_LEFT];
    [screen setJoyChar:[defaults integerForKey:VC64Left2charKey] keymap:2 direction:JOYSTICK_LEFT];
    [screen setJoyKeycode:[defaults integerForKey:VC64Right2keycodeKey] keymap:2 direction:JOYSTICK_RIGHT];
    [screen setJoyChar:[defaults integerForKey:VC64Right2charKey] keymap:2 direction:JOYSTICK_RIGHT];
    [screen setJoyKeycode:[defaults integerForKey:VC64Down2keycodeKey] keymap:2 direction:JOYSTICK_DOWN];
    [screen setJoyChar:[defaults integerForKey:VC64Down2charKey] keymap:2 direction:JOYSTICK_DOWN];
    [screen setJoyKeycode:[defaults integerForKey:VC64Up2keycodeKey] keymap:2 direction:JOYSTICK_UP];
    [screen setJoyChar:[defaults integerForKey:VC64Up2charKey] keymap:2 direction:JOYSTICK_UP];
    [screen setJoyKeycode:[defaults integerForKey:VC64Fire2keycodeKey] keymap:2 direction:JOYSTICK_FIRE];
    [screen setJoyChar:[defaults integerForKey:VC64Fire2charKey] keymap:2 direction:JOYSTICK_FIRE];
    
	// Video 
	[screen setEyeX:[defaults floatForKey:VC64EyeX]];
	[screen setEyeY:[defaults floatForKey:VC64EyeY]];
	[screen setEyeZ:[defaults floatForKey:VC64EyeZ]];

	[screen setAntiAliasing:[defaults integerForKey:VC64VideoFilterKey]];
    [c64 setColorScheme:[defaults integerForKey:VC64ColorSchemeKey]];
}

- (void)loadVirtualMachineUserDefaults
{
    NSLog(@"MyController::Loading virtual machine user defaults");
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    // System
    if ([defaults integerForKey:VC64PALorNTSCKey]) {
        [c64 setNTSC];
    } else {
        [c64 setPAL];
    }
    [[self document] loadRom:[defaults stringForKey:VC64BasicRomFileKey]];
    [[self document] loadRom:[defaults stringForKey:VC64CharRomFileKey]];
    [[self document] loadRom:[defaults stringForKey:VC64KernelRomFileKey]];
    [[self document] loadRom:[defaults stringForKey:VC64VC1541RomFileKey]];
    
    // Peripherals
    [c64 setWarpLoad:[defaults boolForKey:VC64WarpLoadKey]];
    
    // Audio
    [c64 setReSID:[defaults boolForKey:VC64SIDReSIDKey]];
    [c64 setAudioFilter:[defaults boolForKey:VC64SIDFilterKey]];
    [c64 setChipModel:[defaults boolForKey:VC64SIDChipModelKey]];
    [c64 setSamplingMethod:[defaults boolForKey:VC64SIDSamplingMethodKey]];
}

- (void)saveUserDefaults
{
	NSLog(@"MyController::Saving emulator user defaults");
	
	NSUserDefaults *defaults;
	
	// Set standard user defaults
	defaults = [NSUserDefaults standardUserDefaults];
		
    // Joysticks
    [defaults setInteger:[screen joyKeycode:1 direction:JOYSTICK_LEFT] forKey:VC64Left1keycodeKey];
    [defaults setInteger:[screen joyChar:1 direction:JOYSTICK_LEFT] forKey:VC64Left1charKey];
    [defaults setInteger:[screen joyKeycode:1 direction:JOYSTICK_RIGHT] forKey:VC64Right1keycodeKey];
    [defaults setInteger:[screen joyChar:1 direction:JOYSTICK_RIGHT] forKey:VC64Right1charKey];
    [defaults setInteger:[screen joyKeycode:1 direction:JOYSTICK_DOWN] forKey:VC64Down1keycodeKey];
    [defaults setInteger:[screen joyChar:1 direction:JOYSTICK_DOWN] forKey:VC64Down1charKey];
    [defaults setInteger:[screen joyKeycode:1 direction:JOYSTICK_UP] forKey:VC64Up1keycodeKey];
    [defaults setInteger:[screen joyChar:1 direction:JOYSTICK_UP] forKey:VC64Up1charKey];
    [defaults setInteger:[screen joyKeycode:1 direction:JOYSTICK_FIRE] forKey:VC64Fire1keycodeKey];
    [defaults setInteger:[screen joyChar:1 direction:JOYSTICK_FIRE] forKey:VC64Fire1charKey];
    
    [defaults setInteger:[screen joyKeycode:2 direction:JOYSTICK_LEFT] forKey:VC64Left2keycodeKey];
    [defaults setInteger:[screen joyChar:2 direction:JOYSTICK_LEFT] forKey:VC64Left2charKey];
    [defaults setInteger:[screen joyKeycode:2 direction:JOYSTICK_RIGHT] forKey:VC64Right2keycodeKey];
    [defaults setInteger:[screen joyChar:2 direction:JOYSTICK_RIGHT] forKey:VC64Right2charKey];
    [defaults setInteger:[screen joyKeycode:2 direction:JOYSTICK_DOWN] forKey:VC64Down2keycodeKey];
    [defaults setInteger:[screen joyChar:2 direction:JOYSTICK_DOWN] forKey:VC64Down2charKey];
    [defaults setInteger:[screen joyKeycode:2 direction:JOYSTICK_UP] forKey:VC64Up2keycodeKey];
    [defaults setInteger:[screen joyChar:2 direction:JOYSTICK_UP] forKey:VC64Up2charKey];
    [defaults setInteger:[screen joyKeycode:2 direction:JOYSTICK_FIRE] forKey:VC64Fire2keycodeKey];
    [defaults setInteger:[screen joyChar:2 direction:JOYSTICK_FIRE] forKey:VC64Fire2charKey];
    
	// Video 
    [defaults setFloat:[screen eyeX] forKey:VC64EyeX];
    [defaults setFloat:[screen eyeY] forKey:VC64EyeY];
    [defaults setFloat:[screen eyeZ] forKey:VC64EyeZ];
    [defaults setInteger:[c64 colorScheme] forKey:VC64ColorSchemeKey];
}

- (void)saveVirtualMachineUserDefaults
{
    NSLog(@"MyController::Saving virtual machine user defaults");
    
    NSUserDefaults *defaults;
    
    // Set standard user defaults
    defaults = [NSUserDefaults standardUserDefaults];
    
    // System
    [defaults setInteger:[c64 isNTSC] forKey:VC64PALorNTSCKey];
    
    // VC1541
    [defaults setBool:[c64 warpLoad] forKey:VC64WarpLoadKey];
    
    // Audio
    [defaults setBool:[c64 reSID] forKey:VC64SIDReSIDKey];
    [defaults setBool:[c64 audioFilter] forKey:VC64SIDFilterKey];
    [defaults setBool:[c64 chipModel] forKey:VC64SIDChipModelKey];
    [defaults setBool:[c64 samplingMethod] forKey:VC64SIDSamplingMethodKey];
}


// --------------------------------------------------------------------------------
//                                Notifications
// --------------------------------------------------------------------------------

- (void) windowDidBecomeMain:(NSNotification *)notification
{
	// NSLog(@"windowDidBecomeMain");
	[c64 enableAudio];	
}

- (void) windowDidResignMain:(NSNotification *)notification
{
	// NSLog(@"windowDidResignMain");
	[c64 disableAudio];
}

// --------------------------------------------------------------------------------
//                           Timer and message processing
// --------------------------------------------------------------------------------

- (void)timerFunc
{	
	if (timerLock == NULL)
		NSLog(@"TIMER IS NIL");
	[timerLock lock];
	
	// Do 6 times a second ...
	animationCounter++;
	
	// Process pending messages
	Message *message;
	while ((message = [c64 message]) != NULL) {
		[self processMessage:message];
	}
	
	// Refresh debug panel if open
	if ([c64 isRunning] && ([debugPanel state] == NSDrawerOpenState || [debugPanel state] == NSDrawerOpeningState)) {
		[self refresh];
	}
	
	// Do less times ... 
	if ((animationCounter & 0x01) == 0) {	
		[speedometer updateWithCurrentCycle:[c64 cycles] currentFrame:[screen frames]];
		[clockSpeed setStringValue:[NSString stringWithFormat:@"%.2f MHz %02d fps", [speedometer mhz], (int)[speedometer fps]]];
		[clockSpeedBar setFloatValue:10.0 * [speedometer mhz]];
	}
	
	[timerLock unlock];
}

- (void)processMessage:(Message *)msg
{
	switch (msg->id) {
			
		case MSG_ROM_MISSING:
			
			NSLog(@"MSG_ROM_MISSING");			
			assert(msg->i != 0);
			[self enableUserEditing:YES];	
			[self refresh];
            [self showRomDialog:msg];
            break;
			
		case MSG_ROM_LOADED:
			
			// Update ROM dialog
			if (romDialog != NULL) {
				[romDialog update:[c64 missingRoms]];
			}
			break;
			
		case MSG_ROM_COMPLETE:
			
			// Close ROM dialog if open
			if (romDialog) {					
				[NSApp endSheet:romDialog];
				[romDialog orderOut:nil];
				romDialog = NULL;
			}

            // Check for attached snapshot
            if ([[self document] snapshot]) {
                NSLog(@"Found attached snapshot");
                [c64 _loadFromSnapshot:[[self document] snapshot]];
            }				

			// Check for attached cartridge
			if ([[self document] cartridge]) {
				NSLog(@"Found attached cartridge");
				[self mountCartridge];
			}				

			// Start emulator
			[c64 run];
			//[screen zoom];
			//[screen scroll];
			[screen fadeIn];
			[screen setDrawC64texture:true];

			// Check for attached archive
			if ([[self document] archive]) {
                NSLog(@"Found attached archive");
                [self showMountDialog];
            }
			
			break;
						
		case MSG_RUN:
			[info setStringValue:@""];
			[self enableUserEditing:NO];
			[self refresh];
			[cheatboxPanel close];
			
			// disable undo because the internal state changes permanently
			[[self document] updateChangeCount:NSChangeDone];
			[[self undoManager] removeAllActions];			
			break;
			
		case MSG_HALT:
			[self enableUserEditing:YES];
			[self refresh];			
			break;
			
		case MSG_CPU:
			switch(msg->i) {
				case CPU::OK: 
				case CPU::SOFT_BREAKPOINT_REACHED:
					[info setStringValue:@""];
					break;
				case CPU::HARD_BREAKPOINT_REACHED:
                    [self debugOpenAction:self];
					break;
				case CPU::ILLEGAL_INSTRUCTION:
                    [self debugOpenAction:self];
					break;
				default:
					assert(0);
			}
			[self refresh];			
			break;
			
		case MSG_WARP:
			break;
			
		case MSG_LOG:
			break;
			
		case MSG_VC1541_ATTACHED:
			if (msg->i)
				[greenLED setImage:[NSImage imageNamed:@"LEDgreen"]];
			else
				[greenLED setImage:[NSImage imageNamed:@"LEDgray"]];	
			break;
			
		case MSG_VC1541_DISK:
			[drive setHidden:!msg->i];
			[eject setHidden:!msg->i];			
			break;
			
		case MSG_VC1541_LED:
			if (msg->i)
				[redLED setImage:[NSImage imageNamed:@"LEDred"]];
			else
				[redLED setImage:[NSImage imageNamed:@"LEDgray"]];			
			break;
			
		case MSG_VC1541_DATA:
			if (msg->i) {
				[c64 setIecBusIsBusy:true];
			} else {
				[c64 setIecBusIsBusy:false];
			}			
			break;
			
		case MSG_VC1541_MOTOR:
			break;
			
		case MSG_CARTRIDGE:
			[cartridgeIcon setHidden:!msg->i];
			[cartridgeEject setHidden:!msg->i];			
			break;

        case MSG_JOYSTICK_ATTACHED:
        case MSG_JOYSTICK_REMOVED:
            [self validateJoystickItems];
            break;
            
		default:
			assert(0);
	}
}

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
		N, Z, C, I, B, D, V,
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


// --------------------------------------------------------------------------------
// Action methods (main window area)
// --------------------------------------------------------------------------------

// Simulation speed

- (IBAction)warpAction:(id)sender
{
	NSLog(@"warpAction");	
	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] warpAction:@((int)![c64 warp])];
	if (![undo isUndoing]) [undo setActionName:@"Native speed"];
	
	[c64 setAlwaysWarp:![c64 warp]];
	[self refresh];
}

// Disk drive

- (IBAction)ejectAction:(id)sender
{
	NSLog(@"Ejecting disk...");
	[[c64 vc1541] ejectDisk];
}

- (IBAction)driveAction:(id)sender
{
	NSLog(@"Drive action...");
	if ([[c64 iec] isDriveConnected]) {
		[[c64 iec] disconnectDrive];
	} else {
		[[c64 iec] connectDrive];
	}
}

// Cartridge

- (IBAction)cartridgeEjectAction:(id)sender
{
	NSLog(@"cartridgeEjectAction");	
	[c64 detachCartridge];
	[[self document] setCartridge:NULL];
	[c64 reset];
}


// --------------------------------------------------------------------------------
//                                     Cartridges
// --------------------------------------------------------------------------------

- (BOOL)mountCartridge
{
	if ([[self document] cartridge] == NULL)
		return NO;

	[c64 attachCartridge:[[self document] cartridge]];
	[c64 reset];
	
	return YES;
}


// --------------------------------------------------------------------------------
//                                     Dialogs
// --------------------------------------------------------------------------------

- (bool)showPropertiesDialog
{
    // Initialize dialog
    [propertiesDialog initialize:self];

    // Open sheet
    [NSApp beginSheet:propertiesDialog
       modalForWindow:[[self document] windowForSheet]
        modalDelegate:self
       didEndSelector:NULL
          contextInfo:NULL];
    
    return YES;
}

- (IBAction)cancelPropertiesDialog:(id)sender
{
	// Hide sheet
	[propertiesDialog orderOut:sender];
	
	// Return to normal event handling
	[NSApp endSheet:propertiesDialog returnCode:1];
}

- (bool)showHardwareDialog
{
    // The hardware dialog required the disk name as argument (if any disk is present).
    // As the name is not directly acessible, we first convert the disk contents to an
    // archive, pick the name, and delete the archive. A NULL pointer is passed to
    // the hardware dialog, if no disk is present.
    
    NSString *name = NULL;
    unsigned files = 0;
    if ([[c64 vc1541] hasDisk]) {
        D64Archive *archive = [[c64 vc1541] archiveFromDrive];
        NSLog(@"Archive found");
        if (archive != NULL) {
            name = [NSString stringWithFormat:@"%s", archive->getName()];
            files = archive->getNumberOfItems();
            delete archive;
        }
    }
    
    // Initialize dialog
    [hardwareDialog initialize:self archiveName:name noOfFiles:files];
    
    // Open sheet
    [NSApp beginSheet:hardwareDialog
       modalForWindow:[[self document] windowForSheet]
        modalDelegate:self
       didEndSelector:NULL
          contextInfo:NULL];
    
    return YES;
}

- (IBAction)cancelHardwareDialog:(id)sender
{
    // Hide sheet
    [hardwareDialog orderOut:sender];
    
    // Return to normal event handling
    [NSApp endSheet:hardwareDialog returnCode:1];
}

- (bool)showRomDialog:(Message *)msg
{
    // Initialize dialog
    [romDialog initialize:msg->i];

    // Open sheet
    [NSApp beginSheet:romDialog
       modalForWindow:[[self document] windowForSheet]
        modalDelegate:self
       didEndSelector:NULL
          contextInfo:NULL];

    return YES;
}

- (IBAction)cancelRomDialog:(id)sender
{
	// Hide sheet
	[romDialog orderOut:sender];
	
	// Exit
	[[NSApplication sharedApplication] terminate: nil];
	
	// OLD BEHAVIOUR
	// Return to normal event handling
	// [NSApp endSheet:romDialog returnCode:1];
}

- (bool)showMountDialog
{
    // Only proceed if a an archive is present
	if (![[self document] archive])
		return NO;
	
    // Initialize dialog
    [mountDialog initializeAsMountDialog:[[self document] archive] c64proxy:c64];

    // Open sheet
	[NSApp beginSheet:mountDialog
	   modalForWindow:[[self document] windowForSheet]
		modalDelegate:self
	   didEndSelector:NULL
		  contextInfo:NULL];
	
	return YES;
}

- (IBAction)cancelMountDialog:(id)sender
{
    bool doEject = [mountDialog doEjectOnCancel];
    
	// Hide sheet
	[mountDialog orderOut:sender];
	
	// Return to normal event handling
	[NSApp endSheet:mountDialog returnCode:1];
    
    // Eject disk if requested
    if (doEject) {
        [[c64 vc1541] ejectDisk];
    }
}

- (IBAction)endMountDialog:(id)sender
{
    NSString *textToType =[mountDialog loadCommand];
    bool doMount = [mountDialog doMount];
    bool doType = [mountDialog doType];
    bool doFlash = [mountDialog doFlash];

    NSLog(@"Should mount: %ld", (long)doMount);
    NSLog(@"Should flash: %ld", (long)doFlash);
    NSLog(@"Should type:  %ld (%@)", (long)doType, textToType);
    
	// Rotate C64 screen
    if (doMount || doFlash)
        [screen rotate];
	
	// Hide sheet
	[mountDialog orderOut:sender];
	
	// Return to normal event handling
	[NSApp endSheet:mountDialog returnCode:1];
	
	// Mount image if requested
    if (doMount) {
        if (![c64 mountArchive:[[self document] archive]]) {
            NSLog(@"FAILED TO MOUNT ARCHIVE");
        }
    }
    
    // Flash data if requested
    if (doFlash) {
        [c64 flushArchive:[[self document] archive] item:[mountDialog selection]];
    }
    
    // Type command if requested
    if (doType) {
        usleep(100000);
        [[c64 keyboard] typeText:textToType];
        [[c64 keyboard] typeText:@"\n"];
    }
}


- (BOOL)exportToD64:(NSString *)path
{
    NSLog(@"Writing drive contents to D64 archive in %@...",path);

    // Determine full destination path
    NSString *archivePath = [NSString stringWithFormat:@"%s", [[self document] archive]->getPath()];
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

- (void)keyDown:(NSEvent *)event
{
	// Pass all keyboard events to C64
	[screen keyDown:event];
}

- (void)keyUp:(NSEvent *)event
{
	// Pass all keyboard events to C64
	[screen keyUp:event];
}


@end
