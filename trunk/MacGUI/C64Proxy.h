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


// --------------------------------------------------------------------------
//                                    CPU
// --------------------------------------------------------------------------

@interface CPUProxy : NSObject {
	CPU *cpu;
}

- (id) initWithCPU:(CPU *)c;
- (bool) tracingEnabled;
- (void) setTraceMode:(bool)b;
- (void) dump;
- (uint16_t) getPC;
- (void) setPC:(uint16_t)pc;
- (uint8_t) getSP;
- (void) setSP:(uint8_t)sp;
- (uint8_t) getA;
- (void) setA:(uint8_t)a;
- (uint8_t) getX;
- (void) setX:(uint8_t)x;
- (uint8_t) getY;
- (void) setY:(uint8_t)y;
- (bool) getN;
- (void) setN:(bool)b;
- (bool) getZ;
- (void) setZ:(bool)b;
- (bool) getC;
- (void) setC:(bool)b;
- (bool) getI;
- (void) setI:(bool)b;
- (bool) getB;
- (void) setB:(bool)b;
- (bool) getD;
- (void) setD:(bool)b;
- (bool) getV;
- (void) setV:(bool)b;

- (uint16_t) peekPC;
- (uint8_t) getLengthOfInstruction:(uint8_t)opcode;
- (uint8_t) getLengthOfCurrentInstruction;
- (uint16_t) getAddressOfNextIthInstruction:(int)i from:(uint16_t)addr;
- (uint16_t) getAddressOfNextInstruction;
- (char *) getMnemonic:(uint8_t)opcode;
- (CPU::AddressingMode) getAddressingMode:(uint8_t)opcode;

- (int) getTopOfCallStack;
- (int) getBreakpoint:(int)addr;
- (void) setBreakpoint:(int)addr tag:(uint8_t)t;
- (void) setHardBreakpoint:(int)addr;
- (void) deleteHardBreakpoint:(int)addr;
- (void) toggleHardBreakpoint:(int)addr;
- (void) setSoftBreakpoint:(int)addr;
- (void) deleteSoftBreakpoint:(int)addr;
- (void) toggleSoftBreakpoint:(int)addr;

@end

// --------------------------------------------------------------------------
//                                    VIC
// --------------------------------------------------------------------------

@interface VICProxy : NSObject {
	VIC *vic;
}

- (id) initWithVIC:(VIC *)v;
- (void *) screenBuffer;
- (NSColor *) getColor:(VIC::ColorScheme)scheme nr:(int)nr;
- (void) setColor:(int)color rgba:(NSColor *)rgba;
- (void) setColorInt:(int)color rgba:(int)rgba;
- (void) setColorScheme:(VIC::ColorScheme)scheme;

- (uint16_t) getMemoryBankAddr;
- (void) setMemoryBankAddr:(uint16_t)addr;
- (uint16_t) getScreenMemoryAddr;
- (void) setScreenMemoryAddr:(uint16_t)addr;
- (uint16_t) getCharacterMemoryAddr;
- (void) setCharacterMemoryAddr:(uint16_t)addr;

- (int) getDisplayMode;
- (void) setDisplayMode:(int)mode;
- (int) getScreenGeometry;
- (void) setScreenGeometry:(int)mode;
- (int) getHorizontalRasterScroll;
- (void) setHorizontalRasterScroll:(int)offset;
- (int) getVerticalRasterScroll;
- (void) setVerticalRasterScroll:(int)offset;

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

- (uint16_t) getRasterLine;
- (void) setRasterLine:(uint16_t)line;
- (uint16_t) getRasterInterruptLine;
- (void) setRasterInterruptLine:(uint16_t)line;
- (bool) getRasterInterruptFlag;
- (void) setRasterInterruptFlag:(bool)b;
- (void) toggleRasterInterruptFlag;

- (bool) hideSprites;
- (void) setHideSprites:(bool)b;
- (bool) showIrqLines;
- (void) setShowIrqLines:(bool)b;
- (bool) showDmaLines;
- (void) setShowDmaLines:(bool)b;

@end

// --------------------------------------------------------------------------
//                                     CIA
// --------------------------------------------------------------------------

@interface CIAProxy : NSObject {
	CIA *cia;
}

- (id) initWithCIA:(CIA *)c;
- (uint8_t) getDataPortA;
- (void) setDataPortA:(uint8_t)v;
- (uint8_t) getDataPortDirectionA;
- (void) setDataPortDirectionA:(uint8_t)v;
- (uint16_t) getTimerA;
- (void) setTimerA:(uint16_t)v;
- (uint16_t) getTimerLatchA;
- (void) setTimerLatchA:(uint16_t)v;
- (bool) getStartFlagA;
- (void) setStartFlagA:(bool)b;
- (void) toggleStartFlagA;
- (bool) getOneShotFlagA;
- (void) setOneShotFlagA:(bool)b;
- (void) toggleOneShotFlagA;
- (bool) getUnderflowFlagA;
- (void) setUnderflowFlagA:(bool)b;
- (void) toggleUnderflowFlagA;
- (bool) getPendingSignalFlagA;
- (void) setPendingSignalFlagA:(bool)b;
- (void) togglePendingSignalFlagA;
- (bool) getInterruptEnableFlagA;
- (void) setInterruptEnableFlagA:(bool)b;
- (void) toggleInterruptEnableFlagA;

- (uint8_t) getDataPortB;
- (void) setDataPortB:(uint8_t)v;
- (uint8_t) getDataPortDirectionB;
- (void) setDataPortDirectionB:(uint8_t)v;
- (uint16_t) getTimerB;
- (void) setTimerB:(uint16_t)v;
- (uint16_t) getTimerLatchB;
- (void) setTimerLatchB:(uint16_t)v;
- (bool) getStartFlagB;
- (void) setStartFlagB:(bool)b;
- (void) toggleStartFlagB;
- (bool) getOneShotFlagB;
- (void) setOneShotFlagB:(bool)b;
- (void) toggleOneShotFlagB;
- (bool) getUnderflowFlagB;
- (void) setUnderflowFlagB:(bool)b;
- (void) toggleUnderflowFlagB;
- (bool) getPendingSignalFlagB;
- (void) setPendingSignalFlagB:(bool)b;
- (void) togglePendingSignalFlagB;
- (bool) getInterruptEnableFlagB;
- (void) setInterruptEnableFlagB:(bool)b;
- (void) toggleInterruptEnableFlagB;

- (uint8_t) getTodHours;
- (void) setTodHours:(uint8_t)value;
- (uint8_t) getTodMinutes;
- (void) setTodMinutes:(uint8_t)value;
- (uint8_t) getTodSeconds;
- (void) setTodSeconds:(uint8_t)value;
- (uint8_t) getTodTenth;
- (void) setTodTenth:(uint8_t)value;

- (uint8_t) getAlarmHours;
- (void) setAlarmHours:(uint8_t)value;
- (uint8_t) getAlarmMinutes;
- (void) setAlarmMinutes:(uint8_t)value;
- (uint8_t) getAlarmSeconds;
- (void) setAlarmSeconds:(uint8_t)value;
- (uint8_t) getAlarmTenth;
- (void) setAlarmTenth:(uint8_t)value;
- (bool) todIsInterruptEnabled;
- (void) todSetInterruptEnabled:(bool)b;

@end 

// --------------------------------------------------------------------------
//                                  Keyboard
// --------------------------------------------------------------------------

@interface KeyboardProxy : NSObject {
	Keyboard *keyboard;
}

- (id) initWithKeyboard:(Keyboard *)kb;
- (void) pressRunstopKey;
- (void) releaseRunstopKey;
- (void) pressCommodoreKey;
- (void) releaseCommodoreKey;
- (void) typeFormat;

@end 

// --------------------------------------------------------------------------
//                                    SID
// --------------------------------------------------------------------------

@interface SIDProxy : NSObject {
	SID *sid;
}

- (id) initWithSID:(SID *)s;
- (float) getVolumeControl;
- (void) setVolumeControl:(float)value;

@end

// --------------------------------------------------------------------------
//                                    VC1541
// -------------------------------------------------------------------------

@interface VC1541Proxy : NSObject {
	// CPUProxy cpuproxy;
	VC1541 *vc1541;
}

- (id) initWithComponent:(VC1541 *)vc1541;
// - (bool) loadVC1541Rom:(NSString *)filename;

@end








@interface C64Proxy : NSObject {	
	
	// Sub proxys
	CPUProxy *cpuproxy;
	VICProxy *vicproxy;
	CIAProxy *ciaproxy1;
	CIAProxy *ciaproxy2;
	SIDProxy *sidproxy;
	KeyboardProxy *keyboardproxy;

	// ListenerProxy *listener;	
	C64 *c64;
	CIA *cia[3];
	IEC *iec;
	// CPU *cpu;    // CPU to observe (can be switched between C64 and VC1541)
	Memory *mem; // Memory to observe (can be switched between C64 and VC1541)
	AudioDevice *audioDevice;
}

// Initialization
- (id) initWithDocument:(MyDocument *)d;
- (id) initWithDocument:(MyDocument *)d withScreen:(VICScreen *)s;
- (void) release;
- (C64 *) getC64;

//! Getter
- (CPUProxy *) cpu;
- (VICProxy *) vic;
- (SIDProxy *) sid;
- (CIAProxy *) cia:(int)num;
- (KeyboardProxy *) keyboard;

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

// - (bool) cpuTracingEnabled;
//- (void) cpuSetTraceMode:(bool)b;
- (bool) iecTracingEnabled;
- (void) iecSetTraceMode:(bool)b;
- (bool) vc1541CpuTracingEnabled;
- (void) vc1541CpuSetTraceMode:(bool)b;
- (bool) viaTracingEnabled;
- (void) viaSetTraceMode:(bool)b;

- (void) dumpC64;
// - (void) dumpC64CPU;
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

- (bool) c64GetWarpMode;
- (void) c64SetWarpMode:(bool)b;
- (long) c64GetCycles;

// CPU

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


- (void) fastReset;

// audio hardware
- (void) enableAudio;
- (void) disableAudio;


- (void) keyboardPressRunstopRestore;

// Keyboard
#if 0
- (void) keyboardPressRunstopKey;
- (void) keyboardReleaseRunstopKey;
- (void) keyboardPressCommodoreKey;
- (void) keyboardReleaseCommodoreKey;
- (void) keyboardTypeFormat;
#endif

// Drive
- (void) ejectDisk;
- (bool) isDriveConnected;
- (void) connectDrive;
- (void) disconnectDrive;

//- (void) disconnectDrive;

@end

#endif
