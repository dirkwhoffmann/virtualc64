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
@class MyOpenGLView;


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

- (uint16_t) memoryBankAddr;
- (void) setMemoryBankAddr:(uint16_t)addr;
- (uint16_t) screenMemoryAddr;
- (void) setScreenMemoryAddr:(uint16_t)addr;
- (uint16_t) characterMemoryAddr;
- (void) setCharacterMemoryAddr:(uint16_t)addr;

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

- (int) spriteX:(int)nr;
- (void) setSpriteX:(int)nr value:(int)x;
- (int) spriteY:(int)nr;
- (void) setSpriteY:(int)nr value:(int)y;

- (int) spriteColor:(int)nr;
- (void) setSpriteColor:(int)nr value:(int)c;
- (bool) spriteMulticolorFlag:(int)nr;
- (void) setSpriteMulticolorFlag:(int)nr value:(bool)flag;
- (void) toggleSpriteMulticolorFlag:(int)nr;

- (bool) spriteStretchXFlag:(int)nr;
- (void) setSpriteStretchXFlag:(int)nr value:(bool)flag;
- (void) toggleSpriteStretchXFlag:(int)nr;
- (bool) spriteStretchYFlag:(int)nr;
- (void) setSpriteStretchYFlag:(int)nr value:(bool)flag;
- (void) toggleSpriteStretchYFlag:(int)nr;

- (bool) spriteSpriteCollisionFlag;
- (void) setSpriteSpriteCollisionFlag:(bool)flag;
- (void) toggleSpriteSpriteCollisionFlag;

- (bool) spriteBackgroundCollisionFlag;
- (void) setSpriteBackgroundCollisionFlag:(bool)flag;
- (void) toggleSpriteBackgroundCollisionFlag;

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

- (void) pressKey:(char)c;
- (void) releaseKey:(char)c;
- (void) pressRunstopKey;
- (void) releaseRunstopKey;
- (void) pressCommodoreKey;
- (void) releaseCommodoreKey;
- (void) pressClearKey;
- (void) releaseClearKey;
- (void) pressHomeKey;
- (void) releaseHomeKey;
- (void) pressInsertKey;
- (void) releaseInsertKey;

- (void) typeText:(NSString *)text;

@end 

// --------------------------------------------------------------------------
//                                 Joystick
// -------------------------------------------------------------------------

@interface JoystickProxy : NSObject {
    Joystick *joystick;
}

- (id) initWithJoystick:(Joystick *)joy;
- (void) dump;

@end

// --------------------------------------------------------------------------
//                                    SID
// --------------------------------------------------------------------------

@interface SIDProxy : NSObject {
	SIDWrapper *sid;
}

- (id) initWithSID:(SIDWrapper *)s;
- (void) dump;

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
//                                 Expansion port
// -------------------------------------------------------------------------

@interface ExpansionPortProxy : NSObject {
    ExpansionPort *expansionPort;
}

- (id) initWithExpansionPort:(ExpansionPort *)v;
- (void) dump;
- (bool) cartridgeAttached; 
- (int) cartridgeType;
- (unsigned) numberOfChips;
- (unsigned) numberOfBytes;

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
- (bool) hasRedLED;
- (bool) hasDisk;
- (bool) writeProtection;
- (void) setWriteProtection:(bool)b;
- (bool) soundMessagesEnabled;
- (void) setSendSoundMessages:(bool)b;
- (bool) exportToD64:(NSString *)path;
- (D64Archive *)archiveFromDrive;
- (void) ejectDisk;


- (void) playSound:(NSString *)name volume:(float)v;

@end


// --------------------------------------------------------------------------
//                                    C64
// -------------------------------------------------------------------------

@interface C64Proxy : NSObject {	
	
    IBOutlet MyOpenGLView* screen; 

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
    JoystickProxy *joystick1;
    JoystickProxy *joystick2;
	IECProxy *iec;
    ExpansionPortProxy *expansionport;
	VC1541Proxy *vc1541;
	
	//! Indicates that data is transmitted on the IEC bus
	BOOL iecBusIsBusy;

    //! Currently used color scheme
    int colorScheme;
}

@property (readonly) C64 *c64;
@property (readonly) CPUProxy *cpu;
@property (readonly) MemoryProxy *mem;
@property (readonly) VICProxy *vic;
@property (readonly) CIAProxy *cia1;
@property (readonly) CIAProxy *cia2;
@property (readonly) SIDProxy *sid;
@property (readonly) KeyboardProxy *keyboard;
@property (readonly) JoystickProxy *joystick1;
@property (readonly) JoystickProxy *joystick2;
@property (readonly) IECProxy *iec;
@property (readonly) ExpansionPortProxy *expansionport;
@property (readonly) VC1541Proxy *vc1541;

@property BOOL iecBusIsBusy;

// Initialization
- (void) kill;

// Hardware configuration
- (bool) reSID;
- (void) setReSID:(bool)b;
- (bool) audioFilter;
- (void) setAudioFilter:(bool)b;
- (int) samplingMethod;
- (void) setSamplingMethod:(int)value;
- (int) chipModel;
- (void) setChipModel:(int)value;

// Loadind and saving
- (void)_loadFromSnapshot:(Snapshot *) snapshot;
- (void)loadFromSnapshot:(V64Snapshot *) snapshot;
- (void)_saveToSnapshot:(Snapshot *) snapshot;
- (void)saveToSnapshot:(V64Snapshot *) snapshot;

- (CIAProxy *) cia:(int)num;

- (void) dump;

- (Message *)message;
- (void) putMessage:(int)msg;

- (void) reset;
- (void) ping;
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

//- (int) numberOfMissingRoms;
- (uint8_t) missingRoms;
- (bool) isBasicRom:(NSString *)filename;
- (bool) loadBasicRom:(NSString *)filename;
- (bool) isCharRom:(NSString *)filename;
- (bool) loadCharRom:(NSString *)filename;
- (bool) isKernelRom:(NSString *)filename;
- (bool) loadKernelRom:(NSString *)filename;
- (bool) isVC1541Rom:(NSString *)filename;
- (bool) loadVC1541Rom:(NSString *)filename;
- (bool) isRom:(NSString *)filename;
- (bool) loadRom:(NSString *)filename;

- (bool) attachCartridge:(Cartridge *)c;
- (void) detachCartridge;
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
- (long) frames;

// Cheatbox
- (int) historicSnapshots;
- (int) historicSnapshotHeaderSize:(int)nr;
- (uint8_t *) historicSnapshotHeader:(int)nr;
- (int) historicSnapshotDataSize:(int)nr;
- (uint8_t *) historicSnapshotData:(int)nr;

- (unsigned char *)historicSnapshotImageData:(int)nr;
- (unsigned)historicSnapshotImageWidth:(int)nr;
- (unsigned)historicSnapshotImageHeight:(int)nr;
- (time_t)historicSnapshotTimestamp:(int)nr;
- (bool)revertToHistoricSnapshot:(int)nr;

// Joystick
- (Joystick *) joystick:(int)nr; 

// Audio hardware
- (void) enableAudio;
- (void) disableAudio;

// User triggered interrupts
- (void) keyboardPressRunstopRestore;

@end


// --------------------------------------------------------------------------
//                                  Snapshot
// --------------------------------------------------------------------------

// Get rid of this class. Too much overhead and no real use


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

- (unsigned char *)imageData;
- (time_t)timeStamp;
	
- (bool) readDataFromFile:(NSString *)path;
- (bool) writeDataToFile:(NSString *)path;

@end

