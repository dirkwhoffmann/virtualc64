// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>
#import "Utils.h"

// Forward declarations of proxy classes
@class MyController;
@class C64Proxy;
@class CPUProxy;
@class MemoryProxy;
@class VICProxy;
@class CIAProxy;
@class SIDProxy;
@class KeyboardProxy;
@class ControlPortProxy;
@class IECProxy;
@class ExpansionPortProxy;
@class DriveProxy;
@class VIAProxy;
@class DiskProxy;
@class DatasetteProxy;
@class MouseProxy;

@class AnyC64FileProxy;
@class AnyArchiveProxy;
@class AnyDiskProxy;
@class CRTFileProxy;
@class TAPFileProxy;
@class SnapshotProxy;
@class T64FileProxy;
@class PRGFileProxy;
@class P00FileProxy;
@class D64FileProxy;
@class G64FileProxy;

// Forward declarations of wrappers for C++ classes.
// We wrap classes into normal C structs to avoid any reference to C++.

struct C64Wrapper;
struct CpuWrapper;
struct MemoryWrapper;
struct VicWrapper;
struct CiaWrapper;
struct SidBridgeWrapper;
struct KeyboardWrapper;
struct ControlPortWrapper;
struct IecWrapper;
struct ExpansionPortWrapper;
struct DriveWrapper;
struct ViaWrapper;
struct DiskWrapper;
struct DatasetteWrapper;
struct MouseWrapper;
struct AnyC64FileWrapper;

//
// C64 proxy
//

@interface C64Proxy : NSObject {
    
    struct C64Wrapper *wrapper;
    
    CPUProxy *cpu;
    MemoryProxy *mem;
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
    MouseProxy *mouse;
}

@property (readonly) struct C64Wrapper *wrapper;
@property (readonly) CPUProxy *cpu;
@property (readonly) MemoryProxy *mem;
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
@property (readonly) MouseProxy *mouse;

- (DriveProxy *) drive:(DriveID)id;

- (void) dealloc;
- (void) kill;

- (BOOL) releaseBuild;

- (BOOL) isReady:(ErrorCode *)error;
- (BOOL) isReady;
- (void) powerOn;
- (void) powerOff;
- (void) reset;
- (void) ping;
- (void) dump;

- (BOOL) isPoweredOn;
- (BOOL) isPoweredOff;
- (BOOL) isRunning;
- (BOOL) isPaused;
- (void) run;
- (void) pause;

- (void) suspend;
- (void) resume;

- (C64Configuration) config;
- (NSInteger) getConfig:(ConfigOption)opt;
- (NSInteger) getConfig:(ConfigOption)opt drive:(DriveID)id;
- (BOOL) configure:(ConfigOption)opt value:(NSInteger)val;
- (BOOL) configure:(ConfigOption)opt enable:(BOOL)val;
- (BOOL) configure:(ConfigOption)opt drive:(DriveID)id value:(NSInteger)val;
- (BOOL) configure:(ConfigOption)opt drive:(DriveID)id enable:(BOOL)val;

// Configuring the emulator
- (C64Model) model;
- (void) setModel:(C64Model)value;

// Accessing the message queue
- (Message)message;
- (void) addListener:(const void *)sender function:(Callback *)func;
- (void) removeListener:(const void *)sender;

// Running the emulator
- (void) stopAndGo;
- (void) stepInto;
- (void) stepOver;

// Managing the execution thread
- (BOOL) warp;
- (BOOL) alwaysWarp;
- (void) setAlwaysWarp:(BOOL)b;
- (BOOL) warpLoad;
- (void) setWarpLoad:(BOOL)b;

// Handling snapshots
- (BOOL) takeAutoSnapshots;
- (void) setTakeAutoSnapshots:(BOOL)b;
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
- (BOOL) hasBasicRom;
- (BOOL) hasCharRom;
- (BOOL) hasKernalRom;
- (BOOL) hasVC1541Rom;

- (BOOL) hasMega65BasicRom;
- (BOOL) hasMega65CharRom;
- (BOOL) hasMega65KernelRom;

- (BOOL) isBasicRom:(NSURL *)url;
- (BOOL) isCharRom:(NSURL *)url;
- (BOOL) isKernalRom:(NSURL *)url;
- (BOOL) isVC1541Rom:(NSURL *)url;

- (BOOL) loadBasicRomFromFile:(NSURL *)url;
- (BOOL) loadCharRomFromFile:(NSURL *)url;
- (BOOL) loadKernalRomFromFile:(NSURL *)url;
- (BOOL) loadVC1541RomFromFile:(NSURL *)url;

- (BOOL) loadBasicRomFromBuffer:(NSData *)buffer;
- (BOOL) loadCharRomFromBuffer:(NSData *)buffer;
- (BOOL) loadKernalRomFromBuffer:(NSData *)buffer;
- (BOOL) loadVC1541RomFromBuffer:(NSData *)buffer;

- (BOOL) saveBasicRom:(NSURL *)url;
- (BOOL) saveCharRom:(NSURL *)url;
- (BOOL) saveKernalRom:(NSURL *)url;
- (BOOL) saveVC1541Rom:(NSURL *)url;

- (void) deleteBasicRom;
- (void) deleteKernalRom;
- (void) deleteCharRom;
- (void) deleteVC1541Rom;

- (u64) basicRomFingerprint;
- (u64) kernalRomFingerprint;
- (u64) charRomFingerprint;
- (u64) vc1541RomFingerprint;

- (RomIdentifier) basicRomIdentifier;
- (RomIdentifier) kernalRomIdentifier;
- (RomIdentifier) charRomIdentifier;
- (RomIdentifier) vc1541RomIdentifier;

- (NSString *) basicRomTitle;
- (NSString *) charRomTitle;
- (NSString *) kernalRomTitle;
- (NSString *) vc1541RomTitle;

- (NSString *) basicRomSubTitle;
- (NSString *) charRomSubTitle;
- (NSString *) kernalRomSubTitle;
- (NSString *) vc1541RomSubTitle;

- (NSString *) basicRomRevision;
- (NSString *) charRomRevision;
- (NSString *) kernalRomRevision;
- (NSString *) vc1541RomRevision;

- (BOOL) isCommodoreRom:(RomIdentifier)rev;
- (BOOL) isPatchedRom:(RomIdentifier)rev;

- (BOOL) isRom:(NSURL *)url;
- (BOOL) loadRom:(NSURL *)url;

// Flashing files
- (BOOL)flash:(AnyC64FileProxy *)container;
- (BOOL)flash:(AnyArchiveProxy *)archive item:(NSInteger)nr;

@end


// -----------------------------------------------------------------------------
//                                 CPU proxy
// -----------------------------------------------------------------------------

@interface CPUProxy : NSObject {
    
    struct CpuWrapper *wrapper;
}

- (CPUInfo) getInfo;
- (void) dump;

- (BOOL) tracing;
- (void) setTracing:(BOOL)b;

- (i64) cycle;
- (u16) pc;
- (void) setPC:(u16)pc;
- (void) setSP:(u8)sp;
- (void) setA:(u8)a;
- (void) setX:(u8)x;
- (void) setY:(u8)y;
- (void) setNflag:(BOOL)b;
- (void) setZflag:(BOOL)b;
- (void) setCflag:(BOOL)b;
- (void) setIflag:(BOOL)b;
- (void) setBflag:(BOOL)b;
- (void) setDflag:(BOOL)b;
- (void) setVflag:(BOOL)b;

- (BOOL) breakpoint:(u16)addr;
- (void) setBreakpoint:(u16)addr;
- (void) deleteBreakpoint:(u16)addr;
- (void) toggleBreakpoint:(u16)addr;

- (NSInteger) recordedInstructions;
- (RecordedInstruction) readRecordedInstruction;
- (RecordedInstruction) readRecordedInstruction:(NSInteger)previous;

- (DisassembledInstruction) disassemble:(u16)addr hex:(BOOL)h;
- (DisassembledInstruction) disassembleRecordedInstr:(RecordedInstruction)instr hex:(BOOL)h;

@end


// -----------------------------------------------------------------------------
//                                 Memory proxy
// -----------------------------------------------------------------------------

@interface MemoryProxy : NSObject {
    
    struct MemoryWrapper *wrapper;
}

- (void) dump;

/*
- (NSInteger) ramPattern;
- (void) setRamPattern:(NSInteger)type;
- (void) eraseWithPattern:(NSInteger)type;
*/

- (MemoryType) peekSource:(u16)addr;
- (MemoryType) pokeTarget:(u16)addr;

- (u8) spypeek:(u16)addr source:(MemoryType)source;
- (u8) spypeek:(u16)addr;
- (u8) spypeekIO:(u16)addr;

- (void) poke:(u16)addr value:(u8)value target:(MemoryType)target;
- (void) poke:(u16)addr value:(u8)value;
- (void) pokeIO:(u16)addr value:(u8)value;

@end


// -----------------------------------------------------------------------------
//                                 CIA proxy
// -----------------------------------------------------------------------------

@interface CIAProxy : NSObject {
    
    struct CiaWrapper *wrapper;
}

- (CIAInfo) getInfo;
- (void) dump;
- (BOOL) tracing;
- (void) setTracing:(BOOL)b;

- (void) poke:(u16)addr value:(u8)value;

@end


// -----------------------------------------------------------------------------
//                                VICII proxy
// -----------------------------------------------------------------------------


@interface VICProxy : NSObject {
    
	struct VicWrapper *wrapper;
}

- (NSInteger) videoPalette;
- (void) setVideoPalette:(NSInteger)value;
- (BOOL) isPAL;

- (void *) screenBuffer;
- (NSColor *) color:(NSInteger)nr;
- (UInt32) rgbaColor:(NSInteger)nr palette:(Palette)palette;
- (double)brightness;
- (void)setBrightness:(double)value;
- (double)contrast;
- (void)setContrast:(double)value;
- (double)saturation;
- (void)setSaturation:(double)value;

- (VICInfo) getInfo;
- (SpriteInfo) getSpriteInfo:(NSInteger)sprite;
- (void) dump;

- (void) setMemoryBankAddr:(u16)addr;
- (void) setScreenMemoryAddr:(u16)addr;
- (void) setCharacterMemoryAddr:(u16)addr;

- (void) setDisplayMode:(DisplayMode)mode;
- (void) setScreenGeometry:(ScreenGeometry)mode;
- (void) setHorizontalRasterScroll:(u8)offset;
- (void) setVerticalRasterScroll:(u8)offset;

- (void) setSpriteEnabled:(NSInteger)nr value:(BOOL)flag;
- (void) toggleSpriteEnabled:(NSInteger)nr;
- (void) setSpriteX:(NSInteger)nr value:(NSInteger)x;
- (void) setSpriteY:(NSInteger)nr value:(NSInteger)y;
- (void) setSpritePtr:(NSInteger)nr value:(NSInteger)ptr;
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

- (void) setRasterInterruptLine:(u16)line;
- (void) setRasterInterruptEnabled:(BOOL)b;
- (void) toggleRasterInterruptFlag;

- (BOOL) hideSprites;
- (void) setHideSprites:(BOOL)b;
- (BOOL) showIrqLines;
- (void) setShowIrqLines:(BOOL)b;
- (BOOL) showDmaLines;
- (void) setShowDmaLines:(BOOL)b;

- (u32 *) noise;

@end


// -----------------------------------------------------------------------------
//                                 SID proxy
// -----------------------------------------------------------------------------

@interface SIDProxy : NSObject {
    
    struct SidBridgeWrapper *wrapper;
}

- (SIDInfo) getInfo;
- (VoiceInfo) getVoiceInfo:(NSInteger)voice;
- (void) dump;

- (u32) sampleRate;
- (void) setSampleRate:(u32)rate;

/*
- (BOOL) reSID;
- (void) setReSID:(BOOL)b;
- (BOOL) audioFilter;
- (void) setAudioFilter:(BOOL)b;
- (NSInteger) samplingMethod;
- (void) setSamplingMethod:(NSInteger)value;
*/
 /*
- (NSInteger) model;
- (void) setModel:(NSInteger)value;
*/

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


// -----------------------------------------------------------------------------
//                               Keyboard proxy
// -----------------------------------------------------------------------------

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


// -----------------------------------------------------------------------------
//                             Control port proxy
// -----------------------------------------------------------------------------

@interface ControlPortProxy : NSObject {
    
    struct ControlPortWrapper *wrapper;
}

- (void) dump;

- (void) trigger:(GamePadAction)event;
- (BOOL) autofire;
- (void) setAutofire:(BOOL)value;
- (NSInteger) autofireBullets;
- (void) setAutofireBullets:(NSInteger)value;
- (float) autofireFrequency;
- (void) setAutofireFrequency:(float)value;

@end


// -----------------------------------------------------------------------------
//                             Expansion port proxy
// -----------------------------------------------------------------------------

@interface ExpansionPortProxy : NSObject {
    
    struct ExpansionPortWrapper *wrapper;
}

- (void) dump;

- (BOOL) cartridgeAttached;
- (CartridgeType) cartridgeType;
- (void) attachCartridgeAndReset:(CRTFileProxy *)c;
- (BOOL) attachGeoRamCartridge:(NSInteger)capacity;
- (void) attachIsepicCartridge;
- (void) detachCartridgeAndReset;

- (NSInteger) numButtons;
- (NSString *) getButtonTitle:(NSInteger)nr;
- (void) pressButton:(NSInteger)nr;
- (void) releaseButton:(NSInteger)nr;

- (BOOL) hasSwitch;
- (NSInteger) switchPosition;
- (NSString *) switchDescription:(NSInteger)pos;
- (BOOL) validSwitchPosition:(NSInteger)pos;
- (BOOL) switchIsNeutral;
- (BOOL) switchIsLeft;
- (BOOL) switchIsRight;
- (void) setSwitchPosition:(NSInteger)pos;

- (BOOL) hasLed;
- (BOOL) led;
- (void) setLed:(BOOL)value;

- (BOOL) hasBattery;
- (void) setBattery:(BOOL)value;

@end


// -----------------------------------------------------------------------------
//                               IEC bus proxy
// -----------------------------------------------------------------------------

@interface IECProxy : NSObject {
    
    struct IecWrapper *wrapper;
}

- (void) dump;

- (BOOL) tracing;
- (void) setTracing:(BOOL)b;

@end


// -----------------------------------------------------------------------------
//                                Drive proxy
// -----------------------------------------------------------------------------

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

- (BOOL) isConnected;
// - (BOOL) isConnectable;
- (void) connect;
- (void) disconnect;
- (void) toggleConnection;

// - (void) deleteRom;

- (BOOL) redLED;
- (BOOL) hasDisk;
- (BOOL) hasModifiedDisk;
- (void) setModifiedDisk:(BOOL)b;
- (void) prepareToInsert;
- (void) insertDisk:(AnyArchiveProxy *)disk;
- (void) prepareToEject;
- (void) ejectDisk;
- (BOOL) writeProtected;
- (void) setWriteProtection:(BOOL)b;
- (BOOL) hasWriteProtectedDisk;

- (Halftrack) halftrack;
- (void) setTrack:(Track)t;
- (void) setHalftrack:(Halftrack)ht;
- (u16) sizeOfCurrentHalftrack;
- (u16) offset;
- (void) setOffset:(u16)value;
- (u8) readBitFromHead;
- (void) writeBitToHead:(u8)value;

- (void) moveHeadUp;
- (void) moveHeadDown;
- (BOOL) isRotating;
- (void) rotateDisk;
- (void) rotateBack;

@end


// -----------------------------------------------------------------------------
//                                VIA proxy
// -----------------------------------------------------------------------------

@interface VIAProxy : NSObject {
    
    struct ViaWrapper *wrapper;
}

- (void) dump;
- (BOOL) tracing;
- (void) setTracing:(BOOL)b;

@end


// -----------------------------------------------------------------------------
//                                Disk proxy
// -----------------------------------------------------------------------------

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


// -----------------------------------------------------------------------------
//                               Datasette proxy
// -----------------------------------------------------------------------------

@interface DatasetteProxy : NSObject {
    
    struct DatasetteWrapper *wrapper;
}

- (void) dump;

- (BOOL) hasTape;

- (void) pressPlay;
- (void) pressStop;
- (void) rewind;
- (void) ejectTape;
- (BOOL) insertTape:(TAPFileProxy *)tape;
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


// -----------------------------------------------------------------------------
//                                 Mouse proxy
// -----------------------------------------------------------------------------

@interface MouseProxy : NSObject {
    
    struct MouseWrapper *wrapper;
}

- (NSInteger) model;
- (void) setModel:(NSInteger)model;
- (NSInteger) port;
- (void) connect:(NSInteger)toPort;
- (void) disconnect;
- (void) setXY:(NSPoint)pos;
- (void) setLeftButton:(BOOL)pressed;
- (void) setRightButton:(BOOL)pressed;

@end


// -----------------------------------------------------------------------------
//                        F I L E   T Y P E   P R O X Y S
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                               AnyC64File proxy
// -----------------------------------------------------------------------------

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


// -----------------------------------------------------------------------------
//                               AnyArchive proxy
// -----------------------------------------------------------------------------

@interface AnyArchiveProxy : AnyC64FileProxy {
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


// -----------------------------------------------------------------------------
//                               Snapshot proxy
// -----------------------------------------------------------------------------

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


// -----------------------------------------------------------------------------
//                               CRTFile proxy
// -----------------------------------------------------------------------------

@interface CRTFileProxy : AnyC64FileProxy {
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


// -----------------------------------------------------------------------------
//                               TAPFile proxy
// -----------------------------------------------------------------------------

@interface TAPFileProxy : AnyC64FileProxy {
}

+ (BOOL)isTAPFile:(NSString *)path;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)path;

- (NSInteger)TAPversion;

@end


// -----------------------------------------------------------------------------
//                               T64File proxy
// -----------------------------------------------------------------------------

@interface T64FileProxy : AnyArchiveProxy
{
}
+ (BOOL)isT64File:(NSString *)filename;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)filename;
+ (instancetype)makeWithAnyArchive:(AnyArchiveProxy *)otherArchive;

@end


// -----------------------------------------------------------------------------
//                               PRGFile proxy
// -----------------------------------------------------------------------------

@interface PRGFileProxy : AnyArchiveProxy
{
}
+ (BOOL)isPRGFile:(NSString *)filename;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)filename;
+ (instancetype)makeWithAnyArchive:(AnyArchiveProxy *)otherArchive;

@end


// -----------------------------------------------------------------------------
//                               P00File proxy
// -----------------------------------------------------------------------------

@interface P00FileProxy : AnyArchiveProxy
{
}
+ (BOOL)isP00File:(NSString *)filename;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)filename;
+ (instancetype)makeWithAnyArchive:(AnyArchiveProxy *)otherArchive;

@end


// -----------------------------------------------------------------------------
//                               AnyDisk proxy
// -----------------------------------------------------------------------------

@interface AnyDiskProxy : AnyArchiveProxy {
}

+ (instancetype)make;
+ (instancetype)makeWithFile:(NSString *)path;

- (NSInteger)numberOfHalftracks;
- (void)selectHalftrack:(NSInteger)ht;
- (NSInteger)sizeOfHalftrack;
- (void)seekHalftrack:(NSInteger)offset;
- (NSString *)readHalftrackHex:(NSInteger)num;

@end


// -----------------------------------------------------------------------------
//                               D64File proxy
// -----------------------------------------------------------------------------

@interface D64FileProxy : AnyDiskProxy
{
}
+ (BOOL)isD64File:(NSString *)filename;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)filename;
+ (instancetype)makeWithAnyArchive:(AnyArchiveProxy *)otherArchive;
+ (instancetype)makeWithDisk:(DiskProxy *)disk;

@end


// -----------------------------------------------------------------------------
//                               G64File proxy
// -----------------------------------------------------------------------------

@interface G64FileProxy : AnyDiskProxy
{
}
+ (BOOL)isG64File:(NSString *)filename;
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype) makeWithFile:(NSString *)filename;
+ (instancetype) makeWithDisk:(DiskProxy *)diskProxy;

@end
