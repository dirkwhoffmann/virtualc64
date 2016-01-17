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
@class C64Proxy;
@class SnapshotProxy;
@class D64ArchiveProxy; 
@class MyMetalView;
@class ArchiveProxy;
@class TAPContainerProxy;
class JoystickManager;

// --------------------------------------------------------------------------
//                                    CPU
// --------------------------------------------------------------------------

@interface CPUProxy : NSObject {
        
	CPU *cpu;
}

- (instancetype) initWithCPU:(CPU *)c;
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

- (instancetype) initWithMemory:(Memory *)m;
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

- (instancetype) initWithVIC:(VIC *)v;
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

- (instancetype) initWithCIA:(CIA *)c;
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

- (instancetype) initWithKeyboard:(Keyboard *)kb;
- (void) dump;

- (void) pressKey:(int)c;
- (void) releaseKey:(int)c;
- (void) pressRunstopKey;
- (void) releaseRunstopKey;
- (void) pressShiftRunstopKey;
- (void) releaseShiftRunstopKey;
- (void) pressRestoreKey;
- (void) releaseRestoreKey;
- (void) pressRunstopRestoreKey;
- (void) releaseRunstopRestoreKey;
- (void) pressCommodoreKey;
- (void) releaseCommodoreKey;
- (void) pressClearKey;
- (void) releaseClearKey;
- (void) pressHomeKey;
- (void) releaseHomeKey;
- (void) pressInsertKey;
- (void) releaseInsertKey;

- (void) typeText:(NSString *)text;
- (void) typeText:(NSString *)text withDelay:(int)delay;

@end 

// --------------------------------------------------------------------------
//                                 Joystick
// -------------------------------------------------------------------------

#if 0
@interface JoystickManagerProxy : NSObject {
    JoystickManager *manager;
}

- (instancetype) initWithC64:(C64Proxy *)c64;
// - (instancetype) initWithJoystickManager:(JoystickManager *)m;
// - (instancetype) init;

@end
#endif

@interface JoystickProxy : NSObject {
    Joystick *joystick;
}

- (instancetype) initWithJoystick:(Joystick *)joy;

- (void) setButtonPressed:(BOOL)pressed;
- (void) setAxisX:(JoystickDirection)state;
- (void) setAxisY:(JoystickDirection)state;

- (void) dump;

@end

// --------------------------------------------------------------------------
//                                    SID
// --------------------------------------------------------------------------

@interface SIDProxy : NSObject {
	SIDWrapper *sid;
}

- (instancetype) initWithSID:(SIDWrapper *)s;
- (void) dump;

@end

// --------------------------------------------------------------------------
//                                   IEC bus
// -------------------------------------------------------------------------

@interface IECProxy : NSObject {
	IEC *iec;
}

- (instancetype) initWithIEC:(IEC *)bus;
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

- (instancetype) initWithExpansionPort:(ExpansionPort *)v;
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

- (instancetype) initWithVIA:(VIA6522 *)v;
- (void) dump;
- (bool) tracingEnabled;
- (void) setTraceMode:(bool)b;

@end

// --------------------------------------------------------------------------
//                                5,25" diskette
// -------------------------------------------------------------------------

@interface Disk525Proxy : NSObject {
    Disk525 *disk;
}

- (instancetype)initWithDisk525:(Disk525 *)d;

- (BOOL)isWriteProtected;
- (void)setWriteProtection:(BOOL)b;
- (BOOL)isModified;
- (void)setModified:(BOOL)b;
- (NSInteger)numTracks;

@end

// --------------------------------------------------------------------------
//                                    VC1541
// -------------------------------------------------------------------------

@interface VC1541Proxy : NSObject {
    
	VC1541 *vc1541;
    
    // sub proxys
	CPUProxy *cpu;
	MemoryProxy *mem;
	VIAProxy *via1;
	VIAProxy *via2;
    Disk525Proxy *disk;
}

@property (readonly) CPUProxy *cpu;
@property (readonly) MemoryProxy *mem;
@property (readonly) VIAProxy *via1;
@property (readonly) VIAProxy *via2;
@property (readonly) Disk525Proxy *disk;

- (instancetype) initWithVC1541:(VC1541 *)vc;
- (VIAProxy *) via:(int)num;

- (void) dump;
- (bool) tracingEnabled;
- (void) setTraceMode:(bool)b;
- (bool) hasRedLED;
- (bool) hasDisk;
- (void) ejectDisk;
- (bool) writeProtection;
- (void) setWriteProtection:(bool)b;
- (bool) DiskModified;
- (void) setDiskModified:(bool)b;
- (bool) bitAccuracy;
- (void) setBitAccuracy:(bool)b;
- (bool) soundMessagesEnabled;
- (void) setSendSoundMessages:(bool)b;
- (D64ArchiveProxy *) convertToD64;
- (bool) exportToD64:(NSString *)path;

- (void) playSound:(NSString *)name volume:(float)v;

@end

// --------------------------------------------------------------------------
//                                  Datasette
// --------------------------------------------------------------------------

@interface DatasetteProxy : NSObject {
    Datasette *datasette;
}

- (instancetype) initWithDatasette:(Datasette *)kb;
- (void) dump;

- (bool) hasTape;
- (void) pressPlay;
- (void) pressStop;
- (void) pressRewind;
- (void) ejectTape;
- (NSInteger) getType; 
- (long) durationInCycles;
- (int) durationInSeconds;
- (int) head;
- (long) headInCycles;
- (int) headInSeconds;
- (void) setHeadInCycles:(long)value;
- (BOOL) motor;
- (BOOL) playKey;

@end

// -------------------------------------------------------------------------
//                                    C64
// -------------------------------------------------------------------------

@interface C64Proxy : NSObject {	

	C64 *c64;
	AudioDevice *audioDevice;
    JoystickManager *joystickManager;
    
	// Sub component proxys
	CPUProxy *cpu;
	MemoryProxy *mem;
	VICProxy *vic;
	CIAProxy *cia1;
	CIAProxy *cia2;
	SIDProxy *sid;
	KeyboardProxy *keyboard;
    JoystickProxy *joystickA;
    JoystickProxy *joystickB;
	IECProxy *iec;
    ExpansionPortProxy *expansionport;
	VC1541Proxy *vc1541;
    DatasetteProxy *datasette;

	//! Indicates that data is transmitted on the IEC bus
	BOOL iecBusIsBusy;

    //! Indicates that data is transmitted on the datasette data line
    BOOL tapeBusIsBusy;

    //! Currently used color scheme
    int colorScheme;
}

@property (strong,readonly) MyMetalView *metalScreen;
@property (readonly) CPUProxy *cpu;
@property (readonly) MemoryProxy *mem;
@property (readonly) VICProxy *vic;
@property (readonly) CIAProxy *cia1;
@property (readonly) CIAProxy *cia2;
@property (readonly) SIDProxy *sid;
@property (readonly) KeyboardProxy *keyboard;
@property (readonly) JoystickProxy *joystickA;
@property (readonly) JoystickProxy *joystickB;
@property (readonly) IECProxy *iec;
@property (readonly) ExpansionPortProxy *expansionport;
@property (readonly) VC1541Proxy *vc1541;
@property (readonly) DatasetteProxy *datasette;

@property BOOL iecBusIsBusy;
@property BOOL tapeBusIsBusy;

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
- (void) rampUp;
- (void) rampUpFromZero;
- (void) rampDown;

// Loadind and saving
- (void)_loadFromSnapshot:(Snapshot *) snapshot;
- (void)loadFromSnapshot:(SnapshotProxy *) snapshot;
- (void)_saveToSnapshot:(Snapshot *) snapshot;
- (void)saveToSnapshot:(SnapshotProxy *) snapshot;

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

- (bool) mountArchive:(ArchiveProxy *)a;
- (bool) flushArchive:(ArchiveProxy *)a item:(int)nr;

- (bool) insertTape:(TAPContainerProxy *)a;

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

// Joystick handling
- (BOOL)joystickIsPluggedIn:(int)nr;
- (void)bindJoystickToPortA:(int)nr;
- (void)bindJoystickToPortB:(int)nr;
- (void)unbindJoysticksFromPortA;
- (void)unbindJoysticksFromPortB;

// Audio hardware
- (void) enableAudio;
- (void) disableAudio;

@end


// --------------------------------------------------------------------------
//                                  Snapshot
// --------------------------------------------------------------------------

@interface SnapshotProxy : NSObject
{
	@private Snapshot *snapshot;
}

@property Snapshot *snapshot;

- (instancetype) init;
- (instancetype) initWithSnapshot:(Snapshot *)s;
+ (instancetype) snapshotFromSnapshot:(Snapshot *)snapshot;
+ (instancetype) snapshotFromFile:(NSString *)path;
+ (instancetype) snapshotFromBuffer:(const void *)buffer length:(unsigned)length;

- (bool) readDataFromFile:(NSString *)path;
- (bool) writeDataToFile:(NSString *)path;

@end

// --------------------------------------------------------------------------
//                                Archive
// --------------------------------------------------------------------------

@interface ArchiveProxy : NSObject
{
    Archive *archive;
}

@property Archive *archive;

- (instancetype) initWithArchive:(Archive *)s;

- (NSString *)getPath;
- (NSString *)getName;
- (NSInteger)getType;
- (NSInteger)getNumberOfItems; 
- (BOOL)writeToFile:(NSString *)filename; 

@end

@interface T64ArchiveProxy : ArchiveProxy
{
}
+ (BOOL) isT64File:(NSString *)filename;
+ (instancetype) archiveFromT64File:(NSString *)filename;
+ (instancetype) archiveFromArchive:(ArchiveProxy *)otherArchive;
@end

@interface D64ArchiveProxy : ArchiveProxy
{
}
+ (BOOL) isD64File:(NSString *)filename;
+ (instancetype) archiveFromD64File:(NSString *)filename;
+ (instancetype) archiveFromArbitraryFile:(NSString *)filename;
+ (instancetype) archiveFromD64Archive:(D64ArchiveProxy *)archive;
+ (instancetype) archiveFromArchive:(ArchiveProxy *)archive;
@end

@interface PRGArchiveProxy : ArchiveProxy
{
}
+ (BOOL) isPRGFile:(NSString *)filename;
+ (instancetype) archiveFromPRGFile:(NSString *)filename;
+ (instancetype) archiveFromArchive:(ArchiveProxy *)otherArchive;
@end

@interface P00ArchiveProxy : ArchiveProxy
{
}
+ (BOOL) isP00File:(NSString *)filename;
+ (instancetype) archiveFromP00File:(NSString *)filename;
+ (instancetype) archiveFromArchive:(ArchiveProxy *)otherArchive;
@end

@interface G64ArchiveProxy : ArchiveProxy
{
}
+ (BOOL) isG64File:(NSString *)filename;
+ (instancetype) archiveFromG64File:(NSString *)filename;
@end

@interface NIBArchiveProxy : ArchiveProxy
{
}
+ (BOOL) isNIBFile:(NSString *)filename;
+ (instancetype) archiveFromNIBFile:(NSString *)filename;
@end


@interface TAPContainerProxy : NSObject
{
    TAPArchive *container;
}

@property TAPArchive *container;

- (instancetype) initWithTAPContainer:(TAPArchive *)a;

+ (BOOL) isTAPFile:(NSString *)filename;
+ (instancetype) containerFromTAPFile:(NSString *)filename;

- (NSString *)getPath;
- (NSString *)getName;
- (NSInteger)getType;
- (NSInteger)TAPversion;

@end

