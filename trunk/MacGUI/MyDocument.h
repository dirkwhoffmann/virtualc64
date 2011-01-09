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
#import <OpenGL/OpenGL.h>
#import <OpenGL/glu.h>
#import "C64.h"
#import "Formatter.h"
#import "Disassembler.h"
#import "VICScreen.h"
#import "MountDialog.h"
#import "RomDialog.h"
#import "PreferenceController.h"
#import "AudioDevice.h"
#import "C64Proxy.h"
#import "JoystickManager.h"
#import "TimeTravelTableView.h"

@interface MyDocument : NSDocument
{
	// Proxy object. Used get data from and sent data to the virtual C64
	// Moreover, the class implements a bridge between C++ (simulator) and Objective-C (GUI)
	C64Proxy *c64;
	
	// JoystickManager
	JoystickManager *joystickManager;
	
	// Dialogs
	IBOutlet MountDialog *mountDialog;
	IBOutlet RomDialog *romDialog;
	PreferenceController *preferenceController;
	
	// Main screen
	IBOutlet VICScreen *screen;
	IBOutlet NSDrawer *debug_panel;
	IBOutlet NSDrawer *backInTime_panel;
	IBOutlet NSButton *drive;
	IBOutlet NSButton *eject;
	IBOutlet NSButton *cartridgeIcon;
	IBOutlet NSButton *cartridgeEject;
	IBOutlet NSButton *greenLED;
	IBOutlet NSButton *redLED;
	IBOutlet NSProgressIndicator *driveBusy;
	IBOutlet NSTextField *info;
	IBOutlet NSTextField *clockSpeed;
	IBOutlet NSLevelIndicator *clockSpeedBar;
	IBOutlet NSStepper *clockSpeedStepper;
	IBOutlet NSButton *warpMode;

	// Time travel panel (cheatbox)
	IBOutlet TimeTravelTableView *ttTableView;
	IBOutlet NSButton *revertToSnapshot;
	IBOutlet NSTextField *historyDateField1;
	IBOutlet NSTextField *historyDateField2;
	
	// Debug panel (common)
	IBOutlet NSMatrix *dezHexSelector;
	
	// Debug panel (CPU)
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
	IBOutlet NSButton *stopAndGoButton;
	IBOutlet NSButton *stepIntoButton;
	IBOutlet NSButton *stepOverButton;
	IBOutlet NSButton *stepOutButton;
	IBOutlet NSTextField *breakpoint;

	// Debug panel (Memory)
	IBOutlet NSTableView *memTableView;
	IBOutlet NSSearchField *addr_search;	
	IBOutlet NSButtonCell *ramSource;
	IBOutlet NSButtonCell *romSource;
	IBOutlet NSButtonCell *ioSource;	
	
	// Debug pabel (CIA)
	IBOutlet NSPopUpButton *ciaSelector;
	
	IBOutlet NSTextField *ciaDataPortA;
	IBOutlet NSTextField *ciaDataPortDirectionA;
	IBOutlet NSTextField *ciaTimerA;
	IBOutlet NSTextField *ciaLatchedTimerA;
	IBOutlet NSButton *ciaRunningA;
	IBOutlet NSButton *ciaOneShotA;
	IBOutlet NSButton *ciaCountUnderflowsA;
	IBOutlet NSButton *ciaSignalPendingA;
	IBOutlet NSButton *ciaInterruptEnableA;

	IBOutlet NSTextField *ciaDataPortB;
	IBOutlet NSTextField *ciaDataPortDirectionB;
	IBOutlet NSTextField *ciaTimerB;
	IBOutlet NSTextField *ciaLatchedTimerB;
	IBOutlet NSButton *ciaRunningB;
	IBOutlet NSButton *ciaOneShotB;
	IBOutlet NSButton *ciaCountUnderflowsB;
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
	IBOutlet NSMatrix *spriteSelector;
	IBOutlet NSButtonCell *sprite0; 	
	IBOutlet NSButtonCell *sprite1;
	IBOutlet NSButtonCell *sprite2; 	
	IBOutlet NSButtonCell *sprite3;
	IBOutlet NSButtonCell *sprite4; 	
	IBOutlet NSButtonCell *sprite5;
	IBOutlet NSButtonCell *sprite6;
	IBOutlet NSButtonCell *sprite7;
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
		
	// True iff file transfer should be performed in warp mode
	bool warpLoad;
	
	// True iff the emulator should always run in warp mode
	bool alwaysWarp;
	
	// The 60 Hz timer
	NSTimer *timer;
	
	// Timer lock
	NSLock *timerLock;
	
	// Measured clock frequency
	float mhz;

	// The following variables are needed to calculate the clock frequency
	long cycleCount, frameCount, timeStamp, animationCounter; 

	// Address of the first disassembled instruction in the CPU Debug window
	uint16_t disassembleStartAddr; 	
	
	// Selected sprite in VIC debug panel
	int selectedSprite;
		
	//! Reference to the attached D64 archive
	D64Archive *archive;

	//! Reference to the attached cartridge
	Cartridge *cartridge;

	//! Initial snapshot
	//! If unequal NULL, the newly created document will be initialized with the provided snapshot data
	Snapshot *snapshot;	
}

@property bool warpLoad;
@property bool alwaysWarp;
@property C64Proxy *c64;
@property D64Archive *archive;
@property Cartridge *cartridge;

- (BOOL)setArchiveWithName:(NSString *)path;
- (BOOL)attachCartridge:(NSString *)path;

// Main screen
- (IBAction)fullscreenAction:(id)sender;
- (IBAction)debugAction:(id)sender;
- (IBAction)stepperAction:(id)sender;
- (IBAction)warpAction:(id)sender;
- (IBAction)ejectAction:(id)sender;
- (IBAction)driveAction:(id)sender;
- (IBAction)cartridgeEjectAction:(id)sender;

// Timetravel panel
- (void)updateTimeTravelInfoText:(NSString *)s1 secondText:(NSString *)s2;
- (IBAction)timeTravelAction:(id)sender;

// Debug menu
- (IBAction)hideSpritesAction:(id)sender;
- (IBAction)markIRQLinesAction:(id)sender;
- (IBAction)markDMALinesAction:(id)sender;

- (IBAction)traceC64CpuAction:(id)sender;
- (IBAction)traceIecAction:(id)sender;
- (IBAction)traceVC1541CpuAction:(id)sender;
- (IBAction)traceViaAction:(id)sender;

- (IBAction)dumpC64:(id)sender;
- (IBAction)dumpC64CPU:(id)sender;
- (IBAction)dumpC64CIA1:(id)sender;
- (IBAction)dumpC64CIA2:(id)sender;
- (IBAction)dumpC64VIC:(id)sender;
- (IBAction)dumpC64SID:(id)sender;
- (IBAction)dumpC64Memory:(id)sender;
- (IBAction)dumpVC1541:(id)sender;
- (IBAction)dumpVC1541CPU:(id)sender;
- (IBAction)dumpVC1541VIA1:(id)sender;
- (IBAction)dumpVC1541VIA2:(id)sender;
- (IBAction)dumpVC1541Memory:(id)sender;
- (IBAction)dumpKeyboard:(id)sender;
- (IBAction)dumpIEC:(id)sender;

// Debug panel (common)
- (IBAction)setDecimalAction:(id)sender;
- (IBAction)setHexadecimalAction:(id)sender;
- (IBAction)stepIntoAction:(id)sender;
- (IBAction)stepOutAction:(id)sender;
- (IBAction)stepOverAction:(id)sender;
- (IBAction)stopAndGoAction:(id)sender;
- (IBAction)pauseAction:(id)sender;
- (IBAction)continueAction:(id)sender;
- (IBAction)resetAction:(id)sender;
- (IBAction)fastResetAction:(id)sender;
- (IBAction)showPreferencesAction:(id)sender;

// Keyboard actions
- (IBAction)runstopRestoreAction:(id)sender;
- (IBAction)runstopAction:(id)sender;
- (IBAction)commodoreKeyAction:(id)sender;
- (IBAction)FormatDiskAction:(id)sender;

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
- (IBAction)setHardBreakpointAction:(id)sender;
- (void)doubleClickInCpuTable:(id)sender;
- (void)doubleClickInMemTable:(id)sender;

// Debug panel (Memory)
- (IBAction)searchAction:(id)sender;
- (IBAction)setMemSourceToRAM:(id)sender;
- (IBAction)setMemSourceToROM:(id)sender;
- (IBAction)setMemSourceToIO:(id)sender;

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

- (void)_vicVideoModeAction:(int)mode;
- (IBAction)vicVideoModeAction:(id)sender;
- (IBAction)_vicScreenGeometryAction:(int)mode;
- (IBAction)vicScreenGeometryAction:(id)sender;
- (IBAction)vicMemoryBankAction:(id)sender;
- (IBAction)vicScreenMemoryAction:(id)sender;
- (IBAction)vicCharacterMemoryAction:(id)sender;
- (IBAction)vicDXAction:(id)sender;
- (IBAction)vicDYAction:(id)sender;
- (IBAction)vicDXStepperAction:(id)sender;
- (IBAction)vicDYStepperAction:(id)sender;
- (IBAction)vicSpriteSelectAction:(id)sender;
- (IBAction)vicSpriteActiveAction:(id)sender;
- (IBAction)vicSpriteMulticolorAction:(id)sender;
- (IBAction)vicSpriteStretchXAction:(id)sender;
- (IBAction)vicSpriteStretchYAction:(id)sender;
- (IBAction)vicSpriteInFrontAction:(id)sender;
- (IBAction)vicSpriteSpriteCollisionAction:(id)sender;
- (IBAction)vicSpriteBackgroundCollisionAction:(id)sender;
- (IBAction)vicSpriteXAction:(id)sender;
- (IBAction)vicSpriteYAction:(id)sender;
- (IBAction)vicSpriteColorAction:(id)sender;
- (IBAction)vicRasterlineAction:(id)sender;
- (IBAction)vicEnableRasterInterruptAction:(id)sender;
- (IBAction)vicRasterInterruptAction:(id)sender;
- (IBAction)vicEnableOpenGL:(id)sender;

- (IBAction)cancelRomDialog:(id)sender;
- (IBAction)cancelMountDialog:(id)sender;
- (IBAction)endMountDialogAndMount:(id)sender;
- (IBAction)endMountDialogAndFlash:(id)sender;
- (IBAction)saveScreenshotDialog:(id)sender;

// Helper functions for action methods
- (void)changeMemValue:(uint16_t)addr value:(int16_t)v memtype:(Memory::MemoryType)t;
- (Memory::MemoryType)currentMemSource;
- (int)currentSprite;
- (int)currentCIA;

// Table views handling
- (int)numberOfRowsInTableView:(NSTableView *)aTableView;

// Getter
- (id)objectValueForCpuTableColumn:(NSTableColumn *)aTableColumn row:(int)row;
- (id)objectValueForMemTableColumn:(NSTableColumn *)aTableColumn row:(int)row;
- (id)objectValueForHistoryTableColumn:(NSTableColumn *)aTableColumn row:(int)row;
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

// Loading and saving
- (BOOL)loadRom:(NSString *)filename;

// Misc
- (BOOL)computeRowForAddr:(uint16_t)addr maxRows:(uint16_t)maxRows row:(uint16_t *)row;
- (BOOL)showMountDialog;
- (void)measureEmulationSpeed;

@end

#endif

