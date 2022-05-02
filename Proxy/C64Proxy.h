// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#import "Constants.h"
#import "config.h"
#import "C64Types.h"
#import "CartridgeTypes.h"
#import "CIATypes.h"
#import "ControlPortTypes.h"
#import "CPUTypes.h"
#import "DiskTypes.h"
#import "DiskAnalyzerTypes.h"
#import "DriveTypes.h"
#import "ErrorTypes.h"
#import "ExpansionPortTypes.h"
#import "FileTypes.h"
#import "FSTypes.h"
#import "C64ComponentTypes.h"
#import "JoystickTypes.h"
#import "MemoryTypes.h"
#import "MouseTypes.h"
#import "MsgQueueTypes.h"
#import "ParCableTypes.h"
#import "PowerSupplyTypes.h"
#import "SIDTypes.h"
#import "VICIITypes.h"

#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

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
@class DefaultsProxy;
@class DiskProxy;
@class DmaDebuggerProxy;
@class DriveProxy;
@class ExpansionPortProxy;
@class FileSystemProxy;
@class G64FileProxy;
@class GuardsProxy;
@class IECProxy;
@class JoystickProxy;
@class KeyboardProxy;
@class MemoryProxy;
@class MouseProxy;
@class MyController;
@class P00FileProxy;
@class ParCableProxy;
@class PRGFileProxy;
@class RecorderProxy;
@class RetroShellProxy;
@class RomFileProxy;
@class ScriptProxy;
@class SIDProxy;
@class SnapshotProxy;
@class T64FileProxy;
@class TAPFileProxy;
@class VIAProxy;
@class VICProxy;

//
// Exception wrapper
//

@interface ExceptionWrapper : NSObject {
    
    ErrorCode errorCode;
    NSString *what;
}

@property ErrorCode errorCode;
@property NSString *what;

@end

//
// Base proxies
//

@interface Proxy : NSObject {
    
    // Reference to the wrapped C++ object
    @public void *obj;
}
@end

@interface C64ComponentProxy : Proxy {
    
}
@end

//
// C64
//

@interface C64Proxy : C64ComponentProxy {
        
    CIAProxy *cia1;
    CIAProxy *cia2;
    ControlPortProxy *port1;
    ControlPortProxy *port2;
    CPUProxy *cpu;
    DatasetteProxy *datasette;
    DefaultsProxy *defaults;
    DmaDebuggerProxy *dmaDebugger;
    DriveProxy *drive8;
    DriveProxy *drive9;
    ExpansionPortProxy *expansionport;
    GuardsProxy *breakpoints;
    GuardsProxy *watchpoints;
    IECProxy *iec;
    KeyboardProxy *keyboard;
    MemoryProxy *mem;
    MouseProxy *mouse;
    ParCableProxy *parCable;
    RecorderProxy *recorder;
    RetroShellProxy *retroShell;
    SIDProxy *sid;
    VICProxy *vic;
}

@property (readonly, strong) CIAProxy *cia1;
@property (readonly, strong) CIAProxy *cia2;
@property (readonly, strong) ControlPortProxy *port1;
@property (readonly, strong) ControlPortProxy *port2;
@property (readonly, strong) CPUProxy *cpu;
@property (readonly, strong) DatasetteProxy *datasette;
@property (readonly, strong) DmaDebuggerProxy *dmaDebugger;
@property (readonly, strong) DriveProxy *drive8;
@property (readonly, strong) DriveProxy *drive9;
@property (readonly, strong) ExpansionPortProxy *expansionport;
@property (readonly, strong) GuardsProxy *breakpoints;
@property (readonly, strong) GuardsProxy *watchpoints;
@property (readonly, strong) IECProxy *iec;
@property (readonly, strong) KeyboardProxy *keyboard;
@property (readonly, strong) MemoryProxy *mem;
@property (readonly, strong) ParCableProxy *parCable;
@property (readonly, strong) RecorderProxy *recorder;
@property (readonly, strong) RetroShellProxy *retroShell;
@property (readonly, strong) SIDProxy *sid;
@property (readonly, strong) VICProxy *vic;

@property (class, readonly, strong) DefaultsProxy *defaults;

- (void)dealloc;
- (void)kill;

@property BOOL warpMode;
@property BOOL debugMode;
@property (readonly) NSInteger cpuLoad;

@property InspectionTarget inspectionTarget;
- (void) removeInspectionTarget;
- (void)inspect;

- (void)hardReset;
- (void)softReset;

@property (readonly) BOOL poweredOn;
@property (readonly) BOOL poweredOff;
@property (readonly) BOOL running;
@property (readonly) BOOL paused;

- (void)isReady:(ExceptionWrapper *)ex;
- (void)powerOn:(ExceptionWrapper *)ex;
- (void)powerOff;
- (void)run:(ExceptionWrapper *)ex;
- (void)pause;
- (void)halt;

- (void)suspend;
- (void)resume;
- (void)continueScript;

- (void)requestAutoSnapshot;
- (void)requestUserSnapshot;
@property (readonly) SnapshotProxy *latestAutoSnapshot;
@property (readonly) SnapshotProxy *latestUserSnapshot;

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

- (void)setListener:(const void *)sender function:(Callback *)func;

- (void)stopAndGo;
- (void)stepInto;
- (void)stepOver;

- (BOOL)hasRom:(RomType)type;
- (BOOL)hasMega65Rom:(RomType)type;

- (BOOL)isRom:(RomType)type url:(NSURL *)url;

- (void)loadRom:(NSURL *)url exception:(ExceptionWrapper *)ex;
- (void)loadRom:(RomFileProxy *)proxy;
- (void)saveRom:(RomType)type url:(NSURL *)url exception:(ExceptionWrapper *)ex;
- (void)deleteRom:(RomType)type;

- (RomIdentifier)romIdentifier:(RomType)type;
- (BOOL)isCommodoreRom:(RomIdentifier)rev;
- (BOOL)isPatchedRom:(RomIdentifier)rev;

- (NSString *)romTitle:(RomType)type;
- (NSString *)romSubTitle:(RomType)type;
- (NSString *)romRevision:(RomType)type;

- (void)flash:(AnyFileProxy *)container exception:(ExceptionWrapper *)ex;
- (void)flash:(FileSystemProxy *)proxy item:(NSInteger)nr exception:(ExceptionWrapper *)ex;

@end


//
// Defaults
//

@interface DefaultsProxy : Proxy { }

- (void)load:(NSURL *)url exception:(ExceptionWrapper *)ex;
- (void)save:(NSURL *)url exception:(ExceptionWrapper *)ex;

- (void)register:(NSString *)key value:(NSString *)value;

- (NSString *)getString:(NSString *)key;
- (NSInteger)getInt:(NSString *)key;
- (NSInteger)getOpt:(Option)option;
- (NSInteger)getOpt:(Option)option nr:(NSInteger)nr;

- (void)setKey:(NSString *)key value:(NSString *)value;
- (void)setOpt:(Option)option value:(NSInteger)value;
- (void)setOpt:(Option)option nr:(NSInteger)nr value:(NSInteger)value;

- (void)removeAll;
- (void)removeKey:(NSString *)key;
- (void)remove:(Option)option;
- (void)remove:(Option) option nr:(NSInteger)nr;

@end


//
// Guards (Breakpoints, Watchpoints)
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
// CPU
//

@interface CPUProxy : C64ComponentProxy { }

- (CPUInfo)getInfo;

@property (readonly) i64 cycles;
@property (readonly) u16 pc;

- (NSInteger)loggedInstructions;
- (NSInteger)loggedPCRel:(NSInteger)nr;
- (NSInteger)loggedPCAbs:(NSInteger)nr;
- (void)clearLog;
- (BOOL)isJammed;

- (void)setHex;
- (void)setDec;

// - (i64)cycle;
// - (u16)pc;

- (NSString *)disassembleRecordedInstr:(NSInteger)i length:(NSInteger *)len;
- (NSString *)disassembleRecordedBytes:(NSInteger)i;
- (NSString *)disassembleRecordedFlags:(NSInteger)i;
- (NSString *)disassembleRecordedPC:(NSInteger)i;

- (NSString *)disassembleInstr:(NSInteger)addr length:(NSInteger *)len;
- (NSString *)disassembleBytes:(NSInteger)addr;
- (NSString *)disassembleAddr:(NSInteger)addr;

@end


//
// CIA
//

@interface CIAProxy : C64ComponentProxy { }

- (CIAInfo)getInfo;

@end


//
// Memory
//

@interface MemoryProxy : C64ComponentProxy { }

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
// VICII
//


@interface VICProxy : C64ComponentProxy { }

@property (readonly) NSInteger hPixels;
@property (readonly) NSInteger vPixels;

- (VICIIConfig)getConfig;
- (BOOL)isPAL;
- (NSColor *)color:(NSInteger)nr;
- (UInt32)rgbaColor:(NSInteger)nr palette:(Palette)palette;
- (VICIIInfo)getInfo;
- (SpriteInfo)getSpriteInfo:(NSInteger)sprite;

@property (readonly) u32 *stableEmuTexture;
@property (readonly) u32 *noise;

@end


//
// DmaDebugger
//


@interface DmaDebuggerProxy : C64ComponentProxy { }

- (DmaDebuggerConfig)getConfig;

@end


//
// SID
//

@interface SIDProxy : C64ComponentProxy { }

- (SIDInfo)getInfo:(NSInteger)nr;
- (VoiceInfo)getVoiceInfo:(NSInteger)nr voice:(NSInteger)voice;
- (SIDStats) getStats;

- (double)sampleRate;
- (void)setSampleRate:(double)rate;

- (NSInteger)ringbufferSize;
- (void)ringbufferData:(NSInteger)offset left:(float *)l right:(float *)r;

- (void)copyMono:(float *)target size:(NSInteger)n;
- (void)copyStereo:(float *)target1 buffer2:(float *)target2 size:(NSInteger)n;
- (void)copyInterleaved:(float *)target size:(NSInteger)n;

- (void)rampUp;
- (void)rampUpFromZero;
- (void)rampDown;

@end


//
// Keyboard
//

@interface KeyboardProxy : C64ComponentProxy { }

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
// ControlPort
//

@interface ControlPortProxy : C64ComponentProxy {
    
    JoystickProxy *joystick;
    MouseProxy *mouse;
}

@property (readonly) JoystickProxy *joystick;
@property (readonly) MouseProxy *mouse;

@end


//
// ExpansionPort
//

@interface ExpansionPortProxy : C64ComponentProxy { }

- (BOOL)cartridgeAttached;
- (CartridgeType)cartridgeType;
- (void)attachCartridge:(CRTFileProxy *)c reset:(BOOL)reset exception:(ExceptionWrapper *)ex;
- (void)attachGeoRamCartridge:(NSInteger)capacity;
- (void)attachIsepicCartridge;
- (void)detachCartridgeAndReset;

- (NSInteger)numButtons;
- (NSString *)getButtonTitle:(NSInteger)nr;
- (void)pressButton:(NSInteger)nr;
- (void)releaseButton:(NSInteger)nr;

@property (readonly) BOOL hasSwitch;
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
// IEC bus
//

@interface IECProxy : C64ComponentProxy { }

@property (readonly) BOOL transferring;

@end


//
// Drive
//

@interface DriveProxy : C64ComponentProxy {
    
    VIAProxy *via1;
    VIAProxy *via2;
    DiskProxy *disk;
}

@property (readonly) VIAProxy *via1;
@property (readonly) VIAProxy *via2;
@property (readonly) DiskProxy *disk;
- (VIAProxy *)via:(NSInteger)num;

@property (readonly) DriveID id;

- (DriveConfig)getConfig;

- (BOOL)isConnected;
- (BOOL)isSwitchedOn;

- (BOOL)readMode;
- (BOOL)writeMode;

@property (readonly) BOOL hasDisk;
@property (readonly) BOOL hasModifiedDisk;
@property (readonly) BOOL hasProtectedDisk;
@property (readonly) BOOL hasUnmodifiedDisk;
@property (readonly) BOOL hasUnprotectedDisk;

- (BOOL)redLED;
- (void)setModifiedDisk:(BOOL)b;
- (void)insertNewDisk:(DOSType)fstype name:(NSString *)name;
- (void)insertD64:(D64FileProxy *)proxy protected:(BOOL)wp;
- (void)insertG64:(G64FileProxy *)proxy protected:(BOOL)wp;
- (void)insertCollection:(AnyCollectionProxy *)proxy protected:(BOOL)wp;
- (void)insertFileSystem:(FileSystemProxy *)proxy protected:(BOOL)wp;
- (void)ejectDisk;

- (Track)track;
- (Halftrack)halftrack;
- (NSInteger)sizeOfHalftrack:(Halftrack)ht;
- (NSInteger)sizeOfCurrentHalftrack;
- (NSInteger)offset;
- (u8)readBitFromHead;

- (BOOL)isRotating;

@end


//
// VIA
//

@interface VIAProxy : C64ComponentProxy { }

@end


//
// ParCable
//

@interface ParCableProxy : C64ComponentProxy { }

@end


//
// Disk
//

@interface DiskProxy : C64ComponentProxy { }
    
- (BOOL)writeProtected;
- (void)setWriteProtected:(BOOL)b;
- (void)toggleWriteProtection;

@end

//
// DiskAnalyzer
//

@interface DiskAnalyzerProxy : Proxy { }

- (instancetype) initWithDisk:(DiskProxy *)disk;
- (void)dealloc;

- (NSInteger)lengthOfTrack:(Track)t;
- (NSInteger)lengthOfHalftrack:(Halftrack)ht;

- (SectorInfo)sectorInfo:(Halftrack)ht sector:(Sector)s;
- (const char *)diskNameAsString;
- (const char *)trackBitsAsString:(Halftrack)ht;
- (const char *)sectorHeaderBytesAsString:(Halftrack)ht sector:(Sector)s hex:(BOOL)hex;
- (const char *)sectorDataBytesAsString:(Halftrack)ht sector:(Sector)s hex:(BOOL)hex;

- (NSInteger)numErrors:(Halftrack)ht;
- (NSString *)errorMessage:(Halftrack)ht nr:(NSInteger)nr;
- (NSInteger)firstErroneousBit:(Halftrack)ht nr:(NSInteger)nr;
- (NSInteger)lastErroneousBit:(Halftrack)ht nr:(NSInteger)nr;

@end

//
// Datasette
//

@interface DatasetteProxy : C64ComponentProxy { }

@property (readonly) BOOL hasTape;
@property (readonly) NSInteger type;
@property (readonly) BOOL motor;
@property (readonly) BOOL playKey;
@property (readonly) NSInteger counter;

- (void)pressPlay;
- (void)pressStop;
- (void)rewind;
- (void)ejectTape;
- (void)insertTape:(TAPFileProxy *)tape; // TODO: throw??

@end

//
// Mouse
//

@interface MouseProxy : C64ComponentProxy { }

- (BOOL)detectShakeAbs:(NSPoint)pos;
- (BOOL)detectShakeRel:(NSPoint)pos;
- (void)setXY:(NSPoint)pos;
- (void)setDxDy:(NSPoint)pos;
- (void) trigger:(GamePadAction)event;

@end


//
// Joystick
//

@interface JoystickProxy : C64ComponentProxy { }

- (void) trigger:(GamePadAction)event;

@end


//
// Recorder
//

@interface RecorderProxy : Proxy { }

@property NSString *path;
- (NSString *)findFFmpeg:(NSInteger)nr;
@property (readonly) BOOL hasFFmpeg;
@property (readonly) BOOL recording;
@property (readonly) double duration;
@property (readonly) NSInteger frameRate;
@property (readonly) NSInteger bitRate;
@property (readonly) NSInteger sampleRate;

- (void)startRecording:(NSRect)rect
bitRate:(NSInteger)rate
aspectX:(NSInteger)aspectX
aspectY:(NSInteger)aspectY
exception:(ExceptionWrapper *)ex;
- (void)stopRecording;
- (BOOL)exportAs:(NSString *)path;


@end


//
// RetroShell
//

@interface RetroShellProxy : Proxy { }

@property (readonly) NSInteger cursorRel;

- (NSString *)getText;
- (void)pressUp;
- (void)pressDown;
- (void)pressLeft;
- (void)pressRight;
- (void)pressHome;
- (void)pressEnd;
- (void)pressBackspace;
- (void)pressDelete;
- (void)pressReturn;
- (void)pressTab;
- (void)pressKey:(char)c;

@end


//
// F I L E   T Y P E   P R O X Y S
//

@protocol MakeWithFile
+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex;
@end

@protocol MakeWithBuffer
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex;
@end

@protocol MakeWithDisk
+ (instancetype)makeWithDisk:(DiskProxy *)disk exception:(ExceptionWrapper *)ex;
@end

@protocol MakeWithFileSystem
+ (instancetype)makeWithFileSystem:(FileSystemProxy *)fs exception:(ExceptionWrapper *)ex;
@end

@protocol MakeWithCollection
+ (instancetype)makeWithCollection:(AnyCollectionProxy *)collection exception:(ExceptionWrapper *)ex;
@end

@protocol MakeWithD64
+ (instancetype)makeWithD64:(D64FileProxy *)d64 exception:(ExceptionWrapper *)ex;;
@end

@protocol MakeWithFolder
+ (instancetype)makeWithFolder:(NSString *)path exception:(ExceptionWrapper *)ex;
@end

//
// AnyFile
//

@interface AnyFileProxy : Proxy { }

+ (FileType) typeOfUrl:(NSURL *)url;

@property (readonly) FileType type;
@property (readonly) NSString *name;
@property (readonly) u64 fnv;

- (void)setPath:(NSString *)path;
- (void)writeToFile:(NSString *)path exception:(ExceptionWrapper *)ex;

@end

//
// AnyCollection
//

@interface AnyCollectionProxy : AnyFileProxy { }

@end

//
// Snapshot
//

@interface SnapshotProxy : AnyFileProxy <MakeWithFile, MakeWithBuffer>
{
   NSImage *preview;
}

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithC64:(C64Proxy *)c64proxy;

@property (readonly, strong) NSImage *previewImage;
@property (readonly) time_t timeStamp;

@end

//
// Script
//

@interface ScriptProxy : AnyFileProxy <MakeWithFile, MakeWithBuffer> { }

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex;
 
- (void)execute:(C64Proxy *)proxy;

@end

//
// RomFile
//

@interface RomFileProxy : AnyFileProxy <MakeWithFile, MakeWithBuffer> { }

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex;
 
@end

//
// CRTFile
//

@interface CRTFileProxy : AnyFileProxy <MakeWithFile, MakeWithBuffer> { }

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex;
 
@property (readonly) CartridgeType cartridgeType;
@property (readonly) BOOL isSupported;
@property (readonly) NSInteger initialExromLine;
@property (readonly) NSInteger initialGameLine;
@property (readonly) NSInteger chipCount;

@end

//
// TAPFile
//

@interface TAPFileProxy : AnyFileProxy <MakeWithFile, MakeWithBuffer> { }

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex;

@property (readonly) TAPVersion version;

@end

//
// T64File
//

@interface T64FileProxy :
AnyCollectionProxy <MakeWithFile, MakeWithBuffer, MakeWithFileSystem> { }

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithFileSystem:(FileSystemProxy *)proxy exception:(ExceptionWrapper *)ex;

@end

//
// PRGFile
//

@interface PRGFileProxy :
AnyCollectionProxy <MakeWithFile, MakeWithBuffer, MakeWithFileSystem> { }

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithFileSystem:(FileSystemProxy *)proxy exception:(ExceptionWrapper *)ex;

@end

//
// P00File
//

@interface P00FileProxy :
AnyCollectionProxy <MakeWithFile, MakeWithBuffer, MakeWithFileSystem> { }

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithFileSystem:(FileSystemProxy *)proxy exception:(ExceptionWrapper *)ex;

@end

//
// D64File
//

@interface D64FileProxy :
AnyFileProxy <MakeWithFile, MakeWithBuffer, MakeWithFileSystem> { }

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithFileSystem:(FileSystemProxy *)proxy exception:(ExceptionWrapper *)ex;

@end

//
// G64File
//

@interface G64FileProxy :
AnyFileProxy <MakeWithFile, MakeWithBuffer, MakeWithDisk> { }

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithDisk:(DiskProxy *)diskProxy exception:(ExceptionWrapper *)ex;

@end

//
// Folder
//

@interface FolderProxy :
AnyCollectionProxy <MakeWithFolder> { }

+ (instancetype)makeWithFolder:(NSString *)path exception:(ExceptionWrapper *)ex;

@property (readonly) FileSystemProxy *fileSystem;

@end

//
// FileSystem
//

@interface FileSystemProxy : Proxy <MakeWithDisk, MakeWithCollection, MakeWithD64> { }

+ (instancetype)makeWithD64:(D64FileProxy *)d64 exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithDisk:(DiskProxy *)disk exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithCollection:(AnyCollectionProxy *)collection exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithDiskType:(DiskType)diskType dosType:(DOSType)dosType;

@property NSString *name;
@property (readonly) NSString *idString;
@property (readonly) NSString *capacityString;
@property (readonly) NSString *fillLevelString;
@property (readonly) DOSType dos;
@property (readonly) NSInteger numCyls;
@property (readonly) NSInteger numHeads;
@property (readonly) NSInteger numTracks;
- (NSInteger)numSectors:(NSInteger)track;
@property (readonly) NSInteger numBlocks;

@property (readonly) NSInteger freeBlocks;
@property (readonly) NSInteger usedBlocks;
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
- (NSString *)ascii:(NSInteger)block offset:(NSInteger)offset length:(NSInteger)len;
- (void)export:(NSString *)path exception:(ExceptionWrapper *)ex;

- (void)info;

- (BOOL)isFree:(NSInteger)blockNr;

- (NSString *)fileName:(NSInteger)nr;
- (FSFileType)fileType:(NSInteger)nr;
- (NSInteger)fileSize:(NSInteger)nr;
- (NSInteger)fileBlocks:(NSInteger)nr;

- (FSBlockType)getDisplayType:(NSInteger)column;
- (NSInteger)diagnoseImageSlice:(NSInteger)column;
- (NSInteger)nextBlockOfType:(FSBlockType)type after:(NSInteger)after;
- (NSInteger)nextCorruptedBlock:(NSInteger)after;

@end
