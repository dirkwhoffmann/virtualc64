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
	IBOutlet NSDrawer *debugPanel;
	IBOutlet NSDrawer *cheatboxPanel;
    
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
	
    // Debugger (SID panel)
    IBOutlet NSTextField *volume;
    IBOutlet NSTextField *potX;
    IBOutlet NSTextField *potY;

    IBOutlet NSSegmentedControl *voiceSelector;
    IBOutlet NSPopUpButton *waveform;
    IBOutlet NSTextField *frequency;
    IBOutlet NSTextField *pulseWidth;
    IBOutlet NSTextField *attackRate;
    IBOutlet NSTextField *delayRate;
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

    /*! @brief   Current keyboard modifier flags
     *  @details These flags tell us if one of the special keys
     *           are currently pressed. The flags are utilized, e.g., to
     *           alter behaviour when a key on the TouchBar is pressed.
     */
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

// Debugger (SID Panel)
@property NSTextField *volume;
@property NSTextField *potX;
@property NSTextField *potY;

@property NSSegmentedControl *voiceSelector;
@property NSPopUpButton *waveform;
@property NSTextField *frequency;
@property NSTextField *pulseWidth;
@property NSTextField *attackRate;
@property NSTextField *delayRate;
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


@property PropertiesDialog *propertiesDialog;
@property HardwareDialog *hardwareDialog;
@property MediaDialog *mediaDialog;
@property MountDialog *mountDialog;
@property TapeDialog *tapeDialog;

// Toolbar
@property NSPopUpButton *controlPort1;
@property NSPopUpButton *controlPort2;

// Main screen
@property NSDrawer *debugPanel;

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

// Refresh
- (void)refresh;
- (void)refresh:(NSFormatter *)byteFormatter word:(NSFormatter *)wordFormatter threedigit:(NSFormatter *)threedigitFormatter;
- (void)enableUserEditing:(BOOL)enabled;

@end
