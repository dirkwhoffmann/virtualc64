/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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

#include "C64.h"
#import "MyDocument.h"

static void addToolbarItem(NSMutableDictionary *theDict,NSString *identifier,NSString *label,NSString *paletteLabel,NSString *toolTip,id target,SEL settingSelector, id itemContent,SEL action, NSMenu * menu)
{
    NSMenuItem *mItem;
    // here we create the NSToolbarItem and setup its attributes in line with the parameters
    NSToolbarItem *item = [[[NSToolbarItem alloc] initWithItemIdentifier:identifier] autorelease];
    [item setLabel:label];
    [item setPaletteLabel:paletteLabel];
    [item setToolTip:toolTip];
    [item setTarget:target];
    // the settingSelector parameter can either be @selector(setView:) or @selector(setImage:).  Pass in the right
    // one depending upon whether your NSToolbarItem will have a custom view or an image, respectively
    // (in the itemContent parameter).  Then this next line will do the right thing automatically.
    [item performSelector:settingSelector withObject:itemContent];
    [item setAction:action];
    // If this NSToolbarItem is supposed to have a menu "form representation" associated with it (for text-only mode),
    // we set it up here.  Actually, you have to hand an NSMenuItem (not a complete NSMenu) to the toolbar item,
    // so we create a dummy NSMenuItem that has our real menu as a submenu.
    if (menu!=NULL)
    {
		// we actually need an NSMenuItem here, so we construct one
		mItem=[[[NSMenuItem alloc] init] autorelease];
		[mItem setSubmenu: menu];
		[mItem setTitle: [menu title]];
		[item setMenuFormRepresentation:mItem];
    }
    // Now that we've setup all the settings for this new toolbar item, we add it to the dictionary.
    // The dictionary retains the toolbar item for us, which is why we could autorelease it when we created
    // it (above).
    [theDict setObject:item forKey:identifier];
}


@implementation MyDocument

// --------------------------------------------------------------------------------
//                          Construction and Destruction
// --------------------------------------------------------------------------------

+ (void)initialize {
	NSLog(@"%@ initialize", self);

    self = [super init];

	// Create a dictionary
	NSMutableDictionary *defaultValues = [NSMutableDictionary dictionary];
		
	// Put default values into dictionary

	/* System */
	[defaultValues setObject:[NSNumber numberWithInt:1] forKey:VC64PALorNTSCKey];
		
	/* Peripherals */
	[defaultValues setObject:[NSNumber numberWithBool:YES] forKey:VC64WarpLoadKey];

	/* Audio */
	[defaultValues setObject:[NSNumber numberWithBool:YES] forKey:VC64SIDFilterKey];


	/* Video */
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

#if 0
	[defaultValues setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor yellowColor]] forKey:VC64CustomCol1Key];
	[defaultValues setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor yellowColor]] forKey:VC64CustomCol2Key];
	[defaultValues setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor yellowColor]] forKey:VC64CustomCol3Key];
	[defaultValues setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor yellowColor]] forKey:VC64CustomCol4Key];
	[defaultValues setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor yellowColor]] forKey:VC64CustomCol5Key];
	[defaultValues setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor yellowColor]] forKey:VC64CustomCol6Key];
	[defaultValues setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor yellowColor]] forKey:VC64CustomCol7Key];
	[defaultValues setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor yellowColor]] forKey:VC64CustomCol8Key];
	[defaultValues setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor yellowColor]] forKey:VC64CustomCol9Key];
	[defaultValues setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor yellowColor]] forKey:VC64CustomCol10Key];
	[defaultValues setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor yellowColor]] forKey:VC64CustomCol11Key];
	[defaultValues setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor yellowColor]] forKey:VC64CustomCol12Key];
	[defaultValues setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor yellowColor]] forKey:VC64CustomCol13Key];
	[defaultValues setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor yellowColor]] forKey:VC64CustomCol14Key];
	[defaultValues setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor yellowColor]] forKey:VC64CustomCol15Key];
#endif
	
	// Register the dictionary of defaults
	[[NSUserDefaults standardUserDefaults] registerDefaults:defaultValues];
	
	NSLog(@"Registered user defaults: %@", defaultValues);	

	// Initialize SDL joystick part
	// SDL_JoystickInit();
}

- (id)init  
{	
	NSLog(@"%@ init", self);
	
    self = [super init];
    if (self) {
		// We change the working directory before creating the virtual machine, because
		// ROMs are currenty part of the distrubution and will be found immediately.
		// If we decide not to include ROM files, changing the directory is not 
		// needed at all and can be deleted.
		NSBundle* mainBundle = [NSBundle mainBundle];
		NSString *path = [mainBundle resourcePath];
		if (chdir([path UTF8String]) != 0)
			NSLog(@"WARNING: Could not change working directory.");
		else
			NSLog(@"Changed working directory.");
    }
    return self;
}

- (void)dealloc
{	
	NSLog(@"%@ dealloc", self);

	[preferenceController release];
	NSLog(@"Preference controller released");
	[consoleController release];
	NSLog(@"Console controller released");
	[toolbarItems release];
	NSLog(@"Toolbar released");
	[super dealloc];
	NSLog(@"super released");
}

- (void)windowWillClose:(NSNotification *)aNotification
{
	NSLog(@"%@ windowWillClose", self);

	[timer invalidate];
	timer = nil;
	
	// Delete virtual machine
	[c64 release];
}

- (void)awakeFromNib
{	
	NSLog(@"%@ awakeFromNib", self);

	// Initialize variables
	enableOpenGL = false;
			
	// Create virtual C64
	c64 = [[C64Proxy alloc] initWithDocument:self withScreen:screen];						

	
	disassembleStartAddr = [c64 cpuGetPC];
	
	// get images for port A and B, depends on the available input devices
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

	
	// Create toolbar
	NSToolbar *toolbar = [[[NSToolbar alloc] initWithIdentifier:@"VirtualC64Toolbar"] autorelease];
	toolbarItems = [[NSMutableDictionary dictionary] retain];
    addToolbarItem(toolbarItems,@"Reset",@"Reset",@"Reset",@"Hard reset",
				   self,@selector(setImage:),[NSImage imageNamed:@"restart32"],@selector(resetAction:),NULL);
    addToolbarItem(toolbarItems,@"Pause",@"Pause",@"Pause",@"Pause",
				   self,@selector(setImage:),[NSImage imageNamed:@"pause32"],@selector(pauseAction:),NULL);
    addToolbarItem(toolbarItems,@"Step into",@"Step into",@"Step into",@"Step into",
				   self,@selector(setImage:),[NSImage imageNamed:@"stepInto32"],@selector(stepIntoAction:),NULL);
    addToolbarItem(toolbarItems,@"Step out",@"Step out",@"Step out",@"Step out",
				   self,@selector(setImage:),[NSImage imageNamed:@"stepOut32"],@selector(stepOutAction:),NULL);
    addToolbarItem(toolbarItems,@"Step over",@"Step over",@"Step over",@"Step over",
				   self,@selector(setImage:),[NSImage imageNamed:@"stepOver32"],@selector(stepOverAction:),NULL);
    addToolbarItem(toolbarItems,@"Continue",@"Continue",@"Continue",@"Continue",
				   self,@selector(setImage:),[NSImage imageNamed:@"play32"],@selector(stopAndGoAction:),NULL);
    addToolbarItem(toolbarItems,@"Inspect",@"Inspect",@"Inspect",@"Open / Close inspector panel",
				   self,@selector(setImage:),[NSImage imageNamed:@"debugger32"],@selector(debugAction:),NULL);
	addToolbarItem(toolbarItems,@"Joystick1",@"Port A",@"Joystick1",@"Game port 1 device",
					self,@selector(setImage:),imagePortA,@selector(joystick1Action:),NULL);
	// addToolbarItem(toolbarItems,@"Switch",@"Switch",@"Switch",@"switch joysticks",
	// 				self,@selector(setImage:),[NSImage imageNamed:@"switch_32"],@selector(switchJoysticksAction:),NULL);
	addToolbarItem(toolbarItems,@"Joystick2",@"Port B",@"Joystick2",@"Game port 2 device",
					self,@selector(setImage:),imagePortB,@selector(joystick2Action:),NULL);
    addToolbarItem(toolbarItems,@"Sound",@"Sound",@"Sound",@"Sound",
				   self,@selector(setImage:),[NSImage imageNamed:@"soundOn"],@selector(resetAction:),NULL);
    addToolbarItem(toolbarItems,@"Fullscreen",@"Fullscreen",@"Fullscreen",@"Fullscreen",
				   self,@selector(setImage:),[NSImage imageNamed:@"fullscreen"],@selector(fullscreenAction:),NULL);
    [toolbar setDelegate:self];
    [toolbar setAllowsUserCustomization:YES];
    [toolbar setAutosavesConfiguration: NO]; 
    [toolbar setDisplayMode: NSToolbarDisplayModeIconAndLabel]; // NSToolbarDisplayModeIconOnly
    [theWindow setToolbar:toolbar];	

	// Create and bind number formatters
	[self setHexadecimal:self];

	// Prepare to get double-click messages
	[cpuTableView setTarget:self];
	[cpuTableView setDoubleAction:@selector(doubleClickInCpuTable:)];
	[memTableView setTarget:self];
	[memTableView setDoubleAction:@selector(doubleClickInMemTable:)];
		
	// Create timer
	cycleCount = 0;
	timeStamp  = msec();
	if (!timer)
		timer = [NSTimer scheduledTimerWithTimeInterval:(1.0f/60.0f) 
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
	int colorScheme;
	NSUserDefaults *defaults;
	
	NSLog(@"Loading user defaults");
	
	// Set user defaults
	defaults = [NSUserDefaults standardUserDefaults];

	/* System */
	if ([defaults integerForKey:VC64PALorNTSCKey]) {
		[c64 setNTSC];
	} else {
		[c64 setPAL];
	}
		
	/* Peripherals */
	[c64 setWarpLoad:[defaults boolForKey:VC64WarpLoadKey]];
	
	/* Audio */
	// [c64 sidEnableFilter:[defaults boolForKey:VC64SIDFilterKey]];
	
	/* Video */
	// [c64 vicSetVideoFilter:[defaults integerForKey:VC64VideoFilterKey];
	colorScheme = [defaults integerForKey:VC64ColorSchemeKey];
	if (colorScheme == VIC::CUSTOM_PALETTE) {
		NSLog(@"Applying custom colors...");
		[c64 vicSetColorInt:0 rgba:[defaults integerForKey:VC64CustomCol0Key]];
		[c64 vicSetColorInt:1 rgba:[defaults integerForKey:VC64CustomCol1Key]];
		[c64 vicSetColorInt:2 rgba:[defaults integerForKey:VC64CustomCol2Key]];
		[c64 vicSetColorInt:3 rgba:[defaults integerForKey:VC64CustomCol3Key]];
		[c64 vicSetColorInt:4 rgba:[defaults integerForKey:VC64CustomCol4Key]];
		[c64 vicSetColorInt:5 rgba:[defaults integerForKey:VC64CustomCol5Key]];
		[c64 vicSetColorInt:6 rgba:[defaults integerForKey:VC64CustomCol6Key]];
		[c64 vicSetColorInt:7 rgba:[defaults integerForKey:VC64CustomCol7Key]];
		[c64 vicSetColorInt:8 rgba:[defaults integerForKey:VC64CustomCol8Key]];
		[c64 vicSetColorInt:9 rgba:[defaults integerForKey:VC64CustomCol9Key]];
		[c64 vicSetColorInt:10 rgba:[defaults integerForKey:VC64CustomCol10Key]];
		[c64 vicSetColorInt:11 rgba:[defaults integerForKey:VC64CustomCol11Key]];
		[c64 vicSetColorInt:12 rgba:[defaults integerForKey:VC64CustomCol12Key]];
		[c64 vicSetColorInt:13 rgba:[defaults integerForKey:VC64CustomCol13Key]];
		[c64 vicSetColorInt:14 rgba:[defaults integerForKey:VC64CustomCol14Key]];
		[c64 vicSetColorInt:15 rgba:[defaults integerForKey:VC64CustomCol15Key]];
	} else {
		[c64 vicSetColorScheme:(VIC::ColorScheme)colorScheme];
	}
}

- (void)windowControllerDidLoadNib:(NSWindowController *) aController
{	
	NSLog(@"%@ windowControllerDidLoadNib", self);
	[super windowControllerDidLoadNib:aController];
   	
	// Load user defaults
	[self loadUserDefaults];
		
	// Launch emulator
	[c64 run];
	
	// Start drawing
	enableOpenGL = true;
}

-(NSSize)windowWillResize:(NSWindow *)sender toSize:(NSSize)proposedFrameSize 
{
	const float c64_display_width    = 652.0;
	const float c64_display_height   = 412.0;
	const float bottom_border_height = 20.0; 
	
	NSView *contentView = [sender contentView];
	NSRect size = [contentView frame];

	// Adjust aspect ratio. The window should keep the aspect ratio of the c64 display constant
	float yoffset = bottom_border_height * (c64_display_height / size.size.height);
	[sender setContentAspectRatio:NSMakeSize(c64_display_width / 100.0, (c64_display_height + yoffset) / 100.0)];
	
	return proposedFrameSize;
}

-(NSRect)windowWillUseStandardFrame:(NSWindow *)sender defaultFrame:(NSRect)defaultFrame
{
	float border_height;
	float orig_screen_height, orig_screen_width;
	float new_screen_height, new_screen_width;

	NSView *contentView = [sender contentView];
	NSRect contentFrame = [contentView frame];
	NSRect windowFrame = [sender frame];
	
	border_height = windowFrame.size.height - contentFrame.size.height;
	orig_screen_height = contentFrame.size.height - 20;
	orig_screen_width  = contentFrame.size.width;
	
	new_screen_height  = defaultFrame.size.height - 20 - border_height;
	new_screen_width   = new_screen_height * (orig_screen_width / orig_screen_height);
	
	defaultFrame.size.width = (int)new_screen_width;
	
	return defaultFrame;
}

// --------------------------------------------------------------------------------
// Loading and saving
// --------------------------------------------------------------------------------

+ (bool)isNativeType:(NSString *)aType
{
	return [aType isEqual:@"VC64"];
}

-(bool)writeToFile:(NSString *)filename ofType:(NSString *)type
{
	NSLog(@"writeToFile %@ (type %@)", filename, type);
	return [c64 saveSnapshot:filename];
}

-(bool)readFromFile:(NSString *)filename ofType:(NSString *)type
{
	// C64 *myc64 = [c64 getC64]; 
	// Archive *archive;

	NSLog(@"readFromFile %@ (type %@)", filename, type);

	if ([type isEqualToString:@"D64"]) {
		// not yet supported
		return NO;
	} else {
		return [c64 loadSnapshot:filename];
	}
}

- (bool)revertToSavedFromFile:(NSString *)filename ofType:(NSString *)type
{
	NSLog(@"revertFromFile %@ (type %@)", filename, type);
	return [c64 loadSnapshot:filename];
}


// --------------------------------------------------------------------------------
//                The screen refresh loop (called via the 60 Hz timer)
// --------------------------------------------------------------------------------

- (void) windowDidBecomeMain: (NSNotification *) notification
{
	// NSLog(@"%@ did become main...", self);
	
	// Initialize sound device
	SIDDevice *sid = [c64 getSIDDevice];
	C64 *c = [c64 getC64];
	assert(sid != NULL);
	
	sid->SetupDevice(c->sid);	
}

- (void) windowDidResignMain: (NSNotification *) notification
{
	// NSLog(@"%@ did resign main...", self);
	
	// Release sound device
	SIDDevice *sid = [c64 getSIDDevice];
	assert(sid != NULL);
	
	sid->FreeDevice();
}

// --------------------------------------------------------------------------------
//                The screen refresh loop (called via the 60 Hz timer)
// --------------------------------------------------------------------------------

- (void)timerFunc
{	
	animationCounter++;

	// Do 60 times a second...
	if (enableOpenGL) {
		[screen updateAngles];
		[screen setNeedsDisplay:YES];
	}

	// Do less times ... 
	if (animationCounter & 0x07) {
		return;
	}
	
	if ([c64 isRunning] && ([debug_panel state] == NSDrawerOpenState || [debug_panel state] == NSDrawerOpeningState)) {
		// refresh debug panel information...
		[self refresh];
	}

	// Do even less times...
	if (animationCounter & 0x17) {
		return;
	}

	// Measure clock frequency and frame rate
	float fps;
	long currentTime   = msec();
	long currentCycles = [c64 cpuGetCycles];
	long currentFrames = [screen getFrames];
	long elapsedTime   = currentTime - timeStamp;
	long elapsedCycles = currentCycles - cycleCount;
	long elapsedFrames = currentFrames - frameCount;
	
	// print how fast we're flying
	mhz = (float)elapsedCycles / (float)elapsedTime;
	fps = round(((float)(elapsedFrames * 1000000) / (float)elapsedTime));
	
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
	if ([theItem itemIdentifier] == @"Continue") {
		if ([c64 isRunning]) {
			[theItem setImage:[NSImage imageNamed:@"pause32"]];
			[theItem setLabel:@"Pause"];
		} else {
			[theItem setImage:[NSImage imageNamed:@"play32"]];
			[theItem setLabel:@"Run"];
		}
	}

	if ([theItem itemIdentifier] == @"Step into") {
	}

	if ([theItem itemIdentifier] == @"Step out") {
	}

	if ([theItem itemIdentifier] == @"Step over") {
	}

	if ([theItem itemIdentifier] == @"Joystick1") {
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
	}
	if ([theItem itemIdentifier] == @"Joystick2") {
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
	}
	
    return YES;
}

// This is an optional delegate method, called when a new item is about to be added to the toolbar.
// This is a good spot to set up initial state information for toolbar items, particularly ones
// that you don't directly control yourself (like with NSToolbarPrintItemIdentifier here).
// The notification's object is the toolbar, and the @"item" key in the userInfo is the toolbar item
// being added.
- (void) toolbarWillAddItem: (NSNotification *) notif
{
    NSToolbarItem *addedItem = [[notif userInfo] objectForKey: @"item"];
    // Is this the printing toolbar item?  If so, then we want to redirect it's action to ourselves
    // so we can handle the printing properly; hence, we give it a new target.
    if ([[addedItem itemIdentifier] isEqual: NSToolbarPrintItemIdentifier])
    {
		[addedItem setLabel: @"Print"];
        [addedItem setToolTip: @"Print your document"];
        [addedItem setTarget: self];
    }
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
    [printOperation runOperationModalForWindow:theWindow delegate: nil didRunSelector: NULL contextInfo:NULL];

	[imageView release];
}

// This method is required of NSToolbar delegates.  It takes an identifier, and returns the matching NSToolbarItem.
// It also takes a parameter telling whether this toolbar item is going into an actual toolbar, or whether it's
// going to be displayed in a customization palette.
- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar itemForItemIdentifier:(NSString *)itemIdentifier willBeInsertedIntoToolbar:(BOOL)flag
{
    // We create and autorelease a new NSToolbarItem, and then go through the process of setting up its
    // attributes from the master toolbar item matching that identifier in our dictionary of items.
    NSToolbarItem *newItem = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];
    NSToolbarItem *item=[toolbarItems objectForKey:itemIdentifier];
    
    [newItem setLabel:[item label]];
    [newItem setPaletteLabel:[item paletteLabel]];
    if ([item view]!=NULL)
    {
		[newItem setView:[item view]];
    }
    else
    {
		[newItem setImage:[item image]];
    }
    [newItem setToolTip:[item toolTip]];
    [newItem setTarget:[item target]];
    [newItem setAction:[item action]];
    [newItem setMenuFormRepresentation:[item menuFormRepresentation]];
    // If we have a custom view, we *have* to set the min/max size - otherwise, it'll default to 0,0 and the custom
    // view won't show up at all!  This doesn't affect toolbar items with images, however.
    if ([newItem view]!=NULL)
    {
		[newItem setMinSize:[[item view] bounds].size];
		[newItem setMaxSize:[[item view] bounds].size];
    }
	
    return newItem;
}

// This method is required of NSToolbar delegates.  It returns an array holding identifiers for the default
// set of toolbar items.  It can also be called by the customization palette to display the default toolbar.    
- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar*)toolbar
{
    return [NSArray arrayWithObjects:
		@"Continue",
		// NSToolbarSeparatorItemIdentifier,
		@"Step into", @"Step out", @"Step over", 
		//NSToolbarSeparatorItemIdentifier,
		//NSToolbarSpaceItemIdentifier,
		NSToolbarFlexibleSpaceItemIdentifier,
        @"Joystick1", @"Joystick2", 
		//@"Sound", 
		@"Fullscreen",
		// NSToolbarPrintItemIdentifier,
		// NSToolbarSeparatorItemIdentifier,
		//NSToolbarFlexibleSpaceItemIdentifier,
        //NSToolbarSpaceItemIdentifier,
		// @"Sound", 
		//NSToolbarSpaceItemIdentifier,
		// NSToolbarSeparatorItemIdentifier,
		NSToolbarFlexibleSpaceItemIdentifier,
		@"Reset", 
		@"Inspect", nil];
}

// This method is required of NSToolbar delegates.  It returns an array holding identifiers for all allowed
// toolbar items in this toolbar.  Any not listed here will not be available in the customization palette.
- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar*)toolbar
{
	return [NSArray arrayWithObjects: 
		@"Reset", @"Continue", @"Pause", @"Step into", @"Step out", @"Step over", 
		@"Joystick1", @"Joystick2", @"Sound", @"Fullscreen", @"Inspect",
        NSToolbarPrintItemIdentifier,
        // NSToolbarCustomizeToolbarItemIdentifier,
        NSToolbarFlexibleSpaceItemIdentifier,
        NSToolbarSpaceItemIdentifier,
        NSToolbarSeparatorItemIdentifier, nil];
}


// --------------------------------------------------------------------------------
// Action methods (Main screen)
// --------------------------------------------------------------------------------

- (IBAction)stepperAction:(id)sender
{
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
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] warpAction:[NSNumber numberWithInt:![c64 cpuGetWarpMode]]];
	if (![undo isUndoing]) [undo setActionName:@"Native speed"];
		
	NSLog(@"Warping");
	
	[c64 cpuToggleWarpMode];
	[self refresh];
}



// --------------------------------------------------------------------------------
// Action methods (Debug panel, common area)
// --------------------------------------------------------------------------------

- (IBAction)ejectAction:(id)sender
{
	NSLog(@"Ejecting disk...");
	[c64 ejectDisk];
}

- (IBAction)driveAction:(id)sender
{
	NSLog(@"Drive action...");
	if ([c64 isDriveConnected]) {
		[c64 disconnectDrive];
	} else {
		[c64 connectDrive];
	}
}

- (IBAction)debugAction:(id)sender
{	
	[debug_panel toggle:self];
	[self refresh];
}

- (IBAction)setHardBreakpoint:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] setHardBreakpoint:[NSNumber numberWithInt:[sender intValue]]];
	if (![undo isUndoing]) [undo setActionName:@"Breakpoint"];
	
	[c64 cpuToggleHardBreakpoint:[sender intValue]];
	[self refresh];
}

- (IBAction)setDecimal:(id)sender
{
	DezFormatter *bF = [[DezFormatter alloc] init];
	DezFormatter *wF = [[DezFormatter alloc] init:@"%05d" min:0 max:65535];
	Disassembler *dis = [[Disassembler alloc] init:c64 byteFormatter:bF wordFormatter:wF ];
	
	[self refresh:bF word:wF disassembler:dis];
	
	[wF release];
	[bF release];
	[dis release];	
}

- (IBAction)setHexadecimal:(id)sender
{
	HexFormatter *bF  = [[HexFormatter alloc] init];
	HexFormatter *wF  = [[HexFormatter alloc] init:@"%04X" min:0 max:65535];
	Disassembler *dis = [[Disassembler alloc] init:c64 byteFormatter:bF wordFormatter:wF ];
	
	[self refresh:bF word:wF disassembler:dis];
	
	[wF release];
	[bF release];
	[dis release];
}

- (IBAction)pauseAction:(id)sender
{
	[c64 halt];
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
	int addr = [c64 cpuGetTopOfCallStack];
	if (addr < 0)
		return;

	// Set soft breakpoint at next command and run
	[c64 cpuSetSoftBreakpoint:(addr+3)];	
	[c64 run];
}


- (IBAction)stepOverAction:(id)sender
{	
	[self updateChangeCount:NSChangeDone];
	// Is the next instruction a JSR instruction?
	uint8_t opcode = [c64 cpuPeekPC];
	if (opcode == 0x20) {
		// set soft breakpoint at next command
		[c64 cpuSetSoftBreakpoint:[c64 cpuGetAddressOfNextInstruction]];	
		[c64 run];
	} else {
		// same as step
		[self stepIntoAction:self];
	}
}

- (IBAction)stopAndGoAction:(id)sender
{	
	[self updateChangeCount:NSChangeDone];

	if ([c64 isHalted]) {
		[c64 run];
	} else {
		[c64 halt];
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
	[c64 keyboardPressRunstopKey];
	sleepMicrosec(100000);
	[c64 keyboardReleaseRunstopKey];	
	[self refresh];
}
	
- (IBAction)commodoreKeyAction:(id)sender
{
	NSLog(@"Commodore key pressed");
	[self updateChangeCount:NSChangeDone];
	[c64 keyboardPressCommodoreKey];	
	sleepMicrosec(100000);
	[c64 keyboardReleaseCommodoreKey];	
	[self refresh];
}


#if 0
- (IBAction)fullscreenAction:(id)sender
{
	[self refresh];
}
#endif

- (IBAction)drawSpritesAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] drawSpritesAction:sender];
	if (![undo isUndoing]) [undo setActionName:@"Draw sprites"];
	
	[c64 vicToggleDrawSprites];
}

- (IBAction)markIRQLinesAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] markIRQLinesAction:sender];
	if (![undo isUndoing]) [undo setActionName:@"Mark IRQ lines"];

	[c64 vicToggleMarkIRQLines];
}

- (IBAction)showPreferencesAction:(id)sender
{
	NSLog(@"Showing preferences window...");
	if (!preferenceController) {
		preferenceController = [[PreferenceController alloc] init];
		[preferenceController setC64:c64];
		[preferenceController setDoc:self];
	}
	[preferenceController showWindow:self];
}

- (IBAction)showConsoleAction:(id)sender
{
	char buf[64];		
			
	if (!consoleController) {
		sprintf(buf, "Build %d", [c64 buildNr]);
		ConsoleController *controller = [[ConsoleController alloc] init];
		[controller setC64:c64];
		[controller setDoc:self];
		[controller insertText:strdup("Welcome to Virtual C64")];		
		[controller insertText:strdup(buf)];		
		[controller insertText:strdup("")];		
		
		consoleController = controller;
	}
	[consoleController showWindow:self];
}


// --------------------------------------------------------------------------------
// Action methods (CPU Debug panel)
// --------------------------------------------------------------------------------

- (IBAction)pcAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] pcAction:[NSNumber numberWithInt:[c64 cpuGetPC]]];
	if (![undo isUndoing]) [undo setActionName:@"Set program counter"];
	
	[c64 cpuSetPC:[sender intValue]];
	[self refresh];
}

- (IBAction)spAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spAction:[NSNumber numberWithInt:[c64 cpuGetSP]]];
	if (![undo isUndoing]) [undo setActionName:@"Set stack pointer"];

	[c64 cpuSetSP:[sender intValue]];
	[self refresh];
}

- (IBAction)aAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] aAction:[NSNumber numberWithInt:[c64 cpuGetA]]];
	if (![undo isUndoing]) [undo setActionName:@"Set accumulator"];

	[c64 cpuSetA:[sender intValue]];
	[self refresh];
}

- (IBAction)xAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] xAction:[NSNumber numberWithInt:[c64 cpuGetX]]];
	if (![undo isUndoing]) [undo setActionName:@"Set X register"];

	[c64 cpuSetX:[sender intValue]];
	[self refresh];
}

- (IBAction)yAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] yAction:[NSNumber numberWithInt:[c64 cpuGetY]]];
	if (![undo isUndoing]) [undo setActionName:@"Set Y register"];

	[c64 cpuSetY:[sender intValue]];
	[self refresh];
}

- (IBAction)NAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] NAction:[NSNumber numberWithInt:[c64 cpuGetN]]];
	if (![undo isUndoing]) [undo setActionName:@"Negative Flag"];

	[c64 cpuSetN:[sender intValue]];
	[self refresh];
}

- (IBAction)ZAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ZAction:[NSNumber numberWithInt:[c64 cpuGetZ]]];
	if (![undo isUndoing]) [undo setActionName:@"Zero Flag"];

	[c64 cpuSetZ:[sender intValue]];
	[self refresh];
}

- (IBAction)CAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] CAction:[NSNumber numberWithInt:[c64 cpuGetC]]];
	if (![undo isUndoing]) [undo setActionName:@"Carry Flag"];

	[c64 cpuSetC:[sender intValue]];
	[self refresh];
}

- (IBAction)IAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] IAction:[NSNumber numberWithInt:[c64 cpuGetI]]];
	if (![undo isUndoing]) [undo setActionName:@"Interrupt Flag"];

	[c64 cpuSetI:[sender intValue]];
	[self refresh];
}

- (IBAction)BAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] BAction:[NSNumber numberWithInt:[c64 cpuGetB]]];
	if (![undo isUndoing]) [undo setActionName:@"Break Flag"];

	[c64 cpuSetB:[sender intValue]];
	[self refresh];
}

- (IBAction)DAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] DAction:[NSNumber numberWithInt:[c64 cpuGetD]]];
	if (![undo isUndoing]) [undo setActionName:@"Decimal Flag"];

	[c64 cpuSetD:[sender intValue]];
	[self refresh];
}

- (IBAction)VAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] VAction:[NSNumber numberWithInt:[c64 cpuGetV]]];
	if (![undo isUndoing]) [undo setActionName:@"Overflow Flag"];

	[c64 cpuSetV:[sender intValue]];
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
	// Can we do anything meaningful here?
	// It'll be nice to set watchpoints by double clicking a cell
	// For doing that, we need to determine the clicked column. 
	// Can we determine the clicked column at all?
	[self refresh];
}

- (void)doubleClickInCpuTable:(id)sender
{
	uint16_t addr;

	addr = [c64 cpuGetAddressOfNextIthInstruction:[sender selectedRow] from:disassembleStartAddr];
	[self setHardBreakpoint:[NSNumber numberWithInt:addr]];
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

- (void)setWatchPoint:(int)addr type:(Memory::WatchpointType)t value:(uint8_t)v
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] setWatchPoint:addr type:[c64 memGetWatchpointType:addr] value:[c64 memGetWatchValue:addr]];
	if (![undo isUndoing]) [undo setActionName:@"Watchpoint"];

	switch(t) {
		case Memory::NO_WATCHPOINT:
			NSLog(@"Deleting watchpoint at address %d", addr);
			break;
		case Memory::WATCH_FOR_ALL:
			NSLog(@"Watching for any change at address %d", addr);
			break;
		case Memory::WATCH_FOR_VALUE:
			NSLog(@"Watching for value %d at address %d", v, addr);
			break;
		default:
			assert(false);
	}
	[c64 memSetWatchpoint:addr tag:t watchvalue:v];
	[self refresh];
}

- (IBAction)setWatchForNone:(id)sender;
{
	uint16_t addr = [addr_search intValue];

	[self setWatchPoint:addr type:Memory::NO_WATCHPOINT value:[c64 memGetWatchValue:addr]];
}

- (IBAction)setWatchForAll:(id)sender;
{

	uint16_t addr = [addr_search intValue];
	[self setWatchPoint:addr type:Memory::WATCH_FOR_ALL value:[c64 memGetWatchValue:addr]];
}

- (IBAction)setWatchForValue:(id)sender;
{
	uint16_t addr = [addr_search intValue];
	[self setWatchPoint:addr type:Memory::WATCH_FOR_VALUE value:[watchValField intValue]];
}

- (IBAction)setWatchValue:(id)sender;
{
	uint16_t addr = [addr_search intValue];
	[self setWatchPoint:addr type:Memory::WATCH_FOR_VALUE value:[sender intValue]];
}


// --------------------------------------------------------------------------------
// Action methods (CIA 1)
// --------------------------------------------------------------------------------

- (void)ciaSetDataPortA:(int)nr value:(uint8_t)v
{
	[c64 ciaSetDataPortA:nr value:v];
	[self refresh];
}

- (IBAction)ciaDataPortAAction:(id)sender 
{	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetDataPortA:[self currentCIA] value:[c64 ciaGetDataPortA:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"CIA data port A"];
	
	[self ciaSetDataPortA:[self currentCIA] value:[sender intValue]];
} 

- (void)ciaSetDataPortDirectionA:(int)nr value:(uint8_t)v
{
	[c64 ciaSetDataPortDirectionA:nr value:v];
	[self refresh];
}

- (IBAction)ciaDataPortDirectionAAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetDataPortDirectionA:[self currentCIA] value:[c64 ciaGetDataPortDirectionA:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"CIA data port direction A"];
	
	[self ciaSetDataPortDirectionA:[self currentCIA] value:[sender intValue]];
}

- (void)ciaSetTimerA:(int)nr value:(uint16_t)v
{
	[c64 ciaSetTimerA:nr value:v];
	[self refresh];
}

- (IBAction)ciaTimerAAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetTimerA:[self currentCIA] value:[c64 ciaGetDataPortDirectionA:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"CIA timer A"];
	
	[self ciaSetTimerA:[self currentCIA] value:[sender intValue]];
}

- (void)ciaSetTimerLatchA:(int)nr value:(uint16_t)v
{
	[c64 ciaSetTimerLatchA:nr value:v];
	[self refresh];
}

- (IBAction)ciaLatchedTimerAAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetTimerLatchA:[self currentCIA] value:[c64 ciaGetTimerLatchA:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"CIA timer latch A"];
	
	[self ciaSetTimerLatchA:[self currentCIA] value:[sender intValue]];
}

- (void)ciaSetStartFlagA:(int)nr value:(bool)b
{
	[c64 ciaSetStartFlagA:nr value:b];
	[self refresh];
}

- (IBAction)ciaRunningAAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetStartFlagA:[self currentCIA] value:[c64 ciaGetStartFlagA:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"Start/Stop CIA timer A"];

	[self ciaSetStartFlagA:[self currentCIA] value:[sender intValue]];
}


- (void)ciaSetOneShotFlagA:(int)nr value:(bool)b
{
	[c64 ciaSetOneShotFlagA:nr value:b];
	[self refresh];
}

- (IBAction)ciaOneShotAAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetOneShotFlagA:[self currentCIA] value:[c64 ciaGetOneShotFlagA:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"CIA one shot flag A"];
	
	[self ciaSetOneShotFlagA:[self currentCIA] value:[sender intValue]];
}

- (void)ciaSetUnderflowFlagA:(int)nr value:(bool)b
{
	[c64 ciaSetUnderflowFlagA:nr value:b];
	[self refresh];
}

- (IBAction)ciaCountUnterflowsAAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetUnderflowFlagA:[self currentCIA] value:[c64 ciaGetUnderflowFlagA:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"CIA underflow flag A"];
	
	[self ciaSetUnderflowFlagA:[self currentCIA] value:[sender intValue]];
}

- (void)ciaSetPendingSignalFlagA:(int)nr value:(bool)b
{
	[c64 ciaSetPendingSignalFlagA:nr value:b];
	[self refresh];
}

- (IBAction)ciaSignalPendingAAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetPendingSignalFlagA:[self currentCIA] value:[c64 ciaGetPendingSignalFlagA:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"CIA signal pending A"];
	
	[self ciaSetPendingSignalFlagA:[self currentCIA] value:[sender intValue]];
}

- (void)ciaSetInterruptEnableFlagA:(int)nr value:(bool)b
{
	[c64 ciaSetInterruptEnableFlagA:nr value:b];
	[self refresh];
}

- (IBAction)ciaInterruptEnableAAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetInterruptEnableFlagA:[self currentCIA] value:[c64 ciaGetInterruptEnableFlagA:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"CIA interrupt enable flag A"];
	
	[self ciaSetInterruptEnableFlagA:[self currentCIA] value:[sender intValue]];
}

- (void)ciaSetDataPortB:(int)nr value:(uint8_t)v
{
	[c64 ciaSetDataPortB:nr value:v];
	[self refresh];
}

- (IBAction)ciaDataPortBAction:(id)sender 
{	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetDataPortB:[self currentCIA] value:[c64 ciaGetDataPortB:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"CIA data port B"];
	
	[self ciaSetDataPortB:[self currentCIA] value:[sender intValue]];
} 

- (void)ciaSetDataPortDirectionB:(int)nr value:(uint8_t)v
{
	[c64 ciaSetDataPortDirectionB:nr value:v];
	[self refresh];
}

- (IBAction)ciaDataPortDirectionBAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetDataPortDirectionB:[self currentCIA] value:[c64 ciaGetDataPortDirectionB:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"CIA data port direction B"];
	
	[self ciaSetDataPortDirectionB:[self currentCIA] value:[sender intValue]];
}

- (void)ciaSetTimerB:(int)nr value:(uint16_t)v
{
	[c64 ciaSetTimerB:nr value:v];
	[self refresh];
}

- (IBAction)ciaTimerBAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetTimerB:[self currentCIA] value:[c64 ciaGetTimerB:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"CIA timer B"];
	
	[self ciaSetTimerB:[self currentCIA] value:[sender intValue]];
}

- (void)ciaSetTimerLatchB:(int)nr value:(uint16_t)v
{
	[c64 ciaSetTimerLatchB:nr value:v];
	[self refresh];
}

- (IBAction)ciaLatchedTimerBAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetTimerLatchB:[self currentCIA] value:[c64 ciaGetTimerLatchB:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"CIA timer latch B"];
	
	[self ciaSetTimerLatchB:[self currentCIA] value:[sender intValue]];
}

- (void)ciaSetStartFlagB:(int)nr value:(bool)b
{
	[c64 ciaSetStartFlagB:nr value:b];
	[self refresh];
}

- (IBAction)ciaRunningBAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetStartFlagB:[self currentCIA] value:[c64 ciaGetStartFlagB:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"Start/Stop CIA timer B"];

	[self ciaSetStartFlagB:[self currentCIA] value:[sender intValue]];
}

- (void)ciaSetOneShotFlagB:(int)nr value:(bool)b
{
	[c64 ciaSetOneShotFlagB:nr value:b];
	[self refresh];
}

- (IBAction)ciaOneShotBAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetOneShotFlagB:[self currentCIA] value:[c64 ciaGetOneShotFlagB:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"CIA one shot flag B"];
	
	[self ciaSetOneShotFlagB:[self currentCIA] value:[sender intValue]];
}

- (void)ciaSetUnderflowFlagB:(int)nr value:(bool)b
{
	[c64 ciaSetUnderflowFlagB:nr value:b];
	[self refresh];
}

- (IBAction)ciaCountUnterflowsBAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetUnderflowFlagB:[self currentCIA] value:[c64 ciaGetUnderflowFlagB:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"CIA underflow flag B"];
	
	[self ciaSetUnderflowFlagB:[self currentCIA] value:[sender intValue]];
}

- (void)ciaSetPendingSignalFlagB:(int)nr value:(bool)b
{
	[c64 ciaSetPendingSignalFlagB:nr value:b];
	[self refresh];
}

- (IBAction)ciaSignalPendingBAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetPendingSignalFlagB:[self currentCIA] value:[c64 ciaGetPendingSignalFlagB:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"CIA signal pending B"];
	
	[self ciaSetPendingSignalFlagB:[self currentCIA] value:[sender intValue]];
}

- (void)ciaSetInterruptEnableFlagB:(int)nr value:(bool)b
{
	[c64 ciaSetInterruptEnableFlagB:nr value:b];
	[self refresh];
}

- (IBAction)ciaInterruptEnableBAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ciaSetInterruptEnableFlagB:[self currentCIA] value:[c64 ciaGetInterruptEnableFlagB:[self currentCIA]]];
	if (![undo isUndoing]) [undo setActionName:@"CIA interrupt enable flag B"];
	
	[self ciaSetInterruptEnableFlagB:[self currentCIA] value:[sender intValue]];
}

// TODO: IMPLEMENTATION
- (IBAction)todHoursAction:(id)sender { [self refresh];}

// TODO: IMPLEMENTATION
- (IBAction)todMinutesAction:(id)sender { [self refresh];}

// TODO: IMPLEMENTATION
- (IBAction)todSecondsAction:(id)sender { [self refresh];}

// TODO: IMPLEMENTATION
- (IBAction)todTenthAction:(id)sender { [self refresh];}

// TODO: IMPLEMENTATION
- (IBAction)alarmHoursAction:(id)sender { [self refresh];}

// TODO: IMPLEMENTATION
- (IBAction)alarmMinutesAction:(id)sender { [self refresh];}

// TODO: IMPLEMENTATION
- (IBAction)alarmSecondsAction:(id)sender { [self refresh];}

// TODO: IMPLEMENTATION
- (IBAction)alarmTenthAction:(id)sender { [self refresh];}

// TODO: IMPLEMENTATION
- (IBAction)todInterruptEnabledAction:(id)sender { [self refresh];}


// --------------------------------------------------------------------------------
// Action methods (VIC debug panel)
// --------------------------------------------------------------------------------

- (void)vicSetDisplayMode:(int)mode
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicSetDisplayMode:[c64 vicGetDisplayMode]];
	if (![undo isUndoing]) [undo setActionName:@"Display mode"];

	[c64 vicSetDisplayMode:mode];
	[self refresh];
}

- (IBAction)vicVideoModeAction:(id)sender
{
	[self vicSetDisplayMode:[[sender selectedItem] tag]];
}

- (void)vicSetScreenGeometry:(int)mode
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicSetScreenGeometry:[c64 vicGetScreenGeometry]];
	if (![undo isUndoing]) [undo setActionName:@"Screen geometry"];

	[c64 vicSetScreenGeometry:mode];
	[self refresh];
}

- (IBAction)vicScreenGeometryAction:(id)sender
{
	[self vicSetScreenGeometry:[[sender selectedItem] tag]];
}

- (void)vicSetMemoryBank:(int)addr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicSetMemoryBank:[c64 vicGetMemoryBankAddr]];
	if (![undo isUndoing]) [undo setActionName:@"Memory bank"];

	[c64 vicSetMemoryBankAddr:addr];
	[self refresh];
}

- (IBAction)vicMemoryBankAction:(id)sender
{
	[self vicSetMemoryBank:[[sender selectedItem] tag]];
}

- (void)vicSetScreenMemory:(int)addr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicSetScreenMemory:[c64 vicGetScreenMemoryAddr]];
	if (![undo isUndoing]) [undo setActionName:@"Screen memory"];

	[c64 vicSetScreenMemoryAddr:addr];
	[self refresh];
}

- (IBAction)vicScreenMemoryAction:(id)sender
{
	[self vicSetScreenMemory:[[sender selectedItem] tag]];
}

- (void)vicSetCharacterMemory:(int)addr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicSetCharacterMemory:[c64 vicGetCharacterMemoryAddr]];
	if (![undo isUndoing]) [undo setActionName:@"Character memory"];

	[c64 vicSetCharacterMemoryAddr:addr];
	[self refresh];
}

- (IBAction)vicCharacterMemoryAction:(id)sender
{
	[self vicSetCharacterMemory:[[sender selectedItem] tag]];
}

- (IBAction)vicDXAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicDXAction:[NSNumber numberWithInt:[c64 vicGetHorizontalRasterScroll]]];
	if (![undo isUndoing]) [undo setActionName:@"Horizontal raster scroll"];
	
	[c64 vicSetHorizontalRasterScroll:[sender intValue]];
	[self refresh];
}

- (IBAction)vicDYAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicDYAction:[NSNumber numberWithInt:[c64 vicGetVerticalRasterScroll]]];
	if (![undo isUndoing]) [undo setActionName:@"Vertical raster scroll"];
	
	[c64 vicSetVerticalRasterScroll:[sender intValue]];
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

	[c64 spriteToggleVisibilityFlag:nr];
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

	[c64 spriteToggleMulticolorFlag:nr];
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

	[c64 spriteToggleStretchXFlag:nr];
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

	[c64 spriteToggleStretchYFlag:nr];
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

	[c64 spriteToggleBackgroundPriorityFlag:nr];
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

	[c64 spriteToggleSpriteSpriteCollisionFlag:nr];
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

	[c64 spriteToggleSpriteBackgroundCollisionFlag:nr];
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
	[[undo prepareWithInvocationTarget:self] spriteSetX:nr value:[c64 spriteGetX:nr]];
	if (![undo isUndoing]) [undo setActionName:@"Sprite X"];

	[c64 spriteSetX:nr value:v];
	[self refresh];
}

- (IBAction)vicSpriteXAction:(id)sender
{
	[self spriteSetX:[self currentSprite] value:[sender intValue]];
}

- (void)spriteSetY:(int)nr value:(int)v
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteSetY:nr value:[c64 spriteGetY:nr]];
	if (![undo isUndoing]) [undo setActionName:@"Sprite Y"];

	[c64 spriteSetY:nr value:v];
	[self refresh];
}

- (IBAction)vicSpriteYAction:(id)sender
{
	[self spriteSetY:[self currentSprite] value:[sender intValue]];
}

- (void)spriteSetColor:(int)nr value:(int)v
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteSetColor:nr value:[c64 spriteGetColor:nr]];
	if (![undo isUndoing]) [undo setActionName:@"Sprite color"];

	[c64 spriteSetColor:nr value:v];
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
	[[undo prepareWithInvocationTarget:self] vicRasterlineAction:[NSNumber numberWithInt:[c64 vicGetRasterLine]]];
	if (![undo isUndoing]) [undo setActionName:@"Raster line"];
	
	[c64 vicSetRasterLine:[sender intValue]];
	[self refresh];
}

- (IBAction)vicEnableRasterInterruptAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicEnableRasterInterruptAction:sender];
	if (![undo isUndoing]) [undo setActionName:@"Raster interrupt"];
	
	[c64 vicToggleRasterInterruptFlag];
	[self refresh];
}

- (IBAction)vicRasterInterruptAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicRasterInterruptAction:[NSNumber numberWithInt:[c64 vicGetRasterInterruptLine]]];
	if (![undo isUndoing]) [undo setActionName:@"Raster interrupt line"];
	
	[c64 vicSetRasterInterruptLine:[sender intValue]];
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
	[a setIntValue:[c64 cpuGetA]];
	[x setIntValue:[c64 cpuGetX]];
	[y setIntValue:[c64 cpuGetY]];
	[pc setIntValue:[c64 cpuGetPC]];
	[sp setIntValue:[c64 cpuGetSP]];
	// [mhzField setObjectValue:[c64 cpuGetMHz]];
	[mhzField setFloatValue:mhz];
	
	[N setIntValue:[c64 cpuGetN]];
	[V setIntValue:[c64 cpuGetV]];
	[B setIntValue:[c64 cpuGetB]];
	[D setIntValue:[c64 cpuGetD]];
	[I setIntValue:[c64 cpuGetI]];
	[Z setIntValue:[c64 cpuGetZ]];
	[C setIntValue:[c64 cpuGetC]];	
}

- (void)refreshMemory
{
	uint16_t addr = [addr_search intValue];
	switch ([c64 memGetWatchpointType:addr]) {
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
	[watchValField setIntValue:[c64 memGetWatchValue:addr]];	
}

- (void)refreshCIA
{
	[ciaDataPortA setIntValue:[c64 ciaGetDataPortA:[self currentCIA]]];
	[ciaDataPortDirectionA setIntValue:[c64 ciaGetDataPortDirectionA:[self currentCIA]]];
	[ciaTimerA setIntValue:[c64 ciaGetTimerA:[self currentCIA]]];
	[ciaLatchedTimerA setIntValue:[c64 ciaGetTimerLatchA:[self currentCIA]]];
	[ciaRunningA setIntValue:[c64 ciaGetStartFlagA:[self currentCIA]]];
	[ciaOneShotA setIntValue:[c64 ciaGetOneShotFlagA:[self currentCIA]]];
	[ciaSignalPendingA setIntValue:[c64 ciaGetPendingSignalFlagA:[self currentCIA]]];
	[ciaInterruptEnableA setIntValue:[c64 ciaGetInterruptEnableFlagA:[self currentCIA]]];
	
	[ciaDataPortB setIntValue:[c64 ciaGetDataPortB:[self currentCIA]]];
	[ciaDataPortDirectionB setIntValue:[c64 ciaGetDataPortDirectionB:[self currentCIA]]];
	[ciaTimerB setIntValue:[c64 ciaGetTimerB:[self currentCIA]]];
	[ciaLatchedTimerB setIntValue:[c64 ciaGetTimerLatchB:[self currentCIA]]];
	[ciaRunningB setIntValue:[c64 ciaGetStartFlagB:[self currentCIA]]];
	[ciaOneShotB setIntValue:[c64 ciaGetOneShotFlagB:[self currentCIA]]];
	[ciaSignalPendingB setIntValue:[c64 ciaGetPendingSignalFlagB:[self currentCIA]]];
	[ciaInterruptEnableB setIntValue:[c64 ciaGetInterruptEnableFlagB:[self currentCIA]]];
	
	[todHours setIntValue:[c64 ciaGetTodHours:[self currentCIA]]];
	[todMinutes setIntValue:[c64 ciaGetTodMinutes:[self currentCIA]]];
	[todSeconds setIntValue:[c64 ciaGetTodSeconds:[self currentCIA]]];
	[todTenth setIntValue:[c64 ciaGetTodTenth:[self currentCIA]]];
	
	[alarmHours setIntValue:[c64 ciaGetAlarmHours:[self currentCIA]]];
	[alarmMinutes setIntValue:[c64 ciaGetAlarmMinutes:[self currentCIA]]];
	[alarmSeconds setIntValue:[c64 ciaGetAlarmSeconds:[self currentCIA]]];
	[alarmTenth setIntValue:[c64 ciaGetAlarmTenth:[self currentCIA]]];	
}

- (void)refreshVIC
{
	if (![VicVideoMode selectItemWithTag:[c64 vicGetDisplayMode]])
		[VicVideoMode selectItemWithTag:1];
	if (![VicScreenGeometry selectItemWithTag:[c64 vicGetScreenGeometry]])
		NSLog(@"Can't refresh screen geometry field");
	if (![VicMemoryBank selectItemWithTag:[c64 vicGetMemoryBankAddr]])
		NSLog(@"Can't refresh memory bank field");
	if (![VicScreenMemory selectItemWithTag:[c64 vicGetScreenMemoryAddr]])
		NSLog(@"Can't refresh screen memory field");
	if (![VicCharacterMemory selectItemWithTag:[c64 vicGetCharacterMemoryAddr]])
		NSLog(@"Can't refresh screen memory field");
	[VicDX setIntValue:[c64 vicGetHorizontalRasterScroll]];
	[VicDXStepper setIntValue:[c64 vicGetHorizontalRasterScroll]];
	[VicDY setIntValue:[c64 vicGetVerticalRasterScroll]];
	[VicDYStepper setIntValue:[c64 vicGetVerticalRasterScroll]];
	[VicSpriteActive setIntValue:[c64 spriteGetVisibilityFlag:[self currentSprite]]];
	[VicSpriteMulticolor setIntValue:[c64 spriteGetMulticolorFlag:[self currentSprite]]];
	[VicSpriteStretchX setIntValue:[c64 spriteGetStretchXFlag:[self currentSprite]]];
	[VicSpriteStretchY setIntValue:[c64 spriteGetStretchYFlag:[self currentSprite]]];
	[VicSpriteInFront setIntValue:[c64 spriteGetBackgroundPriorityFlag:[self currentSprite]]];
	[VicSpriteSpriteCollision setIntValue:[c64 spriteGetSpriteSpriteCollisionFlag:[self currentSprite]]];
	[VicSpriteBackgroundCollision setIntValue:[c64 spriteGetSpriteBackgroundCollisionFlag:[self currentSprite]]];
	[VicSpriteX setIntValue:[c64 spriteGetX:[self currentSprite]]];
	[VicSpriteY setIntValue:[c64 spriteGetY:[self currentSprite]]];
	[VicSpriteColor setIntValue:[c64 spriteGetColor:[self currentSprite]]];
	[VicRasterline setIntValue:[c64 vicGetRasterLine]];
	[VicEnableRasterInterrupt setIntValue:[c64 vicGetRasterInterruptFlag]];
	[VicRasterInterrupt setIntValue:[c64 vicGetRasterInterruptLine]];
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
	uint16_t address = [c64 cpuGetPC];
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
		// Memory panel
		watchValField,
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
		[ByteFormatterControls[i] setFormatter:byteFormatter];
		[ByteFormatterControls[i] setNeedsDisplay];
	}

	for (int i = 0; WordFormatterControls[i] != NULL; i++) {
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
		// N, Z, C, I, B, D, V,
		// Memory panel
		watchValField,
		// CIA panel
		ciaDataPortA, ciaDataPortDirectionA, ciaTimerA, ciaLatchedTimerA, 
		// ciaRunningA, ciaOneShotA, ciaCountUnderflowsA, ciaSignalPendingA, ciaInterruptEnableA,
		ciaDataPortB, ciaDataPortDirectionB, ciaTimerB, ciaLatchedTimerB, 
		// ciaRunningB, ciaOneShotB, ciaCountUnderflowsB, ciaSignalPendingB, ciaInterruptEnableB,
		todHours, todMinutes, todSeconds, todTenth,
		alarmHours, alarmMinutes, alarmSeconds, alarmTenth,
		// VIC panel
		VicSpriteX, VicSpriteY, VicSpriteColor, VicRasterline, VicRasterInterrupt, VicDX, VicDY,
		NULL };
	
	// Enable / disable controls
	for (int i = 0;; i++) {
		if (controls[i] == NULL) break;
		// NSLog(@"Editing for control %@ %@", controls[i], enabled ? @"enabled" : @"disabled");
		[controls[i] setEnabled:enabled];
	}

	// Enable / disable table columns
	[[memTableView tableColumnWithIdentifier:@"hex0"] setEditable:enabled];
	[[memTableView tableColumnWithIdentifier:@"hex1"] setEditable:enabled];
	[[memTableView tableColumnWithIdentifier:@"hex2"] setEditable:enabled];
	[[memTableView tableColumnWithIdentifier:@"hex3"] setEditable:enabled];
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
	uint16_t addr = [c64 cpuGetAddressOfNextIthInstruction:row from:disassembleStartAddr];
	uint8_t length = [c64 cpuGetLengthOfInstruction:[c64 memPeek:addr]];
	
	if ([[aTableColumn identifier] isEqual:@"addr"]) 
		return [NSNumber numberWithInt:addr];
	else if ([[aTableColumn identifier] isEqual:@"data01"]) 
		return (length > 0 ? [NSNumber numberWithInt:[c64 memPeek:addr]] : nil);
	else if ([[aTableColumn identifier] isEqual:@"data02"]) 
		return (length > 1 ? [NSNumber numberWithInt:[c64 memPeek:(addr+1)]] : nil);
	else if ([[aTableColumn identifier] isEqual:@"data03"]) 
		return (length > 2 ? [NSNumber numberWithInt:[c64 memPeek:(addr+2)]] : nil);
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
		if (![c64 memIsValidAddr:addr memtype:[self currentMemSource]])
			return nil;
		else
			return [NSString stringWithFormat:@"%c%c%c%c", 
				[c64 memPeekFrom:(addr+0) memtype:[self currentMemSource]],
				[c64 memPeekFrom:(addr+1) memtype:[self currentMemSource]],
				[c64 memPeekFrom:(addr+2) memtype:[self currentMemSource]],
				[c64 memPeekFrom:(addr+3) memtype:[self currentMemSource]]];
	}
	
	// One of the hexadecimal columns...
	if ([id isEqual:@"hex1"]) addr += 1;
	if ([id isEqual:@"hex2"]) addr += 2;
	if ([id isEqual:@"hex3"]) addr += 3;
	
	if (![c64 memIsValidAddr:addr memtype:[self currentMemSource]])
		return nil;
	
	return [NSNumber numberWithInt:[c64 memPeekFrom:addr memtype:[self currentMemSource]]];
	
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
	[[undo prepareWithInvocationTarget:self] changeMemValue:addr value:[c64 memPeekFrom:addr memtype:t] memtype:t];
	if (![undo isUndoing]) [undo setActionName:@"Memory contents"];
				
	[c64 memPokeTo:addr value:v memtype:t];
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

	uint8_t oldValue = [c64 memPeekFrom:addr memtype:[self currentMemSource]];
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
		
		uint16_t addr = [c64 cpuGetAddressOfNextIthInstruction:row from:disassembleStartAddr];
		if ([c64 cpuGetBreakpoint:addr] == CPU::HARD_BREAKPOINT) {
			[aCell setTextColor:[NSColor redColor]];
		} else {
			[aCell setTextColor:[NSColor blackColor]];
		}
	} else if (aTableView == memTableView) {
		if ([[aTableColumn identifier] isEqual:@"hex0"])
			[aCell setTextColor:([c64 memGetWatchpointType:(0+4*row)] != Memory::NO_WATCHPOINT ? [NSColor redColor] : [NSColor blackColor])];
		else if ([[aTableColumn identifier] isEqual:@"hex1"])
			[aCell setTextColor:([c64 memGetWatchpointType:(1+4*row)] != Memory::NO_WATCHPOINT ? [NSColor redColor] : [NSColor blackColor])];
		else if ([[aTableColumn identifier] isEqual:@"hex2"])
			[aCell setTextColor:([c64 memGetWatchpointType:(2+4*row)] != Memory::NO_WATCHPOINT ? [NSColor redColor] : [NSColor blackColor])];
		else if ([[aTableColumn identifier] isEqual:@"hex3"])
			[aCell setTextColor:([c64 memGetWatchpointType:(3+4*row)] != Memory::NO_WATCHPOINT ? [NSColor redColor] : [NSColor blackColor])];		
	}
}

// --------------------------------------------------------------------------------
// Mount dialog
// --------------------------------------------------------------------------------

- (BOOL)showMountDialog:(Archive *)archive
{
	[mountDialog initialize:archive];
	
	[NSApp beginSheet:mountDialog
		modalForWindow:theWindow
		modalDelegate:self
		didEndSelector:NULL
		contextInfo:NULL];
		
	return true;
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
	C64 *myc64 = [c64 getC64];

	// Rotate C64 screen
	[screen rotate];
	
	// Hide sheet
	[mountDialog orderOut:sender];

	// Return to normal event handling
	[NSApp endSheet:mountDialog returnCode:1];

	myc64->mountArchive();
}

- (IBAction)endMountDialogAndFlash:(id)sender
{
	C64 *myc64 = [c64 getC64];

	// Rotate C64 screen
	[screen rotate];
	
	// Hide sheet
	[mountDialog orderOut:sender];

	// Return to normal event handling
	[NSApp endSheet:mountDialog returnCode:1];

	// Load clean image and flash file into memory
	myc64->fastReset();
	myc64->flushArchive([mountDialog getSelectedFile]);

	// Type "RUN"
	myc64->keyboard->typeRun();
}


// --------------------------------------------------------------------------------
// Helper functions
// --------------------------------------------------------------------------------

- (int)currentSprite
{
	if ([sprite0 intValue]) return 0;
	if ([sprite1 intValue]) return 1;
	if ([sprite2 intValue]) return 2;
	if ([sprite3 intValue]) return 3;
	if ([sprite4 intValue]) return 4;
	if ([sprite5 intValue]) return 5;
	if ([sprite6 intValue]) return 6;
	if ([sprite7 intValue]) return 7;	
	assert(false);
}

- (Memory::MemoryType)currentMemSource
{
	if ([ramSource intValue]) return Memory::MEM_RAM;
	if ([romSource intValue]) return Memory::MEM_ROM;
	if ([ioSource intValue]) return Memory::MEM_IO;
	assert(false);
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
		currentAddr += [c64 cpuGetLengthOfInstruction:[c64 memPeek:currentAddr]];
	}
	return NO;
}

@end
