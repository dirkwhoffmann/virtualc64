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

#include "C64Constants.h"
#include "ErrorTypes.h"
#include "CIATypes.h"
#include "CPUTypes.h"
#include "FileTypes.h"
#include "MouseTypes.h"
#include "PortTypes.h"
#include "SIDTypes.h"
#include "VICIITypes.h"
#include "C64PublicTypes.h"

//
// Forward declarations
//

@class AnyFileProxy;
@class AnyCollectionProxy;
@class C64Proxy;
@class CIAProxy;
@class ControlPortProxy;
@class CPUProxy;
@class CRTFileProxy;
@class D64FileProxy;
@class DatasetteProxy;
@class DiskProxy;
@class DriveProxy;
@class ExpansionPortProxy;
@class FSDeviceProxy;
@class G64FileProxy;
@class GuardsProxy;
@class IECProxy;
@class JoystickProxy;
@class KeyboardProxy;
@class MemoryProxy;
@class MouseProxy;
@class MyController;
@class P00FileProxy;
@class PRGFileProxy;
@class RomFileProxy;
@class SIDProxy;
@class SnapshotProxy;
@class T64FileProxy;
@class TAPFileProxy;
@class VIAProxy;
@class VICProxy;


//
// Base proxies
//

@interface Proxy : NSObject {
    
    // Reference to the wrapped C++ object
    @public void *obj;
}

@end

@interface HardwareComponentProxy : Proxy { }

- (void)dump;
- (void)dumpConfig;

@end

//
// C64 proxy
//

@interface C64Proxy : HardwareComponentProxy {
        
    CIAProxy *cia1;
    CIAProxy *cia2;
    ControlPortProxy *port1;
    ControlPortProxy *port2;
    CPUProxy *cpu;
    DatasetteProxy *datasette;
    DriveProxy *drive8;
    DriveProxy *drive9;
    ExpansionPortProxy *expansionport;
    GuardsProxy *breakpoints;
    GuardsProxy *watchpoints;
    IECProxy *iec;
    KeyboardProxy *keyboard;
    MemoryProxy *mem;
    MouseProxy *mouse;
    SIDProxy *sid;
    VICProxy *vic;
}

@property (readonly, strong) CIAProxy *cia1;
@property (readonly, strong) CIAProxy *cia2;
@property (readonly, strong) ControlPortProxy *port1;
@property (readonly, strong) ControlPortProxy *port2;
@property (readonly, strong) CPUProxy *cpu;
@property (readonly, strong) DatasetteProxy *datasette;
@property (readonly, strong) DriveProxy *drive8;
@property (readonly, strong) DriveProxy *drive9;
@property (readonly, strong) ExpansionPortProxy *expansionport;
@property (readonly, strong) GuardsProxy *breakpoints;
@property (readonly, strong) GuardsProxy *watchpoints;
@property (readonly, strong) IECProxy *iec;
@property (readonly, strong) KeyboardProxy *keyboard;
@property (readonly, strong) MemoryProxy *mem;
@property (readonly, strong) SIDProxy *sid;
@property (readonly, strong) VICProxy *vic;

- (DriveProxy *) drive:(DriveID)id;

- (void)dealloc;
- (void)kill;

@property (readonly) BOOL isReleaseBuild;
@property BOOL warp;
@property BOOL debugMode;
@property InspectionTarget inspectionTarget;

- (BOOL)isReady:(ErrorCode *)ec;
- (BOOL)isReady;
- (void)powerOn;
- (void)powerOff;
- (void)inspect;
- (void)reset;

@property (readonly) BOOL poweredOn;
@property (readonly) BOOL poweredOff;
@property (readonly) BOOL running;
@property (readonly) BOOL paused;

- (void)run;
- (void)pause;
- (void)suspend;
- (void)resume;

- (void)requestAutoSnapshot;
- (void)requestUserSnapshot;
@property (readonly) SnapshotProxy *latestAutoSnapshot;
@property (readonly) SnapshotProxy *latestUserSnapshot;
- (void)loadFromSnapshot:(SnapshotProxy *)proxy;

// @property (readonly) C64Config config;
- (NSInteger)getConfig:(Option)opt;
- (NSInteger)getConfig:(Option)opt id:(NSInteger)id;
- (NSInteger)getConfig:(Option)opt drive:(DriveID)id;
- (BOOL)configure:(Option)opt value:(NSInteger)val;
- (BOOL)configure:(Option)opt enable:(BOOL)val;
- (BOOL)configure:(Option)opt id:(NSInteger)id value:(NSInteger)val;
- (BOOL)configure:(Option)opt id:(NSInteger)id enable:(BOOL)val;
- (BOOL)configure:(Option)opt drive:(DriveID)id value:(NSInteger)val;
- (BOOL)configure:(Option)opt drive:(DriveID)id enable:(BOOL)val;
- (void)configure:(C64Model)value;

- (Message)message;
- (void)addListener:(const void *)sender function:(Callback *)func;
- (void)removeListener:(const void *)sender;

- (void)stopAndGo;
- (void)stepInto;
- (void)stepOver;

- (BOOL) hasRom:(RomType)type;
- (BOOL) hasMega65Rom:(RomType)type;

- (BOOL) isRom:(RomType)type url:(NSURL *)url;

- (void) loadRom:(RomFileProxy *)proxy;
- (void) saveRom:(RomType)type url:(NSURL *)url error:(ErrorCode *)ec;
- (void) deleteRom:(RomType)type;

- (RomIdentifier) romIdentifier:(RomType)type;
- (BOOL)isCommodoreRom:(RomIdentifier)rev;
- (BOOL)isPatchedRom:(RomIdentifier)rev;

- (NSString *) romTitle:(RomType)type;
- (NSString *) romSubTitle:(RomType)type;
- (NSString *) romRevision:(RomType)type;

- (BOOL)flash:(AnyFileProxy *)container;
- (BOOL)flash:(FSDeviceProxy *)proxy item:(NSInteger)nr;

@end


//
// Guards proxy (Breakpoints, Watchpoints)
//

@interface GuardsProxy : Proxy { }

@property (readonly) NSInteger count;
- (NSInteger)addr:(NSInteger)nr;
- (BOOL)isEnabled:(NSInteger)nr;
- (BOOL)isDisabled:(NSInteger)nr;
- (void)enable:(NSInteger)nr;
- (void)disable:(NSInteger)nr;
- (void)remove:(NSInteger)nr;
- (void)replace:(NSInteger)nr addr:(NSInteger)addr;

- (BOOL)isSetAt:(NSInteger)addr;
- (BOOL)isSetAndEnabledAt:(NSInteger)addr;
- (BOOL)isSetAndDisabledAt:(NSInteger)addr;
- (void)enableAt:(NSInteger)addr;
- (void)disableAt:(NSInteger)addr;
- (void)addAt:(NSInteger)addr;
- (void)removeAt:(NSInteger)addr;

@end


//
// CPU proxy
//

@interface CPUProxy : HardwareComponentProxy { }

- (CPUInfo)getInfo;
- (NSInteger)loggedInstructions;
- (NSInteger)loggedPCRel:(NSInteger)nr;
- (NSInteger)loggedPCAbs:(NSInteger)nr;
- (void)clearLog;
- (bool)isJammed;

- (void)setHex;
- (void)setDec;

- (i64)cycle;
- (u16)pc;

- (NSString *)disassembleRecordedInstr:(NSInteger)i length:(NSInteger *)len;
- (NSString *)disassembleRecordedBytes:(NSInteger)i;
- (NSString *)disassembleRecordedFlags:(NSInteger)i;
- (NSString *)disassembleRecordedPC:(NSInteger)i;

- (NSString *)disassembleInstr:(NSInteger)addr length:(NSInteger *)len;
- (NSString *)disassembleBytes:(NSInteger)addr;
- (NSString *)disassembleAddr:(NSInteger)addr;

@end

//
// Memory proxy
//

@interface MemoryProxy : HardwareComponentProxy { }

- (MemInfo)getInfo;

- (MemoryType)peekSource:(u16)addr;
- (MemoryType)pokeTarget:(u16)addr;

- (u8)spypeek:(u16)addr source:(MemoryType)source;
- (u8)spypeek:(u16)addr;
- (u8)spypeekIO:(u16)addr;
- (u8)spypeekColor:(u16)addr;

- (NSString *)memdump:(NSInteger)addr num:(NSInteger)num hex:(BOOL)hex src:(MemoryType)src;
- (NSString *)txtdump:(NSInteger)addr num:(NSInteger)num src:(MemoryType)src;

@end


//
// CIA proxy
//

@interface CIAProxy : HardwareComponentProxy { }

- (CIAInfo)getInfo;

@end


//
// VICII proxy
//


@interface VICProxy : HardwareComponentProxy { }

- (BOOL)isPAL;
- (void *)stableEmuTexture;
- (NSColor *)color:(NSInteger)nr;
- (UInt32)rgbaColor:(NSInteger)nr palette:(Palette)palette;
- (double)brightness;
- (void)setBrightness:(double)value;
- (double)contrast;
- (void)setContrast:(double)value;
- (double)saturation;
- (void)setSaturation:(double)value;

- (VICIIInfo)getInfo;
- (SpriteInfo)getSpriteInfo:(NSInteger)sprite;

- (u32 *)noise;

@end


//
// SID proxy
//

@interface SIDProxy : HardwareComponentProxy { }

- (SIDInfo)getInfo:(NSInteger)nr;
- (VoiceInfo)getVoiceInfo:(NSInteger)nr voice:(NSInteger)voice;

- (double)sampleRate;
- (void)setSampleRate:(double)rate;

- (NSInteger)ringbufferSize;
- (void)ringbufferData:(NSInteger)offset left:(float *)l right:(float *)r;
- (double)fillLevel;
- (NSInteger)bufferUnderflows;
- (NSInteger)bufferOverflows;

- (void)copyMono:(float *)target size:(NSInteger)n;
- (void)copyStereo:(float *)target1 buffer2:(float *)target2 size:(NSInteger)n;
- (void)copyInterleaved:(float *)target size:(NSInteger)n;

- (void)rampUp;
- (void)rampUpFromZero;
- (void)rampDown;

@end


//
// Keyboard proxy
//

@interface KeyboardProxy : HardwareComponentProxy { }

- (BOOL)keyIsPressed:(NSInteger)nr;
- (BOOL)keyIsPressedAtRow:(NSInteger)row col:(NSInteger)col;
- (BOOL)controlIsPressed;
- (BOOL)commodoreIsPressed;
- (BOOL)leftShiftIsPressed;
- (BOOL)rightShiftIsPressed;
- (BOOL)shiftLockIsPressed;

- (void)pressKey:(NSInteger)nr;
- (void)pressKeyAtRow:(NSInteger)row col:(NSInteger)col;
- (void)pressShiftLock;

- (void)releaseKey:(NSInteger)nr;
- (void)releaseKeyAtRow:(NSInteger)row col:(NSInteger)col;
- (void)releaseShiftLock;
- (void)releaseAll;

- (void)toggleKey:(NSInteger)nr;
- (void)toggleKeyAtRow:(NSInteger)row col:(NSInteger)col;
- (void)toggleShiftLock;

- (void)scheduleKeyPress:(NSInteger)nr delay:(NSInteger)delay;
- (void)scheduleKeyPressAtRow:(NSInteger)row col:(NSInteger)col delay:(NSInteger)delay;
- (void)scheduleKeyRelease:(NSInteger)nr delay:(NSInteger)delay;
- (void)scheduleKeyReleaseAtRow:(NSInteger)row col:(NSInteger)col delay:(NSInteger)delay;
- (void)scheduleKeyReleaseAll:(NSInteger)delay;

@end

//
// ControlPort proxy
//

@interface ControlPortProxy : HardwareComponentProxy {
    
    JoystickProxy *joystick;
    MouseProxy *mouse;
}

@property (readonly) JoystickProxy *joystick;
@property (readonly) MouseProxy *mouse;

@end


//
// ExpansionPort proxy
//

@interface ExpansionPortProxy : HardwareComponentProxy { }

- (BOOL)cartridgeAttached;
- (CartridgeType)cartridgeType;
- (BOOL)attachCartridge:(CRTFileProxy *)c reset:(BOOL)reset;
- (void)attachGeoRamCartridge:(NSInteger)capacity;
- (void)attachIsepicCartridge;
- (void)detachCartridgeAndReset;

- (NSInteger)numButtons;
- (NSString *)getButtonTitle:(NSInteger)nr;
- (void)pressButton:(NSInteger)nr;
- (void)releaseButton:(NSInteger)nr;

- (BOOL)hasSwitch;
- (NSInteger)switchPosition;
- (NSString *)switchDescription:(NSInteger)pos;
- (NSString *)currentSwitchDescription;
- (BOOL)validSwitchPosition:(NSInteger)pos;
- (BOOL)switchIsNeutral;
- (BOOL)switchIsLeft;
- (BOOL)switchIsRight;
- (void)setSwitchPosition:(NSInteger)pos;

- (BOOL)hasLed;
- (BOOL)led;
- (void)setLed:(BOOL)value;

@property (readonly) NSInteger ramCapacity;

- (BOOL)hasBattery;
- (void)setBattery:(BOOL)value;

@end


//
// IEC bus proxy
//

@interface IECProxy : HardwareComponentProxy { }

@property (readonly) BOOL transferring;

@end


//
// Drive proxy
//

@interface DriveProxy : HardwareComponentProxy {
    
    VIAProxy *via1;
    VIAProxy *via2;
    DiskProxy *disk;
}

@property (readonly) VIAProxy *via1;
@property (readonly) VIAProxy *via2;
@property (readonly) DiskProxy *disk;

- (VIAProxy *)via:(NSInteger)num;

- (DriveConfig)getConfig;

- (BOOL)isConnected;
- (BOOL)isSwitchedOn;

- (BOOL)readMode;
- (BOOL)writeMode;

- (BOOL)redLED;
- (BOOL)hasDisk;
- (BOOL)hasModifiedDisk;
- (void)setModifiedDisk:(BOOL)b;
- (void)insertNewDisk:(DOSType)fstype;
- (void)insertG64:(G64FileProxy *)disk;
- (void)insertFileSystem:(FSDeviceProxy *)proxy;
- (void)ejectDisk;
- (BOOL)writeProtected;
- (void)setWriteProtection:(BOOL)b;
- (BOOL)hasWriteProtectedDisk;

- (Track)track;
- (Halftrack)halftrack;
- (u16)sizeOfHalftrack:(Halftrack)ht;
- (u16)sizeOfCurrentHalftrack;
- (u16)offset;
- (u8)readBitFromHead;

- (BOOL)isRotating;

@end


//
// VIA proxy
//

@interface VIAProxy : HardwareComponentProxy { }

@end


//
// Disk proxy
//

@interface DiskProxy : HardwareComponentProxy { }
    
- (BOOL)writeProtected;
- (void)setWriteProtected:(BOOL)b;
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
- (const char *)trackBitsAsString;
- (const char *)sectorHeaderBytesAsString:(Sector)nr hex:(BOOL)hex;
- (const char *)sectorDataBytesAsString:(Sector)nr hex:(BOOL)hex;

@end

//
// Datasette proxy
//

@interface DatasetteProxy : HardwareComponentProxy { }

@property (readonly) BOOL hasTape;
@property (readonly) NSInteger type;
@property (readonly) BOOL motor;
@property (readonly) BOOL playKey;

- (void)pressPlay;
- (void)pressStop;
- (void)rewind;
- (void)ejectTape;
- (BOOL)insertTape:(TAPFileProxy *)tape;

@end

//
// Mouse proxy
//

@interface MouseProxy : HardwareComponentProxy { }

@property MouseModel model;
- (void)setXY:(NSPoint)pos;
- (void)setLeftButton:(BOOL)pressed __attribute__ ((deprecated));
- (void)setRightButton:(BOOL)pressed __attribute__ ((deprecated));
- (void) trigger:(GamePadAction)event;

@end


//
// Joystick proxy
//

@interface JoystickProxy : HardwareComponentProxy { }

- (void) trigger:(GamePadAction)event;
@property BOOL autofire;
@property NSInteger autofireBullets;
@property float autofireFrequency;

@end


//
// F I L E   T Y P E   P R O X Y S
//

@protocol MakeWithFile
+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)ec;
@end

@protocol MakeWithBuffer
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)ec;
@end

@protocol MakeWithDisk
+ (instancetype)makeWithDisk:(DiskProxy *)disk error:(ErrorCode *)ec;
@end

@protocol MakeWithFileSystem
+ (instancetype)makeWithFileSystem:(FSDeviceProxy *)fs error:(ErrorCode *)ec;
@end

@protocol MakeWithCollection
+ (instancetype)makeWithCollection:(AnyCollectionProxy *)collection error:(ErrorCode *)ec;
@end

@protocol MakeWithD64
+ (instancetype)makeWithD64:(D64FileProxy *)d64 error:(ErrorCode *)ec;
@end

//
// AnyFile proxy
//

@interface AnyFileProxy : Proxy { }

- (void)dealloc;

@property (readonly) FileType type;
@property (readonly) NSString *name;
@property (readonly) u64 fnv;

- (void)setPath:(NSString *)path;
- (NSInteger)writeToFile:(NSString *)path error:(ErrorCode *)err;

@end

//
// AnyCollection proxy
//

@interface AnyCollectionProxy : AnyFileProxy { }

@end

//
// Snapshot proxy
//

@interface SnapshotProxy : AnyFileProxy <MakeWithFile, MakeWithBuffer>
{
   NSImage *preview;
}

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err;
+ (instancetype)makeWithC64:(C64Proxy *)c64proxy;

@property (readonly, strong) NSImage *previewImage;
@property (readonly) time_t timeStamp;

@end

//
// RomFile proxy
//

@interface RomFileProxy : AnyFileProxy <MakeWithFile, MakeWithBuffer> { }

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err;
 
@property (readonly) RomType romType;

@end

//
// CRTFile proxy
//

@interface CRTFileProxy : AnyFileProxy <MakeWithFile, MakeWithBuffer> { }

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err;
 
@property (readonly) CartridgeType cartridgeType;
@property (readonly) BOOL isSupported;
@property (readonly) NSInteger initialExromLine;
@property (readonly) NSInteger initialGameLine;
@property (readonly) NSInteger chipCount;

@end

//
// TAPFile proxy
//

@interface TAPFileProxy : AnyFileProxy <MakeWithFile, MakeWithBuffer> { }

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err;

@property (readonly) TAPVersion version;

@end

//
// T64File proxy
//

@interface T64FileProxy :
AnyCollectionProxy <MakeWithFile, MakeWithBuffer, MakeWithFileSystem> { }

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err;
+ (instancetype)makeWithFileSystem:(FSDeviceProxy *)proxy error:(ErrorCode *)err;

@end

//
// PRGFile proxy
//

@interface PRGFileProxy :
AnyCollectionProxy <MakeWithFile, MakeWithBuffer, MakeWithFileSystem> { }

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err;
+ (instancetype)makeWithFileSystem:(FSDeviceProxy *)proxy error:(ErrorCode *)err;

@end

//
// P00File proxy
//

@interface P00FileProxy :
AnyCollectionProxy <MakeWithFile, MakeWithBuffer, MakeWithFileSystem> { }

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err;
+ (instancetype)makeWithFileSystem:(FSDeviceProxy *)proxy error:(ErrorCode *)err;

@end

//
// D64File proxy
//

@interface D64FileProxy :
AnyFileProxy <MakeWithFile, MakeWithBuffer, MakeWithFileSystem> { }

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err;
+ (instancetype)makeWithFileSystem:(FSDeviceProxy *)proxy error:(ErrorCode *)err;

@end

//
// G64File proxy
//

@interface G64FileProxy :
AnyFileProxy <MakeWithFile, MakeWithBuffer, MakeWithDisk> { }

+ (instancetype) makeWithFile:(NSString *)path error:(ErrorCode *)err;
+ (instancetype) makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err;
+ (instancetype) makeWithDisk:(DiskProxy *)diskProxy error:(ErrorCode *)err;

@end

//
// Folder proxy
//

@interface FolderProxy : AnyCollectionProxy { }

+ (instancetype)makeWithFolder:(NSString *)path error:(ErrorCode *)err;

@property (readonly) FSDeviceProxy *fileSystem;

@end

//
// FSDevice proxy
//

@interface FSDeviceProxy : Proxy <MakeWithDisk, MakeWithCollection, MakeWithD64> { }

+ (instancetype)makeWithD64:(D64FileProxy *)d64 error:(ErrorCode *)err;;
+ (instancetype)makeWithDisk:(DiskProxy *)disk error:(ErrorCode *)err;
+ (instancetype)makeWithCollection:(AnyCollectionProxy *)collection error:(ErrorCode *)err;

@property (readonly) DOSType dos;
@property (readonly) NSInteger numCyls;
@property (readonly) NSInteger numHeads;
@property (readonly) NSInteger numTracks;
- (NSInteger)numSectors:(NSInteger)track;
@property (readonly) NSInteger numBlocks;

@property (readonly) NSInteger numFreeBlocks;
@property (readonly) NSInteger numUsedBlocks;
@property (readonly) NSInteger numFiles;

- (NSInteger)cylNr:(NSInteger)t;
- (NSInteger)headNr:(NSInteger)t;
- (NSInteger)trackNr:(NSInteger)c head:(NSInteger)h;
- (TSLink)tsLink:(NSInteger)b;
- (NSInteger)trackNr:(NSInteger)b;
- (NSInteger)sectorNr:(NSInteger)b;
- (NSInteger)blockNr:(TSLink)ts;
- (NSInteger)blockNr:(NSInteger)t sector:(NSInteger)s;
- (NSInteger)blockNr:(NSInteger)c head:(NSInteger)h sector:(NSInteger)s;

- (FSBlockType)blockType:(NSInteger)blockNr;
- (FSUsage)itemType:(NSInteger)blockNr pos:(NSInteger)pos;
- (FSErrorReport)check:(BOOL)strict;
- (ErrorCode)check:(NSInteger)nr pos:(NSInteger)pos expected:(unsigned char *)exp strict:(BOOL)strict;
- (BOOL)isCorrupted:(NSInteger)blockNr;
- (NSInteger)getCorrupted:(NSInteger)blockNr;
- (NSInteger)nextCorrupted:(NSInteger)blockNr;
- (NSInteger)prevCorrupted:(NSInteger)blockNr;
- (void)printDirectory;

- (NSInteger)readByte:(NSInteger)block offset:(NSInteger)offset;
- (BOOL)exportDirectory:(NSString *)path error:(ErrorCode *)err;

- (void)info;

- (NSString *)fileName:(NSInteger)nr;
- (FSFileType)fileType:(NSInteger)nr;
- (NSInteger)fileSize:(NSInteger)nr;
- (NSInteger)fileBlocks:(NSInteger)nr;

@end
