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
#import "CmdQueueTypes.h"
#import "ControlPortTypes.h"
#import "CPUTypes.h"
#import "DatasetteTypes.h"
#import "DiskTypes.h"
#import "DiskAnalyzerTypes.h"
#import "DriveTypes.h"
#import "EmulatorTypes.h"
#import "ErrorTypes.h"
#import "ExpansionPortTypes.h"
#import "FileTypes.h"
#import "FSTypes.h"
#import "CoreComponentTypes.h"
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
@class EmulatorProxy;
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
@class PRGFileProxy;
@class RecorderProxy;
@class RetroShellProxy;
@class RomFileProxy;
@class ScriptProxy;
@class SIDProxy;
@class SnapshotProxy;
@class T64FileProxy;
@class TAPFileProxy;
@class VICIIProxy;


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

@interface CoreComponentProxy : Proxy {
    
}
@end

@interface SubComponentProxy : Proxy {

    @public void *emu;
}
@end


//
// C64
//

@interface EmulatorProxy : CoreComponentProxy {
        
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
    RecorderProxy *recorder;
    RetroShellProxy *retroShell;
    SIDProxy *sid;
    VICIIProxy *vic;
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
@property (readonly, strong) RecorderProxy *recorder;
@property (readonly, strong) RetroShellProxy *retroShell;
@property (readonly, strong) SIDProxy *sid;
@property (readonly, strong) VICIIProxy *vic;

@property (class, readonly, strong) DefaultsProxy *defaults;

- (void)dealloc;
- (void)kill;

- (void)launch:(const void *)listener function:(Callback *)func;

@property (readonly) EmulatorInfo info;
@property (readonly) EmulatorStats stats;
@property (readonly) EventInfo eventInfo;
- (EventSlotInfo)getEventSlotInfo:(NSInteger)slot;

@property (readonly) BOOL poweredOn;
@property (readonly) BOOL poweredOff;
@property (readonly) BOOL paused;
@property (readonly) BOOL running;
@property (readonly) BOOL suspended;
@property (readonly) BOOL halted;
@property (readonly) BOOL warping;
@property (readonly) BOOL tracking;

@property BOOL trackMode;

@property InspectionTarget inspectionTarget;
- (void) removeInspectionTarget;

- (void)hardReset;
- (void)softReset;

- (void)isReady:(ExceptionWrapper *)ex;
- (void)powerOn:(ExceptionWrapper *)ex;
- (void)powerOff;
- (void)run:(ExceptionWrapper *)ex;
- (void)pause;
- (void)halt;

- (void)suspend;
- (void)resume;

- (void)requestAutoSnapshot;
- (void)requestUserSnapshot;

@property (readonly) SnapshotProxy *latestAutoSnapshot;
@property (readonly) SnapshotProxy *latestUserSnapshot;

- (NSInteger)getConfig:(Option)opt;
- (NSInteger)getConfig:(Option)opt id:(NSInteger)id;
- (NSInteger)getConfig:(Option)opt drive:(NSInteger)id;
- (BOOL)configure:(Option)opt value:(NSInteger)val;
- (BOOL)configure:(Option)opt enable:(BOOL)val;
- (BOOL)configure:(Option)opt id:(NSInteger)id value:(NSInteger)val;
- (BOOL)configure:(Option)opt id:(NSInteger)id enable:(BOOL)val;
- (BOOL)configure:(Option)opt drive:(NSInteger)id value:(NSInteger)val;
- (BOOL)configure:(Option)opt drive:(NSInteger)id enable:(BOOL)val;
- (void)configure:(C64Model)value;

- (void)wakeUp;

- (void)stopAndGo;
- (void)stepInto;
- (void)stepOver;
- (void)signalBrk;

- (RomInfo)getRomInfo:(RomType)type;
@property (readonly) RomInfo basicRom;
@property (readonly) RomInfo charRom;
@property (readonly) RomInfo kernalRom;
@property (readonly) RomInfo vc1541Rom;

- (BOOL)isRom:(RomType)type url:(NSURL *)url;

- (void)loadRom:(NSURL *)url exception:(ExceptionWrapper *)ex;
- (void)loadRom:(RomFileProxy *)proxy;
- (void)saveRom:(RomType)type url:(NSURL *)url exception:(ExceptionWrapper *)ex;
- (void)deleteRom:(RomType)type;

- (void)flash:(AnyFileProxy *)container exception:(ExceptionWrapper *)ex;
- (void)flash:(FileSystemProxy *)proxy item:(NSInteger)nr exception:(ExceptionWrapper *)ex;

- (void)send:(CmdType)cmd;
- (void)send:(CmdType)type value:(NSInteger)value;
- (void)send:(CmdType)type key:(KeyCmd)cmd;
- (void)send:(CmdType)type coord:(CoordCmd)cmd;
- (void)send:(CmdType)type action:(GamePadCmd)cmd;

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

@interface CPUProxy : CoreComponentProxy { }

@property (readonly) CPUInfo info;
@property (readonly) i64 clock;
@property (readonly) u16 pc;

- (NSInteger)loggedInstructions;
- (NSInteger)loggedPCRel:(NSInteger)nr;
- (NSInteger)loggedPCAbs:(NSInteger)nr;
- (void)clearLog;

- (void)setHex;
- (void)setDec;

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

@interface CIAProxy : CoreComponentProxy { }

- (CIAInfo)getInfo;

@end


//
// Memory
//

@interface MemoryProxy : CoreComponentProxy { }

- (MemConfig)getConfig;
- (MemInfo)getInfo;

- (NSString *)memdump:(NSInteger)addr num:(NSInteger)num hex:(BOOL)hex src:(MemoryType)src;
- (NSString *)txtdump:(NSInteger)addr num:(NSInteger)num src:(MemoryType)src;

@end


//
// VICII
//


@interface VICIIProxy : CoreComponentProxy { }

@property (readonly) NSInteger hPixels;
@property (readonly) NSInteger vPixels;

- (VICIIConfig)getConfig;
- (VICIIInfo)getInfo;
- (SpriteInfo)getSpriteInfo:(NSInteger)sprite;

- (BOOL)isPAL;
- (NSColor *)color:(NSInteger)nr;
- (UInt32)rgbaColor:(NSInteger)nr palette:(Palette)palette;

@property (readonly) u32 *texture;
@property (readonly) u32 *noise;

@end


//
// DmaDebugger
//

@interface DmaDebuggerProxy : CoreComponentProxy { }

- (DmaDebuggerConfig)getConfig;

@end


//
// SID
//

@interface SIDProxy : CoreComponentProxy { }

- (SIDInfo)getInfo:(NSInteger)nr;
- (VoiceInfo)getVoiceInfo:(NSInteger)nr voice:(NSInteger)voice;
@property (readonly) SIDStats stats;

- (void)copyMono:(float *)target size:(NSInteger)n;
- (void)copyStereo:(float *)target1 buffer2:(float *)target2 size:(NSInteger)n;
- (void)copyInterleaved:(float *)target size:(NSInteger)n;

- (void)rampUp;
- (void)rampUpFromZero;
- (void)rampDown;

- (float)drawWaveform:(u32 *)buffer w:(NSInteger)w h:(NSInteger)h scale:(float)s color:(u32)c source:(NSInteger)source;
- (float)drawWaveform:(u32 *)buffer size:(NSSize)size scale:(float)s color:(u32)c source:(NSInteger)source;

@end


//
// Keyboard
//

@interface KeyboardProxy : SubComponentProxy { }

- (BOOL)isPressed:(NSInteger)nr;

- (void)pressKey:(NSInteger)nr;
- (void)releaseKey:(NSInteger)nr;
- (void)releaseKey:(NSInteger)nr delay:(double)delay;
- (void)releaseAll;
- (void)releaseAllWithDelay:(double)delay;
- (void)toggleKey:(NSInteger)nr;
- (void)toggleKeyAtRow:(NSInteger)row col:(NSInteger)col;

- (void)autoType:(NSString *)text;
- (void)abortAutoTyping;

@end


//
// ControlPort
//

@interface ControlPortProxy : CoreComponentProxy {
    
    JoystickProxy *joystick;
    MouseProxy *mouse;
}

@property (readonly) JoystickProxy *joystick;
@property (readonly) MouseProxy *mouse;

@end


//
// ExpansionPort
//

@interface ExpansionPortProxy : SubComponentProxy { }

@property (readonly) CartridgeTraits traits;
@property (readonly) CartridgeInfo info;
- (CartridgeRomInfo)getRomInfo:(NSInteger)nr;

- (BOOL)cartridgeAttached;
- (void)attachCartridge:(CRTFileProxy *)c reset:(BOOL)reset exception:(ExceptionWrapper *)ex;
- (void)attachReuCartridge:(NSInteger)capacity;
- (void)attachGeoRamCartridge:(NSInteger)capacity;
- (void)attachIsepicCartridge;
- (void)detachCartridge;

@end


//
// IEC bus
//

@interface IECProxy : CoreComponentProxy { }

@end


//
// Drive
//

@interface DriveProxy : SubComponentProxy {

    DiskProxy *disk;
}

@property (readonly) DiskProxy *disk;
@property (readonly) DriveConfig config;
@property (readonly) DriveInfo info;

- (void)insertBlankDisk:(DOSType)fstype name:(NSString *)name;
- (void)insertD64:(D64FileProxy *)proxy protected:(BOOL)wp;
- (void)insertG64:(G64FileProxy *)proxy protected:(BOOL)wp;
- (void)insertCollection:(AnyCollectionProxy *)proxy protected:(BOOL)wp;
- (void)insertFileSystem:(FileSystemProxy *)proxy protected:(BOOL)wp;
- (void)ejectDisk;

@end


//
// Disk
//

@interface DiskProxy : CoreComponentProxy { }

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

@interface DatasetteProxy : SubComponentProxy { }

@property (readonly) DatasetteInfo info;

- (void)pressPlay;
- (void)pressStop;
- (void)rewind;
- (void)insertTape:(TAPFileProxy *)tape;
- (void)ejectTape;

@end


//
// Mouse
//

@interface MouseProxy : CoreComponentProxy { }

- (BOOL)detectShakeAbs:(NSPoint)pos;
- (BOOL)detectShakeRel:(NSPoint)pos;

@end


//
// Joystick
//

@interface JoystickProxy : CoreComponentProxy { }

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
- (void)pressCut;
- (void)pressReturn;
- (void)pressShiftReturn;
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
+ (instancetype)makeWithC64:(EmulatorProxy *)c64proxy;

@property (readonly, strong) NSImage *previewImage;
@property (readonly) time_t timeStamp;

@end


//
// Script
//

@interface ScriptProxy : AnyFileProxy <MakeWithFile, MakeWithBuffer> { }

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex;
 
- (void)execute:(EmulatorProxy *)proxy;

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
