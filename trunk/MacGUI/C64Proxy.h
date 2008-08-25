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

#ifndef _C64_PROXY
#define _C64_PROXY

#include "C64.h"
#import "AudioDevice.h"

// Forward declarations
@class MyDocument;
@class VICScreen;

class ListenerProxy : public  C64Listener {
	
private:
	MyDocument *doc;
	VICScreen *screen;
	
public:	
	ListenerProxy();
	void setDocument(MyDocument *d) { doc = d; }
	MyDocument *getDocument() { return doc; }
	void setScreen(VICScreen *s) { screen = s; }
	VICScreen *getScreen() { return screen; }
	
	void loadRomAction(int rom);
	void missingRomAction(int missingRoms);
	void runAction();
	void haltAction();
	void drawAction(int *screenBuffer);
	void cpuAction(int state);
	void driveAttachedAction(bool connected);
	void driveDiscAction(bool inserted);
	void driveLEDAction(bool on);
	void driveDataAction(bool transfering);
	void driveMotorAction(bool rotating);
	void warpAction(bool warping);
	void logAction(char *message);
};

@interface C64Proxy : NSObject {	
	
	ListenerProxy *listener;	
	C64 *c64;
	CIA *cia[3];
	IEC *iec;
	CPU *cpu;    // CPU to observe (can be switched between C64 and VC1541)
	Memory *mem; // Memory to observe (can be switched between C64 and VC1541)
	AudioDevice *audioDevice;
}

// Initialization
//- (id) init;
- (id) initWithDocument:(MyDocument *)d withScreen:(VICScreen *)s;
- (void) release;
- (C64 *) getC64;

// C64
- (void) reset;
- (void) halt;
- (void) step;
- (bool) isRunnable;
- (void) run;
- (bool) isHalted;
- (bool) isRunning;
- (void) setPAL;
- (void) setNTSC;
- (int) getFrameDelay;
- (void) setFrameDelay:(int)delay;
- (int) buildNr;

//- (bool) isBasicRom:(NSString *)filename;
//- (bool) isCharRom:(NSString *)filename;
//- (bool) isKernelRom:(NSString *)filename;
//- (bool) isVC1541Rom:(NSString *)filename;
//- (bool) loadRom:(NSString *)filename;
- (int) numberOfMissingRoms;
- (int) missingRoms;
- (bool) loadBasicRom:(NSString *)filename;
- (bool) loadCharRom:(NSString *)filename;
- (bool) loadKernelRom:(NSString *)filename;
- (bool) loadVC1541Rom:(NSString *)filename;
- (bool) loadSnapshot:(NSString *)filename;
- (bool) saveSnapshot:(NSString *)filename;

- (void) dumpCPU;
- (void) dumpCIA;
- (void) dumpVIC;
- (void) dumpIEC;
- (void) dumpMemory;
- (void) dumpDrive;

// CPU
- (bool) cpuGetWarpMode;
- (void) cpuSetWarpMode:(bool)b;
- (long) cpuGetCycles;
- (bool) cpuTracingEnabled;
- (void) cpuSetTraceMode:(bool)b;
- (bool) iecTracingEnabled;
- (void) iecSetTraceMode:(bool)b;
- (bool) driveCPUTracingEnabled;
- (void) driveSetCPUTraceMode:(bool)b;
- (bool) driveDataTracingEnabled;
- (void) driveSetDataTraceMode:(bool)b;

- (uint16_t) cpuGetPC;
- (void) cpuSetPC:(uint16_t)pc;
- (uint8_t) cpuGetSP;
- (void) cpuSetSP:(uint8_t)sp;
- (uint8_t) cpuGetA;
- (void) cpuSetA:(uint8_t)a;
- (uint8_t) cpuGetX;
- (void) cpuSetX:(uint8_t)x;
- (uint8_t) cpuGetY;
- (void) cpuSetY:(uint8_t)y;
- (bool) cpuGetN;
- (void) cpuSetN:(bool)b;
- (bool) cpuGetZ;
- (void) cpuSetZ:(bool)b;
- (bool) cpuGetC;
- (void) cpuSetC:(bool)b;
- (bool) cpuGetI;
- (void) cpuSetI:(bool)b;
- (bool) cpuGetB;
- (void) cpuSetB:(bool)b;
- (bool) cpuGetD;
- (void) cpuSetD:(bool)b;
- (bool) cpuGetV;
- (void) cpuSetV:(bool)b;

- (uint16_t) cpuPeekPC;
- (uint8_t) cpuGetLengthOfInstruction:(uint8_t)opcode;
- (uint8_t) cpuGetLengthOfCurrentInstruction;
- (uint16_t) cpuGetAddressOfNextIthInstruction:(int)i from:(uint16_t)addr;
- (uint16_t) cpuGetAddressOfNextInstruction;
- (char *) cpuGetMnemonic:(uint8_t)opcode;
- (CPU::AddressingMode) cpuGetAddressingMode:(uint8_t)opcode;

// - (CPU::ErrorState) cpuGetErrorState;
- (int) cpuGetTopOfCallStack;
- (int) cpuGetBreakpoint:(int)addr;
- (void) cpuSetBreakpoint:(int)addr tag:(uint8_t)t;
- (void) cpuSetHardBreakpoint:(int)addr;
- (void) cpuDeleteHardBreakpoint:(int)addr;
- (void) cpuToggleHardBreakpoint:(int)addr;
- (void) cpuSetSoftBreakpoint:(int)addr;
- (void) cpuDeleteSoftBreakpoint:(int)addr;
- (void) cpuToggleSoftBreakpoint:(int)addr;

// JOYSTICK
- (Joystick *) getJoystickOnPortA;
- (Joystick *) getJoystickOnPortB;
- (void) switchInputDevice:(int)devNo;
- (void) switchInputDevices;
- (uint8_t) getPortAssignment:(int)devNo;


// MEM
- (uint8_t) memPeek:(uint16_t)addr;
- (uint16_t) memPeekWord:(uint16_t)addr;
- (uint8_t) memPeekFrom:(uint16_t)addr memtype:(Memory::MemoryType)source;
- (void) memPoke:(uint16_t)addr value:(uint8_t)val;
- (void) memPokeTo:(uint16_t)addr value:(uint8_t)val memtype:(Memory::MemoryType)source;

- (bool) memIsValidAddr:(uint16_t)addr memtype:(Memory::MemoryType)source;

- (Memory::WatchpointType) memGetWatchpointType:(uint16_t)addr;
- (void) memSetWatchpoint:(uint16_t)addr;
- (void) memSetWatchpoint:(uint16_t)addr watchvalue:(uint8_t)value;
- (void) memSetWatchpoint:(uint16_t)addr tag:(Memory::WatchpointType)type watchvalue:(uint8_t)value;
- (void) memDeleteWatchpoint:(uint16_t)addr;
- (uint8_t) memGetWatchValue:(uint16_t)addr;

// CIA
- (uint8_t) ciaGetDataPortA:(int)nr;
- (void) ciaSetDataPortA:(int)nr value:(uint8_t)v;
- (uint8_t) ciaGetDataPortDirectionA:(int)nr;
- (void) ciaSetDataPortDirectionA:(int)nr value:(uint8_t)v;
- (uint16_t) ciaGetTimerA:(int)nr;
- (void) ciaSetTimerA:(int)nr value:(uint16_t)v;
- (uint16_t) ciaGetTimerLatchA:(int)nr;
- (void) ciaSetTimerLatchA:(int)nr value:(uint16_t)v;
- (bool) ciaGetStartFlagA:(int)nr;
- (void) ciaSetStartFlagA:(int)nr value:(bool)b;
- (void) ciaToggleStartFlagA:(int)nr;
- (bool) ciaGetOneShotFlagA:(int)nr;
- (void) ciaSetOneShotFlagA:(int)nr value:(bool)b;
- (void) ciaToggleOneShotFlagA:(int)nr;
- (bool) ciaGetUnderflowFlagA:(int)nr;
- (void) ciaSetUnderflowFlagA:(int)nr value:(bool)b;
- (void) ciaToggleUnderflowFlagA:(int)nr;
- (bool) ciaGetPendingSignalFlagA:(int)nr;
- (void) ciaSetPendingSignalFlagA:(int)nr value:(bool)b;
- (void) ciaTogglePendingSignalFlagA:(int)nr;
- (bool) ciaGetInterruptEnableFlagA:(int)nr;
- (void) ciaSetInterruptEnableFlagA:(int)nr value:(bool)b;
- (void) ciaToggleInterruptEnableFlagA:(int)nr;

- (uint8_t) ciaGetDataPortB:(int)nr;
- (void) ciaSetDataPortB:(int)nr value:(uint8_t)v;
- (uint8_t) ciaGetDataPortDirectionB:(int)nr;
- (void) ciaSetDataPortDirectionB:(int)nr value:(uint8_t)v;
- (uint16_t) ciaGetTimerB:(int)nr;
- (void) ciaSetTimerB:(int)nr value:(uint16_t)v;
- (uint16_t) ciaGetTimerLatchB:(int)nr;
- (void) ciaSetTimerLatchB:(int)nr value:(uint16_t)v;
- (bool) ciaGetStartFlagB:(int)nr;
- (void) ciaSetStartFlagB:(int)nr value:(bool)b;
- (void) ciaToggleStartFlagB:(int)nr;
- (bool) ciaGetOneShotFlagB:(int)nr;
- (void) ciaSetOneShotFlagB:(int)nr value:(bool)b;
- (void) ciaToggleOneShotFlagB:(int)nr;
- (bool) ciaGetUnderflowFlagB:(int)nr;
- (void) ciaSetUnderflowFlagB:(int)nr value:(bool)b;
- (void) ciaToggleUnderflowFlagB:(int)nr;
- (bool) ciaGetPendingSignalFlagB:(int)nr;
- (void) ciaSetPendingSignalFlagB:(int)nr value:(bool)b;
- (void) ciaTogglePendingSignalFlagB:(int)nr;
- (bool) ciaGetInterruptEnableFlagB:(int)nr;
- (void) ciaSetInterruptEnableFlagB:(int)nr value:(bool)b;
- (void) ciaToggleInterruptEnableFlagB:(int)nr;

- (uint8_t) ciaGetTodHours:(int)nr;
- (uint8_t) ciaGetTodMinutes:(int)nr;
- (uint8_t) ciaGetTodSeconds:(int)nr;
- (uint8_t) ciaGetTodTenth:(int)nr;

- (uint8_t) ciaGetAlarmHours:(int)nr;
- (uint8_t) ciaGetAlarmMinutes:(int)nr;
- (uint8_t) ciaGetAlarmSeconds:(int)nr;
- (uint8_t) ciaGetAlarmTenth:(int)nr;


// VIC
- (NSColor *) vicGetColor:(VIC::ColorScheme)scheme nr:(int)nr;
- (void) vicSetColor:(int)color rgba:(NSColor *)rgba;
- (void) vicSetColorInt:(int)color rgba:(int)rgba;
- (void) vicSetColorScheme:(VIC::ColorScheme)scheme;

- (uint16_t) vicGetMemoryBankAddr;
- (void) vicSetMemoryBankAddr:(uint16_t)addr;
- (uint16_t) vicGetScreenMemoryAddr;
- (void) vicSetScreenMemoryAddr:(uint16_t)addr;
- (uint16_t) vicGetCharacterMemoryAddr;
- (void) vicSetCharacterMemoryAddr:(uint16_t)addr;

- (int) vicGetDisplayMode;
- (void) vicSetDisplayMode:(int)mode;
- (int) vicGetScreenGeometry;
- (void) vicSetScreenGeometry:(int)mode;
- (int) vicGetHorizontalRasterScroll;
- (void) vicSetHorizontalRasterScroll:(int)offset;
- (int) vicGetVerticalRasterScroll;
- (void) vicSetVerticalRasterScroll:(int)offset;

- (bool) spriteGetVisibilityFlag:(int)nr;
- (void) spriteSetVisibilityFlag:(int)nr value:(bool)flag;
- (void) spriteToggleVisibilityFlag:(int)nr;

- (bool) spriteGetSpriteSpriteCollisionFlag:(int)nr;
- (void) spriteSetSpriteSpriteCollisionFlag:(int)nr value:(bool)flag;
- (void) spriteToggleSpriteSpriteCollisionFlag:(int)nr;

- (bool) spriteGetSpriteBackgroundCollisionFlag:(int)nr;
- (void) spriteSetSpriteBackgroundCollisionFlag:(int)nr value:(bool)flag;
- (void) spriteToggleSpriteBackgroundCollisionFlag:(int)nr;

- (bool) spriteGetBackgroundPriorityFlag:(int)nr;
- (void) spriteSetBackgroundPriorityFlag:(int)nr value:(bool)flag;
- (void) spriteToggleBackgroundPriorityFlag:(int)nr;

- (bool) spriteGetMulticolorFlag:(int)nr;
- (void) spriteSetMulticolorFlag:(int)nr value:(bool)flag;
- (void) spriteToggleMulticolorFlag:(int)nr;

- (bool) spriteGetStretchXFlag:(int)nr;
- (void) spriteSetStretchXFlag:(int)nr value:(bool)flag;
- (void) spriteToggleStretchXFlag:(int)nr;

- (bool) spriteGetStretchYFlag:(int)nr;
- (void) spriteSetStretchYFlag:(int)nr value:(bool)flag;
- (void) spriteToggleStretchYFlag:(int)nr;

- (int) spriteGetX:(int)nr;
- (void) spriteSetX:(int)nr value:(int)x;
- (int) spriteGetY:(int)nr;
- (void) spriteSetY:(int)nr value:(int)y;
- (int) spriteGetColor:(int)nr;
- (void) spriteSetColor:(int)nr value:(int)c;

- (uint16_t) vicGetRasterLine;
- (void) vicSetRasterLine:(uint16_t)line;
- (uint16_t) vicGetRasterInterruptLine;
- (void) vicSetRasterInterruptLine:(uint16_t)line;
- (bool) vicGetRasterInterruptFlag;
- (void) vicSetRasterInterruptFlag:(bool)b;
- (void) vicToggleRasterInterruptFlag;

- (void) vicToggleDrawSprites;
- (void) vicToggleMarkIRQLines;

// audio hardware
- (void) enableAudio;
- (void) disableAudio;

// SID
- (float) sidGetVolumeControl;
- (void) sidSetVolumeControl:(float)value;

// Keyboard
- (void) keyboardPressRunstopRestore;

- (void) keyboardPressRunstopKey;
- (void) keyboardReleaseRunstopKey;
- (void) keyboardPressCommodoreKey;
- (void) keyboardReleaseCommodoreKey;

// Drive
- (void) ejectDisk;
- (bool) isDriveConnected;
- (void) connectDrive;
- (void) disconnectDrive;

//- (void) disconnectDrive;

@end

#endif
