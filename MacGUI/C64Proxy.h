/*
 * (C) 2006 - 2011 Dirk W. Hoffmann. All rights reserved.
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
@class MyController;
@class AudioDevice;
@class V64Snapshot;


// --------------------------------------------------------------------------
//                                    CPU
// --------------------------------------------------------------------------

@interface CPUProxy : NSObject {
	CPU *cpu;
}

- (id) initWithCPU:(CPU *)c;
- (void) dump;
- (bool) tracingEnabled;
- (void) setTraceMode:(bool)b;
- (uint16_t) PC;
- (void) setPC:(uint16_t)pc;
- (uint8_t) SP;
- (void) setSP:(uint8_t)sp;
- (uint8_t) A;
- (void) setA:(uint8_t)a;
- (uint8_t) X;
- (void) setX:(uint8_t)x;
- (uint8_t) Y;
- (void) setY:(uint8_t)y;
- (bool) N;
- (void) setN:(bool)b;
- (bool) Z;
- (void) setZ:(bool)b;
- (bool) C;
- (void) setC:(bool)b;
- (bool) I;
- (void) setI:(bool)b;
- (bool) B;
- (void) setB:(bool)b;
- (bool) D;
- (void) setD:(bool)b;
- (bool) V;
- (void) setV:(bool)b;

- (uint16_t) peekPC;
- (uint8_t) lengthOfInstruction:(uint8_t)opcode;
- (uint8_t) lengthOfInstructionAtAddress:(uint16_t)addr;
- (uint8_t) lengthOfCurrentInstruction;
- (uint16_t) addressOfNextInstruction;
- (const char *) mnemonic:(uint8_t)opcode;
- (CPU::AddressingMode) addressingMode:(uint8_t)opcode;

- (int) topOfCallStack;
- (int) breakpoint:(int)addr;
- (void) setBreakpoint:(int)addr tag:(uint8_t)t;
- (void) setHardBreakpoint:(int)addr;
- (void) deleteHardBreakpoint:(int)addr;
- (void) toggleHardBreakpoint:(int)addr;
- (void) setSoftBreakpoint:(int)addr;
- (void) deleteSoftBreakpoint:(int)addr;
- (void) toggleSoftBreakpoint:(int)addr;

@end

// --------------------------------------------------------------------------
//                                  Memory
// --------------------------------------------------------------------------

@interface MemoryProxy : NSObject {
	Memory *mem;
}

- (id) initWithMemory:(Memory *)m;
- (void) dump;

- (uint8_t) peek:(uint16_t)addr;
- (uint16_t) peekWord:(uint16_t)addr;
- (uint8_t) peekFrom:(uint16_t)addr memtype:(Memory::MemoryType)source;
- (void) poke:(uint16_t)addr value:(uint8_t)val;
- (void) pokeTo:(uint16_t)addr value:(uint8_t)val memtype:(Memory::MemoryType)source;

- (bool) isValidAddr:(uint16_t)addr memtype:(Memory::MemoryType)source;

@end

// --------------------------------------------------------------------------
//                                    VIC
// --------------------------------------------------------------------------

@interface VICProxy : NSObject {
	VIC *vic;
}

- (id) initWithVIC:(VIC *)v;
- (void) dump;

- (void *) screenBuffer;
- (NSColor *) color:(int)nr;
- (VIC::ColorScheme) colorScheme;
- (void) setColorScheme:(VIC::ColorScheme)scheme;

- (uint16_t) memoryBankAddr;
- (void) setMemoryBankAddr:(uint16_t)addr;
- (uint16_t) screenMemoryAddr;
- (void) setScreenMemoryAddr:(uint16_t)addr;
- (uint16_t) characterMemoryAddr;
- (void) setCharacterMemoryAddr:(uint16_t)addr;

- (unsigned)totalScreenWidth;
- (unsigned)totalScreenHeight;
- (unsigned)firstVisibleLine;
- (unsigned)lastVisibleLine;

- (int) displayMode;
- (void) setDisplayMode:(int)mode;
- (int) screenGeometry;
- (void) setScreenGeometry:(int)mode;
- (int) horizontalRasterScroll;
- (void) setHorizontalRasterScroll:(int)offset;
- (int) verticalRasterScroll;
- (void) setVerticalRasterScroll:(int)offset;

- (bool) spriteVisibilityFlag:(int)nr;
- (void) setSpriteVisibilityFlag:(int)nr value:(bool)flag;
- (void) toggleSpriteVisibilityFlag:(int)nr;

- (bool) spriteSpriteCollisionFlag:(int)nr;
- (void) setSpriteSpriteCollisionFlag:(int)nr value:(bool)flag;
- (void) toggleSpriteSpriteCollisionFlag:(int)nr;

- (bool) spriteBackgroundCollisionFlag:(int)nr;
- (void) setSpriteBackgroundCollisionFlag:(int)nr value:(bool)flag;
- (void) toggleSpriteBackgroundCollisionFlag:(int)nr;

- (bool) spriteBackgroundPriorityFlag:(int)nr;
- (void) setSpriteBackgroundPriorityFlag:(int)nr value:(bool)flag;
- (void) toggleSpriteBackgroundPriorityFlag:(int)nr;

- (bool) spriteMulticolorFlag:(int)nr;
- (void) setSpriteMulticolorFlag:(int)nr value:(bool)flag;
- (void) toggleSpriteMulticolorFlag:(int)nr;

- (bool) spriteStretchXFlag:(int)nr;
- (void) setSpriteStretchXFlag:(int)nr value:(bool)flag;
- (void) toggleSpriteStretchXFlag:(int)nr;

- (bool) spriteStretchYFlag:(int)nr;
- (void) setSpriteStretchYFlag:(int)nr value:(bool)flag;
- (void) toggleSpriteStretchYFlag:(int)nr;

- (int) spriteX:(int)nr;
- (void) setSpriteX:(int)nr value:(int)x;
- (int) spriteY:(int)nr;
- (void) setSpriteY:(int)nr value:(int)y;
- (int) spriteColor:(int)nr;
- (void) setSpriteColor:(int)nr value:(int)c;

- (uint16_t) rasterline;
- (void) setRasterline:(uint16_t)line;
- (uint16_t) rasterInterruptLine;
- (void) setRasterInterruptLine:(uint16_t)line;
- (bool) rasterInterruptFlag;
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
- (void) dump;
- (bool) tracingEnabled;
- (void) setTraceMode:(bool)b;

- (uint8_t) dataPortA;
- (void) setDataPortA:(uint8_t)v;
- (uint8_t) dataPortDirectionA;
- (void) setDataPortDirectionA:(uint8_t)v;
- (uint16_t) timerA;
- (void) setTimerA:(uint16_t)v;
- (uint16_t) timerLatchA;
- (void) setTimerLatchA:(uint16_t)v;
- (bool) startFlagA;
- (void) setStartFlagA:(bool)b;
- (void) toggleStartFlagA;
- (bool) oneShotFlagA;
- (void) setOneShotFlagA:(bool)b;
- (void) toggleOneShotFlagA;
- (bool) underflowFlagA;
- (void) setUnderflowFlagA:(bool)b;
- (void) toggleUnderflowFlagA;
- (bool) pendingSignalFlagA;
- (void) setPendingSignalFlagA:(bool)b;
- (void) togglePendingSignalFlagA;
- (bool) interruptEnableFlagA;
- (void) setInterruptEnableFlagA:(bool)b;
- (void) toggleInterruptEnableFlagA;

- (uint8_t) dataPortB;
- (void) setDataPortB:(uint8_t)v;
- (uint8_t) dataPortDirectionB;
- (void) setDataPortDirectionB:(uint8_t)v;
- (uint16_t) timerB;
- (void) setTimerB:(uint16_t)v;
- (uint16_t) timerLatchB;
- (void) setTimerLatchB:(uint16_t)v;
- (bool) startFlagB;
- (void) setStartFlagB:(bool)b;
- (void) toggleStartFlagB;
- (bool) oneShotFlagB;
- (void) setOneShotFlagB:(bool)b;
- (void) toggleOneShotFlagB;
- (bool) underflowFlagB;
- (void) setUnderflowFlagB:(bool)b;
- (void) toggleUnderflowFlagB;
- (bool) pendingSignalFlagB;
- (void) setPendingSignalFlagB:(bool)b;
- (void) togglePendingSignalFlagB;
- (bool) interruptEnableFlagB;
- (void) setInterruptEnableFlagB:(bool)b;
- (void) toggleInterruptEnableFlagB;

- (uint8_t) todHours;
- (void) setTodHours:(uint8_t)value;
- (uint8_t) todMinutes;
- (void) setTodMinutes:(uint8_t)value;
- (uint8_t) todSeconds;
- (void) setTodSeconds:(uint8_t)value;
- (uint8_t) todTenth;
- (void) setTodTenth:(uint8_t)value;

- (uint8_t) alarmHours;
- (void) setAlarmHours:(uint8_t)value;
- (uint8_t) alarmMinutes;
- (void) setAlarmMinutes:(uint8_t)value;
- (uint8_t) alarmSeconds;
- (void) setAlarmSeconds:(uint8_t)value;
- (uint8_t) alarmTenth;
- (void) setAlarmTenth:(uint8_t)value;
- (bool) isTodInterruptEnabled;
- (void) setTodInterruptEnabled:(bool)b;

@end 

// --------------------------------------------------------------------------
//                                  Keyboard
// --------------------------------------------------------------------------

@interface KeyboardProxy : NSObject {
	Keyboard *keyboard;
}

- (id) initWithKeyboard:(Keyboard *)kb;
- (void) dump;
- (void) pressRunstopKey;
- (void) releaseRunstopKey;
- (void) pressCommodoreKey;
- (void) releaseCommodoreKey;
- (void) typeFormat;
- (void) typeRun;

@end 

// --------------------------------------------------------------------------
//                                    SID
// --------------------------------------------------------------------------

@interface SIDProxy : NSObject {
	SIDWrapper *sid;
}

- (id) initWithSID:(SIDWrapper *)s;
- (void) dump;
- (bool) audioFilter;
- (void) setAudioFilter:(bool)b;
- (bool) reSID;
- (void) setReSID:(bool)b;
- (int) samplingMethod;
- (void) setSamplingMethod:(int)value;
- (int) chipModel;
- (void) setChipModel:(int)value;

@end

// --------------------------------------------------------------------------
//                                   IEC bus
// -------------------------------------------------------------------------

@interface IECProxy : NSObject {
	IEC *iec;
}

- (id) initWithIEC:(IEC *)bus;
- (void) dump;
- (bool) tracingEnabled;
- (void) setTraceMode:(bool)b;
- (bool) isDriveConnected;
- (void) connectDrive;
- (void) disconnectDrive;

@end

// --------------------------------------------------------------------------
//                                      VIA
// -------------------------------------------------------------------------

@interface VIAProxy : NSObject {
	VIA6522 *via;
}

- (id) initWithVIA:(VIA6522 *)v;
- (void) dump;
- (bool) tracingEnabled;
- (void) setTraceMode:(bool)b;

@end

// --------------------------------------------------------------------------
//                                    VC1541
// -------------------------------------------------------------------------

@interface VC1541Proxy : NSObject {
	VC1541 *vc1541;
	CPUProxy *cpu;
	MemoryProxy *mem;
	VIAProxy *via1;
	VIAProxy *via2;
}

@property (readonly) CPUProxy *cpu;
@property (readonly) MemoryProxy *mem;
@property (readonly) VIAProxy *via1;
@property (readonly) VIAProxy *via2;

- (id) initWithVC1541:(VC1541 *)vc;
- (VIAProxy *) via:(int)num;

- (void) dump;
- (bool) tracingEnabled;
- (void) setTraceMode:(bool)b;
- (void) ejectDisk;

@end


// --------------------------------------------------------------------------
//                                    C64
// -------------------------------------------------------------------------

@interface C64Proxy : NSObject {	
	
	C64 *c64;
	AudioDevice *audioDevice;

	// Sub proxys
	CPUProxy *cpu;
	MemoryProxy *mem;
	VICProxy *vic;
	CIAProxy *cia1;
	CIAProxy *cia2;
	SIDProxy *sid;
	KeyboardProxy *keyboard;
	IECProxy *iec;
	VC1541Proxy *vc1541;
	
	//! Is set to true when data gets transfered on the IEC bus
	BOOL iecBusIsBusy;
}

@property (readonly) C64 *c64;
@property (readonly) CPUProxy *cpu;
@property (readonly) MemoryProxy *mem;
@property (readonly) VICProxy *vic;
@property (readonly) CIAProxy *cia1;
@property (readonly) CIAProxy *cia2;
@property (readonly) SIDProxy *sid;
@property (readonly) KeyboardProxy *keyboard;
@property (readonly) IECProxy *iec;
@property (readonly) VC1541Proxy *vc1541;

@property BOOL iecBusIsBusy;

// Initialization
- (void) kill;

// Poll C64 to refresh internal state
- (void) refresh;

// Loadind and saving
- (void)loadFromSnapshot:(V64Snapshot *) snapshot;
- (void)saveToSnapshot:(V64Snapshot *) snapshot;

- (CIAProxy *) cia:(int)num;

- (void) dump;

- (Message *)message;

- (void) reset;
- (void) fastReset;
- (void) halt;
- (void) step;
- (bool) isRunnable;
- (void) run;
- (void) suspend;
- (void) resume; 
- (bool) isHalted;
- (bool) isRunning;
- (bool) isPAL;
- (bool) isNTSC;
- (void) setPAL;
- (void) setNTSC;
- (int) frameDelayOffset;
- (void) setFrameDelayOffset:(int)delay;
- (int) buildNr;

//- (int) numberOfMissingRoms;
- (uint8_t) missingRoms;
- (bool) loadBasicRom:(NSString *)filename;
- (bool) loadCharRom:(NSString *)filename;
- (bool) loadKernelRom:(NSString *)filename;
- (bool) loadVC1541Rom:(NSString *)filename;

- (bool) attachCartridge:(Cartridge *)c;
- (bool) detachCartridge;
- (bool) isCartridgeAttached;

- (bool) mountArchive:(D64Archive *)a;
- (bool) flushArchive:(Archive *)a item:(int)nr;

- (bool) warp;
- (void) setWarp:(bool)b;
- (bool) alwaysWarp;
- (void) setAlwaysWarp:(bool)b;
- (bool) warpLoad;
- (void) setWarpLoad:(bool)b;
- (long) cycles;

// Cheatbox
- (int) historicSnapshots;
- (uint8_t *) historicSnapshotFileContents:(int)nr;
- (unsigned) historicSnapshotFileContentsSize:(int)nr;
- (unsigned char *)historicSnapshotImageData:(int)nr;
- (time_t)historicSnapshotTimestamp:(int)nr;
- (bool)historicSnapshotIsPAL:(int)nr;
- (bool)revertToHistoricSnapshot:(int)nr;

// Joystick
- (void) switchInputDevice:(int)devNo;
- (void) switchInputDevices;
- (uint8_t) portAssignment:(int)devNo;
- (Joystick *) addJoystick;
- (void) removeJoystick:(Joystick *)joystick;

// Audio hardware
- (void) enableAudio;
- (void) disableAudio;

// User triggered interrupts
- (void) keyboardPressRunstopRestore;

@end


// --------------------------------------------------------------------------
//                                  Snapshot
// --------------------------------------------------------------------------

@interface V64Snapshot : NSObject {
	@private
	Snapshot *snapshot;
}

@property Snapshot *snapshot;

- (id) init;
- (id) initWithSnapshot:(Snapshot *)s;

+ (id) snapshotFromC64:(C64Proxy *)c64;
+ (id) snapshotFromSnapshot:(Snapshot *)snapshot;
+ (id) snapshotFromFile:(NSString *)path;
+ (id) snapshotFromBuffer:(const void *)buffer length:(unsigned)length;

- (uint8_t *)fileContents;
- (unsigned)fileContentsSize;
- (unsigned char *)imageData;
- (time_t)timeStamp;
	
- (bool) readDataFromFile:(NSString *)path;
- (bool) writeDataToFile:(NSString *)path;

@end

