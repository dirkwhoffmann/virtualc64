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


#import "MyController.h"
#import "MyControllerCpuPanel.h"
#import "MyControllerMemoryPanel.h"
#import "MyControllerCiaPanel.h"
#import "MyControllerVicPanel.h"
#import "MyDocument.h"

@implementation MyController

@synthesize c64;

// --------------------------------------------------------------------------------
//                          Construction and Destruction
// --------------------------------------------------------------------------------

+ (void)initialize {
	
	NSLog(@"initialize");
		
	// Create a dictionary
	NSMutableDictionary *defaultValues = [NSMutableDictionary dictionary];
	
	// Put default values into dictionary
	
	// System 
	[defaultValues setObject:[NSNumber numberWithInt:1] forKey:VC64PALorNTSCKey];
	[defaultValues setObject:@"" forKey:VC64BasicRomFileKey];
	[defaultValues setObject:@"" forKey:VC64CharRomFileKey];
	[defaultValues setObject:@"" forKey:VC64KernelRomFileKey];
	[defaultValues setObject:@"" forKey:VC64VC1541RomFileKey];
	
	// Peripherals
	[defaultValues setObject:[NSNumber numberWithBool:YES] forKey:VC64WarpLoadKey];
	
	// Audio
	[defaultValues setObject:[NSNumber numberWithBool:YES] forKey:VC64SIDFilterKey];
	
	// Video
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
	
	// Register the dictionary of defaults
	[[NSUserDefaults standardUserDefaults] registerDefaults:defaultValues];
}

- (id)init  
{	
	NSLog(@"init");
	
    self = [super init];
    if (self) {
		// Change working directory to the main bundle ressource path. We may find some ROMs there...
		NSBundle* mainBundle = [NSBundle mainBundle];
		NSString *path = [mainBundle resourcePath];
		if (chdir([path UTF8String]) != 0)
			NSLog(@"WARNING: Could not change working directory.");
	}
	timer = nil;
	timerLock = nil;
	animationCounter = 0;
	speedometer = nil;
	
	snapshot = NULL;
	
    return self;
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
	
	// Initialize variables
	selectedSprite = 0;
	
	// Create virtual C64
	c64 = [(MyDocument *)[self document] c64];
	[screen setC64:[c64 c64]];
	
	// Load snapshot if applicable
	if (snapshot != NULL) {
		[c64 initWithContentsOfSnapshot:snapshot];
		// snapshot->writeToC64([c64 getC64]);
		delete snapshot;
		snapshot = NULL;
	}
	
	disassembleStartAddr = [[c64 cpu] getPC];
	
	// Joystick handling
	joystickManager = new JoystickManager( c64 );
	joystickManager->Initialize();
	
	int portA = [c64 getPortAssignment:0];
	int portB = [c64 getPortAssignment:1];
	NSLog(@"Port A = %d port B = %d", portA, portB);
	NSImage *imagePortA;
	NSImage *imagePortB;
	
	if(portA == IPD_JOYSTICK_1) {
		imagePortA = [NSImage imageNamed:@"joystick1_32"];
	} else if(portA == IPD_JOYSTICK_2) {
		imagePortA = [NSImage imageNamed:@"joystick2_32"];
	} else if( portA == IPD_KEYBOARD) {
		imagePortA = [NSImage imageNamed:@"keyboard_32"];
	} else if (portA == IPD_UNCONNECTED) {
		imagePortA = [NSImage imageNamed:@"none_32"];
	} else {
		assert(0);
	}
	
	if( portB == IPD_JOYSTICK_1 ) {
		imagePortB = [NSImage imageNamed:@"joystick1_32"];
	} else if( portB == IPD_JOYSTICK_2 ) {
		imagePortB = [NSImage imageNamed:@"joystick2_32"];
	} else if( portB == IPD_KEYBOARD ) {
		imagePortB = [NSImage imageNamed:@"keyboard_32"];
	} else if (portB == IPD_UNCONNECTED) {
		imagePortB = [NSImage imageNamed:@"none_32"];
	} else {
		assert(0);
	}
	
	// Create and bind number formatters
	[self setHexadecimalAction:self];
	
	// Setup table views
	[ttTableView setController:self];
	// [cpuTableView setController:self];
	// [memTableView setController:self];
	
	// DEPRECATED. MOVE TO TABLE VIEW CLASSES
	// Prepare to get double-click messages
	[cpuTableView setTarget:self];
	[cpuTableView setDoubleAction:@selector(doubleClickInCpuTable:)];
	[memTableView setTarget:self];
	[memTableView setDoubleAction:@selector(doubleClickInMemTable:)];
	
	// Create timer and speedometer
	assert(timerLock == nil);
	timerLock = [[NSLock alloc] init];

	assert(timer == nil);
	timer = [NSTimer scheduledTimerWithTimeInterval:(1.0f/6.0f) 
											 target:self 
										   selector:@selector(timerFunc) 
										   userInfo:nil repeats:YES];

	assert(speedometer == nil);
	speedometer = [[Speedometer alloc] init];
	
	NSLog(@"GUI has been initialized, timer is running");	
}

- (void)windowDidLoad
{
	// Load user defaults
	[self loadUserDefaults];
		
	// Mount archive if applicable
	if ([[self document] archive] != NULL) {
		[self showMountDialog];
	}		
}

- (void)loadUserDefaults
{
	NSLog(@"loadUserDefaults");
	
	int colorScheme;
	NSUserDefaults *defaults;
	
	// Set user defaults
	defaults = [NSUserDefaults standardUserDefaults];
	
	/* System */
	if ([defaults integerForKey:VC64PALorNTSCKey]) {
		[c64 setNTSC];
	} else {
		[c64 setPAL];
	}
	[[self document] loadRom:[defaults stringForKey:VC64BasicRomFileKey]];
	[[self document] loadRom:[defaults stringForKey:VC64CharRomFileKey]];
	[[self document] loadRom:[defaults stringForKey:VC64KernelRomFileKey]];
	[[self document] loadRom:[defaults stringForKey:VC64VC1541RomFileKey]];
	
	/* Peripherals */
	//[c64 setWarpLoad:[defaults boolForKey:VC64WarpLoadKey]];
	[[self document] setWarpLoad:[defaults boolForKey:VC64WarpLoadKey]];
	
	/* Audio */
	// [c64 sidEnableFilter:[defaults boolForKey:VC64SIDFilterKey]];
	
	/* Video */
	// [c64 vicSetVideoFilter:[defaults integerForKey:VC64VideoFilterKey];
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
// Loading and saving
// --------------------------------------------------------------------------------

- (IBAction)saveScreenshotDialog:(id)sender
{
	NSArray *fileTypes = [NSArray arrayWithObjects:@"tif", @"jpg", @"gif", @"png", @"psd", @"tga", nil];
	
	// Create the file save panel
	NSSavePanel* sPanel = [NSSavePanel savePanel];
	
	// [sPanel setCanChooseDirectories:NO];
	// [sPanel setCanChooseFiles:YES];
	// [sPanel setCanCreateDirectories:YES];
	// [sPanel setAllowsMultipleSelection:NO];
	// [sPanel setAlphaValue:0.95];
	// [sPanel setTitle:@"Select a file to open"];
	[sPanel setCanSelectHiddenExtension:YES];
	[sPanel setAllowedFileTypes:fileTypes];	
	if ([sPanel runModalForDirectory:nil file:nil] == NSOKButton) {
		
		NSString *selectedFile = [sPanel filename];
		NSLog(@"Writing to file %@", selectedFile);
		
		NSImage *image = [screen screenshot];
		// [image setFlipped:NO];
		NSData *data = [image TIFFRepresentation];
		[data writeToFile:selectedFile atomically:YES];
	}
}

- (void) windowDidBecomeMain: (NSNotification *) notification
{
	[c64 enableAudio];	
}

- (void) windowDidResignMain: (NSNotification *) notification
{
	[c64 disableAudio];
}

// --------------------------------------------------------------------------------
//                               Message processing
// --------------------------------------------------------------------------------

- (void)processMessage:(Message *)msg
{
	switch (msg->id) {
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
			
			// Start drawing when all ROMS are loaded...
			if ([c64 numberOfMissingRoms] == 0) { //	} && [c64 isHalted]) {
				
				// Close ROM dialog
				if (romDialog) {					
					[NSApp endSheet:romDialog];
					[romDialog orderOut:nil];
					romDialog = NULL;
				}
				
				// Start emulator
				[c64 run];
				
				// Trigger a nice zoom animation and start drawing
				[screen zoom];
				[screen drawC64texture:true];
			}
			break;
			
		case MSG_ROM_MISSING:
			
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
			
		case MSG_RUN:
			// NSLog(@"runAction");
			[info setStringValue:@""];
			[self enableUserEditing:NO];
			[self refresh];
			[backInTime_panel close];
			
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
//                The screen refresh loop (callback function for NSTimer)
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

// --------------------------------------------------------------------------------
// Toolbar delegates
// --------------------------------------------------------------------------------

- (BOOL)validateToolbarItem:(NSToolbarItem *)theItem
{
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
	
	/* Jostick port A */
	if ([theItem tag] == 10) { 
		int portA = [c64 getPortAssignment:0];
		
		if( portA == IPD_KEYBOARD )
			[theItem setImage:[NSImage imageNamed:@"keyboard32"]];
		else if( portA == IPD_JOYSTICK_1 )
			[theItem setImage:[NSImage imageNamed:@"joystick1_32"]];
		else if( portA == IPD_JOYSTICK_2 )
			[theItem setImage:[NSImage imageNamed:@"joystick2_32"]];
		else if( portA == IPD_UNCONNECTED )
			[theItem setImage:[NSImage imageNamed:@"none_32"]];
		else 
			assert(0);
		return YES;
	}
	
	/* Jostick port B */	
	if ([theItem tag] == 11) {
		int portB = [c64 getPortAssignment:1];
		
		if( portB == IPD_KEYBOARD )
			[theItem setImage:[NSImage imageNamed:@"keyboard32"]];
		else if( portB == IPD_JOYSTICK_1 )
			[theItem setImage:[NSImage imageNamed:@"joystick1_32"]];
		else if( portB == IPD_JOYSTICK_2 )
			[theItem setImage:[NSImage imageNamed:@"joystick2_32"]];
		else if( portB == IPD_UNCONNECTED )
			[theItem setImage:[NSImage imageNamed:@"none_32"]];
		else 
			assert(0);
		return YES;
	}
	
	/* All other items */
    return YES;
}

- (void) printDocument:(id) sender
{
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
	
	[imageView release];
}

// --------------------------------------------------------------------------------
// Action methods (Main screen)
// --------------------------------------------------------------------------------

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



// --------------------------------------------------------------------------------
// Action methods (Debug panel, common area)
// --------------------------------------------------------------------------------

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

- (IBAction)debugAction:(id)sender
{	
	[debug_panel toggle:self];
	[self refresh];
}

- (IBAction)setDecimalAction:(id)sender
{
	Formatter *bF = [[Formatter alloc] init:DECIMAL_FORMATTER inFormat:@"[0-9]{0,3}" outFormat:@"%02d"];
	Formatter *wF = [[Formatter alloc] init:DECIMAL_FORMATTER inFormat:@"[0-9]{0,5}" outFormat:@"%05d"];
	Disassembler *dis = [[Disassembler alloc] init:c64 byteFormatter:bF wordFormatter:wF ];
	
	[self refresh:bF word:wF disassembler:dis];
	
	[wF release];
	[bF release];
	[dis release];	
}

- (IBAction)setHexadecimalAction:(id)sender
{
	Formatter *bF  = [[Formatter alloc] init:HEXADECIMAL_FORMATTER inFormat:@"[0-9,a-f,A-F]{0,2}" outFormat:@"%02X"];
	Formatter *wF  = [[Formatter alloc] init:HEXADECIMAL_FORMATTER inFormat:@"[0-9,a-f,A-F]{0,4}" outFormat:@"%04X"];
	Disassembler *dis = [[Disassembler alloc] init:c64 byteFormatter:bF wordFormatter:wF ];
	
	[self refresh:bF word:wF disassembler:dis];
	
	[wF release];
	[bF release];
	[dis release];
}

- (IBAction)stepIntoAction:(id)sender
{
	[[self document] updateChangeCount:NSChangeDone];
	[c64 step];
	[self refresh];
}

- (IBAction)stepOutAction:(id)sender
{	
	[[self document] updateChangeCount:NSChangeDone];
	
	// Get return address from callstack
	int addr = [[c64 cpu] getTopOfCallStack];
	if (addr < 0)
		return;
	
	// Set soft breakpoint at next command and run
	[[c64 cpu] setSoftBreakpoint:(addr+1)];	
	[c64 run];
}


- (IBAction)stepOverAction:(id)sender
{	
	[[self document] updateChangeCount:NSChangeDone];
	// Is the next instruction a JSR instruction?
	uint8_t opcode = [[c64 cpu] peekPC];
	if (opcode == 0x20) {
		// set soft breakpoint at next command
		[[c64 cpu] setSoftBreakpoint:[[c64 cpu] getAddressOfNextInstruction]];	
		[c64 run];
	} else {
		// same as step
		[self stepIntoAction:self];
	}
}

- (IBAction)stopAndGoAction:(id)sender
{	
	[[self document] updateChangeCount:NSChangeDone];
	
	NSLog(@"Stop and go action");
	if ([c64 isHalted]) {
		[c64 run];
	} else {
		[c64 halt];
		[debug_panel open];
	}
	
	[self refresh];
}

- (IBAction)pauseAction:(id)sender
{	
	if ([c64 isRunning]) {
		[c64 halt];
		[debug_panel open];
	}
	
	[self refresh];
}

- (IBAction)continueAction:(id)sender
{	
	[[self document] updateChangeCount:NSChangeDone];
	
	if ([c64 isHalted])
		[c64 run];
	
	[self refresh];
}

- (IBAction)timeTravelAction:(id)sender
{	
	if ([backInTime_panel state] == NSDrawerOpenState) {
		[c64 run];
		[backInTime_panel close];
	}
	if ([backInTime_panel state] == NSDrawerClosedState) {
		[c64 halt];
		[ttTableView refresh];
		[backInTime_panel open];
	}	
}

- (IBAction)resetAction:(id)sender
{
	[[self document] updateChangeCount:NSChangeDone];
	[screen rotateBack];
	[c64 reset];
	[self continueAction:self];
}

- (IBAction)runstopRestoreAction:(id)sender
{
	[[self document] updateChangeCount:NSChangeDone];
	
	[c64 keyboardPressRunstopRestore];
	
	[self refresh];
}

// --------------------------------------------------------------------------------
// Action methods (Joystick action, common area)
// --------------------------------------------------------------------------------

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

- (IBAction)runstopAction:(id)sender
{
	NSLog(@"Rustop key pressed");
	[[self document] updateChangeCount:NSChangeDone];
	[[c64 keyboard] pressRunstopKey];
	sleepMicrosec(100000);
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

- (IBAction)FormatDiskAction:(id)sender
{
	NSLog(@"Format disk");
	[[self document] updateChangeCount:NSChangeDone];
	[[c64 keyboard] typeFormat];	
	[self refresh];
}

- (IBAction)fullscreenAction:(id)sender
{
	NSLog(@"fullscreenAction");
	[screen toggleFullscreenMode];
}

- (IBAction)showPreferencesAction:(id)sender
{
	NSLog(@"Showing preferences window...");
	if (!preferenceController) {
		preferenceController = [[PreferenceController alloc] init];
		[preferenceController setC64:c64];
		[preferenceController setMydoc:self];
	}
	[preferenceController showWindow:self];
}

- (IBAction)fastResetAction:(id)sender
{
	[c64 fastReset];
}


// --------------------------------------------------------------------------------
// Refresh methods: Force all GUI items to refresh their value
// 
// refresh: Refresh all components 
// refresh: word: disassebler: Refresh all components using different formatters
// --------------------------------------------------------------------------------


- (void)refresh
{		
	[self refreshCPU];
	[self refreshMemory];
	[self refreshCIA];
	[self refreshVIC];
	
	// Prepare Disassembler window.
	// The implementation is a little tricky. We distinguish two cases:
	// 1. The PC points to an address that is already visible in some row
	//    In this case, we simply select the row and don't modify anything else
	// 2. The PC points to an address that is not yet displayed
	//    In that case, we display the PC address in row 0
	uint16_t rowIndex = 0xffff;
	uint16_t address = [[c64 cpu] getPC];
	NSIndexSet *indexSet;
	if ([self computeRowForAddr:(uint16_t)address maxRows:[self numberOfRowsInTableView:cpuTableView] row:(uint16_t *)&rowIndex]) {
		indexSet = [NSIndexSet indexSetWithIndex:rowIndex];		
		[cpuTableView scrollRowToVisible:rowIndex];
	} else {
		disassembleStartAddr = address;
		indexSet = [NSIndexSet indexSetWithIndex:0];	
		[cpuTableView scrollRowToVisible:0];
	}
	[cpuTableView selectRowIndexes:indexSet byExtendingSelection:NO];
	
	[cpuTableView reloadData];
	[memTableView reloadData];
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
// Responder methods for table views
// --------------------------------------------------------------------------------

- (int)numberOfRowsInTableView:(NSTableView *)aTableView
{
	if (aTableView == memTableView)
		return 65536/4;		
	
	return 128;
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)row
{
	if (aTableView == cpuTableView)
		return [self objectValueForCpuTableColumn:aTableColumn row:row];
	if (aTableView == memTableView)
		return [self objectValueForMemTableColumn:aTableColumn row:row];
	
	return nil;
}

- (void)tableView:(NSTableView *)aTableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn *)aTableColumn row:(int)row
{
	// Only the memory table can be edited, yet
	if (aTableView == memTableView)
		[self setMemObjectValue:anObject forTableColumn:aTableColumn row:(int)row];
}

- (void)tableView: (NSTableView *)aTableView willDisplayCell: (id)aCell forTableColumn: (NSTableColumn *)aTableColumn row: (int)row
{
	if (aTableView == cpuTableView && [[aTableColumn identifier] isEqual:@"addr"]) {
		
		uint16_t addr = [[c64 cpu] getAddressOfNextIthInstruction:row from:disassembleStartAddr];
		if ([[c64 cpu] getBreakpoint:addr] == CPU::HARD_BREAKPOINT) {
			[aCell setTextColor:[NSColor redColor]];
		} else {
			[aCell setTextColor:[NSColor blackColor]];
		}
	} 
}

// --------------------------------------------------------------------------------
// Dialogs
// --------------------------------------------------------------------------------

- (IBAction)cancelRomDialog:(id)sender
{
	// Hide sheet
	[romDialog orderOut:sender];
	
	// Return to normal event handling
	[NSApp endSheet:romDialog returnCode:1];
}

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
	fprintf(stderr,"Wating...\n");
	usleep(1000000);
	fprintf(stderr,"Typing RUN...\n");
	[[c64 keyboard] typeRun];
}

- (IBAction)cartridgeEjectAction:(id)sender
{
	NSLog(@"cartridgeEjectAction");	
	[c64 detachCartridge];
	// delete [[self document] cartridge];
	[[self document] setCartridge:NULL];
	[c64 reset];
}

// --------------------------------------------------------------------------------
// Helper functions
// --------------------------------------------------------------------------------


- (BOOL)computeRowForAddr:(uint16_t)addr maxRows:(uint16_t)maxRows row:(uint16_t *)row
{
	uint16_t currentRow  = 0;
	uint16_t currentAddr = disassembleStartAddr;
	while (currentAddr <= addr && currentRow < maxRows) {
		if (currentAddr == addr) {
			*row = currentRow;
			return YES;
		}
		currentRow++;
		currentAddr += [[c64 cpu] getLengthOfInstruction:[[c64 mem] peek:currentAddr]];
	}
	return NO;
}

- (void)updateTimeTravelInfoText:(NSString *)s1 secondText:(NSString *)s2
{
	[historyDateField1 setStringValue:s1];
	[historyDateField2 setStringValue:s2];	
}

@end
