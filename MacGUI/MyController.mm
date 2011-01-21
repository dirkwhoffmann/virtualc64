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
	NSLog(@"dealloc");
	
	[preferenceController release];
	NSLog(@"Preference controller released");
	[super dealloc];
	NSLog(@"super released");
}

- (void)windowWillClose:(NSNotification *)aNotification
{
	NSLog(@"windowWillClose");
	
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

	// Create virtual C64
	c64 = [(MyDocument *)[self document] c64];
	[screen setC64:[c64 c64]];
	
	// Joystick handling
	joystickManager = new JoystickManager( c64 );
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
	// Load user defaults
	[self loadUserDefaults];
		
	// Launch emulator
	[c64 run];
}

// --------------------------------------------------------------------------------
//                                   User defaults
// --------------------------------------------------------------------------------

+ (void)registerStandardDefaults
{
	NSLog(@"Registering standard user defaults");
	
	NSMutableDictionary *defaultValues = [NSMutableDictionary dictionary];
	
	// System 
	[defaultValues setObject:[NSNumber numberWithInt:0/*PAL*/] forKey:VC64PALorNTSCKey];
	[defaultValues setObject:@"" forKey:VC64BasicRomFileKey];
	[defaultValues setObject:@"" forKey:VC64CharRomFileKey];
	[defaultValues setObject:@"" forKey:VC64KernelRomFileKey];
	[defaultValues setObject:@"" forKey:VC64VC1541RomFileKey];
	
	// Peripherals
	[defaultValues setObject:[NSNumber numberWithBool:YES] forKey:VC64WarpLoadKey];
	
	// Audio
	[defaultValues setObject:[NSNumber numberWithBool:YES] forKey:VC64SIDFilterKey];
	
	// Video
	[defaultValues setObject:[NSNumber numberWithFloat:PAL_INITIAL_EYE_X] forKey:VC64EyeX];
	[defaultValues setObject:[NSNumber numberWithFloat:PAL_INITIAL_EYE_Y] forKey:VC64EyeY];
	[defaultValues setObject:[NSNumber numberWithFloat:PAL_INITIAL_EYE_Z] forKey:VC64EyeZ];

	[defaultValues setObject:[NSNumber numberWithInt:VIC::CUSTOM_PALETTE] forKey:VC64ColorSchemeKey];
	[defaultValues setObject:[NSNumber numberWithInt:0] forKey:VC64VideoFilterKey];	
	
	[defaultValues setObject:[NSNumber numberWithInt:0x101010ff] forKey:VC64CustomCol0Key];
	[defaultValues setObject:[NSNumber numberWithInt:0xffffffff] forKey:VC64CustomCol1Key];
	[defaultValues setObject:[NSNumber numberWithInt:0xe04040ff] forKey:VC64CustomCol2Key];
	[defaultValues setObject:[NSNumber numberWithInt:0x60ffffff] forKey:VC64CustomCol3Key];
	[defaultValues setObject:[NSNumber numberWithInt:0xe060e0ff] forKey:VC64CustomCol4Key];
	[defaultValues setObject:[NSNumber numberWithInt:0x40e040ff] forKey:VC64CustomCol5Key];
	[defaultValues setObject:[NSNumber numberWithInt:0x4040e0ff] forKey:VC64CustomCol6Key];
	[defaultValues setObject:[NSNumber numberWithInt:0xffff40ff] forKey:VC64CustomCol7Key];
	[defaultValues setObject:[NSNumber numberWithInt:0xe0a040ff] forKey:VC64CustomCol8Key];
	[defaultValues setObject:[NSNumber numberWithInt:0x9c7448ff] forKey:VC64CustomCol9Key];
	[defaultValues setObject:[NSNumber numberWithInt:0xffa0a0ff] forKey:VC64CustomCol10Key];
	[defaultValues setObject:[NSNumber numberWithInt:0x545454ff] forKey:VC64CustomCol11Key];
	[defaultValues setObject:[NSNumber numberWithInt:0x888888ff] forKey:VC64CustomCol12Key];
	[defaultValues setObject:[NSNumber numberWithInt:0xa0ffa0ff] forKey:VC64CustomCol13Key];
	[defaultValues setObject:[NSNumber numberWithInt:0xa0a0ffff] forKey:VC64CustomCol14Key];
	[defaultValues setObject:[NSNumber numberWithInt:0xc0c0c0ff] forKey:VC64CustomCol15Key];
	
	// Register dictionary
	[[NSUserDefaults standardUserDefaults] registerDefaults:defaultValues];
}

- (void)loadUserDefaults
{
	NSLog(@"Loading user defaults");
	
	int colorScheme;
	NSUserDefaults *defaults;
	
	// Set standard user defaults
	defaults = [NSUserDefaults standardUserDefaults];
	
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
	// [c64 sidEnableFilter:[defaults boolForKey:VC64SIDFilterKey]];
	
	// Video 
	// [c64 vicSetVideoFilter:[defaults integerForKey:VC64VideoFilterKey];
	
	[screen setEyeX:[defaults floatForKey:VC64EyeX]];
	[screen setEyeY:[defaults floatForKey:VC64EyeY]];
	[screen setEyeZ:[defaults floatForKey:VC64EyeZ]];
	
	colorScheme = [defaults integerForKey:VC64ColorSchemeKey];
	if (colorScheme == VIC::CUSTOM_PALETTE) {
		NSLog(@"Applying custom colors...");
		[[c64 vic] setColorInt:0 rgba:[defaults integerForKey:VC64CustomCol0Key]];
		[[c64 vic] setColorInt:1 rgba:[defaults integerForKey:VC64CustomCol1Key]];
		[[c64 vic] setColorInt:2 rgba:[defaults integerForKey:VC64CustomCol2Key]];
		[[c64 vic] setColorInt:3 rgba:[defaults integerForKey:VC64CustomCol3Key]];
		[[c64 vic] setColorInt:4 rgba:[defaults integerForKey:VC64CustomCol4Key]];
		[[c64 vic] setColorInt:5 rgba:[defaults integerForKey:VC64CustomCol5Key]];
		[[c64 vic] setColorInt:6 rgba:[defaults integerForKey:VC64CustomCol6Key]];
		[[c64 vic] setColorInt:7 rgba:[defaults integerForKey:VC64CustomCol7Key]];
		[[c64 vic] setColorInt:8 rgba:[defaults integerForKey:VC64CustomCol8Key]];
		[[c64 vic] setColorInt:9 rgba:[defaults integerForKey:VC64CustomCol9Key]];
		[[c64 vic] setColorInt:10 rgba:[defaults integerForKey:VC64CustomCol10Key]];
		[[c64 vic] setColorInt:11 rgba:[defaults integerForKey:VC64CustomCol11Key]];
		[[c64 vic] setColorInt:12 rgba:[defaults integerForKey:VC64CustomCol12Key]];
		[[c64 vic] setColorInt:13 rgba:[defaults integerForKey:VC64CustomCol13Key]];
		[[c64 vic] setColorInt:14 rgba:[defaults integerForKey:VC64CustomCol14Key]];
		[[c64 vic] setColorInt:15 rgba:[defaults integerForKey:VC64CustomCol15Key]];
	} else {
		[[c64 vic] setColorScheme:(VIC::ColorScheme)colorScheme];
	}
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
	while ((message = [c64 getMessage]) != NULL) {
		[self processMessage:message];
	}
	
	// Refresh debug panel if open
	if ([c64 isRunning] && ([debug_panel state] == NSDrawerOpenState || [debug_panel state] == NSDrawerOpeningState)) {
		[self refresh];
	}
	
	// Do less times ... 
	if ((animationCounter & 0x01) == 0) {	
		[speedometer updateWithCurrentCycle:[c64 getCycles] currentFrame:[screen frames]];
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
			[romDialog initialize:msg->i];
			[NSApp beginSheet:romDialog
			   modalForWindow:[[self document] windowForSheet]
				modalDelegate:self
			   didEndSelector:NULL
				  contextInfo:NULL];	
			break;
			
		case MSG_ROM_LOADED:
			
			switch (msg->i) {
				case BASIC_ROM:
					[info setStringValue:@"Basic Rom loaded"];
					// NSLog(@"BASIC ROM loaded");
					break;
				case CHAR_ROM:
					[info setStringValue:@"Character Rom loaded"];
					// NSLog(@"CHARACTER ROM loaded");
					break;
				case KERNEL_ROM:
					[info setStringValue:@"Kernel Rom loaded"];
					// NSLog(@"KERNEL ROM loaded");
					break;
				case VC1541_ROM:
					[info setStringValue:@"VC1541 Rom loaded"];
					// NSLog(@"VC1541 ROM loaded");
					break;
			}		
			
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

			// Check for attached cartridge
			if ([[self document] cartridge]) {
				NSLog(@"Found attached cartridge");
				[self mountCartridge];
			}				

			// Start emulator
			[c64 run];
			[screen zoom];
			//[screen scroll];
			[screen setDrawC64texture:true];

			// Check for attached archive
			if ([[self document] archive]) {
				NSLog(@"Found attached archive");
				[self showMountDialog];
			}
			
			break;
						
		case MSG_RUN:
			// NSLog(@"runAction");
			[info setStringValue:@""];
			[self enableUserEditing:NO];
			[self refresh];
			[cheatboxPanel close];
			
			// disable undo because the internal state changes permanently
			[[self document] updateChangeCount:NSChangeDone];
			[[self undoManager] removeAllActions];			
			break;
			
		case MSG_HALT:
			// NSLog(@"haltAction");
			[self enableUserEditing:YES];	
			[self refresh];			
			break;
			
		case MSG_CPU:
			// NSLog(@"cpuAction");
			switch(msg->i) {
				case CPU::OK: 
				case CPU::SOFT_BREAKPOINT_REACHED:
					[info setStringValue:@""];
					break;
				case CPU::HARD_BREAKPOINT_REACHED:
					[info setStringValue:@"Breakpoint reached"];
					break;
				case CPU::ILLEGAL_INSTRUCTION:
					[info setStringValue:@"Illegal instruction"];
					break;
				default:
					assert(0);
			}
			[self refresh];			
			break;
			
		case MSG_WARP:
			// NSLog(@"warpmodeAction");
			break;
			
		case MSG_LOG:
			break;
			
		case MSG_VC1541_ATTACHED:
			// NSLog(@"driveAttachedAction");
			if (msg->i)
				[greenLED setImage:[NSImage imageNamed:@"LEDgreen"]];
			else
				[greenLED setImage:[NSImage imageNamed:@"LEDgray"]];			
			break;
			
		case MSG_VC1541_DISC:
			// NSLog(@"driveDiscAction");
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
			// NSLog(@"driveDataAction (%s)", msg->i ? "on" : "off");
			
			if (msg->i) {
				[driveBusy setHidden:false];
				[driveBusy startAnimation:self];
			} else {
				[driveBusy stopAnimation:self];
				[driveBusy setHidden:true];		
			}			
			break;
			
		case MSG_VC1541_MOTOR:
			// NSLog(@"driveMotorAction");
			break;
			
		case MSG_CARTRIDGE:
			// NSLog(@"MSG_CARTRIDGE");
			[cartridgeIcon setHidden:!msg->i];
			[cartridgeEject setHidden:!msg->i];			
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

- (void)refresh:(NSFormatter *)byteFormatter word:(NSFormatter *)wordFormatter disassembler:(NSFormatter *)disassembler
{		
	NSControl *ByteFormatterControls[] = { 
		// CPU panel
		sp, a, x, y,
		// CIA panel
		ciaDataPortA, ciaDataPortDirectionA, ciaDataPortB, ciaDataPortDirectionB,
		todHours, todMinutes, todSeconds, todTenth, alarmHours, alarmMinutes, alarmSeconds, alarmTenth,
		// VIC panel
		VicSpriteX, VicSpriteY, VicSpriteColor, 
 		NULL };
	
	NSControl *WordFormatterControls[] = { 
		// CPU panel
		pc, breakpoint,
		// Memory panel
		addr_search,
		// CIA panel
		ciaTimerA, ciaLatchedTimerA, ciaTimerB, ciaLatchedTimerB,
		// VIC panel
		VicRasterline, VicRasterInterrupt,
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
		ciaDataPortA, ciaDataPortDirectionA, ciaTimerA, ciaLatchedTimerA, 
		ciaRunningA, ciaOneShotA, ciaCountUnderflowsA, ciaSignalPendingA, ciaInterruptEnableA,
		ciaDataPortB, ciaDataPortDirectionB, ciaTimerB, ciaLatchedTimerB, 
		ciaRunningB, ciaOneShotB, ciaCountUnderflowsB, ciaSignalPendingB, ciaInterruptEnableB,
		todHours, todMinutes, todSeconds, todTenth,
		alarmHours, alarmMinutes, alarmSeconds, alarmTenth, todInterruptEnabled,
		// VIC panel
		VicSpriteX, VicSpriteY, VicSpriteColor, VicRasterline, VicRasterInterrupt, VicDX, VicDY,
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

- (IBAction)stepperAction:(id)sender
{
	NSLog(@"stepperAction");
	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] stepperAction:[NSNumber numberWithInt:-[sender intValue]]];
	if (![undo isUndoing]) [undo setActionName:@"Clock frequency"];
	
	int newFrameDelay = [c64 getFrameDelay] - 1000*[sender intValue];
	
	if (newFrameDelay < 0)
		newFrameDelay = 0;
	
	[c64 setFrameDelay:newFrameDelay];
}

- (IBAction)warpAction:(id)sender
{
	NSLog(@"warpAction");	
	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] warpAction:[NSNumber numberWithInt:![c64 warp]]];
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
	// delete [[self document] cartridge];
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

- (BOOL)showMountDialog
{
	if ([[self document] archive] == NULL)
		return NO;
	
	[mountDialog initialize:[[self document] archive]];
	
	[NSApp beginSheet:mountDialog
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
	
	// Return to normal event handling
	[NSApp endSheet:romDialog returnCode:1];
}

- (IBAction)cancelMountDialog:(id)sender
{
	// Hide sheet
	[mountDialog orderOut:sender];
	
	// Return to normal event handling
	[NSApp endSheet:mountDialog returnCode:1];
}

- (IBAction)endMountDialogAndMount:(id)sender
{
	// Rotate C64 screen
	[screen rotate];
	
	// Hide sheet
	[mountDialog orderOut:sender];
	
	// Return to normal event handling
	[NSApp endSheet:mountDialog returnCode:1];
	
	[c64 mountArchive:[[self document] archive]];
}

- (IBAction)endMountDialogAndFlash:(id)sender
{
	// Rotate C64 screen
	[screen rotate];
	
	// Hide sheet
	[mountDialog orderOut:sender];
	
	// Return to normal event handling
	[NSApp endSheet:mountDialog returnCode:1];
	
	// Try to mount archive
	[c64 mountArchive:[[self document] archive]];
	
	// Load clean image 
	[c64 fastReset];
	
	// Flash selected file into memory
	[c64 flushArchive:[[self document] archive] item:[mountDialog getSelectedFile]];
	
	// Wait and type "RUN"
	usleep(1000000);
	[[c64 keyboard] typeRun];
}

@end
