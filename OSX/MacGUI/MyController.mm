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

@implementation MyController

@synthesize c64, metalScreen;

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
	
	// stop metal view
    [metalScreen cleanup];
    
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
	
    // Add bottom bar
    [[self window] setAutorecalculatesContentBorderThickness:YES forEdge:NSMinYEdge];
    [[self window] setContentBorderThickness:32.0 forEdge: NSMinYEdge];
    
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
    fps = PAL_REFRESH_RATE;
    mhz = CPU::CLOCK_FREQUENCY_PAL / 100000;

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
    
    // Get metal running
    [metalScreen setupMetal];
    NSLog(@"Metal is up and running");
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    NSLog(@"MyController::applicationDidFinishLaunching\n");
    
    // Start the crash reporter
    [self checkForCrashReports];
}

// --------------------------------------------------------------------------------
//                                   Full screen
// --------------------------------------------------------------------------------

- (void)windowWillEnterFullScreen:(NSNotification *)notification
{
    NSLog(@"windowWillEnterFullScreen");
    
    // [metalScreen setDrawIn3D:NO];
    [metalScreen setFullscreen:YES];
    [self hideStatusBar];
}

- (void)windowDidEnterFullScreen:(NSNotification *)notification
{
    NSLog(@"windowDidEnterFullScreen");
}

- (void)windowWillExitFullScreen:(NSNotification *)notification
{
    NSLog(@"windowWillExitFullScreen");
    
    // [metalScreen setDrawIn3D:YES];
    // MOVE TO windowDidExitFullScreen??
    [metalScreen setFullscreen:NO];
    [self showStatusBar];
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
    
    // NSRect myRect = [screen bounds];
    NSRect myRect = [metalScreen bounds];
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
    [defaultValues setObject:@YES forKey:VC64DriveNoiseKey];
    [defaultValues setObject:@YES forKey:VC64BitAccuracyKey];

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
	[defaultValues setObject:@0 forKey:VC64VideoFilterKey];
    [defaultValues setObject:@NO forKey:VC64FullscreenKeepAspectRatioKey];
		
	// Register dictionary
	[[NSUserDefaults standardUserDefaults] registerDefaults:defaultValues];
}

- (void)loadUserDefaults
{
	NSLog(@"MyController::Loading emulator user defaults");
	
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
			
    // Joysticks
    [metalScreen setJoyKeycode:[defaults integerForKey:VC64Left1keycodeKey] keymap:1 direction:JOYSTICK_LEFT];
    [metalScreen setJoyChar:[defaults integerForKey:VC64Left1charKey] keymap:1 direction:JOYSTICK_LEFT];
    [metalScreen setJoyKeycode:[defaults integerForKey:VC64Right1keycodeKey] keymap:1 direction:JOYSTICK_RIGHT];
    [metalScreen setJoyChar:[defaults integerForKey:VC64Right1charKey] keymap:1 direction:JOYSTICK_RIGHT];
    [metalScreen setJoyKeycode:[defaults integerForKey:VC64Down1keycodeKey] keymap:1 direction:JOYSTICK_DOWN];
    [metalScreen setJoyChar:[defaults integerForKey:VC64Down1charKey] keymap:1 direction:JOYSTICK_DOWN];
    [metalScreen setJoyKeycode:[defaults integerForKey:VC64Up1keycodeKey] keymap:1 direction:JOYSTICK_UP];
    [metalScreen setJoyChar:[defaults integerForKey:VC64Up1charKey] keymap:1 direction:JOYSTICK_UP];
    [metalScreen setJoyKeycode:[defaults integerForKey:VC64Fire1keycodeKey] keymap:1 direction:JOYSTICK_FIRE];
    [metalScreen setJoyChar:[defaults integerForKey:VC64Fire1charKey] keymap:1 direction:JOYSTICK_FIRE];

    [metalScreen setJoyKeycode:[defaults integerForKey:VC64Left2keycodeKey] keymap:2 direction:JOYSTICK_LEFT];
    [metalScreen setJoyChar:[defaults integerForKey:VC64Left2charKey] keymap:2 direction:JOYSTICK_LEFT];
    [metalScreen setJoyKeycode:[defaults integerForKey:VC64Right2keycodeKey] keymap:2 direction:JOYSTICK_RIGHT];
    [metalScreen setJoyChar:[defaults integerForKey:VC64Right2charKey] keymap:2 direction:JOYSTICK_RIGHT];
    [metalScreen setJoyKeycode:[defaults integerForKey:VC64Down2keycodeKey] keymap:2 direction:JOYSTICK_DOWN];
    [metalScreen setJoyChar:[defaults integerForKey:VC64Down2charKey] keymap:2 direction:JOYSTICK_DOWN];
    [metalScreen setJoyKeycode:[defaults integerForKey:VC64Up2keycodeKey] keymap:2 direction:JOYSTICK_UP];
    [metalScreen setJoyChar:[defaults integerForKey:VC64Up2charKey] keymap:2 direction:JOYSTICK_UP];
    [metalScreen setJoyKeycode:[defaults integerForKey:VC64Fire2keycodeKey] keymap:2 direction:JOYSTICK_FIRE];
    [metalScreen setJoyChar:[defaults integerForKey:VC64Fire2charKey] keymap:2 direction:JOYSTICK_FIRE];
    
	// Video
    [metalScreen setEyeX:[defaults floatForKey:VC64EyeX]];
    [metalScreen setEyeY:[defaults floatForKey:VC64EyeY]];
    [metalScreen setEyeZ:[defaults floatForKey:VC64EyeZ]];

	// [c64 setVideoFilter:[defaults integerForKey:VC64VideoFilterKey]];
    [c64 setColorScheme:[defaults integerForKey:VC64ColorSchemeKey]];
    [metalScreen setVideoFilter:[defaults integerForKey:VC64VideoFilterKey]];
    [metalScreen setFullscreenKeepAspectRatio:[defaults boolForKey:VC64FullscreenKeepAspectRatioKey]];
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
    [[c64 vc1541] setSendSoundMessages:[defaults boolForKey:VC64DriveNoiseKey]];
    [[c64 vc1541] setBitAccuracy:[defaults boolForKey:VC64BitAccuracyKey]];
    
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
    [defaults setInteger:[metalScreen joyKeycode:1 direction:JOYSTICK_LEFT] forKey:VC64Left1keycodeKey];
    [defaults setInteger:[metalScreen joyChar:1 direction:JOYSTICK_LEFT] forKey:VC64Left1charKey];
    [defaults setInteger:[metalScreen joyKeycode:1 direction:JOYSTICK_RIGHT] forKey:VC64Right1keycodeKey];
    [defaults setInteger:[metalScreen joyChar:1 direction:JOYSTICK_RIGHT] forKey:VC64Right1charKey];
    [defaults setInteger:[metalScreen joyKeycode:1 direction:JOYSTICK_DOWN] forKey:VC64Down1keycodeKey];
    [defaults setInteger:[metalScreen joyChar:1 direction:JOYSTICK_DOWN] forKey:VC64Down1charKey];
    [defaults setInteger:[metalScreen joyKeycode:1 direction:JOYSTICK_UP] forKey:VC64Up1keycodeKey];
    [defaults setInteger:[metalScreen joyChar:1 direction:JOYSTICK_UP] forKey:VC64Up1charKey];
    [defaults setInteger:[metalScreen joyKeycode:1 direction:JOYSTICK_FIRE] forKey:VC64Fire1keycodeKey];
    [defaults setInteger:[metalScreen joyChar:1 direction:JOYSTICK_FIRE] forKey:VC64Fire1charKey];
    
    [defaults setInteger:[metalScreen joyKeycode:2 direction:JOYSTICK_LEFT] forKey:VC64Left2keycodeKey];
    [defaults setInteger:[metalScreen joyChar:2 direction:JOYSTICK_LEFT] forKey:VC64Left2charKey];
    [defaults setInteger:[metalScreen joyKeycode:2 direction:JOYSTICK_RIGHT] forKey:VC64Right2keycodeKey];
    [defaults setInteger:[metalScreen joyChar:2 direction:JOYSTICK_RIGHT] forKey:VC64Right2charKey];
    [defaults setInteger:[metalScreen joyKeycode:2 direction:JOYSTICK_DOWN] forKey:VC64Down2keycodeKey];
    [defaults setInteger:[metalScreen joyChar:2 direction:JOYSTICK_DOWN] forKey:VC64Down2charKey];
    [defaults setInteger:[metalScreen joyKeycode:2 direction:JOYSTICK_UP] forKey:VC64Up2keycodeKey];
    [defaults setInteger:[metalScreen joyChar:2 direction:JOYSTICK_UP] forKey:VC64Up2charKey];
    [defaults setInteger:[metalScreen joyKeycode:2 direction:JOYSTICK_FIRE] forKey:VC64Fire2keycodeKey];
    [defaults setInteger:[metalScreen joyChar:2 direction:JOYSTICK_FIRE] forKey:VC64Fire2charKey];
    
	// Video 
    [defaults setFloat:[metalScreen eyeX] forKey:VC64EyeX];
    [defaults setFloat:[metalScreen eyeY] forKey:VC64EyeY];
    [defaults setFloat:[metalScreen eyeZ] forKey:VC64EyeZ];
    [defaults setInteger:[c64 colorScheme] forKey:VC64ColorSchemeKey];
    [defaults setInteger:[metalScreen videoFilter] forKey:VC64VideoFilterKey];
    [defaults setInteger:[metalScreen fullscreenKeepAspectRatio] forKey:VC64FullscreenKeepAspectRatioKey];
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
    [defaults setBool:[[c64 vc1541] soundMessagesEnabled] forKey:VC64DriveNoiseKey];
    [defaults setBool:[[c64 vc1541] bitAccuracy] forKey:VC64BitAccuracyKey];
    
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
    
    // Update tape progress icon
    // Note: The tape progress icon is not switched on or off by a "push" message, because
    // some games continously switch on and off the datasette motor. This would quickly
    // overflow the message queue.
    if ([[c64 datasette] motor] != [c64 tapeBusIsBusy]) {
        if ([[c64 datasette] motor] && [[c64 datasette] playKey]) {
            [tapeProgress startAnimation:nil];
            [c64 setTapeBusIsBusy:YES];
        } else {
            [tapeProgress stopAnimation:nil];
            [c64 setTapeBusIsBusy:NO];
        }
    }
        
	// Refresh debug panel if open
	if ([c64 isRunning] && ([debugPanel state] == NSDrawerOpenState || [debugPanel state] == NSDrawerOpeningState)) {
		[self refresh];
	}
	
	// Do less times ... 
	if ((animationCounter & 0x1) == 0) {
		[speedometer updateWithCurrentCycle:[c64 cycles] currentFrame:[c64 frames] expectedSpeed:0.0];
        mhz = 0.6 * mhz + 0.4 * (round([speedometer mhz] * 100.0) / 100.0);
        fps = 0.6 * fps + 0.4 * round([speedometer fps]);
		[clockSpeed setStringValue:[NSString stringWithFormat:@"%.2f MHz %.0f fps", mhz, fps]];
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
            [metalScreen blendIn];
            [metalScreen setDrawC64texture:true];

            // Check for attached tape
            if ([[self document]  tape]) {
                NSLog(@"Found attached tape");
                [self showTapeDialog];
            }

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
        case MSG_ALWAYS_WARP:
            if ([c64 alwaysWarp]) {
                [warpIcon setImage:[NSImage imageNamed:@"pin_red"]];
            } else if ([c64 warp]) {
                [warpIcon setImage:[NSImage imageNamed:@"clock_red"]];
            } else {
                [warpIcon setImage:[NSImage imageNamed:@"clock_green"]];
            }
            break;
            			
		case MSG_LOG:
			break;
			
		case MSG_VC1541_ATTACHED:
            if (msg->i)
				[greenLED setImage:[NSImage imageNamed:@"LEDgreen"]];
            else
				[greenLED setImage:[NSImage imageNamed:@"LEDgray"]];
			break;
			
        case MSG_VC1541_ATTACHED_SOUND:
            if (msg->i) {
                // [[c64 vc1541] playSound:@"1541_power_on_0" volume:0.2];
            } else {
                // [[c64 vc1541] playSound:@"1541_track_change_0" volume:0.6];
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
			if (msg->i)
				[redLED setImage:[NSImage imageNamed:@"LEDred"]];
			else
				[redLED setImage:[NSImage imageNamed:@"LEDgray"]];
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
            
		case MSG_CARTRIDGE:
			[cartridgeIcon setHidden:!msg->i];
			[cartridgeEject setHidden:!msg->i];			
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
            
        case MSG_JOYSTICK_ATTACHED:
        case MSG_JOYSTICK_REMOVED:
            [self validateJoystickItems];
            break;
        
        case MSG_PAL:
        case MSG_NTSC:
            [metalScreen updateScreenGeometry];
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

- (IBAction)driveAction:(id)sender
{
    NSLog(@"Drive action...");
    if ([[c64 iec] isDriveConnected]) {
        [[c64 iec] disconnectDrive];
    } else {
        [[c64 iec] connectDrive];
    }
}

- (IBAction)driveEjectAction:(id)sender
{
    NSLog(@"driveEjectAction");

    if (![[c64 vc1541] DiskModified]) {
        [[c64 vc1541] ejectDisk];
        return;
    }
        
    NSAlert *alert = [[NSAlert alloc] init];
    
    [alert setIcon:[NSImage imageNamed:@"diskette"]];
    [alert addButtonWithTitle:@"Export..."];
    [alert addButtonWithTitle:@"Eject"];
    [alert addButtonWithTitle:@"Cancel"];
    [alert setMessageText: @"Do you want to export the currently inserted disk to a D64 archive?"];
    [alert setInformativeText: @"Your changes will be lost if you don’t save them."];
    [alert setAlertStyle: NSCriticalAlertStyle];
    
    unsigned result = [alert runModal];
    
    if (result == NSAlertFirstButtonReturn) {

        if ([self exportDiskDialogWorker:3 /* D64 format tag */]) {
            NSLog(@"Disk saved. Ejecting...");
            [[c64 vc1541] ejectDisk];
        } else {
            NSLog(@"Export dialog cancelled. Ask again...");
            [self driveEjectAction:sender];
        }
    }

    if (result == NSAlertSecondButtonReturn) {
        NSLog(@"Ejecting disk...");
        [[c64 vc1541] ejectDisk];
    }

    if (result == NSAlertThirdButtonReturn) {
        NSLog(@"Canceling disk data loss warning dialog...");
    }
}

- (IBAction)tapeEjectAction:(id)sender
{
    NSLog(@"tapeEjectAction");
    [[c64 datasette] ejectTape];
    // [[self document] setTape:NULL];
}

- (IBAction)cartridgeEjectAction:(id)sender
{
	NSLog(@"cartridgeEjectAAction");
	[c64 detachCartridge];
	[[self document] setCartridge:NULL];
	[c64 reset];
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
#if 0
    [[[self document] windowForSheet] beginSheet:propertiesDialog
                               completionHandler:^(NSModalResponse returnCode) {
                                   NSLog(@"completionHandler called");
                               }];
#endif
    
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
    // [[[self document] windowForSheet] endSheet:[self window] returnCode:NSModalResponseCancel];
    [NSApp endSheet:propertiesDialog returnCode:1];
}

- (bool)showHardwareDialog
{
    // DEPRECATED: ONLY APPLIES TO MEDIA DIALOG
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

- (bool)showMediaDialog
{
    // The media dialog required the disk name as argument (if any disk is present).
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
    [mediaDialog initialize:self archiveName:name noOfFiles:files];
    
    // Open sheet
    [NSApp beginSheet:mediaDialog
       modalForWindow:[[self document] windowForSheet]
        modalDelegate:self
       didEndSelector:NULL
          contextInfo:NULL];
    
    return YES;
}

- (IBAction)cancelMediaDialog:(id)sender
{
    // Hide sheet
    [mediaDialog orderOut:sender];
    
    // Return to normal event handling
    [NSApp endSheet:mediaDialog returnCode:1];
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
    [mountDialog initialize:[[self document] archive] c64proxy:c64];

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
	// Hide sheet
	[mountDialog orderOut:sender];
	
	// Return to normal event handling
	[NSApp endSheet:mountDialog returnCode:1];    
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
    if (doMount || doFlash) {
        [metalScreen rotate];
    }
    
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
        [[c64 keyboard] typeText:[NSString stringWithFormat:@"%@\n", textToType] withDelay:500000];
    }
}

- (bool)showTapeDialog
{
    // Only proceed if a a tape image is present
    if (![[self document] tape])
        return NO;
    
    // Initialize dialog
    [tapeDialog initialize:[[self document] tape] c64proxy:c64];
    
    // Open sheet
    [NSApp beginSheet:tapeDialog
       modalForWindow:[[self document] windowForSheet]
        modalDelegate:self
       didEndSelector:NULL
          contextInfo:NULL];
    
    return YES;
}

- (IBAction)cancelTapeDialog:(id)sender
{
    // Hide sheet
    [tapeDialog orderOut:sender];
    
    // Return to normal event handling
    [NSApp endSheet:tapeDialog returnCode:1];
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
    
    // Return to normal event handling
    [NSApp endSheet:tapeDialog returnCode:1];
    
    // Insert tape into datasette
    [c64 insertTape:[[self document] tape]];
    
    // Type command if requested
    if (doAutoType) {
        [[c64 keyboard] typeText:textToType withDelay:500000];
    }
    
    if (doAutoType && doPressPlay) {
        dispatch_async(dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
                       ^{ usleep(400000); [[c64 datasette] pressPlay]; });
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
    [metalScreen keyDown:event];
}

- (void)keyUp:(NSEvent *)event
{
	// Pass all keyboard events to C64
    [metalScreen keyUp:event];
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
    
    [[c64 keyboard] typeText:text];
}


@end
