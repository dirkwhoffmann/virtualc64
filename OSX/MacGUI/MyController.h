/*
 * (C) 2011 - 2018 Dirk W. Hoffmann. All rights reserved.
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

/*
#import <Cocoa/Cocoa.h>

// Forward declarations
@class C64Proxy;
@class GamePadManager;
@class KeyboardController;
@class MetalView;
@class PropertiesDialog;
@class HardwareDialog;
@class MediaDialog;
@class MountDialog;
@class TapeDialog;
@class MetalView;
@class CpuTableView;
@class MemTableView;
@class WaveformView;
@class Speedometer;


@interface MyController : NSWindowController 
{
	// Proxy object. Used get data from and sent data to the virtual C64
	// Implements a bridge between C++ (simulator) and Objective-C (GUI)
	// IBOutlet
    C64Proxy *__strong c64;
    
    // Game pad manager
    GamePadManager *gamePadManager;

    // Keyboard controller
    KeyboardController *keyboardcontroller;
    
    IBOutlet MetalView *metalScreen;
	    
    // Toolbar
    IBOutlet NSPopUpButton *controlPort1;
    IBOutlet NSPopUpButton *controlPort2;

    // Main screen
	IBOutlet NSDrawer *debugger;
	// IBOutlet NSDrawer *cheatboxPanel;
    
    // Bottom bar
    IBOutlet NSButton *greenLED;
    IBOutlet NSButton *redLED;
    IBOutlet NSProgressIndicator *progress;
    IBOutlet NSButton *driveIcon;
    IBOutlet NSButton *cartridgeIcon;
    IBOutlet NSButton *tapeIcon;
    IBOutlet NSProgressIndicator *tapeProgress;
	IBOutlet NSTextField *clockSpeed;
	IBOutlet NSLevelIndicator *clockSpeedBar;
	IBOutlet NSButton *warpIcon;
	
	// Debug panel (commons)
    
    BOOL hex;
    IBOutlet NSTabView *debugPanel;
	IBOutlet NSMatrix *dezHexSelector;
	IBOutlet NSButton *stopAndGoButton;
	IBOutlet NSButton *stepIntoButton;
	IBOutlet NSButton *stepOverButton;
	
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
    IBOutlet NSTextField *breakAt;
    
	// Debug panel (Memory)
	IBOutlet MemTableView *memTableView;
	
	// Debug panel (CIA)
    IBOutlet NSSegmentedControl *ciaSelector;
    
    IBOutlet NSTextField *ciaPA;
    IBOutlet NSTextField *ciaPAbinary;
    IBOutlet NSTextField *ciaPRA;
    IBOutlet NSTextField *ciaDDRA;
    
    IBOutlet NSTextField *ciaPB;
    IBOutlet NSTextField *ciaPBbinary;
    IBOutlet NSTextField *ciaPRB;
    IBOutlet NSTextField *ciaDDRB;
    
	IBOutlet NSTextField *ciaTimerA;
    IBOutlet NSTextField *ciaLatchA;
    IBOutlet NSButton *ciaRunningA;
	IBOutlet NSButton *ciaToggleA;
    IBOutlet NSButton *ciaPBoutA;
    IBOutlet NSButton *ciaOneShotA;
    
    IBOutlet NSTextField *ciaTimerB;
    IBOutlet NSTextField *ciaLatchB;
    IBOutlet NSButton *ciaRunningB;
    IBOutlet NSButton *ciaToggleB;
    IBOutlet NSButton *ciaPBoutB;
    IBOutlet NSButton *ciaOneShotB;
    
    IBOutlet NSTextField *todHours;
    IBOutlet NSTextField *todMinutes;
    IBOutlet NSTextField *todSeconds;
    IBOutlet NSTextField *todTenth;
    IBOutlet NSButton *todIntEnable;
    IBOutlet NSTextField *alarmHours;
    IBOutlet NSTextField *alarmMinutes;
    IBOutlet NSTextField *alarmSeconds;
    IBOutlet NSTextField *alarmTenth;
    
    IBOutlet NSTextField *ciaIcr;
    IBOutlet NSTextField *ciaIcrBinary;
    IBOutlet NSTextField *ciaImr;
    IBOutlet NSTextField *ciaImrBinary;
    IBOutlet NSButton *ciaIntLineLow;
   
	// Debug pabel (VIC)	
	IBOutlet NSPopUpButton *vicVideoMode;
	IBOutlet NSPopUpButton *vicScreenGeometry;
	IBOutlet NSPopUpButton *vicMemoryBank;
	IBOutlet NSPopUpButton *vicScreenMemory;
	IBOutlet NSPopUpButton *vicCharacterMemory;
	IBOutlet NSTextField *vicDX;
	IBOutlet NSStepper *vicDXStepper;
	IBOutlet NSTextField *vicDY;
	IBOutlet NSStepper *vicDYStepper;
    
    IBOutlet NSButton *vicSpriteSpriteCollision;
    IBOutlet NSButton *vicSpriteBackgroundCollision;

	IBOutlet NSTextField *vicRasterline;
	IBOutlet NSButton *vicEnableRasterInterrupt;
	IBOutlet NSTextField *vicRasterInterrupt;
	
    // Debugger (SID panel)
    NSInteger selectedVoice;
    
    IBOutlet NSTextField *volume;
    IBOutlet NSTextField *potX;
    IBOutlet NSTextField *potY;

    IBOutlet NSSegmentedControl *voiceSelector;
    IBOutlet NSPopUpButton *waveform;
    IBOutlet NSTextField *frequency;
    IBOutlet NSTextField *pulseWidth;
    IBOutlet NSTextField *attackRate;
    IBOutlet NSTextField *decayRate;
    IBOutlet NSTextField *sustainRate;
    IBOutlet NSTextField *releaseRate;
    IBOutlet NSButton *gateBit;
    IBOutlet NSButton *testBit;
    IBOutlet NSButton *syncBit;
    IBOutlet NSButton *ringBit;

    IBOutlet NSPopUpButton *filterType;
    IBOutlet NSTextField *filterCutoff;
    IBOutlet NSTextField *filterResonance;
    IBOutlet NSButton *filter1;
    IBOutlet NSButton *filter2;
    IBOutlet NSButton *filter3;
    
    IBOutlet WaveformView *waveformView;
    IBOutlet NSLevelIndicator *audioBufferLevel;
    IBOutlet NSTextField *audioBufferLevelText;
    IBOutlet NSTextField *bufferUnderflows;
    IBOutlet NSTextField *bufferOverflows;
    
	//! @brief   Update loop timer
	NSTimer *timer;
	
	// Timer lock
	NSLock *timerLock;
	
	// Used inside timer function to fine tune timed events
	long animationCounter; 

	// Speedometer to measure clock frequence and frames per second
	Speedometer *speedometer;    

    //! @brief   Current keyboard modifier flags
    NSEventModifierFlags modifierFlags;
    
    //! @brief   Current mouse coordinate
    NSPoint mouseXY;

    //! @brief   Indicates if mouse is currently hidden
    bool hideMouse;

    //! @brief   Indicates if a status bar is shown
    bool statusBar;
    
    //! @brief   Selected game pad slot for joystick in port A
    NSInteger gamepadSlot1;

    //! @brief   Selected game pad slot for joystick in port B
    NSInteger gamepadSlot2;
    
    //! @brief   Default image for USB devices
    NSImage *genericDeviceImage;
    
    //! @brief   Indicates if user dialog should be skipped when opening archives
    bool autoMount;
}

@property (strong) C64Proxy *c64;
@property GamePadManager *gamePadManager;
@property KeyboardController *keyboardcontroller;
@property MetalView *metalScreen;
@property CpuTableView *cpuTableView;
@property MemTableView *memTableView;
@property Speedometer *speedometer;
@property long animationCounter;

// Debugger
@property NSTabView *debugPanel;
@property NSButton *stopAndGoButton;
@property NSButton *stepIntoButton;
@property NSButton *stepOverButton;
@property BOOL hex;

// Debugger (CPU panel)
@property NSTextField *pc;
@property NSTextField *sp;
@property NSTextField *a;
@property NSTextField *x;
@property NSTextField *y;
@property NSButton *Nflag;
@property NSButton *Zflag;
@property NSButton *Cflag;
@property NSButton *Iflag;
@property NSButton *Bflag;
@property NSButton *Dflag;
@property NSButton *Vflag;
@property NSTextField *breakAt;

// Debugger (CIA panel)
@property NSSegmentedControl *ciaSelector;

@property NSTextField *ciaPA;
@property NSTextField *ciaPAbinary;
@property NSTextField *ciaPRA;
@property NSTextField *ciaDDRA;

@property NSTextField *ciaPB;
@property NSTextField *ciaPBbinary;
@property NSTextField *ciaPRB;
@property NSTextField *ciaDDRB;

@property NSTextField *ciaTimerA;
@property NSTextField *ciaLatchA;
@property NSButton *ciaRunningA;
@property NSButton *ciaToggleA;
@property NSButton *ciaPBoutA;
@property NSButton *ciaOneShotA;

@property NSTextField *ciaTimerB;
@property NSTextField *ciaLatchB;
@property NSButton *ciaRunningB;
@property NSButton *ciaToggleB;
@property NSButton *ciaPBoutB;
@property NSButton *ciaOneShotB;

@property NSTextField *todHours;
@property NSTextField *todMinutes;
@property NSTextField *todSeconds;
@property NSTextField *todTenth;
@property NSButton *todIntEnable;
@property NSTextField *alarmHours;
@property NSTextField *alarmMinutes;
@property NSTextField *alarmSeconds;
@property NSTextField *alarmTenth;

@property NSTextField *ciaIcr;
@property NSTextField *ciaIcrBinary;
@property NSTextField *ciaImr;
@property NSTextField *ciaImrBinary;
@property NSButton *ciaIntLineLow;

// Debugger (VIC panel)
@property NSTextField *vicDX;
@property NSTextField *vicDY;
@property NSTextField *vicRasterline;
@property NSTextField *vicRasterInterrupt;


// Debugger (SID Panel)
@property NSInteger selectedVoice; 
@property NSTextField *volume;
@property NSTextField *potX;
@property NSTextField *potY;

@property NSSegmentedControl *voiceSelector;
@property NSPopUpButton *waveform;
@property NSTextField *frequency;
@property NSTextField *pulseWidth;
@property NSTextField *attackRate;
@property NSTextField *decayRate;
@property NSTextField *sustainRate;
@property NSTextField *releaseRate;
@property NSButton *gateBit;
@property NSButton *testBit;
@property NSButton *syncBit;
@property NSButton *ringBit;

@property NSPopUpButton *filterType;
@property NSTextField *filterCutoff;
@property NSTextField *filterResonance;
@property NSButton *filter1;
@property NSButton *filter2;
@property NSButton *filter3;

@property WaveformView *waveformView;
@property NSLevelIndicator *audioBufferLevel;
@property NSTextField *audioBufferLevelText;
@property NSTextField *bufferUnderflows;
@property NSTextField *bufferOverflows;

@property PropertiesDialog *propertiesDialog;
@property HardwareDialog *hardwareDialog;
@property MediaDialog *mediaDialog;
@property MountDialog *mountDialog;
@property TapeDialog *tapeDialog;

// Toolbar
@property NSPopUpButton *controlPort1;
@property NSPopUpButton *controlPort2;

// Main screen
@property NSDrawer *debugger;

// Bottom bar
@property NSButton *greenLED;
@property NSButton *redLED;
@property NSProgressIndicator *progress;
@property NSButton *driveIcon;
@property NSButton *cartridgeIcon;
@property NSButton *tapeIcon;
@property NSProgressIndicator *tapeProgress;
@property NSTextField *clockSpeed;
@property NSLevelIndicator *clockSpeedBar;
@property NSButton *warpIcon;

@property NSMenuItem *menuItemFinalIII;
@property NSEventModifierFlags modifierFlags;
@property NSPoint mouseXY;
@property bool hideMouse;
@property bool statusBar;
@property NSInteger gamepadSlot1;
@property NSInteger gamepadSlot2;
@property NSImage *genericDeviceImage;
@property bool autoMount;

@property NSTimer *timer;
@property NSLock *timerLock;

@end
*/

