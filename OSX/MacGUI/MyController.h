/*
 * (C) 2011 - 2017 Dirk W. Hoffmann. All rights reserved.
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
@class GamePadManager;
@class PropertiesDialog;
@class HardwareDialog;
@class MediaDialog;
@class MountDialog;
@class TapeDialog;
@class RomDialog;
@class MyMetalView;
@class CpuTableView;
@class MemTableView;
@class Speedometer;

typedef NS_ENUM(NSInteger, INPUT_DEVICES) {
    IPD_UNCONNECTED = 0,
    IPD_KEYSET_1,
    IPD_KEYSET_2,
    IPD_JOYSTICK_1,
    IPD_JOYSTICK_2
};

/*! @brief    Fingerprint that uniquely identifies a key combination on the physical Mac keyboard
 *  @seealso  C64KeyFingerprint
 */
typedef unsigned long MacKeyFingerprint;

// @interface MyController : NSWindowController <NSApplicationDelegate, NSMenuDelegate>
@interface MyController : NSWindowController
{
	// Proxy object. Used get data from and sent data to the virtual C64
	// Implements a bridge between C++ (simulator) and Objective-C (GUI)
	// IBOutlet
    C64Proxy *__strong c64;
    
    // Game pad manager
    GamePadManager *gamePadManager;
    
	// Dialogs
    IBOutlet PropertiesDialog *propertiesDialog;
    IBOutlet HardwareDialog *hardwareDialog;
    IBOutlet MediaDialog *mediaDialog;
	IBOutlet MountDialog *mountDialog;
    IBOutlet TapeDialog *tapeDialog;
	IBOutlet RomDialog *romDialog;
	    
    // Toolbar
    IBOutlet NSPopUpButton *joystickPortA;
    IBOutlet NSPopUpButton *joystickPortB;

    // Main screen
	IBOutlet NSDrawer *debugPanel;
	IBOutlet NSDrawer *cheatboxPanel;
    
    // Bottom bar
    IBOutlet NSButton *greenLED;
    IBOutlet NSButton *redLED;
    IBOutlet NSProgressIndicator *progress;
    IBOutlet NSButton *driveIcon;
	IBOutlet NSButton *driveEject;
    IBOutlet NSButton *cartridgeIcon;
    IBOutlet NSButton *cartridgeEject;
    IBOutlet NSButton *tapeIcon;
    IBOutlet NSButton *tapeEject;
    IBOutlet NSProgressIndicator *tapeProgress;
	IBOutlet NSTextField *info;
	IBOutlet NSTextField *clockSpeed;
	IBOutlet NSLevelIndicator *clockSpeedBar;
	IBOutlet NSButton *warpIcon;
	
	// Cheatbox panel
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
	IBOutlet NSButton *Nflag;
	IBOutlet NSButton *Zflag;
	IBOutlet NSButton *Cflag;
	IBOutlet NSButton *Iflag;
	IBOutlet NSButton *Bflag;
	IBOutlet NSButton *Dflag;
	IBOutlet NSButton *Vflag;
	IBOutlet NSTextField *breakpoint;
	
	// Debug panel (Memory)
	IBOutlet MemTableView *memTableView;
	IBOutlet NSSearchField *addr_search;	
	IBOutlet NSButtonCell *ramSource;
	IBOutlet NSButtonCell *romSource;
	IBOutlet NSButtonCell *ioSource;	
	
	// Debug pabel (CIA)	
	IBOutlet NSTextField *cia1DataPortA;
	IBOutlet NSTextField *cia1DataPortDirectionA;
	IBOutlet NSTextField *cia1TimerA;
	IBOutlet NSTextField *cia1LatchedTimerA;
	IBOutlet NSButton *cia1RunningA;
	IBOutlet NSButton *cia1OneShotA;
	IBOutlet NSButton *cia1CountUnderflowsA;
	IBOutlet NSButton *cia1SignalPendingA;
	IBOutlet NSButton *cia1InterruptEnableA;
	
	IBOutlet NSTextField *cia1DataPortB;
	IBOutlet NSTextField *cia1DataPortDirectionB;
	IBOutlet NSTextField *cia1TimerB;
	IBOutlet NSTextField *cia1LatchedTimerB;
	IBOutlet NSButton *cia1RunningB;
	IBOutlet NSButton *cia1OneShotB;
	IBOutlet NSButton *cia1CountUnderflowsB;
	IBOutlet NSButton *cia1SignalPendingB;
	IBOutlet NSButton *cia1InterruptEnableB;
	
	IBOutlet NSTextField *tod1Hours;
	IBOutlet NSTextField *tod1Minutes;
	IBOutlet NSTextField *tod1Seconds;
	IBOutlet NSTextField *tod1Tenth;
	
	IBOutlet NSTextField *alarm1Hours;
	IBOutlet NSTextField *alarm1Minutes;
	IBOutlet NSTextField *alarm1Seconds;
	IBOutlet NSTextField *alarm1Tenth;
	IBOutlet NSButton *tod1InterruptEnabled;

    IBOutlet NSTextField *cia2DataPortA;
	IBOutlet NSTextField *cia2DataPortDirectionA;
	IBOutlet NSTextField *cia2TimerA;
	IBOutlet NSTextField *cia2LatchedTimerA;
	IBOutlet NSButton *cia2RunningA;
	IBOutlet NSButton *cia2OneShotA;
	IBOutlet NSButton *cia2CountUnderflowsA;
	IBOutlet NSButton *cia2SignalPendingA;
	IBOutlet NSButton *cia2InterruptEnableA;
	
	IBOutlet NSTextField *cia2DataPortB;
	IBOutlet NSTextField *cia2DataPortDirectionB;
	IBOutlet NSTextField *cia2TimerB;
	IBOutlet NSTextField *cia2LatchedTimerB;
	IBOutlet NSButton *cia2RunningB;
	IBOutlet NSButton *cia2OneShotB;
	IBOutlet NSButton *cia2CountUnderflowsB;
	IBOutlet NSButton *cia2SignalPendingB;
	IBOutlet NSButton *cia2InterruptEnableB;
	
	IBOutlet NSTextField *tod2Hours;
	IBOutlet NSTextField *tod2Minutes;
	IBOutlet NSTextField *tod2Seconds;
	IBOutlet NSTextField *tod2Tenth;
	
	IBOutlet NSTextField *alarm2Hours;
	IBOutlet NSTextField *alarm2Minutes;
	IBOutlet NSTextField *alarm2Seconds;
	IBOutlet NSTextField *alarm2Tenth;
	IBOutlet NSButton *tod2InterruptEnabled;

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
    
    IBOutlet NSButton *VicSpriteActive1;
    IBOutlet NSTextField *VicSpriteX1;
    IBOutlet NSTextField *VicSpriteY1;
    IBOutlet NSButton *VicSpriteCol1;
    IBOutlet NSButton *VicSpriteMulticolor1;
    IBOutlet NSButton *VicSpriteStretchX1;
    IBOutlet NSButton *VicSpriteStretchY1;

    IBOutlet NSButton *VicSpriteActive2;
    IBOutlet NSTextField *VicSpriteX2;
    IBOutlet NSTextField *VicSpriteY2;
    IBOutlet NSButton *VicSpriteCol2;
    IBOutlet NSButton *VicSpriteMulticolor2;
    IBOutlet NSButton *VicSpriteStretchX2;
    IBOutlet NSButton *VicSpriteStretchY2;

    IBOutlet NSButton *VicSpriteActive3;
    IBOutlet NSTextField *VicSpriteX3;
    IBOutlet NSTextField *VicSpriteY3;
    IBOutlet NSButton *VicSpriteCol3;
    IBOutlet NSButton *VicSpriteMulticolor3;
    IBOutlet NSButton *VicSpriteStretchX3;
    IBOutlet NSButton *VicSpriteStretchY3;

    IBOutlet NSButton *VicSpriteActive4;
    IBOutlet NSTextField *VicSpriteX4;
    IBOutlet NSTextField *VicSpriteY4;
    IBOutlet NSButton *VicSpriteCol4;
    IBOutlet NSButton *VicSpriteMulticolor4;
    IBOutlet NSButton *VicSpriteStretchX4;
    IBOutlet NSButton *VicSpriteStretchY4;

    IBOutlet NSButton *VicSpriteActive5;
    IBOutlet NSTextField *VicSpriteX5;
    IBOutlet NSTextField *VicSpriteY5;
    IBOutlet NSButton *VicSpriteCol5;
    IBOutlet NSButton *VicSpriteMulticolor5;
    IBOutlet NSButton *VicSpriteStretchX5;
    IBOutlet NSButton *VicSpriteStretchY5;

    IBOutlet NSButton *VicSpriteActive6;
    IBOutlet NSTextField *VicSpriteX6;
    IBOutlet NSTextField *VicSpriteY6;
    IBOutlet NSButton *VicSpriteCol6;
    IBOutlet NSButton *VicSpriteMulticolor6;
    IBOutlet NSButton *VicSpriteStretchX6;
    IBOutlet NSButton *VicSpriteStretchY6;

    IBOutlet NSButton *VicSpriteActive7;
    IBOutlet NSTextField *VicSpriteX7;
    IBOutlet NSTextField *VicSpriteY7;
    IBOutlet NSButton *VicSpriteCol7;
    IBOutlet NSButton *VicSpriteMulticolor7;
    IBOutlet NSButton *VicSpriteStretchX7;
    IBOutlet NSButton *VicSpriteStretchY7;

    IBOutlet NSButton *VicSpriteActive8;
    IBOutlet NSTextField *VicSpriteX8;
    IBOutlet NSTextField *VicSpriteY8;
    IBOutlet NSButton *VicSpriteCol8;
    IBOutlet NSButton *VicSpriteMulticolor8;
    IBOutlet NSButton *VicSpriteStretchX8;
    IBOutlet NSButton *VicSpriteStretchY8;

    IBOutlet NSButton *VicSpriteSpriteCollision;
    IBOutlet NSButton *VicSpriteBackgroundCollision;

	IBOutlet NSTextField *VicRasterline;
	IBOutlet NSButton *VicEnableRasterInterrupt;
	IBOutlet NSTextField *VicRasterInterrupt;
		
	/*! @brief   Update loop timer
     *  @details The update task activated 60 times a second
     *           and performs everything from drawing frames or 
     *           checking the message queue. 
     */
	NSTimer *timer;
	
	// Timer lock
	NSLock *timerLock;
	
	// Used inside timer function to fine tune timed events
	long animationCounter; 

	// Speedometer to measure clock frequence and frames per second
	Speedometer *speedometer;    

    //! Stores how fast the emulator is currently running
    double mhz;
    
    //! Stores how many frames per second are currently drawn
    double fps;

    /*! @brief   Current keyboard modifier flags
     *  @details These flags tell us if one of the special keys
     *           are currently pressed. The flags are utilized, e.g., to
     *           alter behaviour when a key on the TouchBar is pressed.
     */
    NSEventModifierFlags modifierFlags;
    
    //! @brief   Indicates if a status bar is shown
    bool statusBar;
}

@property (strong) C64Proxy *c64;
@property (readonly) GamePadManager *gamePadManager;
@property NSDrawer *debugPanel;
@property NSDrawer *cheatboxPanel;
@property CheatboxImageBrowserView *cheatboxImageBrowserView;
@property NSMenuItem *menuItemFinalIII;
@property NSPopUpButton *joystickPortA;
@property NSPopUpButton *joystickPortB;
@property INPUT_DEVICES inputDeviceA;
@property INPUT_DEVICES inputDeviceB;
@property NSEventModifierFlags modifierFlags;
@property bool statusBar;

// Initialization
- (void)configureWindow;

// Window handling
- (void)adjustWindowSize;

// Undo manager
- (NSUndoManager *)undoManager;

// User defaults
+ (void)registerStandardDefaults;
- (void)loadUserDefaults;
- (void)loadVirtualMachineUserDefaults;
- (void)saveUserDefaults;
- (void)saveVirtualMachineUserDefaults;
- (void)restoreFactorySettingsKeyboard;

// MetalView API
- (BOOL)fullscreen;
- (NSImage *)screenshot;
- (void)rotateBack;
- (void)shrink;
- (void)expand;
- (float)eyeX;
- (void)setEyeX:(float)x;
- (float)eyeY;
- (void)setEyeY:(float)y;
- (float)eyeZ;
- (void)setEyeZ:(float)z;
- (long)videoUpscaler;
- (void)setVideoUpscaler:(long)val;
- (long)videoFilter;
- (void)setVideoFilter:(long)val;
- (BOOL)fullscreenKeepAspectRatio;
- (void)setFullscreenKeepAspectRatio:(BOOL)val;
 
 
// Timer and message processing
- (void)timerFunc;
- (void)processMessage:(Message *)msg;

// Refresh
- (void)refresh;
- (void)refresh:(NSFormatter *)byteFormatter word:(NSFormatter *)wordFormatter threedigit:(NSFormatter *)threedigitFormatter disassembler:(NSFormatter *)disassembler;
- (void)enableUserEditing:(BOOL)enabled;

// Main window controls
- (IBAction)driveAction:(id)sender;
- (IBAction)driveEjectAction:(id)sender;
- (IBAction)tapeEjectAction:(id)sender;
- (IBAction)cartridgeEjectAction:(id)sender;
- (IBAction)alwaysWarpAction:(id)sender;


// Dialogs
- (bool)showPropertiesDialog;
- (IBAction)cancelPropertiesDialog:(id)sender;

- (bool)showHardwareDialog;
- (IBAction)cancelHardwareDialog:(id)sender;

- (bool)showMediaDialog;
- (IBAction)cancelMediaDialog:(id)sender;

- (bool)showRomDialog:(Message *)msg;
- (IBAction)cancelRomDialog:(id)sender;

- (bool)showMountDialog;
- (IBAction)cancelMountDialog:(id)sender;
- (IBAction)endMountDialog:(id)sender;

- (bool)showTapeDialog;
- (IBAction)cancelTapeDialog:(id)sender;
- (IBAction)endTapeDialog:(id)sender;

// Exporting
- (BOOL)exportToD64:(NSString *)path;
@end
