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

#import <Cocoa/Cocoa.h>

// Forward declarations
@class C64Proxy;
@class MountDialog;
@class RomDialog;
@class PreferenceController;
@class MyOpenGLView;
@class CpuTableView;
@class MemTableView;
@class Speedometer;

class JoystickManager;
class Snapshot;

// @class MyDocument;

@interface MyController : NSWindowController 
{
	// MOVE TO MYDOCUMENT
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
	IBOutlet MyOpenGLView *screen;
	IBOutlet NSDrawer *debug_panel;
	IBOutlet NSDrawer *cheatboxPanel;
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
	
	// Cheatbox panel
	IBOutlet NSToolbarItem *cheatboxIcon;
	IBOutlet CheatboxImageBrowserView *cheatboxImageBrowserView;
	
	// Debug panel (common)
	IBOutlet NSMatrix *dezHexSelector;
	IBOutlet NSButton *stopAndGoButton;
	IBOutlet NSButton *stepIntoButton;
	IBOutlet NSButton *stepOverButton;
	IBOutlet NSButton *stepOutButton;
	
	// Debug panel (CPU)
	IBOutlet CpuTableView *cpuTableView;
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
	IBOutlet MemTableView *memTableView;
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
		
	// The 60 Hz timer
	NSTimer *timer;
	
	// Timer lock
	NSLock *timerLock;
	
	// Used inside timer function to fine tune timed events
	long animationCounter; 

	// Speedometer to measure clock frequence and frames per second
	Speedometer *speedometer;
		
	//! Attached snapshot
	/*! Used to communicate between mount dialog panel and controller.
		There might be a better way to do this. Getting rid of this variable is appreciated. */
	// Snapshot *snapshot;
}

@property C64Proxy *c64;

// User defaults
+ (void)registerStandardDefaults;
- (void)loadUserDefaults;

// Timer and message processing
- (void)timerFunc;
- (void)processMessage:(Message *)msg;

// Refresh
- (void)refresh;
- (void)refresh:(NSFormatter *)byteFormatter word:(NSFormatter *)wordFormatter disassembler:(NSFormatter *)disassembler;
- (void)enableUserEditing:(BOOL)enabled;

// Main window controls
- (IBAction)stepperAction:(id)sender;
- (IBAction)warpAction:(id)sender;
- (IBAction)ejectAction:(id)sender;
- (IBAction)driveAction:(id)sender;
- (IBAction)cartridgeEjectAction:(id)sender;

// Cheatbox actions
// - (void)revertToSnapshot:(V64Snapshot *)snapshot;
// - (void)revertToSnapshotWithNumber:(int)nr;

// Dialogs
- (BOOL)showMountDialog;
- (IBAction)cancelRomDialog:(id)sender;
- (IBAction)cancelMountDialog:(id)sender;
- (IBAction)endMountDialogAndMount:(id)sender;
- (IBAction)endMountDialogAndFlash:(id)sender;

@end
