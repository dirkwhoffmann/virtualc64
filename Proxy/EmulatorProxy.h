// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#import "config.h"
#import "VirtualC64Types.h"
#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

using namespace vc64;

@class AudioPortProxy;
@class AnyFileProxy;
@class AnyCollectionProxy;
@class EmulatorProxy;
@class C64Proxy;
@class CIAProxy;
@class Constants;
@class ControlPortProxy;
@class CPUProxy;
@class DatasetteProxy;
@class DefaultsProxy;
@class DiskProxy;
@class DmaDebuggerProxy;
@class DriveProxy;
@class ExpansionPortProxy;
@class FileSystemProxy;
@class G64FileProxy;
@class SerialPortProxy;
@class JoystickProxy;
@class KeyboardProxy;
@class MediaFileProxy;
@class MemoryProxy;
@class MouseProxy;
@class MyController;
@class RecorderProxy;
@class RetroShellProxy;
@class ScriptProxy;
@class SIDProxy;
@class T64FileProxy;
@class VICIIProxy;
@class VideoPortProxy;

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
    
    // Reference to the wrapped C++ instance
    @public void *obj;
}

- (instancetype) initWith:(void *)ref;

@end

@interface SubComponentProxy : Proxy {

    // Reference to the emulator instance
    @public void *emu;
}
@end

//
// Constants
//

@interface Constants : NSObject {

}

@property (class, readonly) NSInteger texWidth;
@property (class, readonly) NSInteger texHeight;

@end

//
// Emulator
//

@interface EmulatorProxy : Proxy {
        
    AudioPortProxy *audioPort;
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
    SerialPortProxy *iec;
    KeyboardProxy *keyboard;
    MemoryProxy *mem;
    MouseProxy *mouse;
    RecorderProxy *recorder;
    RetroShellProxy *retroShell;
    SIDProxy *sid;
    VICIIProxy *vic;
}

@property (class, readonly, strong) DefaultsProxy *defaults;

@property (readonly, strong) AudioPortProxy *audioPort;
@property (readonly, strong) C64Proxy *c64;
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
@property (readonly, strong) SerialPortProxy *iec;
@property (readonly, strong) KeyboardProxy *keyboard;
@property (readonly, strong) MemoryProxy *mem;
@property (readonly, strong) RecorderProxy *recorder;
@property (readonly, strong) RetroShellProxy *retroShell;
@property (readonly, strong) SIDProxy *sid;
@property (readonly, strong) VICIIProxy *vic;
@property (readonly, strong) VideoPortProxy *videoPort;


- (void)dealloc;
- (void)kill;

@property (class, readonly) NSString *build;
@property (class, readonly) NSString *version;

@property (readonly) EmulatorConfig config;
@property (readonly) EmulatorInfo info;
@property (readonly) EmulatorInfo cachedInfo;
@property (readonly) EmulatorStats stats;

@property (readonly) BOOL poweredOn;
@property (readonly) BOOL poweredOff;
@property (readonly) BOOL paused;
@property (readonly) BOOL running;
@property (readonly) BOOL suspended;
@property (readonly) BOOL halted;
@property (readonly) BOOL warping;
@property (readonly) BOOL tracking;

- (void)isReady:(ExceptionWrapper *)ex;
- (void)powerOn:(ExceptionWrapper *)ex;
- (void)powerOff;
- (void)run:(ExceptionWrapper *)ex;
- (void)pause;
- (void)halt;
- (void)suspend;
- (void)resume;
- (void)warpOn;
- (void)warpOn:(NSInteger)source;
- (void)warpOff;
- (void)warpOff:(NSInteger)source;
- (void)trackOn;
- (void)trackOn:(NSInteger)source;
- (void)trackOff;
- (void)trackOff:(NSInteger)source;

- (void)stepInto;
- (void)stepOver;

- (void)wakeUp;

- (void)launch:(const void *)listener function:(Callback *)func;

- (NSInteger)get:(Option)opt;
- (NSInteger)get:(Option)opt id:(NSInteger)id;
- (NSInteger)get:(Option)opt drive:(NSInteger)id;
- (BOOL)set:(Option)opt value:(NSInteger)val;
- (BOOL)set:(Option)opt enable:(BOOL)val;
- (BOOL)set:(Option)opt id:(NSInteger)id value:(NSInteger)val;
- (BOOL)set:(Option)opt id:(NSInteger)id enable:(BOOL)val;
- (BOOL)set:(Option)opt drive:(NSInteger)id value:(NSInteger)val;
- (BOOL)set:(Option)opt drive:(NSInteger)id enable:(BOOL)val;
- (void)set:(C64Model)value;

- (void)exportConfig:(NSURL *)url exception:(ExceptionWrapper *)ex;

- (void)put:(CmdType)cmd;
- (void)put:(CmdType)type value:(NSInteger)value;
- (void)put:(CmdType)type value:(NSInteger)value value2:(NSInteger)value2;
- (void)put:(CmdType)type key:(KeyCmd)cmd;
- (void)put:(CmdType)type coord:(CoordCmd)cmd;
- (void)put:(CmdType)type action:(GamePadCmd)cmd;

- (BOOL)isRom:(RomType)type url:(NSURL *)url;

- (void)loadRom:(NSURL *)url exception:(ExceptionWrapper *)ex;
- (void)loadRom:(MediaFileProxy *)proxy;
- (void)saveRom:(RomType)type url:(NSURL *)url exception:(ExceptionWrapper *)ex;
- (void)deleteRom:(RomType)type;

- (void)flash:(MediaFileProxy *)container exception:(ExceptionWrapper *)ex;
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
// C64
//

@interface C64Proxy : SubComponentProxy { }

@property (readonly) C64Info info;
@property (readonly) C64Info cachedInfo;
- (EventSlotInfo)getEventSlotInfo:(NSInteger)slot;

- (RomTraits)getRomTraits:(RomType)type;
@property (readonly) RomTraits basicRom;
@property (readonly) RomTraits charRom;
@property (readonly) RomTraits kernalRom;
@property (readonly) RomTraits vc1541Rom;

- (void)hardReset;
- (void)softReset;

@property InspectionTarget inspectionTarget;
- (void) removeInspectionTarget;

- (MediaFileProxy *) takeSnapshot;

@end

//
// CPU
//

struct GuardInfo {

    u32 addr;
    bool enabled;
    long hits;
    long ignore;
};

@interface CPUProxy : SubComponentProxy { }

@property (readonly) CPUInfo info;
@property (readonly) CPUInfo cachedInfo;

- (NSInteger)loggedInstructions;
- (void)clearLog;

- (void)setHex;
- (void)setDec;

- (NSString *)disassemble:(NSInteger)addr format:(NSString *)fmt length:(NSInteger *)len;
- (NSString *)disassembleRecorded:(NSInteger)i format:(NSString *)fmt length:(NSInteger *)len;

- (BOOL) hasBreakpointWithNr:(NSInteger)nr;
- (GuardInfo) breakpointWithNr:(NSInteger)nr;
- (BOOL) hasBreakpointAtAddr:(NSInteger)addr;
- (GuardInfo) breakpointAtAddr:(NSInteger)addr;

- (BOOL) hasWatchpointWithNr:(NSInteger)nr;
- (GuardInfo) watchpointWithNr:(NSInteger)nr;
- (BOOL) hasWatchpointAtAddr:(NSInteger)addr;
- (GuardInfo) watchpointAtAddr:(NSInteger)addr;

@end


//
// CIA
//

@interface CIAProxy : SubComponentProxy { }

@property (readonly) CIAConfig config;
@property (readonly) CIAInfo info;
@property (readonly) CIAInfo cachedInfo;
@property (readonly) CIAStats stats;

@end


//
// Memory
//

@interface MemoryProxy : SubComponentProxy { }

@property (readonly) MemConfig config;
@property (readonly) MemInfo info;
@property (readonly) MemInfo cachedInfo;

- (NSString *)memdump:(NSInteger)addr num:(NSInteger)num hex:(BOOL)hex src:(MemoryType)src;
- (NSString *)txtdump:(NSInteger)addr num:(NSInteger)num src:(MemoryType)src;

- (void)drawHeatmap:(u32 *)buffer w:(NSInteger)w h:(NSInteger)h;

@end


//
// VICII
//


@interface VICIIProxy : SubComponentProxy { }

@property (readonly) VICIITraits traits;
@property (readonly) VICIIConfig config;
@property (readonly) VICIIInfo info;
@property (readonly) VICIIInfo cachedInfo;
- (SpriteInfo)getSpriteInfo:(NSInteger)sprite;

- (NSColor *)color:(NSInteger)nr;
- (UInt32)rgbaColor:(NSInteger)nr palette:(Palette)palette;

@end


//
// DmaDebugger
//

@interface DmaDebuggerProxy : SubComponentProxy { }

- (DmaDebuggerConfig)getConfig;

@end


//
// SID
//

@interface SIDProxy : SubComponentProxy { }

- (SIDInfo)getInfo:(NSInteger)nr;

- (float)drawWaveform:(u32 *)buffer w:(NSInteger)w h:(NSInteger)h scale:(float)s color:(u32)c source:(NSInteger)source;
- (float)drawWaveform:(u32 *)buffer size:(NSSize)size scale:(float)s color:(u32)c source:(NSInteger)source;

@end


//
// Audio port
//

@interface AudioPortProxy : SubComponentProxy { }

@property (readonly) AudioPortStats stats;

- (void)copyMono:(float *)target size:(NSInteger)n;
- (void)copyStereo:(float *)target1 buffer2:(float *)target2 size:(NSInteger)n;
- (void)copyInterleaved:(float *)target size:(NSInteger)n;

@end


//
// Video port
//

@interface VideoPortProxy : SubComponentProxy { }

@property (readonly) u32 *texture;

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

@interface ControlPortProxy : SubComponentProxy {

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
@property (readonly) CartridgeInfo cachedInfo;
- (CartridgeRomInfo)getRomInfo:(NSInteger)nr;

- (BOOL)cartridgeAttached;
- (void)attachCartridge:(MediaFileProxy *)c reset:(BOOL)reset exception:(ExceptionWrapper *)ex;
- (void)attachReuCartridge:(NSInteger)capacity;
- (void)attachGeoRamCartridge:(NSInteger)capacity;
- (void)attachIsepicCartridge;
- (void)detachCartridge;

@end


//
// Serial port
//

@interface SerialPortProxy : SubComponentProxy { }

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
@property (readonly) DriveInfo cachedInfo;

- (void)insertBlankDisk:(DOSType)fstype name:(NSString *)name;
- (void)insertMedia:(MediaFileProxy *)proxy protected:(BOOL)wp;
- (void)insertCollection:(AnyCollectionProxy *)proxy protected:(BOOL)wp;
- (void)insertFileSystem:(FileSystemProxy *)proxy protected:(BOOL)wp;
- (void)ejectDisk;

@end


//
// Disk
//

@interface DiskProxy : SubComponentProxy { }

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

- (NSString *)getLogbook:(Halftrack)ht;

@end


//
// Datasette
//

@interface DatasetteProxy : SubComponentProxy { }

@property (readonly) DatasetteInfo info;
@property (readonly) DatasetteInfo cachedInfo;

- (void)pressPlay;
- (void)pressStop;
- (void)rewind;
- (void)insertTape:(MediaFileProxy *)tape;
- (void)ejectTape;

@end


//
// Mouse
//

@interface MouseProxy : SubComponentProxy { }

- (BOOL)detectShakeAbs:(NSPoint)pos;
- (BOOL)detectShakeRel:(NSPoint)pos;

@end


//
// Joystick
//

@interface JoystickProxy : SubComponentProxy { }

@end


//
// Recorder
//

@interface RecorderProxy : SubComponentProxy { }

@property (readonly) RecorderConfig config;
@property (readonly) RecorderInfo info;
@property (readonly) RecorderInfo cachedInfo;

@property NSString *path;
- (NSString *)findFFmpeg:(NSInteger)nr;
@property (readonly) BOOL hasFFmpeg;
@property (readonly) BOOL recording;

- (void)startRecording:(NSRect)rect exception:(ExceptionWrapper *)ex;
- (void)stopRecording;
- (BOOL)exportAs:(NSString *)path;


@end


//
// RetroShell
//

@interface RetroShellProxy : SubComponentProxy { }

@property (readonly) NSInteger cursorRel;

- (NSString *)getText;
- (void)pressKey:(char)c;
- (void)pressSpecialKey:(RetroShellKey)key;

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
// MediaFile
//

@interface MediaFileProxy : AnyFileProxy
{
    NSImage *preview;
}

+ (FileType) typeOfUrl:(NSURL *)url;

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithFile:(NSString *)path type:(FileType)t exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len type:(FileType)t exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithC64:(EmulatorProxy *)c64proxy;
+ (instancetype)makeWithDrive:(DriveProxy *)proxy type:(FileType)t exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithFileSystem:(FileSystemProxy *)proxy type:(FileType)t exception:(ExceptionWrapper *)ex;

@property (readonly) FileType type;
@property (readonly) NSString *name;
@property (readonly) u64 fnv;

- (void)writeToFile:(NSString *)path exception:(ExceptionWrapper *)ex;

@property (readonly, strong) NSImage *previewImage;
@property (readonly) time_t timeStamp;

@end


//
// AnyCollection
//

@interface AnyCollectionProxy : AnyFileProxy { }

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
// T64File
//

@interface T64FileProxy :
AnyCollectionProxy <MakeWithFile, MakeWithBuffer, MakeWithFileSystem> { }

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

@interface FileSystemProxy : Proxy <MakeWithDisk, MakeWithCollection> { }

+ (instancetype)makeWithDisk:(DiskProxy *)disk exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithCollection:(AnyCollectionProxy *)collection exception:(ExceptionWrapper *)ex;
+ (instancetype)makeWithDiskType:(DiskType)diskType dosType:(DOSType)dosType;
+ (instancetype)makeWithMediaFile:(MediaFileProxy *)file exception:(ExceptionWrapper *)ex;

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
