/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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

#ifndef INC_MYDOCUMENT
#define INC_MYDOCUMENT

#import <Cocoa/Cocoa.h>
#import "C64.h"
#import "HexFormatter.h"
#import "DezFormatter.h"
#import "Disassembler.h"
#import "VICScreen.h"
#import "MountDialog.h"
#import "PreferenceController.h"
#import "ConsoleController.h"
#import "C64Proxy.h"

@interface MyDocument : NSDocument
{
	IBOutlet id theWindow;
	
	// Toolbar
	NSMutableDictionary *toolbarItems;
	
	// Dialogs
	IBOutlet MountDialog *mountDialog;
	PreferenceController *preferenceController;
	ConsoleController *consoleController;
	
	// Main screen
	IBOutlet VICScreen *screen;
	IBOutlet NSDrawer *debug_panel;
	IBOutlet NSButton *eject;
	IBOutlet NSButton *drive;
	IBOutlet NSButton *greenLED;
	IBOutlet NSButton *redLED;
	// IBOutlet NSProgressIndicator *driveBusy;
	// IBOutlet NSTextField *driveProgress;
	IBOutlet NSTextField *info;
	IBOutlet NSTextField *clockSpeed;
	IBOutlet NSLevelIndicator *clockSpeedBar;
	IBOutlet NSStepper *clockSpeedStepper;
	IBOutlet NSButton *warpMode;

	// Debug panel (common)
	IBOutlet NSMatrix *dezHexSelector;
	
	// Debug panel (CPU)
	IBOutlet NSTextField *mhzField;
	IBOutlet NSTableView *cpuTableView;
	IBOutlet NSTextField *pc;
	IBOutlet NSTextField *sp;
	IBOutlet NSTextField *a;
	IBOutlet NSTextField *x;
	IBOutlet NSTextField *y;
	IBOutlet NSButton *N;
	IBOutlet NSButton *Z;
	IBOutlet NSButton *C;
	IBOutlet NSButton *I;
	IBOutlet NSButton *B;
	IBOutlet NSButton *D;
	IBOutlet NSButton *V;
	IBOutlet NSTextField *breakpoint;

	// Debug panel (Memory)
	IBOutlet NSTableView *memTableView;
	IBOutlet NSSearchField *addr_search;	
	IBOutlet NSMatrix *watchMode;
	IBOutlet NSTextField *watchValField;
	
	// Debug pabel (CIA)
	IBOutlet NSPopUpButton *ciaSelector;
	
	IBOutlet NSTextField *ciaDataPortA;
	IBOutlet NSTextField *ciaDataPortDirectionA;
	IBOutlet NSTextField *ciaTimerA;
	IBOutlet NSTextField *ciaLatchedTimerA;
	IBOutlet NSButton *ciaRunningA;
	IBOutlet NSButton *ciaOneShotA;
	IBOutlet NSButton *ciaCountUnterflowsA;
	IBOutlet NSButton *ciaSignalPendingA;
	IBOutlet NSButton *ciaInterruptEnableA;

	IBOutlet NSTextField *ciaDataPortB;
	IBOutlet NSTextField *ciaDataPortDirectionB;
	IBOutlet NSTextField *ciaTimerB;
	IBOutlet NSTextField *ciaLatchedTimerB;
	IBOutlet NSButton *ciaRunningB;
	IBOutlet NSButton *ciaOneShotB;
	IBOutlet NSButton *ciaCountUnterflowsB;
	IBOutlet NSButton *ciaSignalPendingB;
	IBOutlet NSButton *ciaInterruptEnableB;
	
	IBOutlet NSTextField *todHours;
	IBOutlet NSTextField *todMinutes;
	IBOutlet NSTextField *todSeconds;
	IBOutlet NSTextField *todTenth;

	IBOutlet NSTextField *alarmHours;
	IBOutlet NSTextField *alarmMinutes;
	IBOutlet NSTextField *alarmSeconds;
	IBOutlet NSTextField *alarmTenth;
	IBOutlet NSButton *todInterruptEnabled;
		
	// Debug pabel (VIC)	
	IBOutlet NSPopUpButton *VicVideoMode;
	IBOutlet NSPopUpButton *VicScreenGeometry;
	IBOutlet NSPopUpButton *VicMemoryBank;
	IBOutlet NSPopUpButton *VicScreenMemory;
	IBOutlet NSPopUpButton *VicCharacterMemory;
	IBOutlet NSTextField *VicDX;
	IBOutlet NSStepper *VicDXStepper;
	IBOutlet NSTextField *VicDY;
	IBOutlet NSStepper *VicDYStepper;
	IBOutlet NSButton *VicSpriteActive;
	IBOutlet NSButton *VicSpriteMulticolor;
	IBOutlet NSButton *VicSpriteStretchX;
	IBOutlet NSButton *VicSpriteStretchY;
	IBOutlet NSButton *VicSpriteInFront;
	IBOutlet NSButton *VicSpriteSpriteCollision;
	IBOutlet NSButton *VicSpriteBackgroundCollision;
	IBOutlet NSTextField *VicSpriteX;
	IBOutlet NSTextField *VicSpriteY;
	IBOutlet NSTextField *VicSpriteColor;
	IBOutlet NSTextField *VicRasterline;
	IBOutlet NSButton *VicEnableRasterInterrupt;
	IBOutlet NSTextField *VicRasterInterrupt;
	IBOutlet NSButton *VicEnableOpenGL;
	
	// Connected components
	C64Proxy *c64;
	
	// The 60 Hz timer
	NSTimer *timer;
	long cycleCount, frameCount, timeStamp, animationCounter; // Used in timerFunc
	uint16_t disassembleStartAddr; // Address of the first disassembled instruction in the CPU Debug window
	bool enableOpenGL;
	bool warpLoad;
	bool redLight, greenLight, updateLight;
	NSString *infoString;
	BOOL needsRefresh; // If set to true, the GUI will refresh
	
	// Peek and poke callbacks
	Memory::MemoryType memsource;
		
	// The currently selected CIA chip (1=cia1 or 2=cia2)
	int currentCIA;
	
	// The currently selected sprite (0 .. 7)
	int selectedSprite;
}

// Toolbar delegate methods
- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar itemForItemIdentifier:(NSString *)itemIdentifier willBeInsertedIntoToolbar:(BOOL)flag;    
- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar*)toolbar;
- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar*)toolbar;

// Main screen
- (IBAction)debugAction:(id)sender;
- (IBAction)stepperAction:(id)sender;
- (IBAction)warpAction:(id)sender;
- (IBAction)ejectAction:(id)sender;
- (IBAction)driveAction:(id)sender;

// Debug panel (common)
- (IBAction)setDecimal:(id)sender;
- (IBAction)setHexadecimal:(id)sender;
- (IBAction)pauseAction:(id)sender;
- (IBAction)stepIntoAction:(id)sender;
- (IBAction)stepOverAction:(id)sender;
- (IBAction)stopAndGoAction:(id)sender;
- (IBAction)continueAction:(id)sender;
- (IBAction)resetAction:(id)sender;
- (IBAction)drawSpritesAction:(id)sender;
- (IBAction)markIRQLinesAction:(id)sender;
// - (IBAction)loggingAction:(id)sender;
- (IBAction)showPreferencesAction:(id)sender;

// Keyboard actions
- (IBAction)runstopRestoreAction:(id)sender;
- (IBAction)runstopAction:(id)sender;
- (IBAction)commodoreKeyAction:(id)sender;

// Debug panel (CPU)
- (IBAction)mhzAction:(id)sender;
- (IBAction)pcAction:(id)sender;
- (IBAction)spAction:(id)sender;
- (IBAction)aAction:(id)sender;
- (IBAction)xAction:(id)sender;
- (IBAction)yAction:(id)sender;
- (IBAction)NAction:(id)sender;
- (IBAction)ZAction:(id)sender;
- (IBAction)CAction:(id)sender;
- (IBAction)IAction:(id)sender;
- (IBAction)BAction:(id)sender;
- (IBAction)DAction:(id)sender;
- (IBAction)VAction:(id)sender;
- (IBAction)setHardBreakpoint:(id)sender;
- (void)doubleClickInCpuTable:(id)sender;
- (void)doubleClickInMemTable:(id)sender;

// Debug panel (Memory)
- (IBAction)searchAction:(id)sender;
- (IBAction)setMemSourceToRAM:(id)sender;
- (IBAction)setMemSourceToROM:(id)sender;
- (IBAction)setMemSourceToIO:(id)sender;
- (IBAction)setWatchForNone:(id)sender;
- (IBAction)setWatchForAll:(id)sender;
- (IBAction)setWatchForValue:(id)sender;
- (IBAction)setWatchValue:(id)sender;

// Debug pabel (CIA)
- (IBAction)ciaSelectCiaAction:(id)sender;

- (IBAction)ciaDataPortAAction:(id)sender;
- (IBAction)ciaDataPortDirectionAAction:(id)sender;
- (IBAction)ciaTimerAAction:(id)sender;
- (IBAction)ciaLatchedTimerAAction:(id)sender;
- (IBAction)ciaRunningAAction:(id)sender;
- (IBAction)ciaOneShotAAction:(id)sender;
- (IBAction)ciaCountUnterflowsAAction:(id)sender;
- (IBAction)ciaSignalPendingAAction:(id)sender;
- (IBAction)ciaInterruptEnableAAction:(id)sender;

- (IBAction)ciaDataPortBAction:(id)sender;
- (IBAction)ciaDataPortDirectionBAction:(id)sender;
- (IBAction)ciaTimerBAction:(id)sender;
- (IBAction)ciaLatchedTimerBAction:(id)sender;
- (IBAction)ciaRunningBAction:(id)sender;
- (IBAction)ciaOneShotBAction:(id)sender;
- (IBAction)ciaCountUnterflowsBAction:(id)sender;
- (IBAction)ciaSignalPendingBAction:(id)sender;
- (IBAction)ciaInterruptEnableBAction:(id)sender;

- (IBAction)todHoursAction:(id)sender;
- (IBAction)todMinutesAction:(id)sender;
- (IBAction)todSecondsAction:(id)sender;
- (IBAction)todTenthAction:(id)sender;

- (IBAction)alarmHoursAction:(id)sender;
- (IBAction)alarmMinutesAction:(id)sender;
- (IBAction)alarmSecondsAction:(id)sender;
- (IBAction)alarmTenthAction:(id)sender;
- (IBAction)todInterruptEnabledAction:(id)sender;


// Debug panel (VIC)
- (IBAction)vicVideoModeAction:(id)sender;
- (IBAction)vicScreenGeometryAction:(id)sender;
- (IBAction)vicMemoryBankAction:(id)sender;
- (IBAction)vicScreenMemoryAction:(id)sender;
- (IBAction)vicCharacterMemoryAction:(id)sender;
- (IBAction)vicDXAction:(id)sender;
- (IBAction)vicDYAction:(id)sender;
- (IBAction)vicDXStepperAction:(id)sender;
- (IBAction)vicDYStepperAction:(id)sender;
- (IBAction)vicSpriteActiveAction:(id)sender;
- (IBAction)vicSpriteMulticolorAction:(id)sender;
- (IBAction)vicSpriteStretchXAction:(id)sender;
- (IBAction)vicSpriteStretchYAction:(id)sender;
- (IBAction)vicSpriteInFrontAction:(id)sender;
- (IBAction)vicSpriteSpriteCollisionAction:(id)sender;
- (IBAction)vicSpriteBackgroundCollisionAction:(id)sender;
- (IBAction)vicSpriteXAction:(id)sender;
- (IBAction)vicSpriteYAction:(id)sender;
// - (IBAction)vicSpriteDataAction:(id)sender;
- (IBAction)vicSpriteColorAction:(id)sender;
- (IBAction)vicRasterlineAction:(id)sender;
- (IBAction)vicEnableRasterInterruptAction:(id)sender;
- (IBAction)vicRasterInterruptAction:(id)sender;
- (IBAction)vicEnableOpenGL:(id)sender;
- (IBAction)vicSelectSprite0:(id)sender;
- (IBAction)vicSelectSprite1:(id)sender;
- (IBAction)vicSelectSprite2:(id)sender;
- (IBAction)vicSelectSprite3:(id)sender;
- (IBAction)vicSelectSprite4:(id)sender;
- (IBAction)vicSelectSprite5:(id)sender;
- (IBAction)vicSelectSprite6:(id)sender;
- (IBAction)vicSelectSprite7:(id)sender;
//- (IBAction)pressPlayOnTapeAction:(id)sender;
//- (IBAction)rewindTapeAction:(id)sender;
//- (IBAction)ejectTapeAction:(id)sender;
//- (IBAction)connectDriveAction:(id)sender;
//- (IBAction)disconnectDriveAction:(id)sender;

- (IBAction)cancelMountDialog:(id)sender;
- (IBAction)endMountDialogAndMount:(id)sender;
- (IBAction)endMountDialogAndFlash:(id)sender;

// Helper functions for action methods
// - (void)toggleBreakpointAtAddr:(uint16_t)addr;
- (void)changeMemValue:(uint16_t)addr value:(int16_t)v memtype:(Memory::MemoryType)t;

// Listener functions (called by the CPU to inform the GUI about important changes)
//- (void) drawAction;
- (void) runAction;
- (void) haltAction;
- (void) okAction;
- (void) breakpointAction;
- (void) watchpointAction;
- (void) illegalInstructionAction;
- (void) missingRomAction;
- (void) connectDriveAction;
- (void) insertDiskAction;
- (void) ejectDiskAction;
- (void) disconnectDriveAction;
- (void) startDiskAction;
- (void) stopDiskAction;
- (void) startWarpAction;
- (void) stopWarpAction;	
- (void) logAction:(char *)message;	

// Table views handling
- (int)numberOfRowsInTableView:(NSTableView *)aTableView;

// Getter
- (id)objectValueForCpuTableColumn:(NSTableColumn *)aTableColumn row:(int)row;
- (id)objectValueForMemTableColumn:(NSTableColumn *)aTableColumn row:(int)row;
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)row;

// Setter
- (void)setMemObjectValue:(id)anObject forTableColumn:(NSTableColumn *)aTableColumn row:(int)rowIndex;
- (void)tableView:(NSTableView *)aTableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn *)aTableColumn row:(int)rowIndex;

// Delegate
- (void)tableView:(NSTableView *)aTableView willDisplayCell: (id)aCell forTableColumn: (NSTableColumn *)aTableColumn row: (int)aRowIndex;

// End table view handling

// Refresh methods
- (void)loadUserDefaults;
- (void)refreshCPU;
- (void)refreshMemory;
- (void)refreshCIA;
- (void)refreshVIC;
- (void)refreshSID;
- (void)refresh;
- (void)refresh:(NSFormatter *)byteFormatter word:(NSFormatter *)wordFormatter disassembler:(NSFormatter *)disassembler;

// Enable / disable editing
- (void)enableUserEditing:(BOOL)enabled;

// Misc
// - (uint16_t)nextInstructionAddr:(int)i from:(uint16_t)addr;
// - (uint16_t)nextInstructionAddr:(int)i;
- (BOOL)computeRowForAddr:(uint16_t)addr maxRows:(uint16_t)maxRows row:(uint16_t *)row;

- (BOOL)showMountDialog:(Archive *)archive;

@end

#endif

