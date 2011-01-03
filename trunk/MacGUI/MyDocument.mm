/*
 * (C) 2006 - 2009 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published byc64
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

#import "MyDocument.h"

@implementation MyDocument

@synthesize warpLoad;
@synthesize alwaysWarp;
@synthesize archive;
@synthesize cartridge;

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
	NSLog(@"awakeFromNib");

	// Initialize variables
	alwaysWarp = false;
	enableOpenGL = false;  
	selectedSprite = 0;
	
	// Create virtual C64
	c64 = [[C64Proxy alloc] initWithDocument:self withScreen:screen];
	// cia = [c64 cia:1];
	
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

	// Prepare to get double-click messages
	[cpuTableView setTarget:self];
	[cpuTableView setDoubleAction:@selector(doubleClickInCpuTable:)];
	[memTableView setTarget:self];
	[memTableView setDoubleAction:@selector(doubleClickInMemTable:)];
		
	// Create timer
	cycleCount = 0;
	timeStamp  = msec();
	if (!timer)
		timer = [NSTimer scheduledTimerWithTimeInterval:(1.0f/6.0f) 
			    target:self 
				selector:@selector(timerFunc) 
			    userInfo:nil repeats:YES];

	NSLog(@"GUI has been initialized, timer is running");	
}

- (NSString *)windowNibName
{
    // Override returning the nib file name of the document
    // If you need to use a subclass of NSWindowController or if your document 
	// supports multiple NSWindowControllers, you should remove this method and 
	// override -makeWindowControllers instead.
    return @"MyDocument";
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
	[self loadRom:[defaults stringForKey:VC64BasicRomFileKey]];
	[self loadRom:[defaults stringForKey:VC64CharRomFileKey]];
	[self loadRom:[defaults stringForKey:VC64KernelRomFileKey]];
	[self loadRom:[defaults stringForKey:VC64VC1541RomFileKey]];
	
	/* Peripherals */
	//[c64 setWarpLoad:[defaults boolForKey:VC64WarpLoadKey]];
	[self setWarpLoad:[defaults boolForKey:VC64WarpLoadKey]];
	
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

- (void)windowControllerDidLoadNib:(NSWindowController *) aController
{	
	NSLog(@"windowControllerDidLoadNib");

	[super windowControllerDidLoadNib:aController];
   		
	// Fix window aspect ratio
	[[self windowForSheet] setContentAspectRatio:NSMakeSize(652,432)];

	// Load user defaults
	[self loadUserDefaults];
		
	// Launch emulator
	[c64 run];
	
	// Start drawing
	enableOpenGL = true;
	
	// Mount archive if applicable
	if (archive != NULL) {
		[self showMountDialog];
	}		
}


// --------------------------------------------------------------------------------
// Loading and saving
// --------------------------------------------------------------------------------

#if 0
+ (NSArray *)readableTypes 
{ 
	NSLog(@"+ (NSArray *)readableTypes"); 
	return [NSArray arrayWithObjects:@"VC64", nil]; 
} 

+ (NSArray *)writableTypes 
{ 
	NSLog(@"+ (NSArray *)writableTypes"); 
	return [NSArray arrayWithObjects:@"VC64", nil]; 
} 

+ (bool)isNativeType:(NSString *)aType
{
	NSLog(@"+ (bool)isNativeType"); 
	return [aType isEqual:@"VC64"];
}

- (BOOL)shouldShowFilename:(NSString *)filename { 
	NSLog(@"shouldShowFilename");
    NSString *lpc = [filename lastPathComponent]; 
    if ([lpc characterAtIndex:0] == '_') 
        return NO; 
    return YES; 
} 

-(NSData *)dataRepresentationOfType:(NSString *)type
{
	NSLog(@"dataRepresentationOfType:%@", type);
	NSLog(@"c64 == %p", c64);

	return nil;
}

-(bool)loadDataRepresentation:(NSData *)docData ofType:(NSString *)type
{
	NSLog(@"loadDataRepresentation:%@", type);
	NSLog(@"c64 == %p", c64);
	
	return NO;
}
#endif

-(BOOL)writeToFile:(NSString *)filename ofType:(NSString *)type
{
	NSLog(@"writeToFile %@ (type %@)", filename, type);
	
	if (![type isEqualToString:@"VC64"]) {
		NSLog(@"File is not of type VC64\n");
		return NO;
	}

#if 0
	snapshot = new Snapshot();
	[c64 dumpContentsToSnapshot:snapshot];
	// snapshot->initWithContentsOfC64([c64 getC64]);
	snapshot->writeToFile([filename UTF8String]);
	delete snapshot;
	snapshot = NULL;
#endif
	V64Snapshot *s = [V64Snapshot snapshotFromC64:c64];
	[s writeDataToFile:filename];
	[s release];
	
	return YES;
}

-(BOOL)readFromFile:(NSString *)filename ofType:(NSString *)type
{
	NSLog(@"readFromFile %@ (type %@)", filename, type);

	if ([type isEqualToString:@"VC64"]) {
		snapshot = Snapshot::snapshotFromFile([filename UTF8String]);
		if (!snapshot) {
			NSLog(@"Error while reading snapshot\n");
			return NO;
		}
	} else if ([type isEqualToString:@"D64"] || [type isEqualToString:@"T64"] || [type isEqualToString:@"PRG"] || [type isEqualToString:@"P00"]) {
		if (![self setArchiveWithName:filename]) {
			NSLog(@"Error while reading archive\n");
			return NO;
		}
	}			
	
	return YES;
}

- (BOOL)revertToSavedFromFile:(NSString *)filename ofType:(NSString *)type
{
	bool success = NO;
	
	if ([type isEqualToString:@"VC64"]) {
		snapshot = Snapshot::snapshotFromFile([filename UTF8String]);
		if (snapshot) {
			// snapshot->writeToC64([c64 getC64]);
			[c64 initWithContentsOfSnapshot:snapshot];
			delete snapshot;
			snapshot = NULL;
			success = YES;
		}
	} else if ([type isEqualToString:@"D64"] || [type isEqualToString:@"T64"] || [type isEqualToString:@"PRG"] || [type isEqualToString:@"P00"]) {
		if ([self setArchiveWithName:filename]) {
			[self showMountDialog];
			success = YES;
		}
	}
	return success;
}

- (BOOL)loadRom:(NSString *)filename
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	bool success = NO;
	
	if ([c64 loadBasicRom:filename]) {
		[defaults setObject:filename forKey:VC64BasicRomFileKey];
		success = YES;
	}
	else if ([c64 loadCharRom:filename]) {
		[defaults setObject:filename forKey:VC64CharRomFileKey];
		success = YES;
	}
	else if ([c64 loadKernelRom:filename]) {
		[defaults setObject:filename forKey:VC64KernelRomFileKey];
		success = YES;
	}
	else if ([c64 loadVC1541Rom:filename]) {
		[defaults setObject:filename forKey:VC64VC1541RomFileKey];
		success = YES;
	}
		
	return success;
}

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
			[romDialog initialize:msg->i];
			[NSApp beginSheet:romDialog
			   modalForWindow:[self windowForSheet]
				modalDelegate:self
			   didEndSelector:NULL
				  contextInfo:NULL];	
			break;
			
		case MSG_RUN:
			NSLog(@"runAction");
			[info setStringValue:@""];
			[self enableUserEditing:NO];
			[self refresh];
			// disable undo because the internal state changes permanently
			[self updateChangeCount:NSChangeDone];
			[[self undoManager] removeAllActions];			
			break;
			
		case MSG_HALT:
			NSLog(@"haltAction");
			[self enableUserEditing:YES];	
			[self refresh];			
			break;
			
		//case MSG_DRAW:
		//	[screen updateTexture:(int *)msg->p];
		//	break;
			
		case MSG_CPU:
			NSLog(@"cpuAction");
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
			NSLog(@"warpmodeAction");
			break;
			
		case MSG_LOG:
			break;
			
		case MSG_VC1541_ATTACHED:
			NSLog(@"driveAttachedAction");
			if (msg->i)
				[greenLED setImage:[NSImage imageNamed:@"LEDgreen"]];
			else
				[greenLED setImage:[NSImage imageNamed:@"LEDgray"]];			
			break;
			
		case MSG_VC1541_DISC:
			NSLog(@"driveDiscAction");
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
			NSLog(@"driveDataAction (%s)", msg->i ? "on" : "off");
			
			if (msg->i) {
				[driveBusy setHidden:false];
				[driveBusy startAnimation:self];
				if (warpLoad)
					[c64 setWarpMode:YES];			
			} else {
				[driveBusy stopAnimation:self];
				[driveBusy setHidden:true];		
				if (!alwaysWarp)
					[c64 setWarpMode:NO];
			}			
			break;
			
		case MSG_VC1541_MOTOR:
			NSLog(@"driveMotorAction");
			break;
			
		case MSG_CARTRIDGE:
			NSLog(@"MSG_CARTRIDGE");
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
	[timerLock lock];
	
	// Do 6 times a second...
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
		[self measureEmulationSpeed];
	}

	[timerLock unlock];
}

- (void)measureEmulationSpeed
{
	// Measure clock frequency and frame rate
	long currentTime   = msec();
	long currentCycles = [c64 getCycles];
	long currentFrames = [screen frames];
	long elapsedTime   = currentTime - timeStamp;
	long elapsedCycles = currentCycles - cycleCount;
	long elapsedFrames = currentFrames - frameCount;
	
	// print how fast we're flying
	mhz = (float)elapsedCycles / (float)elapsedTime;
	float fps = round(((float)(elapsedFrames * 1000000) / (float)elapsedTime));
	
	[clockSpeed setStringValue:[NSString stringWithFormat:@"%.2f MHz %02d fps", mhz, (int)fps]];
	[clockSpeedBar setFloatValue:10.0 * (float)elapsedCycles / (float)elapsedTime];
	timeStamp  = currentTime;
	cycleCount = currentCycles;
	frameCount = currentFrames;		
}

// --------------------------------------------------------------------------------
// Toolbar delegates
// --------------------------------------------------------------------------------

- (BOOL)validateToolbarItem:(NSToolbarItem *)theItem
{
	/* */
	if ([c64 isRunning]) {
		[self updateChangeCount:NSChangeDone];
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
	NSPrintInfo *myPrintInfo = [self printInfo];
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
    [printOperation runOperationModalForWindow:[self windowForSheet] delegate: nil didRunSelector: NULL contextInfo:NULL];

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
	[[undo prepareWithInvocationTarget:self] warpAction:[NSNumber numberWithInt:![c64 getWarpMode]]];
	if (![undo isUndoing]) [undo setActionName:@"Native speed"];
		
	[self setAlwaysWarp:![self alwaysWarp]];
	[c64 setWarpMode:alwaysWarp];
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

- (IBAction)setHardBreakpointAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] setHardBreakpointAction:[NSNumber numberWithInt:[sender intValue]]];
	if (![undo isUndoing]) [undo setActionName:@"Breakpoint"];
	
	[[c64 cpu] toggleHardBreakpoint:[sender intValue]];
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
	[self updateChangeCount:NSChangeDone];
	[c64 step];
	[self refresh];
}

- (IBAction)stepOutAction:(id)sender
{	
	[self updateChangeCount:NSChangeDone];

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
	[self updateChangeCount:NSChangeDone];
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
	[self updateChangeCount:NSChangeDone];

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
	[self updateChangeCount:NSChangeDone];

	if ([c64 isHalted])
		[c64 run];
	
	[self refresh];
}

- (IBAction)resetAction:(id)sender
{
	[self updateChangeCount:NSChangeDone];
	[screen rotateBack];
	[c64 reset];
	[self continueAction:self];
}

- (IBAction)runstopRestoreAction:(id)sender
{
	[self updateChangeCount:NSChangeDone];
	
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
	[self updateChangeCount:NSChangeDone];
	[[c64 keyboard] pressRunstopKey];
	sleepMicrosec(100000);
	[[c64 keyboard] releaseRunstopKey];	
	[self refresh];
}
	
- (IBAction)commodoreKeyAction:(id)sender
{
	NSLog(@"Commodore key pressed");
	[self updateChangeCount:NSChangeDone];
	[[c64 keyboard] pressCommodoreKey];	
	sleepMicrosec(100000);
	[[c64 keyboard] releaseCommodoreKey];	
	[self refresh];
}

- (IBAction)FormatDiskAction:(id)sender
{
	NSLog(@"Format disk");
	[self updateChangeCount:NSChangeDone];
	[[c64 keyboard] typeFormat];	
	[self refresh];
}

- (IBAction)fullscreenAction:(id)sender
{
	NSLog(@"fullscreenAction");
	[screen toggleFullscreenMode];
}

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

- (IBAction)fastResetAction:(id)sender
{
	[c64 fastReset];
}

- (IBAction)dumpC64:(id)sender
{
	[c64 dump];
}

- (IBAction)dumpC64CPU:(id)sender
{
	[[c64 cpu] dump];
}

- (IBAction)dumpC64CIA1:(id)sender
{
	[[c64 cia:1] dump];
}

- (IBAction)dumpC64CIA2:(id)sender
{
	[[c64 cia:2] dump];
}

- (IBAction)dumpC64VIC:(id)sender
{
	[[c64 vic] dump];
}

- (IBAction)dumpC64SID:(id)sender
{
	[[c64 sid] dump];
}

- (IBAction)dumpC64Memory:(id)sender
{
	[[c64 mem] dump];
}

- (IBAction)dumpVC1541:(id)sender
{
	[[c64 vc1541] dump];
}

- (IBAction)dumpVC1541CPU:(id)sender
{
	[[[c64 vc1541] cpu] dump];
}

- (IBAction)dumpVC1541VIA1:(id)sender
{
	[[[c64 vc1541] via:1] dump];
}

- (IBAction)dumpVC1541VIA2:(id)sender
{
	[[[c64 vc1541] via:2] dump];
}

- (IBAction)dumpVC1541Memory:(id)sender
{
	[[[c64 vc1541] mem] dump];
}

- (IBAction)dumpKeyboard:(id)sender
{
	[[c64 keyboard] dump];
}

- (IBAction)dumpIEC:(id)sender
{
	[[c64 iec] dump];
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


// --------------------------------------------------------------------------------
// Action methods (CPU Debug panel)
// --------------------------------------------------------------------------------

- (IBAction)pcAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] pcAction:[NSNumber numberWithInt:[[c64 cpu] getPC]]];
	if (![undo isUndoing]) [undo setActionName:@"Set program counter"];
	
	[[c64 cpu] setPC:[sender intValue]];
	[self refresh];
}

- (IBAction)spAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spAction:[NSNumber numberWithInt:[[c64 cpu] getSP]]];
	if (![undo isUndoing]) [undo setActionName:@"Set stack pointer"];

	[[c64 cpu] setSP:[sender intValue]];
	[self refresh];
}

- (IBAction)aAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] aAction:[NSNumber numberWithInt:[[c64 cpu] getA]]];
	if (![undo isUndoing]) [undo setActionName:@"Set accumulator"];

	[[c64 cpu] setA:[sender intValue]];
	[self refresh];
}

- (IBAction)xAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] xAction:[NSNumber numberWithInt:[[c64 cpu] getX]]];
	if (![undo isUndoing]) [undo setActionName:@"Set X register"];

	[[c64 cpu] setX:[sender intValue]];
	[self refresh];
}

- (IBAction)yAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] yAction:[NSNumber numberWithInt:[[c64 cpu] getY]]];
	if (![undo isUndoing]) [undo setActionName:@"Set Y register"];

	[[c64 cpu] setY:[sender intValue]];
	[self refresh];
}

- (IBAction)NAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] NAction:[NSNumber numberWithInt:[[c64 cpu] getN]]];
	if (![undo isUndoing]) [undo setActionName:@"Negative Flag"];

	[[c64 cpu] setN:[sender intValue]];
	[self refresh];
}

- (IBAction)ZAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ZAction:[NSNumber numberWithInt:[[c64 cpu] getZ]]];
	if (![undo isUndoing]) [undo setActionName:@"Zero Flag"];

	[[c64 cpu] setZ:[sender intValue]];
	[self refresh];
}

- (IBAction)CAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] CAction:[NSNumber numberWithInt:[[c64 cpu] getC]]];
	if (![undo isUndoing]) [undo setActionName:@"Carry Flag"];

	[[c64 cpu] setC:[sender intValue]];
	[self refresh];
}

- (IBAction)IAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] IAction:[NSNumber numberWithInt:[[c64 cpu] getI]]];
	if (![undo isUndoing]) [undo setActionName:@"Interrupt Flag"];

	[[c64 cpu] setI:[sender intValue]];
	[self refresh];
}

- (IBAction)BAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] BAction:[NSNumber numberWithInt:[[c64 cpu] getB]]];
	if (![undo isUndoing]) [undo setActionName:@"Break Flag"];

	[[c64 cpu] setB:[sender intValue]];
	[self refresh];
}

- (IBAction)DAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] DAction:[NSNumber numberWithInt:[[c64 cpu] getD]]];
	if (![undo isUndoing]) [undo setActionName:@"Decimal Flag"];

	[[c64 cpu] setD:[sender intValue]];
	[self refresh];
}

- (IBAction)VAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] VAction:[NSNumber numberWithInt:[[c64 cpu] getV]]];
	if (![undo isUndoing]) [undo setActionName:@"Overflow Flag"];

	[[c64 cpu] setV:[sender intValue]];
	[self refresh];
}

- (IBAction)mhzAction:(id)sender
{
#if 0
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] mhzAction:[c64 cpuGetMHz]];
	if (![undo isUndoing]) [undo setActionName:@"Clock frequency"];
	
	// float value = clip([sender floatValue], 0.1f, 10.0f);
	[c64 cpuSetMHz:sender];
	[self refresh];
#endif
}

- (void)doubleClickInMemTable:(id)sender
{
	[self refresh];
}

- (void)doubleClickInCpuTable:(id)sender
{
	uint16_t addr;

	addr = [[c64 cpu] getAddressOfNextIthInstruction:[sender selectedRow] from:disassembleStartAddr];
	[self setHardBreakpointAction:[NSNumber numberWithInt:addr]];
}


// --------------------------------------------------------------------------------
// Action methods (Memory debug panel)
// --------------------------------------------------------------------------------

- (IBAction)searchAction:(id)sender
{
	uint16_t addr = [sender intValue];
	[memTableView scrollRowToVisible:addr/4];
	[self refresh];
}

- (void)setMemSource:(Memory::MemoryType)source
{
	// memsource = source;
	[self refresh];
}

- (IBAction)setMemSourceToRAM:(id)sender
{
	[self setMemSource:Memory::MEM_RAM];
}

- (IBAction)setMemSourceToROM:(id)sender
{
	[self setMemSource:Memory::MEM_ROM];
}

- (IBAction)setMemSourceToIO:(id)sender
{
	[self setMemSource:Memory::MEM_IO];
}

// --------------------------------------------------------------------------------
// Action methods (CIA)
// --------------------------------------------------------------------------------

- (IBAction)ciaSelectCiaAction:(id)sender
{
	[self refresh];
}

- (void)_ciaDataPortAAction:(int)nr value:(uint8_t)v
{
	[[c64 cia:nr] setDataPortA:v];
	[self refresh];
}

- (IBAction)ciaDataPortAAction:(id)sender 
{	
	int nr = [self currentCIA];
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaDataPortAAction:nr value:[[c64 cia:nr] getDataPortA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA data port A"];
	
	[self _ciaDataPortAAction:nr value:[sender intValue]];
} 

- (void)_ciaDataPortDirectionAAction:(int)nr value:(uint8_t)v
{
	[[c64 cia:nr] setDataPortDirectionA:v];
	[self refresh];
}

- (IBAction)ciaDataPortDirectionAAction:(id)sender 
{
	int nr = [self currentCIA];
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaDataPortDirectionAAction:nr value:[[c64 cia:nr] getDataPortDirectionA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA data port direction A"];
	
	[self _ciaDataPortDirectionAAction:nr value:[sender intValue]];
}

- (void)_ciaTimerAAction:(int)nr value:(uint16_t)v
{
	[[c64 cia:nr] setTimerA:v];
	[self refresh];
}

- (IBAction)ciaTimerAAction:(id)sender 
{
	int nr = [self currentCIA];
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaTimerAAction:nr value:[[c64 cia:nr] getDataPortDirectionA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA timer A"];
	
	[self _ciaTimerAAction:nr value:[sender intValue]];
}

- (void)_ciaLatchedTimerAAction:(int)nr value:(uint16_t)v
{
	[[c64 cia:nr] setTimerLatchA:v];
	[self refresh];
}

- (IBAction)ciaLatchedTimerAAction:(id)sender
{
	int nr = [self currentCIA];	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaLatchedTimerAAction:nr value:[[c64 cia:nr] getTimerLatchA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA timer latch A"];
	
	[self _ciaLatchedTimerAAction:nr value:[sender intValue]];
}

- (void)_ciaRunningAAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setStartFlagA:b];
	[self refresh];
}

- (IBAction)ciaRunningAAction:(id)sender
{
	int nr = [self currentCIA];	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaRunningAAction:nr value:[[c64 cia:nr] getStartFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"Start/Stop CIA timer A"];

	[self _ciaRunningAAction:nr value:[sender intValue]];
}


- (void)_ciaOneShotAAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setOneShotFlagA:b];
	[self refresh];
}

- (IBAction)ciaOneShotAAction:(id)sender
{
	int nr = [self currentCIA];	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaOneShotAAction:nr value:[[c64 cia:nr] getOneShotFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA one shot flag A"];
	
	[self _ciaOneShotAAction:nr value:[sender intValue]];
}

- (void)_ciaCountUnterflowsAAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setUnderflowFlagA:b];
	[self refresh];
}

- (IBAction)ciaCountUnterflowsAAction:(id)sender
{
	int nr = [self currentCIA];	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaCountUnterflowsAAction:nr value:[[c64 cia:nr] getUnderflowFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA underflow flag A"];
	
	[self _ciaCountUnterflowsAAction:nr value:[sender intValue]];
}

- (void)_ciaSignalPendingAAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setPendingSignalFlagA:b];
	[self refresh];
}

- (IBAction)ciaSignalPendingAAction:(id)sender
{
	int nr = [self currentCIA];	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaSignalPendingAAction:nr value:[[c64 cia:nr] getPendingSignalFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA signal pending A"];
	
	[self _ciaSignalPendingAAction:nr value:[sender intValue]];
}

- (void)_ciaInterruptEnableAAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setInterruptEnableFlagA:b];
	[self refresh];
}

- (IBAction)ciaInterruptEnableAAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaInterruptEnableAAction:nr value:[[c64 cia:nr] getInterruptEnableFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA interrupt enable flag A"];
	
	[self _ciaInterruptEnableAAction:nr value:[sender intValue]];
}

- (void)_ciaDataPortBAction:(int)nr value:(uint8_t)v
{
	[[c64 cia:nr] setDataPortB:v];
	[self refresh];
}

- (IBAction)ciaDataPortBAction:(id)sender 
{	
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaDataPortBAction:nr value:[[c64 cia:nr] getDataPortB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA data port B"];
	
	[self _ciaDataPortBAction:nr value:[sender intValue]];
} 

- (void)_ciaDataPortDirectionBAction:(int)nr value:(uint8_t)v
{
	[[c64 cia:nr] setDataPortDirectionB:v];
	[self refresh];
}

- (IBAction)ciaDataPortDirectionBAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaDataPortDirectionBAction:nr value:[[c64 cia:nr] getDataPortDirectionB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA data port direction B"];
	
	[self _ciaDataPortDirectionBAction:nr value:[sender intValue]];
}

- (void)_ciaTimerBAction:(int)nr value:(uint16_t)v
{
	[[c64 cia:nr] setTimerB:v];
	[self refresh];
}

- (IBAction)ciaTimerBAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaTimerBAction:nr value:[[c64 cia:nr] getTimerB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA timer B"];
	
	[self _ciaTimerBAction:nr value:[sender intValue]];
}

- (void)_ciaLatchedTimerBAction:(int)nr value:(uint16_t)v
{
	[[c64 cia:nr] setTimerLatchB:v];
	[self refresh];
}

- (IBAction)ciaLatchedTimerBAction:(id)sender
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaLatchedTimerBAction:nr value:[[c64 cia:nr] getTimerLatchB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA timer latch B"];
	
	[self _ciaLatchedTimerBAction:nr value:[sender intValue]];
}

- (void)_ciaRunningBAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setStartFlagB:b];
	[self refresh];
}

- (IBAction)ciaRunningBAction:(id)sender
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaRunningBAction:nr value:[[c64 cia:nr] getStartFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"Start/Stop CIA timer B"];

	[self _ciaRunningBAction:nr value:[sender intValue]];
}

- (void)_ciaOneShotBAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setOneShotFlagB:b];
	[self refresh];
}

- (IBAction)ciaOneShotBAction:(id)sender
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaOneShotBAction:nr value:[[c64 cia:nr] getOneShotFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA one shot flag B"];
	
	[self _ciaOneShotBAction:nr value:[sender intValue]];
}

- (void)_ciaCountUnterflowsBAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setUnderflowFlagB:b];
	[self refresh];
}

- (IBAction)ciaCountUnterflowsBAction:(id)sender
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaCountUnterflowsBAction:nr value:[[c64 cia:nr] getUnderflowFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA underflow flag B"];
	
	[self _ciaCountUnterflowsBAction:nr value:[sender intValue]];
}

- (void)_ciaSignalPendingBAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setPendingSignalFlagB:b];
	[self refresh];
}

- (IBAction)ciaSignalPendingBAction:(id)sender
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaSignalPendingBAction:nr value:[[c64 cia:nr] getPendingSignalFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA signal pending B"];
	
	[self _ciaSignalPendingBAction:nr value:[sender intValue]];
}

- (void)_ciaInterruptEnableBAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setInterruptEnableFlagB:b];
	[self refresh];
}

- (IBAction)ciaInterruptEnableBAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaInterruptEnableBAction:nr value:[[c64 cia:nr] getInterruptEnableFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA interrupt enable flag B"];
	
	[self _ciaInterruptEnableBAction:nr value:[sender intValue]];
}

- (IBAction)_todHoursAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setTodHours:value];
	[self refresh];
}

- (IBAction)todHoursAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todHoursAction:nr value:[[c64 cia:nr] getTodHours]];
	if (![undo isUndoing]) [undo setActionName:@"TOD hours"];
	
	[self _todHoursAction:nr value:[sender intValue]];
}	

- (IBAction)_todMinutesAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setTodMinutes:value];
	[self refresh];
}

- (IBAction)todMinutesAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todMinutesAction:nr value:[[c64 cia:nr] getTodMinutes]];
	if (![undo isUndoing]) [undo setActionName:@"TOD minutes"];
	
	[self _todMinutesAction:nr value:[sender intValue]];
}

- (IBAction)_todSecondsAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setTodSeconds:value];
	[self refresh];
}

- (IBAction)todSecondsAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todSecondsAction:nr value:[[c64 cia:nr] getTodSeconds]];
	if (![undo isUndoing]) [undo setActionName:@"TOD seconds"];
	
	[self _todSecondsAction:nr value:[sender intValue]];
}

- (IBAction)_todTenthAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setTodTenth:value];
	[self refresh];
}

- (IBAction)todTenthAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todTenthAction:nr value:[[c64 cia:nr] getTodTenth]];
	if (![undo isUndoing]) [undo setActionName:@"TOD hours"];

	[self _todTenthAction:nr value:[sender intValue]];
}

- (IBAction)_alarmHoursAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setAlarmHours:value];
	[self refresh];
}

- (IBAction)alarmHoursAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _alarmHoursAction:nr value:[[c64 cia:nr] getAlarmHours]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm hours"];
	
	[self _alarmHoursAction:nr value:[sender intValue]];
}

- (IBAction)_alarmMinutesAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setAlarmMinutes:value];
	[self refresh];
}

- (IBAction)alarmMinutesAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _alarmMinutesAction:nr value:[[c64 cia:nr] getAlarmMinutes]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm minutes"];
	
	[self _alarmMinutesAction:nr value:[sender intValue]];
}

- (IBAction)_alarmSecondsAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setAlarmSeconds:value];
	[self refresh];
}

- (IBAction)alarmSecondsAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _alarmSecondsAction:nr value:[[c64 cia:nr] getAlarmSeconds]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm seconds"];
	
	[self _alarmSecondsAction:nr value:[sender intValue]];
}

- (IBAction)_alarmTenthAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setAlarmTenth:value];
	[self refresh];
}

- (IBAction)alarmTenthAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _alarmTenthAction:nr value:[[c64 cia:nr] getAlarmTenth]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm hours"];
	
	[self _alarmTenthAction:nr value:[sender intValue]];
}

- (IBAction)_todInterruptEnabledAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] todSetInterruptEnabled:b];
	[self refresh];
}

- (IBAction)todInterruptEnabledAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todInterruptEnabledAction:nr value:[[c64 cia:nr] todIsInterruptEnabled]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm enable"];
	
	[self _todInterruptEnabledAction:nr value:[sender intValue]];
}


// --------------------------------------------------------------------------------
// Action methods (VIC debug panel)
// --------------------------------------------------------------------------------

- (void)_vicVideoModeAction:(int)mode
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _vicVideoModeAction:[[c64 vic] getDisplayMode]];
	if (![undo isUndoing]) [undo setActionName:@"Display mode"];

	[[c64 vic] setDisplayMode:mode];
	[self refresh];
}

- (IBAction)vicVideoModeAction:(id)sender
{
	[self _vicVideoModeAction:[[sender selectedItem] tag]];
}

- (void)_vicScreenGeometryAction:(int)mode
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _vicScreenGeometryAction:[[c64 vic] getScreenGeometry]];
	if (![undo isUndoing]) [undo setActionName:@"Screen geometry"];

	[[c64 vic] setScreenGeometry:mode];
	[self refresh];
}

- (IBAction)vicScreenGeometryAction:(id)sender
{
	[self _vicScreenGeometryAction:[[sender selectedItem] tag]];
}

- (void)vicSetMemoryBank:(int)addr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicSetMemoryBank:[[c64 vic] getMemoryBankAddr]];
	if (![undo isUndoing]) [undo setActionName:@"Memory bank"];

	[[c64 vic] setMemoryBankAddr:addr];
	[self refresh];
}

- (IBAction)vicMemoryBankAction:(id)sender
{
	[self vicSetMemoryBank:[[sender selectedItem] tag]];
}

- (void)vicSetScreenMemory:(int)addr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicSetScreenMemory:[[c64 vic] getScreenMemoryAddr]];
	if (![undo isUndoing]) [undo setActionName:@"Screen memory"];

	[[c64 vic] setScreenMemoryAddr:addr];
	[self refresh];
}

- (IBAction)vicScreenMemoryAction:(id)sender
{
	[self vicSetScreenMemory:[[sender selectedItem] tag]];
}

- (void)vicSetCharacterMemory:(int)addr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicSetCharacterMemory:[[c64 vic] getCharacterMemoryAddr]];
	if (![undo isUndoing]) [undo setActionName:@"Character memory"];

	[[c64 vic] setCharacterMemoryAddr:addr];
	[self refresh];
}

- (IBAction)vicCharacterMemoryAction:(id)sender
{
	[self vicSetCharacterMemory:[[sender selectedItem] tag]];
}

- (IBAction)vicDXAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicDXAction:[NSNumber numberWithInt:[[c64 vic] getHorizontalRasterScroll]]];
	if (![undo isUndoing]) [undo setActionName:@"Horizontal raster scroll"];
	
	[[c64 vic] setHorizontalRasterScroll:[sender intValue]];
	[self refresh];
}

- (IBAction)vicDYAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicDYAction:[NSNumber numberWithInt:[[c64 vic] getVerticalRasterScroll]]];
	if (![undo isUndoing]) [undo setActionName:@"Vertical raster scroll"];
	
	[[c64 vic] setVerticalRasterScroll:[sender intValue]];
	[self refresh];
}

- (IBAction)vicDXStepperAction:(id)sender
{
	[self vicDXAction:sender];
}

- (IBAction)vicDYStepperAction:(id)sender
{
	[self vicDYAction:sender];
}

- (void)spriteToggleVisibilityFlag:(int)nr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteToggleVisibilityFlag:nr];
	if (![undo isUndoing]) [undo setActionName:@"Sprite visability"];

	[[c64 vic] spriteToggleVisibilityFlag:nr];
	[self refresh];
}

- (IBAction)vicSpriteSelectAction:(id)sender
{
	selectedSprite = [sender selectedTag];
	
	NSLog(@"selectedSprite = %d", selectedSprite);
	[self refresh];
}

- (IBAction)vicSpriteActiveAction:(id)sender
{	
	// debug("Selected sprinte = %d\n", [
	[self spriteToggleVisibilityFlag:[self currentSprite]];
}

- (void)spriteToggleMulticolorFlag:(int)nr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteToggleMulticolorFlag:nr];
	if (![undo isUndoing]) [undo setActionName:@"Sprite multicolor"];

	[[c64 vic] spriteToggleMulticolorFlag:nr];
	[self refresh];
}

- (IBAction)vicSpriteMulticolorAction:(id)sender
{
	[self spriteToggleMulticolorFlag:[self currentSprite]];
}

- (void)spriteToggleStretchXFlag:(int)nr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteToggleStretchXFlag:nr];
	if (![undo isUndoing]) [undo setActionName:@"Sprite stretch X"];

	[[c64 vic] spriteToggleStretchXFlag:nr];
	[self refresh];
}

- (IBAction)vicSpriteStretchXAction:(id)sender
{
	[self spriteToggleStretchXFlag:[self currentSprite]];
}

- (void)spriteToggleStretchYFlag:(int)nr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteToggleStretchYFlag:nr];
	if (![undo isUndoing]) [undo setActionName:@"Sprite stretch Y"];

	[[c64 vic] spriteToggleStretchYFlag:nr];
	[self refresh];
}

- (IBAction)vicSpriteStretchYAction:(id)sender
{
	[self spriteToggleStretchYFlag:[self currentSprite]];
}

- (void)spriteToggleBackgroundPriorityFlag:(int)nr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteToggleBackgroundPriorityFlag:nr];
	if (![undo isUndoing]) [undo setActionName:@"Sprite background priority"];

	[[c64 vic] spriteToggleBackgroundPriorityFlag:nr];
	[self refresh];
}

- (IBAction)vicSpriteInFrontAction:(id)sender
{	
	[self spriteToggleBackgroundPriorityFlag:[self currentSprite]];
}

- (void)spriteToggleSpriteSpriteCollisionFlag:(int)nr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteToggleSpriteSpriteCollisionFlag:nr];
	if (![undo isUndoing]) [undo setActionName:@"Detect sprite/sprite collisions"];

	[[c64 vic] spriteToggleSpriteSpriteCollisionFlag:nr];
	[self refresh];
}

- (IBAction)vicSpriteSpriteCollisionAction:(id)sender
{
	[self spriteToggleSpriteSpriteCollisionFlag:[self currentSprite]];
}

- (void)spriteToggleSpriteBackgroundCollisionFlag:(int)nr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteToggleSpriteBackgroundCollisionFlag:nr];
	if (![undo isUndoing]) [undo setActionName:@"Detect sprite/background collisions"];

	[[c64 vic] spriteToggleSpriteBackgroundCollisionFlag:nr];
	[self refresh];
}

- (IBAction)vicSpriteBackgroundCollisionAction:(id)sender
{
	NSLog(@"%d", [sprite1 intValue]);
	NSLog(@"%d", [sprite2 intValue]);
	[self spriteToggleSpriteBackgroundCollisionFlag:[self currentSprite]];
}

- (void)spriteSetX:(int)nr value:(int)v
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteSetX:nr value:[[c64 vic] spriteGetX:nr]];
	if (![undo isUndoing]) [undo setActionName:@"Sprite X"];

	[[c64 vic] spriteSetX:nr value:v];
	[self refresh];
}

- (IBAction)vicSpriteXAction:(id)sender
{
	[self spriteSetX:[self currentSprite] value:[sender intValue]];
}

- (void)spriteSetY:(int)nr value:(int)v
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteSetY:nr value:[[c64 vic] spriteGetY:nr]];
	if (![undo isUndoing]) [undo setActionName:@"Sprite Y"];

	[[c64 vic] spriteSetY:nr value:v];
	[self refresh];
}

- (IBAction)vicSpriteYAction:(id)sender
{
	[self spriteSetY:[self currentSprite] value:[sender intValue]];
}

- (void)spriteSetColor:(int)nr value:(int)v
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteSetColor:nr value:[[c64 vic] spriteGetColor:nr]];
	if (![undo isUndoing]) [undo setActionName:@"Sprite color"];

	[[c64 vic] spriteSetColor:nr value:v];
	[self refresh];
}

- (IBAction)vicSpriteColorAction:(id)sender
{
	[self spriteSetColor:[self currentSprite] value:[sender intValue]];
	[self refresh];
}

- (IBAction)vicRasterlineAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicRasterlineAction:[NSNumber numberWithInt:[[c64 vic] getRasterLine]]];
	if (![undo isUndoing]) [undo setActionName:@"Raster line"];
	
	[[c64 vic] setRasterLine:[sender intValue]];
	[self refresh];
}

- (IBAction)vicEnableRasterInterruptAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicEnableRasterInterruptAction:sender];
	if (![undo isUndoing]) [undo setActionName:@"Raster interrupt"];
	
	[[c64 vic] toggleRasterInterruptFlag];
	[self refresh];
}

- (IBAction)vicRasterInterruptAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicRasterInterruptAction:[NSNumber numberWithInt:[[c64 vic] getRasterInterruptLine]]];
	if (![undo isUndoing]) [undo setActionName:@"Raster interrupt line"];
	
	[[c64 vic] setRasterInterruptLine:[sender intValue]];
	[self refresh];
}

- (IBAction)vicEnableOpenGL:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicEnableOpenGL:self];
	if (![undo isUndoing]) [undo setActionName:@"OpenGL"];
	
	enableOpenGL = !enableOpenGL;
	[self refresh];
}

// --------------------------------------------------------------------------------
// Refresh methods: Force all GUI items to refresh their value
// 
// refresh: Refresh all components 
// refresh: word: disassebler: Refresh all components using different formatters
// --------------------------------------------------------------------------------

- (void)refreshCPU
{
	[a setIntValue:[[c64 cpu] getA]];
	[x setIntValue:[[c64 cpu] getX]];
	[y setIntValue:[[c64 cpu] getY]];
	[pc setIntValue:[[c64 cpu] getPC]];
	[sp setIntValue:[[c64 cpu] getSP]];
	[mhzField setFloatValue:mhz];
	
	[N setIntValue:[[c64 cpu] getN]];
	[V setIntValue:[[c64 cpu] getV]];
	[B setIntValue:[[c64 cpu] getB]];
	[D setIntValue:[[c64 cpu] getD]];
	[I setIntValue:[[c64 cpu] getI]];
	[Z setIntValue:[[c64 cpu] getZ]];
	[C setIntValue:[[c64 cpu] getC]];	
}

- (void)refreshMemory
{
#if 0
	uint16_t addr = [addr_search intValue];
	switch ([[c64 mem] getWatchpointType:addr]) {
		case Memory::NO_WATCHPOINT:
			[watchMode selectCellWithTag:1];
			break;
		case Memory::WATCH_FOR_ALL:
			[watchMode selectCellWithTag:2];
			break;
		case Memory::WATCH_FOR_VALUE:
			[watchMode selectCellWithTag:3];
			break;
	}	
	[watchValField setIntValue:[[c64 mem] getWatchValue:addr]];	
#endif	
}

- (void)refreshCIA
{
	CIAProxy *cia = [c64 cia:[self currentCIA]];
	
	[ciaDataPortA setIntValue:[cia getDataPortA]];
	[ciaDataPortDirectionA setIntValue:[cia getDataPortDirectionA]];
	[ciaTimerA setIntValue:[cia getTimerA]];
	[ciaLatchedTimerA setIntValue:[cia getTimerLatchA]];
	[ciaRunningA setIntValue:[cia getStartFlagA]];
	[ciaOneShotA setIntValue:[cia getOneShotFlagA]];
	[ciaSignalPendingA setIntValue:[cia getPendingSignalFlagA]];
	[ciaInterruptEnableA setIntValue:[cia getInterruptEnableFlagA]];
	
	[ciaDataPortB setIntValue:[cia getDataPortB]];
	[ciaDataPortDirectionB setIntValue:[cia getDataPortDirectionB]];
	[ciaTimerB setIntValue:[cia getTimerB]];
	[ciaLatchedTimerB setIntValue:[cia getTimerLatchB]];
	[ciaRunningB setIntValue:[cia getStartFlagB]];
	[ciaOneShotB setIntValue:[cia getOneShotFlagB]];
	[ciaSignalPendingB setIntValue:[cia getPendingSignalFlagB]];
	[ciaInterruptEnableB setIntValue:[cia getInterruptEnableFlagB]];
	
	[todHours setIntValue:[cia getTodHours]];
	[todMinutes setIntValue:[cia getTodMinutes]];
	[todSeconds setIntValue:[cia getTodSeconds]];
	[todTenth setIntValue:[cia getTodTenth]];
	
	[alarmHours setIntValue:[cia getAlarmHours]];
	[alarmMinutes setIntValue:[cia getAlarmMinutes]];
	[alarmSeconds setIntValue:[cia getAlarmSeconds]];
	[alarmTenth setIntValue:[cia getAlarmTenth]];	
	[todInterruptEnabled setIntValue:[cia todIsInterruptEnabled]];
}

- (void)refreshVIC
{
	if (![VicVideoMode selectItemWithTag:[[c64 vic] getDisplayMode]])
		[VicVideoMode selectItemWithTag:1];
	if (![VicScreenGeometry selectItemWithTag:[[c64 vic] getScreenGeometry]])
		NSLog(@"Can't refresh screen geometry field");
	if (![VicMemoryBank selectItemWithTag:[[c64 vic] getMemoryBankAddr]])
		NSLog(@"Can't refresh memory bank field");
	if (![VicScreenMemory selectItemWithTag:[[c64 vic] getScreenMemoryAddr]])
		NSLog(@"Can't refresh screen memory field");
	if (![VicCharacterMemory selectItemWithTag:[[c64 vic] getCharacterMemoryAddr]])
		NSLog(@"Can't refresh screen memory field");
	[VicDX setIntValue:[[c64 vic] getHorizontalRasterScroll]];
	[VicDXStepper setIntValue:[[c64 vic] getHorizontalRasterScroll]];
	[VicDY setIntValue:[[c64 vic] getVerticalRasterScroll]];
	[VicDYStepper setIntValue:[[c64 vic] getVerticalRasterScroll]];
	[VicSpriteActive setIntValue:[[c64 vic] spriteGetVisibilityFlag:[self currentSprite]]];
	[VicSpriteMulticolor setIntValue:[[c64 vic] spriteGetMulticolorFlag:[self currentSprite]]];
	[VicSpriteStretchX setIntValue:[[c64 vic] spriteGetStretchXFlag:[self currentSprite]]];
	[VicSpriteStretchY setIntValue:[[c64 vic] spriteGetStretchYFlag:[self currentSprite]]];
	[VicSpriteInFront setIntValue:[[c64 vic] spriteGetBackgroundPriorityFlag:[self currentSprite]]];
	[VicSpriteSpriteCollision setIntValue:[[c64 vic] spriteGetSpriteSpriteCollisionFlag:[self currentSprite]]];
	[VicSpriteBackgroundCollision setIntValue:[[c64 vic] spriteGetSpriteBackgroundCollisionFlag:[self currentSprite]]];
	[VicSpriteX setIntValue:[[c64 vic] spriteGetX:[self currentSprite]]];
	[VicSpriteY setIntValue:[[c64 vic] spriteGetY:[self currentSprite]]];
	[VicSpriteColor setIntValue:[[c64 vic] spriteGetColor:[self currentSprite]]];
	[VicRasterline setIntValue:[[c64 vic] getRasterLine]];
	[VicEnableRasterInterrupt setIntValue:[[c64 vic] getRasterInterruptFlag]];
	[VicRasterInterrupt setIntValue:[[c64 vic] getRasterInterruptLine]];
}

- (void)refreshSID
{
	// SID yet to come...
}

- (void)refresh
{		
	[self refreshCPU];
	[self refreshMemory];
	[self refreshCIA];
	[self refreshVIC];
	[self refreshSID];

	// Prepare Disassembler window.
	// The implementation is a little tricky. We distinguish two cases:
	// 1. The PC points to an address that is already visible in some row
	//    In this case, we simply select the row and don't modify anything else
	// 2. The PC points to an address that is not yet displayes
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
		// mhzField,
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
	
	// The following components are always disabled
	[mhzField setEnabled:NO];
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

- (id)objectValueForCpuTableColumn:(NSTableColumn *)aTableColumn row:(int)row
{
	uint16_t addr = [[c64 cpu] getAddressOfNextIthInstruction:row from:disassembleStartAddr];
	uint8_t length = [[c64 cpu] getLengthOfInstruction:[[c64 mem] peek:addr]];
	
	if ([[aTableColumn identifier] isEqual:@"addr"]) 
		return [NSNumber numberWithInt:addr];
	else if ([[aTableColumn identifier] isEqual:@"data01"]) 
		return (length > 0 ? [NSNumber numberWithInt:[[c64 mem] peek:addr]] : nil);
	else if ([[aTableColumn identifier] isEqual:@"data02"]) 
		return (length > 1 ? [NSNumber numberWithInt:[[c64 mem] peek:(addr+1)]] : nil);
	else if ([[aTableColumn identifier] isEqual:@"data03"]) 
		return (length > 2 ? [NSNumber numberWithInt:[[c64 mem] peek:(addr+2)]] : nil);
	else if ([[aTableColumn identifier] isEqual:@"ascii"]) 
		return [NSNumber numberWithInt:addr];

	return @"???";
}

- (id)objectValueForMemTableColumn:(NSTableColumn *)aTableColumn row:(int)row
{
	uint16_t addr = row * 4;
	NSString *id  = [aTableColumn identifier];
		
	// Address column...
	if ([[aTableColumn identifier] isEqual:@"addr"]) 
		return [NSNumber numberWithInt:4*row];

	// ASCII column...
	if ([[aTableColumn identifier] isEqual:@"ascii"]) {
		if (![[c64 mem] isValidAddr:addr memtype:[self currentMemSource]])
			return nil;
		else
			return [NSString stringWithFormat:@"%c%c%c%c", 
				toASCII([[c64 mem] peekFrom:(addr+0) memtype:[self currentMemSource]]),
				toASCII([[c64 mem] peekFrom:(addr+1) memtype:[self currentMemSource]]),
				toASCII([[c64 mem] peekFrom:(addr+2) memtype:[self currentMemSource]]),
				toASCII([[c64 mem] peekFrom:(addr+3) memtype:[self currentMemSource]])];
	}
	
	// One of the hexadecimal columns...
	if ([id isEqual:@"hex1"]) addr += 1;
	if ([id isEqual:@"hex2"]) addr += 2;
	if ([id isEqual:@"hex3"]) addr += 3;
	
	if (![[c64 mem] isValidAddr:addr memtype:[self currentMemSource]])
		return nil;
	
	return [NSNumber numberWithInt:[[c64 mem] peekFrom:addr memtype:[self currentMemSource]]];
	
	return nil;
}
	
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)row
{
	if (aTableView == cpuTableView)
		return [self objectValueForCpuTableColumn:aTableColumn row:row];
	if (aTableView == memTableView)
		return [self objectValueForMemTableColumn:aTableColumn row:row];
	return nil;
}

- (void)changeMemValue:(uint16_t)addr value:(int16_t)v memtype:(Memory::MemoryType)t
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] changeMemValue:addr value:[[c64 mem] peekFrom:addr memtype:t] memtype:t];
	if (![undo isUndoing]) [undo setActionName:@"Memory contents"];
				
	[[c64 mem] pokeTo:addr value:v memtype:t];
	[self refresh];	
}

- (void)setMemObjectValue:(id)anObject forTableColumn:(NSTableColumn *)aTableColumn row:(int)row
{

	uint16_t addr = row * 4;
	int16_t value = [anObject intValue];
	NSString *id  = [aTableColumn identifier];
	
	// Compute exact address
	if ([id isEqual:@"hex1"]) addr += 1;
	if ([id isEqual:@"hex2"]) addr += 2;
	if ([id isEqual:@"hex3"]) addr += 3;

	uint8_t oldValue = [[c64 mem] peekFrom:addr memtype:[self currentMemSource]];
	if (oldValue == value)
		return; 

	[self changeMemValue:addr value:value memtype:[self currentMemSource]];
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
#if 0
	else if (aTableView == memTableView) {
		if ([[aTableColumn identifier] isEqual:@"hex0"])
			[aCell setTextColor:([[c64 mem] getWatchpointType:(0+4*row)] != Memory::NO_WATCHPOINT ? [NSColor redColor] : [NSColor blackColor])];
		else if ([[aTableColumn identifier] isEqual:@"hex1"])
			[aCell setTextColor:([[c64 mem] getWatchpointType:(1+4*row)] != Memory::NO_WATCHPOINT ? [NSColor redColor] : [NSColor blackColor])];
		else if ([[aTableColumn identifier] isEqual:@"hex2"])
			[aCell setTextColor:([[c64 mem] getWatchpointType:(2+4*row)] != Memory::NO_WATCHPOINT ? [NSColor redColor] : [NSColor blackColor])];
		else if ([[aTableColumn identifier] isEqual:@"hex3"])
			[aCell setTextColor:([[c64 mem] getWatchpointType:(3+4*row)] != Memory::NO_WATCHPOINT ? [NSColor redColor] : [NSColor blackColor])];		
	}
#endif
}

// --------------------------------------------------------------------------------
// Dialogs
// --------------------------------------------------------------------------------

- (BOOL)setArchiveWithName:(NSString *)path
{
	if ((archive = T64Archive::archiveFromFile([path UTF8String])) != NULL)
		return YES;

	if ((archive = D64Archive::archiveFromFile([path UTF8String])) != NULL)
		return YES;

	if ((archive = PRGArchive::archiveFromFile([path UTF8String])) != NULL)
		return YES;

	if ((archive = P00Archive::archiveFromFile([path UTF8String])) != NULL)
		return YES;

	return NO;
}

- (IBAction)cancelRomDialog:(id)sender
{
	// Hide sheet
	[romDialog orderOut:sender];
	
	// Return to normal event handling
	[NSApp endSheet:romDialog returnCode:1];
}

- (BOOL)showMountDialog
{
	if (archive == NULL)
		return NO;
	
	[mountDialog initialize:archive];
	
	[NSApp beginSheet:mountDialog
		modalForWindow:[self windowForSheet]
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
	
	[c64 mountArchive:archive];
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
	[c64 mountArchive:archive];
	
	// Load clean image 
	[c64 fastReset];
	
	// Flash selected file into memory
	[c64 flushArchive:archive item:[mountDialog getSelectedFile]];

	// Wait and type "RUN"
	fprintf(stderr,"Wating...\n");
	usleep(1000000);
	fprintf(stderr,"Typing RUN...\n");
	[[c64 keyboard] typeRun];
}

- (BOOL)attachCartridge:(NSString *)path
{
	if (!(cartridge = Cartridge::cartridgeFromFile([path UTF8String])))
		return NO;
			
	// Try to mount archive
	[c64 attachCartridge:cartridge];
	
	// reset
	[c64 reset];
	
	return YES;
}

- (BOOL)detachCartridge
{
	[c64 detachCartridge];	
	delete cartridge;
	cartridge = NULL;
	
	return YES;
}

- (IBAction)cartridgeEjectAction:(id)sender
{
	NSLog(@"cartridgeEjectAction");	
	[c64 detachCartridge];
	delete cartridge;
	cartridge = NULL;
	[c64 reset];
}

// --------------------------------------------------------------------------------
// Helper functions
// --------------------------------------------------------------------------------

- (int)currentSprite
{
	return selectedSprite; 
	
#if 0	
	if ([sprite0 intValue]) return 0;
	if ([sprite1 intValue]) return 1;
	if ([sprite2 intValue]) return 2;
	if ([sprite3 intValue]) return 3;
	if ([sprite4 intValue]) return 4;
	if ([sprite5 intValue]) return 5;
	if ([sprite6 intValue]) return 6;
	if ([sprite7 intValue]) return 7;	

	assert(false);
	return 0;
#endif
}

- (Memory::MemoryType)currentMemSource
{
	if ([ramSource intValue]) return Memory::MEM_RAM;
	if ([romSource intValue]) return Memory::MEM_ROM;
	if ([ioSource intValue]) return Memory::MEM_IO;

	assert(false);
	return Memory::MEM_RAM;
}

- (int)currentCIA
{
	if ([ciaSelector indexOfSelectedItem] == 0)
		return 1;
	else
		return 2;
}

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

@end
