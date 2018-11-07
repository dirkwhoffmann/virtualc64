//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//


#import <Cocoa/Cocoa.h>
#import "C64_types.h"
#import "basic.h"

// Forward declarations of proxy classes
@class MyController;
@class C64Proxy;
@class AnyC64FileProxy;
@class SnapshotProxy;
@class ArchiveProxy;
@class TAPProxy;
@class CRTProxy;

// Forward declarations of wrappers for C++ classes.
// We wrap classes into normal C structs to avoid any reference to C++.

struct C64Wrapper;
struct CpuWrapper;
struct MemoryWrapper;
struct VicWrapper;
struct CiaWrapper;
struct KeyboardWrapper;
struct ControlPortWrapper;
struct SidBridgeWrapper;
struct IecWrapper;
struct ExpansionPortWrapper;
struct Via6522Wrapper;
struct DiskWrapper;
struct DriveWrapper;
struct DatasetteWrapper;
struct AnyC64FileWrapper;

//
// CPU
//

@interface CPUProxy : NSObject {
        
	struct CpuWrapper *wrapper;
}

- (CPUInfo) getInfo;
- (void) dump;

- (BOOL) tracing;
- (void) setTracing:(BOOL)b;

- (UInt64) cycle;
- (uint16_t) pc;
- (void) setPC:(uint16_t)pc;
- (void) setSP:(uint8_t)sp;
- (void) setA:(uint8_t)a;
- (void) setX:(uint8_t)x;
- (void) setY:(uint8_t)y;
- (void) setNflag:(BOOL)b;
- (void) setZflag:(BOOL)b;
- (void) setCflag:(BOOL)b;
- (void) setIflag:(BOOL)b;
- (void) setBflag:(BOOL)b;
- (void) setDflag:(BOOL)b;
- (void) setVflag:(BOOL)b;

- (BOOL) breakpoint:(uint16_t)addr;
- (void) setBreakpoint:(uint16_t)addr;
- (void) deleteBreakpoint:(uint16_t)addr;
- (void) toggleBreakpoint:(uint16_t)addr;

- (NSInteger) recordedInstructions;
- (RecordedInstruction) readRecordedInstruction;
- (RecordedInstruction) readRecordedInstruction:(NSInteger)previous;

- (DisassembledInstruction) disassemble:(uint16_t)addr hex:(BOOL)h;
- (DisassembledInstruction) disassembleRecordedInstr:(RecordedInstruction)instr hex:(BOOL)h;

@end

//
// Memory
//

@interface MemoryProxy : NSObject {
    
	struct MemoryWrapper *wrapper;
}

- (void) dump;

- (NSInteger) ramInitPattern;
- (void) setRamInitPattern:(NSInteger)type;
- (void) eraseWithPattern:(NSInteger)type;

- (MemoryType) peekSource:(uint16_t)addr;
- (MemoryType) pokeTarget:(uint16_t)addr;

- (uint8_t) spypeek:(uint16_t)addr source:(MemoryType)source;
- (uint8_t) spypeek:(uint16_t)addr;
- (uint8_t) spypeekIO:(uint16_t)addr;

- (void) poke:(uint16_t)addr value:(uint8_t)value target:(MemoryType)target;
- (void) poke:(uint16_t)addr value:(uint8_t)value;
- (void) pokeIO:(uint16_t)addr value:(uint8_t)value;

@end

//
// CIA
//

@interface CIAProxy : NSObject {
    
    struct CiaWrapper *wrapper;
}

- (CIAInfo) getInfo;
- (void) dump;
- (BOOL) tracing;
- (void) setTracing:(BOOL)b;

- (NSInteger) chipModel;
- (void) setChipModel:(NSInteger)value;
- (BOOL) emulateTimerBBug;
- (void) setEmulateTimerBBug:(BOOL)value;

- (void) poke:(uint16_t)addr value:(uint8_t)value;

@end


//
// VIC
//

@interface VICProxy : NSObject {
    
	struct VicWrapper *wrapper;
}

- (NSInteger) chipModel;
- (void) setChipModel:(NSInteger)value;
- (NSInteger) videoPalette;
- (void) setVideoPalette:(NSInteger)value;
- (NSInteger) glueLogic;
- (void) setGlueLogic:(NSInteger)value;
- (BOOL) hasGrayDotBug;
- (BOOL) emulateGrayDotBug;
- (void) setEmulateGrayDotBug:(BOOL)value;
- (BOOL) isPAL;

- (void *) screenBuffer;
- (NSColor *) color:(NSInteger)nr;
- (double)brightness;
- (void)setBrightness:(double)value;
- (double)contrast;
- (void)setContrast:(double)value;
- (double)saturation;
- (void)setSaturation:(double)value;

- (VICInfo) getInfo;
- (SpriteInfo) getSpriteInfo:(NSInteger)sprite;
- (void) dump;

- (void) setMemoryBankAddr:(uint16_t)addr;
- (void) setScreenMemoryAddr:(uint16_t)addr;
- (void) setCharacterMemoryAddr:(uint16_t)addr;

- (void) setDisplayMode:(DisplayMode)mode;
- (void) setScreenGeometry:(ScreenGeometry)mode;
- (void) setHorizontalRasterScroll:(uint8_t)offset;
- (void) setVerticalRasterScroll:(uint8_t)offset;

- (void) setSpriteEnabled:(NSInteger)nr value:(BOOL)flag;
- (void) toggleSpriteEnabled:(NSInteger)nr;
- (void) setSpriteX:(NSInteger)nr value:(NSInteger)x;
- (void) setSpriteY:(NSInteger)nr value:(NSInteger)y;
- (void) setSpriteStretchX:(NSInteger)nr value:(BOOL)flag;
- (void) toggleSpriteStretchX:(NSInteger)nr;
- (void) setSpriteStretchY:(NSInteger)nr value:(BOOL)flag;
- (void) toggleSpriteStretchY:(NSInteger)nr;
- (void) setSpriteColor:(NSInteger)nr value:(int)c;
- (void) setSpritePriority:(NSInteger)nr value:(BOOL)flag;
- (void) toggleSpritePriority:(NSInteger)nr;
- (void) setSpriteMulticolor:(NSInteger)nr value:(BOOL)flag;
- (void) toggleSpriteMulticolor:(NSInteger)nr;

- (void) setIrqOnSpriteSpriteCollision:(BOOL)value;
- (void) toggleIrqOnSpriteSpriteCollision;
- (void) setIrqOnSpriteBackgroundCollision:(BOOL)value;
- (void) toggleIrqOnSpriteBackgroundCollision;

- (void) setRasterInterruptLine:(uint16_t)line;
- (void) setRasterInterruptEnabled:(BOOL)b;
- (void) toggleRasterInterruptFlag;

- (BOOL) hideSprites;
- (void) setHideSprites:(BOOL)b;
- (BOOL) showIrqLines;
- (void) setShowIrqLines:(BOOL)b;
- (BOOL) showDmaLines;
- (void) setShowDmaLines:(BOOL)b;

@end


//
// SID
//

@interface SIDProxy : NSObject {
    
    struct SidBridgeWrapper *wrapper;
}

- (SIDInfo) getInfo;
- (VoiceInfo) getVoiceInfo:(NSInteger)voice;
- (void) dump;

- (BOOL) reSID;
- (void) setReSID:(BOOL)b;
- (uint32_t) sampleRate;
- (void) setSampleRate:(uint32_t)rate;
- (BOOL) audioFilter;
- (void) setAudioFilter:(BOOL)b;
- (NSInteger) samplingMethod;
- (void) setSamplingMethod:(NSInteger)value;
- (NSInteger) chipModel;
- (void) setChipModel:(NSInteger)value;

- (NSInteger) ringbufferSize;
- (float) ringbufferData:(NSInteger)offset;
- (double) fillLevel;
- (NSInteger) bufferUnderflows;
- (NSInteger) bufferOverflows;

- (void) readMonoSamples:(float *)target size:(NSInteger)n;
- (void) readStereoSamples:(float *)target1 buffer2:(float *)target2 size:(NSInteger)n;
- (void) readStereoSamplesInterleaved:(float *)target size:(NSInteger)n;

- (void) rampUp;
- (void) rampUpFromZero;
- (void) rampDown;

@end


//
// IEC bus
//

@interface IECProxy : NSObject {
    
    struct IecWrapper *wrapper;
}

- (void) dump;

- (BOOL) tracing;
- (void) setTracing:(BOOL)b;

@end


//
// Keyboard
//

@interface KeyboardProxy : NSObject {
    
    struct KeyboardWrapper *wrapper;
}

- (void) dump;

- (void) pressKeyAtRow:(NSInteger)row col:(NSInteger)col;
- (void) pressRestoreKey;

- (void) releaseKeyAtRow:(NSInteger)row col:(NSInteger)col;
- (void) releaseRestoreKey;
- (void) releaseAll;

- (BOOL) leftShiftIsPressed;
- (BOOL) rightShiftIsPressed;
- (BOOL) controlIsPressed;
- (BOOL) commodoreIsPressed;

- (BOOL) shiftLockIsHoldDown;
- (void) lockShift;
- (void) unlockShift;

- (BOOL) inUpperCaseMode;

@end 

//
// Control port
//

@interface ControlPortProxy : NSObject {
    
    struct ControlPortWrapper *wrapper;
}

- (void) dump;
- (BOOL) autofire;
- (void) setAutofire:(BOOL)value;
- (NSInteger) autofireBullets;
- (void) setAutofireBullets:(NSInteger)value;
- (float) autofireFrequency;
- (void) setAutofireFrequency:(float)value;

- (void) trigger:(JoystickEvent)event;

@end


//
// Expansion port
//

@interface ExpansionPortProxy : NSObject {
    
    struct ExpansionPortWrapper *wrapper;
}

- (void) dump;

- (BOOL) cartridgeAttached;
- (CartridgeType) cartridgeType;
- (BOOL) attachCartridgeAndReset:(CRTProxy *)c;
- (BOOL) attachGeoRamCartridge:(NSInteger)capacity;
- (void) detachCartridgeAndReset;
- (BOOL) hasFreezeButton;
- (void) pressFreezeButton;
- (void) releaseFreezeButton;
- (BOOL) hasResetButton;
- (void) pressResetButton;
- (void) releaseResetButton;
- (BOOL) hasBattery;
- (void) setBattery:(BOOL)value;

@end


//
// Diskette
//

@interface DiskProxy : NSObject {
    
    struct DiskWrapper *wrapper;
}

@property (readonly) struct DiskWrapper *wrapper;

- (void) dump;
- (BOOL)writeProtected;
- (void)setWriteProtection:(BOOL)b;
- (void)toggleWriteProtection;
- (NSInteger)nonemptyHalftracks;
- (void)analyzeTrack:(Track)t;
- (void)analyzeHalftrack:(Halftrack)ht;
- (NSInteger)numErrors;
- (NSString *)errorMessage:(NSInteger)nr;
- (NSInteger)firstErroneousBit:(NSInteger)nr;
- (NSInteger)lastErroneousBit:(NSInteger)nr;
- (SectorInfo)sectorInfo:(Sector)s;
- (const char *)diskNameAsString;
- (const char *)trackDataAsString;
- (const char *)sectorHeaderAsString:(Sector)nr;
- (const char *)sectorDataAsString:(Sector)nr;
@end


//
// VIA
//

@interface VIAProxy : NSObject {
    
    struct Via6522Wrapper *wrapper;
}

- (void) dump;
- (BOOL) tracing;
- (void) setTracing:(BOOL)b;

@end


//
// VC 1541 disk drive
//

@interface DriveProxy : NSObject {
    
	struct DriveWrapper *wrapper;
    
    // Sub proxys
	CPUProxy *cpu;
	VIAProxy *via1;
	VIAProxy *via2;
    DiskProxy *disk;
}

@property (readonly) struct DriveWrapper *wrapper;
@property (readonly) CPUProxy *cpu;
@property (readonly) VIAProxy *via1;
@property (readonly) VIAProxy *via2;
@property (readonly) DiskProxy *disk;

- (VIAProxy *) via:(NSInteger)num;

- (void) dump;
- (BOOL) tracing;
- (void) setTracing:(BOOL)b;

- (BOOL) isPoweredOn;
- (void) powerOn;
- (void) powerOff;
- (void) togglePowerSwitch;

- (BOOL) redLED;
- (BOOL) hasDisk;
- (BOOL) hasModifiedDisk;
- (void) setModifiedDisk:(BOOL)b;
- (void) prepareToInsert;
- (void) insertDisk:(ArchiveProxy *)disk;
- (void) prepareToEject;
- (void) ejectDisk;
- (BOOL) writeProtected;
- (void) setWriteProtection:(BOOL)b;
- (BOOL) hasWriteProtectedDisk;
// - (BOOL) diskModified;
// - (void) setDiskModified:(BOOL)b;
- (BOOL) sendSoundMessages;
- (void) setSendSoundMessages:(BOOL)b;

- (Halftrack) halftrack;
- (void) setTrack:(Track)t;
- (void) setHalftrack:(Halftrack)ht;
- (uint16_t) sizeOfCurrentHalftrack;
- (uint16_t) offset;
- (void) setOffset:(uint16_t)value;
- (uint8_t) readBitFromHead;
- (void) writeBitToHead:(uint8_t)value;

- (void) moveHeadUp;
- (void) moveHeadDown;
- (BOOL) isRotating;
- (void) rotateDisk;
- (void) rotateBack;

@end


//
// Datasette
//

@interface DatasetteProxy : NSObject {
    
    struct DatasetteWrapper *wrapper;
}

- (void) dump;

- (BOOL) hasTape;

- (void) pressPlay;
- (void) pressStop;
- (void) rewind;
- (void) ejectTape;
- (BOOL) insertTape:(TAPProxy *)tape;
- (NSInteger) getType; 
- (long) durationInCycles;
- (int) durationInSeconds;
- (NSInteger) head;
- (NSInteger) headInCycles;
- (int) headInSeconds;
- (void) setHeadInCycles:(long)value;
- (BOOL) motor;
- (BOOL) playKey;

@end


//
// C64
//

@interface C64Proxy : NSObject {
    
	struct C64Wrapper *wrapper;
    
	// Hardware components
    MemoryProxy *mem;
	CPUProxy *cpu;
	VICProxy *vic;
	CIAProxy *cia1;
	CIAProxy *cia2;
	SIDProxy *sid;
	KeyboardProxy *keyboard;
    ControlPortProxy *port1;
    ControlPortProxy *port2;
	IECProxy *iec;
    ExpansionPortProxy *expansionport;
	DriveProxy *drive1;
    DriveProxy *drive2;
    DatasetteProxy *datasette;
}

@property (readonly) MemoryProxy *mem;
@property (readonly) CPUProxy *cpu;
@property (readonly) VICProxy *vic;
@property (readonly) CIAProxy *cia1;
@property (readonly) CIAProxy *cia2;
@property (readonly) SIDProxy *sid;
@property (readonly) KeyboardProxy *keyboard;
@property (readonly) ControlPortProxy *port1;
@property (readonly) ControlPortProxy *port2;
@property (readonly) IECProxy *iec;
@property (readonly) ExpansionPortProxy *expansionport;
@property (readonly) DriveProxy *drive1;
@property (readonly) DriveProxy *drive2;
@property (readonly) DatasetteProxy *datasette;

- (void) kill;

- (struct C64Wrapper *)wrapper;
- (DriveProxy *)drive:(NSInteger)nr;

- (void) ping;
- (void) dump;
- (BOOL) developmentMode;

// Configuring the emulator
- (NSInteger) model;
- (void) setModel:(NSInteger)value;

// Accessing the message queue
- (Message)message;
- (void) addListener:(const void *)sender function:(Callback *)func;
- (void) removeListener:(const void *)sender;

// Running the emulator
- (void) powerUp;
- (void) run;
- (void) halt;
- (void) suspend;
- (void) resume;
- (BOOL) isRunnable;
- (BOOL) isRunning;
- (BOOL) isHalted;
- (void) step;
- (void) stepOver;

// Handling mice
- (NSInteger) mouseModel;
- (void) setMouseModel:(NSInteger)model;
- (void) connectMouse:(NSInteger)toPort;
- (void) disconnectMouse;
- (void) setMouseXY:(NSPoint)position;
- (void) setMouseLeftButton:(BOOL)pressed;
- (void) setMouseRightButton:(BOOL)pressed;

// Managing the execution thread
- (BOOL) warp;
- (BOOL) alwaysWarp;
- (void) setAlwaysWarp:(BOOL)b;
- (BOOL) warpLoad;
- (void) setWarpLoad:(BOOL)b;

// Handling snapshots
- (void) disableAutoSnapshots;
- (void) enableAutoSnapshots;
- (void) suspendAutoSnapshots;
- (void) resumeAutoSnapshots;
- (NSInteger) snapshotInterval;
- (void) setSnapshotInterval:(NSInteger)value;

- (BOOL) restoreAutoSnapshot:(NSInteger)nr;
- (BOOL) restoreUserSnapshot:(NSInteger)nr;
- (BOOL) restoreLatestAutoSnapshot;
- (BOOL) restoreLatestUserSnapshot;
- (NSInteger) numAutoSnapshots;
- (NSInteger) numUserSnapshots;

- (NSData *) autoSnapshotData:(NSInteger)nr;
- (NSData *) userSnapshotData:(NSInteger)nr;
- (unsigned char *) autoSnapshotImageData:(NSInteger)nr;
- (unsigned char *) userSnapshotImageData:(NSInteger)nr;
- (NSSize) autoSnapshotImageSize:(NSInteger)nr;
- (NSSize) userSnapshotImageSize:(NSInteger)nr;
- (time_t) autoSnapshotTimestamp:(NSInteger)nr;
- (time_t) userSnapshotTimestamp:(NSInteger)nr;

- (void) takeUserSnapshot;

- (void) deleteAutoSnapshot:(NSInteger)nr;
- (void) deleteUserSnapshot:(NSInteger)nr;

// Handling ROMs
- (BOOL) isBasicRom:(NSURL *)url;
- (BOOL) loadBasicRom:(NSURL *)url;
- (BOOL) isBasicRomLoaded;
- (BOOL) isCharRom:(NSURL *)url;
- (BOOL) loadCharRom:(NSURL *)url;
- (BOOL) isCharRomLoaded;
- (BOOL) isKernalRom:(NSURL *)url;
- (BOOL) loadKernalRom:(NSURL *)url;
- (BOOL) isKernalRomLoaded;
- (BOOL) isVC1541Rom:(NSURL *)url;
- (BOOL) loadVC1541Rom:(NSURL *)url;
- (BOOL) isVC1541RomLoaded;
- (BOOL) isRom:(NSURL *)url;
- (BOOL) loadRom:(NSURL *)url;

// Flashing files
- (BOOL)flash:(AnyC64FileProxy *)container;
- (BOOL)flash:(ArchiveProxy *)archive item:(NSInteger)nr;

@end


//
// F I L E   P R O X Y   C L A S S E S
//

//
// AnyC64FileProxy
//

@interface AnyC64FileProxy : NSObject {
    
    struct AnyC64FileWrapper *wrapper;
}

- (struct AnyC64FileWrapper *)wrapper;

- (C64FileType)type;
- (void)setPath:(NSString *)path;
- (NSString *)name;
- (NSInteger)sizeOnDisk;
- (void)readFromBuffer:(const void *)buffer length:(NSInteger)length;
- (NSInteger)writeToBuffer:(void *)buffer;

@end


//
// SnapshotProxy
//

@interface SnapshotProxy : AnyC64FileProxy {
}

+ (BOOL)isSupportedSnapshot:(const void *)buffer length:(NSInteger)length;
+ (BOOL)isUnsupportedSnapshot:(const void *)buffer length:(NSInteger)length;
+ (BOOL)isSupportedSnapshotFile:(NSString *)path;
+ (BOOL)isUnsupportedSnapshotFile:(NSString *)path;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)path;
+ (instancetype)makeWithC64:(C64Proxy *)c64proxy;

@end


//
// CRTProxy
//

@interface CRTProxy : AnyC64FileProxy {
}

+ (CartridgeType)typeOfCRTBuffer:(const void *)buffer length:(NSInteger)length;
+ (NSString *)typeNameOfCRTBuffer:(const void *)buffer length:(NSInteger)length;
+ (BOOL)isSupportedCRTBuffer:(const void *)buffer length:(NSInteger)length;
+ (BOOL)isUnsupportedCRTBuffer:(const void *)buffer length:(NSInteger)length;
+ (BOOL)isCRTFile:(NSString *)path;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)path;
 
- (CartridgeType)cartridgeType;
- (NSString *)cartridgeTypeName;
- (NSInteger)initialExromLine;
- (NSInteger)initialGameLine;
- (NSInteger)chipCount;
- (NSInteger)chipType:(NSInteger)nr;
- (NSInteger)chipAddr:(NSInteger)nr;
- (NSInteger)chipSize:(NSInteger)nr;

@end


//
// TAPProxy
//

@interface TAPProxy : AnyC64FileProxy {
}

+ (BOOL)isTAPFile:(NSString *)path;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)path;

- (NSInteger)TAPversion;
@end


//
// ArchiveProxy
//

@interface ArchiveProxy : AnyC64FileProxy {
}

+ (instancetype)make;
+ (instancetype)makeWithFile:(NSString *)path;

- (NSInteger)numberOfItems;
- (void)selectItem:(NSInteger)item;
- (NSString *)nameOfItem;
- (NSString *)unicodeNameOfItem;
- (NSInteger)sizeOfItem;
- (NSInteger)sizeOfItemInBlocks;
- (void)seekItem:(NSInteger)offset;
- (NSString *)typeOfItem;
- (NSString *)readItemHex:(NSInteger)num;
- (NSInteger)destinationAddrOfItem;

@end

@interface T64Proxy : ArchiveProxy
{
}
+ (BOOL)isT64File:(NSString *)filename;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)filename;
+ (instancetype)makeWithAnyArchive:(ArchiveProxy *)otherArchive;
@end

@interface PRGProxy : ArchiveProxy
{
}
+ (BOOL)isPRGFile:(NSString *)filename;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)filename;
+ (instancetype)makeWithAnyArchive:(ArchiveProxy *)otherArchive;
@end

@interface P00Proxy : ArchiveProxy
{
}
+ (BOOL)isP00File:(NSString *)filename;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)filename;
+ (instancetype)makeWithAnyArchive:(ArchiveProxy *)otherArchive;
@end

//
// AnyDiskProxy
//

@interface AnyDiskProxy : ArchiveProxy {
}

+ (instancetype)make;
+ (instancetype)makeWithFile:(NSString *)path;

- (NSInteger)numberOfHalftracks;
- (void)selectHalftrack:(NSInteger)ht;
- (NSInteger)sizeOfHalftrack;
- (void)seekHalftrack:(NSInteger)offset;
- (NSString *)readHalftrackHex:(NSInteger)num;

@end

@interface D64Proxy : AnyDiskProxy
{
}
+ (BOOL)isD64File:(NSString *)filename;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)filename;
+ (instancetype)makeWithAnyArchive:(ArchiveProxy *)otherArchive;
+ (instancetype)makeWithDisk:(DiskProxy *)disk;
// + (instancetype)makeWithDrive:(DriveProxy *)drive;
@end

@interface G64Proxy : AnyDiskProxy
{
}
+ (BOOL)isG64File:(NSString *)filename;
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype) makeWithFile:(NSString *)filename;
+ (instancetype) makeWithDisk:(DiskProxy *)diskProxy;
@end

/*
@interface FileProxy : ArchiveProxy
{
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype) makeWithFile:(NSString *)filename;
@end
*/
