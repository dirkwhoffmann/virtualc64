/*
 * (C) 2006 - 2017 Dirk W. Hoffmann. All rights reserved.
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
#import "C64.h"

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

// Forward declarations of wrappers for C++ classes.
// We wrap classes into normal C structs to avoid any reference to C++ here.

struct C64Wrapper;
struct CpuWrapper;
struct MemoryWrapper;
struct VicWrapper;
struct CiaWrapper;
struct KeyboardWrapper;
struct JoystickWrapper;
struct SidWrapperWrapper; // Yes, it's a double wrapper
struct IecWrapper;
struct ExpansionPortWrapper;
struct Via6522Wrapper;
struct Disk525Wrapper;
struct Vc1541Wrapper;
struct DatasetteWrapper;
struct SnapshotWrapper;
struct ArchiveWrapper;
struct TAPArchive;


// --------------------------------------------------------------------------
//                                    CPU
// --------------------------------------------------------------------------

@interface CPUProxy : NSObject {
        
	CpuWrapper *wrapper;
}

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
- (AddressingMode) addressingMode:(uint8_t)opcode;

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
    
	MemoryWrapper *wrapper;
}

- (void) dump;

- (uint8_t) peek:(uint16_t)addr;
- (uint16_t) peekWord:(uint16_t)addr;
- (uint8_t) peekFrom:(uint16_t)addr memtype:(MemoryType)source;
- (void) poke:(uint16_t)addr value:(uint8_t)val;
- (void) pokeTo:(uint16_t)addr value:(uint8_t)val memtype:(MemoryType)source;
- (bool) isValidAddr:(uint16_t)addr memtype:(MemoryType)source;

@end

// --------------------------------------------------------------------------
//                                    VIC
// --------------------------------------------------------------------------

@interface VICProxy : NSObject {
    
	VicWrapper *wrapper;
}

- (void) dump;

- (void *) screenBuffer;

- (NSColor *) color:(int)nr;
- (void) setColor:(unsigned)nr rgba:(int)rgba;

- (uint16_t) memoryBankAddr;
- (void) setMemoryBankAddr:(uint16_t)addr;
- (uint16_t) screenMemoryAddr;
- (void) setScreenMemoryAddr:(uint16_t)addr;
- (uint16_t) characterMemoryAddr;
- (void) setCharacterMemoryAddr:(uint16_t)addr;

- (int) displayMode;
- (void) setDisplayMode:(long)mode;
- (int) screenGeometry;
- (void) setScreenGeometry:(long)mode;
- (int) horizontalRasterScroll;
- (void) setHorizontalRasterScroll:(int)offset;
- (int) verticalRasterScroll;
- (void) setVerticalRasterScroll:(int)offset;

- (bool) spriteVisibilityFlag:(NSInteger)nr;
- (void) setSpriteVisibilityFlag:(NSInteger)nr value:(bool)flag;
- (void) toggleSpriteVisibilityFlag:(NSInteger)nr;

- (int) spriteX:(NSInteger)nr;
- (void) setSpriteX:(NSInteger)nr value:(int)x;
- (int) spriteY:(NSInteger)nr;
- (void) setSpriteY:(NSInteger)nr value:(int)y;

- (int) spriteColor:(NSInteger)nr;
- (void) setSpriteColor:(NSInteger)nr value:(int)c;
- (bool) spriteMulticolorFlag:(NSInteger)nr;
- (void) setSpriteMulticolorFlag:(NSInteger)nr value:(bool)flag;
- (void) toggleSpriteMulticolorFlag:(NSInteger)nr;

- (bool) spriteStretchXFlag:(NSInteger)nr;
- (void) setSpriteStretchXFlag:(NSInteger)nr value:(bool)flag;
- (void) toggleSpriteStretchXFlag:(NSInteger)nr;
- (bool) spriteStretchYFlag:(NSInteger)nr;
- (void) setSpriteStretchYFlag:(NSInteger)nr value:(bool)flag;
- (void) toggleSpriteStretchYFlag:(NSInteger)nr;

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
    
	CiaWrapper *wrapper;
}

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
    
    KeyboardWrapper *wrapper;
}

- (void) dump;

- (void) pressKey:(int)c;
- (void) releaseKey:(int)c;
- (void) pressRunstopKey;
- (void) releaseRunstopKey;
- (void) pressShiftRunstopKey;
- (void) releaseShiftRunstopKey;
- (void) pressRestoreKey;
- (void) releaseRestoreKey;
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


@interface JoystickProxy : NSObject {
    
    JoystickWrapper *wrapper;
}

- (void) setButtonPressed:(BOOL)pressed;
- (void) setAxisX:(JoystickDirection)state;
- (void) setAxisY:(JoystickDirection)state;

- (void) dump;

@end

// --------------------------------------------------------------------------
//                                    SID
// --------------------------------------------------------------------------

@interface SIDProxy : NSObject {
    
	SidWrapperWrapper *wrapper;
}

- (void) dump;

@end

// --------------------------------------------------------------------------
//                                   IEC bus
// -------------------------------------------------------------------------

@interface IECProxy : NSObject {

    IecWrapper *wrapper;
}

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
    
    ExpansionPortWrapper *wrapper;
}

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
    
	Via6522Wrapper *wrapper;
}

- (void) dump;
- (bool) tracingEnabled;
- (void) setTraceMode:(bool)b;

@end

// --------------------------------------------------------------------------
//                                5,25" diskette
// -------------------------------------------------------------------------

@interface Disk525Proxy : NSObject {
    
    Disk525Wrapper *wrapper;
}

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
    
	Vc1541Wrapper *wrapper;
    
    // sub proxys
	CPUProxy *cpu;
	MemoryProxy *mem;
	VIAProxy *via1;
	VIAProxy *via2;
    Disk525Proxy *disk;
}

@property (readonly) Vc1541Wrapper *wrapper;
@property (readonly) CPUProxy *cpu;
@property (readonly) MemoryProxy *mem;
@property (readonly) VIAProxy *via1;
@property (readonly) VIAProxy *via2;
@property (readonly) Disk525Proxy *disk;

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
// - (D64ArchiveProxy *) convertToD64;
- (bool) exportToD64:(NSString *)path;

- (void) playSound:(NSString *)name volume:(float)v;

@end

// --------------------------------------------------------------------------
//                                  Datasette
// --------------------------------------------------------------------------

@interface DatasetteProxy : NSObject {
    
    DatasetteWrapper *wrapper;
}

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
    
	C64Wrapper *wrapper;
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
    long colorScheme;
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
- (void) setSamplingMethod:(long)value;
- (int) chipModel;
- (void) setChipModel:(long)value;
- (void) rampUp;
- (void) rampUpFromZero;
- (void) rampDown;

// Loadind and saving
- (void)_loadFromSnapshotWrapper:(SnapshotWrapper *) snapshot;
- (void)loadFromSnapshot:(SnapshotProxy *) snapshot;
- (void)_saveToSnapshotWrapper:(SnapshotWrapper *) snapshot;
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
- (bool) flushArchive:(ArchiveProxy *)a item:(NSInteger)nr;

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
- (int) historicSnapshotHeaderSize:(NSInteger)nr;
- (uint8_t *) historicSnapshotHeader:(NSInteger)nr;
- (int) historicSnapshotDataSize:(NSInteger)nr;
- (uint8_t *) historicSnapshotData:(NSInteger)nr;

- (unsigned char *)historicSnapshotImageData:(NSInteger)nr;
- (unsigned)historicSnapshotImageWidth:(NSInteger)nr;
- (unsigned)historicSnapshotImageHeight:(NSInteger)nr;
- (time_t)historicSnapshotTimestamp:(NSInteger)nr;
- (bool)revertToHistoricSnapshot:(NSInteger)nr;

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

@interface SnapshotProxy : NSObject {
    
	SnapshotWrapper *wrapper;
}

- (instancetype) init;
+ (instancetype) snapshotFromFile:(NSString *)path;
+ (instancetype) snapshotFromBuffer:(const void *)buffer length:(unsigned)length;

- (SnapshotWrapper *)wrapper; 
- (bool) readDataFromFile:(NSString *)path;
- (bool) writeDataToFile:(NSString *)path;

@end


// --------------------------------------------------------------------------
//                                Archive
// --------------------------------------------------------------------------

@interface ArchiveProxy : NSObject {
    
    ArchiveWrapper *wrapper;
}

- (ArchiveWrapper *)wrapper;
- (NSString *)getPath;
- (NSString *)getName;
- (NSInteger)getType;
- (NSInteger)getNumberOfItems; 
- (NSString *)getNameOfItem:(NSInteger)item;
- (const char *)getNameOfItemUTF8:(NSInteger)item;
- (NSInteger) getSizeOfItem:(NSInteger)item;
- (NSInteger) getSizeOfItemInBlocks:(NSInteger)item;
- (NSString *) getTypeOfItem:(NSInteger)item;
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
+ (instancetype) archiveFromVC1541:(VC1541Proxy *)vc1541;
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

+ (BOOL) isTAPFile:(NSString *)filename;
+ (instancetype) containerFromTAPFile:(NSString *)filename;

- (NSString *)getPath;
- (NSString *)getName;
- (NSInteger)getType;
- (NSInteger)TAPversion;

@end

