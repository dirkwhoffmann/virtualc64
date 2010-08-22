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

#import <Cocoa/Cocoa.h>

// Forward declarations
@class MyDocument;
@class VICScreen;
@class AudioDevice;


@interface C64Proxy : NSObject {	
	
	// ListenerProxy *listener;	
	C64 *c64;
	CIA *cia[3];
	IEC *iec;
	CPU *cpu;    // CPU to observe (can be switched between C64 and VC1541)
	Memory *mem; // Memory to observe (can be switched between C64 and VC1541)
	AudioDevice *audioDevice;
}

// Initialization
- (id) initWithDocument:(MyDocument *)d;
- (id) initWithDocument:(MyDocument *)d withScreen:(VICScreen *)s;
- (void) release;
- (C64 *) getC64;

// C64
- (Message *)getMessage;

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

- (int) numberOfMissingRoms;
- (int) missingRoms;
- (bool) loadBasicRom:(NSString *)filename;
- (bool) loadCharRom:(NSString *)filename;
- (bool) loadKernelRom:(NSString *)filename;
- (bool) loadVC1541Rom:(NSString *)filename;
- (bool) isCartridgeAttached;

- (bool) cpuTracingEnabled;
- (void) cpuSetTraceMode:(bool)b;
- (bool) iecTracingEnabled;
- (void) iecSetTraceMode:(bool)b;
- (bool) vc1541CpuTracingEnabled;
- (void) vc1541CpuSetTraceMode:(bool)b;
- (bool) viaTracingEnabled;
- (void) viaSetTraceMode:(bool)b;

- (void) dumpC64;
- (void) dumpC64CPU;
- (void) dumpC64CIA1;
- (void) dumpC64CIA2;
- (void) dumpC64VIC;
- (void) dumpC64SID;
- (void) dumpC64Memory;
- (void) dumpVC1541;
- (void) dumpVC1541CPU;
- (void) dumpVC1541VIA1;
- (void) dumpVC1541VIA2;
- (void) dumpVC1541Memory;
- (void) dumpKeyboard;
- (void) dumpIEC;

// CPU
- (bool) cpuGetWarpMode;
- (void) cpuSetWarpMode:(bool)b;
- (long) cpuGetCycles;
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
- (void) switchInputDevice:(int)devNo;
- (void) switchInputDevices;
- (uint8_t) getPortAssignment:(int)devNo;
- (Joystick *) addJoystick;
- (void) removeJoystick:(Joystick *)joystick;


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
- (void) ciaSetTodHours:(int)nr value:(uint8_t)value;
- (uint8_t) ciaGetTodMinutes:(int)nr;
- (void) ciaSetTodMinutes:(int)nr value:(uint8_t)value;
- (uint8_t) ciaGetTodSeconds:(int)nr;
- (void) ciaSetTodSeconds:(int)nr value:(uint8_t)value;
- (uint8_t) ciaGetTodTenth:(int)nr;
- (void) ciaSetTodTenth:(int)nr value:(uint8_t)value;

- (uint8_t) ciaGetAlarmHours:(int)nr;
- (void) ciaSetAlarmHours:(int)nr value:(uint8_t)value;
- (uint8_t) ciaGetAlarmMinutes:(int)nr;
- (void) ciaSetAlarmMinutes:(int)nr value:(uint8_t)value;
- (uint8_t) ciaGetAlarmSeconds:(int)nr;
- (void) ciaSetAlarmSeconds:(int)nr value:(uint8_t)value;
- (uint8_t) ciaGetAlarmTenth:(int)nr;
- (void) ciaSetAlarmTenth:(int)nr value:(uint8_t)value;
- (bool) todIsInterruptEnabled:(int)nr;
- (void) todSetInterruptEnabled:(int)nr value:(bool)b;

// VIC
- (void *) vicScreenBuffer;
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

- (bool) vicHideSprites;
- (void) vicSetHideSprites:(bool)b;
- (bool) vicShowIrqLines;
- (void) vicSetShowIrqLines:(bool)b;
- (bool) vicShowDmaLines;
- (void) vicSetShowDmaLines:(bool)b;
- (void) fastReset;

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
- (void) keyboardTypeFormat;

// Drive
- (void) ejectDisk;
- (bool) isDriveConnected;
- (void) connectDrive;
- (void) disconnectDrive;

//- (void) disconnectDrive;

@end

#endif
