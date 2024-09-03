// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#import "config.h"
#import "EmulatorProxy.h"
#import "VirtualC64.h"
#import "DiskAnalyzer.h"
#import "FFmpeg.h"
#import "FileSystem.h"

using namespace vc64;

//
// Exception wrapper
//

@implementation ExceptionWrapper

@synthesize errorCode;
@synthesize what;

- (instancetype)init {

    if (self = [super init]) {
        
        errorCode = VC64ERROR_OK;
        what = @"";
    }
    return self;
}

- (void)save:(const Error &)exception
{
    errorCode = ErrorCode(exception.data);
    what = @(exception.what());
}

@end


//
// Base Proxy
//

@implementation Proxy

- (instancetype) initWith:(void *)ref
{
    if (ref == nil) {
        return nil;
    }
    if (self = [super init]) {
        obj = ref;
    }
    return self;
}

@end


//
// SubComponentProxy proxy
//

@implementation SubComponentProxy

- (instancetype) initWith:(void *)ref emu:(VirtualC64 *)emuref
{
    if (ref == nil || emuref == nil) {
        return nil;
    }
    if (self = [super init]) {
        obj = ref;
        emu = emuref;
    }
    return self;
}

@end

//
// Defaults
//

@implementation DefaultsProxy

- (DefaultsAPI *)props
{
    return (DefaultsAPI *)obj;
}

- (void)load:(NSURL *)url exception:(ExceptionWrapper *)ex
{
    try { return [self props]->load([url fileSystemRepresentation]); }
    catch (Error &error) { [ex save:error]; }
}

- (void)save:(NSURL *)url exception:(ExceptionWrapper *)ex
{
    try { return [self props]->save([url fileSystemRepresentation]); }
    catch (Error &error) { [ex save:error]; }
}

- (void)register:(NSString *)key value:(NSString *)value
{
    [self props]->setFallback(string([key UTF8String]), string([value UTF8String]));
}

- (NSString *)getString:(NSString *)key
{
    auto result = [self props]->getRaw([key UTF8String]);
    return @(result.c_str());
}

- (NSInteger)getInt:(NSString *)key
{
    return [self props]->get([key UTF8String]);
}

- (NSInteger)getOpt:(Option)option
{
    return [self props]->get(option);
}

- (NSInteger)getOpt:(Option)option nr:(NSInteger)nr
{
    return [self props]->get(option, nr);
}

- (void)setKey:(NSString *)key value:(NSString *)value
{
    [self props]->set(string([key UTF8String]), string([value UTF8String]));
}

- (void)setOpt:(Option)option value:(NSInteger)value
{
    [self props]->set(option, value);
}

- (void)setOpt:(Option)option nr:(NSInteger)nr value:(NSInteger)value
{
    [self props]->set(option, value, {nr});
}

- (void)removeAll
{
    [self props]->remove();
}

- (void)removeKey:(NSString *)key
{
    [self props]->remove(string([key UTF8String]));
}

- (void)remove:(Option)option
{
    [self props]->remove(option);
}

- (void)remove:(Option) option nr:(NSInteger)nr
{
    [self props]->remove(option, {nr});
}

@end


//
// CPU proxy
//

@implementation CPUProxy

- (CPUAPI *)cpu
{
    return (CPUAPI *)obj;
}

- (CPUInfo)info
{
    return [self cpu]->getInfo();
}

- (CPUInfo)cachedInfo
{
    return [self cpu]->getCachedInfo();
}

- (NSInteger)loggedInstructions
{
    return [self cpu]->loggedInstructions();
}

- (void)clearLog
{
    [self cpu]->clearLog();
}

- (void)setHex
{
    DasmNumberFormat instrFormat = {

        .prefix = "",
        .radix = 16,
        .upperCase = true,
        .fill = '0',
        .plainZero = false
    };

    DasmNumberFormat dataFormat = {

        .prefix = "",
        .radix = 16,
        .upperCase = true,
        .fill = '0',
        .plainZero = false
    };

    [self cpu]->setNumberFormat(instrFormat, dataFormat);
}

- (void)setDec
{
    DasmNumberFormat instrFormat = {

        .prefix = "",
        .radix = 10,
        .upperCase = true,
        .fill = '\0',
        .plainZero = false
    };

    DasmNumberFormat dataFormat = {

        .prefix = "",
        .radix = 10,
        .upperCase = true,
        .fill = '0',
        .plainZero = false
    };

    [self cpu]->setNumberFormat(instrFormat, dataFormat);
}

- (NSString *)disassemble:(NSInteger)addr format:(NSString *)fmt length:(NSInteger *)len
{
    char result[128];

    auto length = [self cpu]->disassemble(result, [fmt UTF8String], u16(addr));
    *len = (NSInteger)length;
    return @(result);
}

- (NSString *)disassembleRecorded:(NSInteger)addr format:(NSString *)fmt length:(NSInteger *)len
{
    char result[128];

    auto length = [self cpu]->disassembleRecorded(result, [fmt UTF8String], u16(addr));
    *len = (NSInteger)length;
    return @(result);
}

- (GuardInfo) guardInfo:(Guard *)guard
{
    GuardInfo result;

    result.addr = guard ? guard->addr : 0;
    result.enabled = guard ? guard->enabled : 0;
    result.hits = guard ? guard->hits : 0;
    result.ignore = guard ? guard->ignore : 0;

    return result;
}

- (BOOL) hasBreakpointWithNr:(NSInteger)nr
{
    return [self cpu]->breakpointNr(nr) != nullptr;
}

- (GuardInfo) breakpointWithNr:(NSInteger)nr
{
    return [self guardInfo:[self cpu]->breakpointNr(nr)];
}

- (BOOL) hasBreakpointAtAddr:(NSInteger)addr
{
    return [self cpu]->breakpointAt(u32(addr)) != nullptr;
}

- (GuardInfo) breakpointAtAddr:(NSInteger)addr
{
    return [self guardInfo:[self cpu]->breakpointAt(u32(addr))];
}

- (BOOL) hasWatchpointWithNr:(NSInteger)nr
{
    return [self cpu]->watchpointNr(nr) != nullptr;
}

- (GuardInfo) watchpointWithNr:(NSInteger)nr
{
    return [self guardInfo:[self cpu]->watchpointNr(nr)];
}

- (BOOL) hasWatchpointAtAddr:(NSInteger)addr
{
    return [self cpu]->watchpointAt(u32(addr)) != nullptr;
}
- (GuardInfo) watchpointAtAddr:(NSInteger)addr
{
    return [self guardInfo:[self cpu]->watchpointAt(u32(addr))];
}

@end


//
// Memory proxy
//

@implementation MemoryProxy

- (MemoryAPI *)mem
{
    return (MemoryAPI *)obj;
}

- (MemConfig)config
{
    return [self mem]->getConfig();
}

- (MemInfo)info
{
    return [self mem]->getInfo();
}

- (MemInfo)cachedInfo
{
    return [self mem]->getCachedInfo();
}

- (NSString *)memdump:(NSInteger)addr num:(NSInteger)num hex:(BOOL)hex src:(MemoryType)src
{
    return @([self mem]->memdump((u16)addr, num, hex, hex ? 2 : 1, src).c_str());
}

- (NSString *)txtdump:(NSInteger)addr num:(NSInteger)num src:(MemoryType)src
{
    return @([self mem]->txtdump((u16)addr, num, src).c_str());
}

- (void)drawHeatmap:(u32 *)buffer w:(NSInteger)w h:(NSInteger)h
{
    [self mem]->drawHeatmap(buffer, w, h);
}

@end


//
// CIA
//

@implementation CIAProxy

- (CIAAPI *)cia
{
    return (CIAAPI *)obj;
}

- (CIAConfig)config
{
    return [self cia]->getConfig();
}

- (CIAInfo)info
{
    return [self cia]->getInfo();
}

- (CIAInfo)cachedInfo
{
    return [self cia]->getCachedInfo();
}

- (CIAStats)stats
{
    return [self cia]->getStats();
}

@end


//
// VICII
//

@implementation VICIIProxy

- (VICIIAPI *)vicii
{
    return (VICIIAPI *)obj;
}

- (VICIITraits)traits
{
    return [self vicii]->getTraits();
}

- (VICIIConfig)config
{
    return [self vicii]->getConfig();
}

- (VICIIInfo)info 
{
    return [self vicii]->getInfo();
}

- (VICIIInfo)cachedInfo
{
    return [self vicii]->getCachedInfo();
}

- (SpriteInfo)getSpriteInfo:(NSInteger)sprite
{
    return [self vicii]->getSpriteInfo((unsigned)sprite);
}

- (NSColor *)color:(NSInteger)nr
{
    assert (0 <= nr && nr < 16);
    
    u32 color = [self vicii]->getColor((unsigned)nr);
    u8 r = color & 0xFF;
    u8 g = (color >> 8) & 0xFF;
    u8 b = (color >> 16) & 0xFF;
    
	return [NSColor colorWithCalibratedRed:(float)r/255.0
                                     green:(float)g/255.0
                                      blue:(float)b/255.0
                                     alpha:1.0];
}

- (UInt32)rgbaColor:(NSInteger)nr palette:(Palette)palette
{
    assert (0 <= nr && nr < 16);
    return [self vicii]->getColor((unsigned)nr, palette);
}

@end


//
// Dma Debugger
//

@implementation DmaDebuggerProxy

- (DmaDebuggerAPI *)debugger
{
    return (DmaDebuggerAPI *)obj;
}

- (DmaDebuggerConfig)getConfig
{
    return [self debugger]->getConfig();
}

@end


//
// SID
//

@implementation SIDProxy

- (SIDAPI *)bridge
{
    return (SIDAPI *)obj;
}

- (SIDInfo)getInfo:(NSInteger)nr
{
    return [self bridge]->getInfo((unsigned)nr);
}

- (float)drawWaveform:(u32 *)buffer
                    w:(NSInteger)w
                    h:(NSInteger)h
                scale:(float)s
                color:(u32)c
               source:(NSInteger)source
{
    return [self bridge]->draw(buffer, w, h, s, c, source);
}

- (float)drawWaveform:(u32 *)buffer
                 size:(NSSize)size
                scale:(float)s
                color:(u32)c
               source:(NSInteger)source
{
    return [self drawWaveform:buffer
                            w:(NSInteger)size.width
                            h:(NSInteger)size.height
                        scale:s
                        color:c
                       source:source];
}

@end


//
// Audio port
//

@implementation AudioPortProxy

- (AudioPortAPI *)port
{
    return (AudioPortAPI *)obj;
}

- (AudioPortStats)stats
{
    return [self port]->getStats();
}

- (NSInteger)copyMono:(float *)target size:(NSInteger)n
{
    return [self port]->copyMono(target, n);
}

- (NSInteger)copyStereo:(float *)target1 buffer2:(float *)target2 size:(NSInteger)n
{
    return [self port]->copyStereo(target1, target2, n);
}

- (NSInteger)copyInterleaved:(float *)target size:(NSInteger)n
{
    return [self port]->copyInterleaved(target, n);
}

@end


//
// Video port
//

@implementation VideoPortProxy

- (VideoPortAPI *)port
{
    return (VideoPortAPI *)obj;
}

- (u32 *)texture
{
    return [self port]->getTexture();
}

@end


//
// Serial port
//

@implementation SerialPortProxy

- (SerialPortAPI *)iec
{
    return (SerialPortAPI *)obj;
}

@end


//
// Keyboard
//

@implementation KeyboardProxy

- (KeyboardAPI *)kb
{
    return (KeyboardAPI *)obj;
}

- (VirtualC64 *)emu
{
    return (VirtualC64 *)emu;
}

- (BOOL)isPressed:(NSInteger)nr
{
    return [self kb]->isPressed(nr);
}

- (void)pressKey:(NSInteger)nr
{
    [self kb]->press(C64Key(nr));
}

- (void)releaseKey:(NSInteger)nr
{
    [self kb]->release(C64Key(nr));
}

- (void)releaseKey:(NSInteger)nr delay:(double)delay
{
    [self kb]->release(C64Key(nr), delay);
}

- (void)releaseAll
{
    [self kb]->releaseAll();
}

- (void)releaseAllWithDelay:(double)delay
{
    [self kb]->releaseAll(delay);
}

- (void)toggleKey:(NSInteger)nr
{
    [self kb]->toggle(C64Key(nr));
}

- (void)toggleKeyAtRow:(NSInteger)row col:(NSInteger)col
{
    [self kb]->toggle(C64Key(row, col).nr);
}

- (void)autoType:(NSString *)text
{
    [self kb]->autoType(string([text UTF8String]));
}

- (void)abortAutoTyping
{
    [self kb]->abortAutoTyping();
}

@end


//
// Control port
//

@implementation ControlPortProxy

@synthesize mouse;
@synthesize joystick;

- (instancetype) initWith:(void *)ref emu:(VirtualC64 *)emuref
{
    if (self = [super initWith:ref emu:emuref]) {

        ControlPortAPI *port = (ControlPortAPI *)obj;
        joystick = [[JoystickProxy alloc] initWith:&port->joystick];
        mouse = [[MouseProxy alloc] initWith:&port->mouse];
    }
    return self;
}

- (ControlPortAPI *)port
{
    return (ControlPortAPI *)obj;
}

@end


//
// Expansion port
//

@implementation ExpansionPortProxy

- (ExpansionPortAPI *)eport
{
    return (ExpansionPortAPI *)obj;
}

- (VirtualC64 *)emu
{
    return (VirtualC64 *)emu;
}

- (CartridgeTraits)traits
{
    return [self eport]->getCartridgeTraits();
}

- (CartridgeInfo)info
{
    return [self eport]->getInfo();
}

- (CartridgeInfo)cachedInfo
{
    return [self eport]->getCachedInfo();
}

- (CartridgeRomInfo)getRomInfo:(NSInteger)nr
{
    return [self eport]->getRomInfo(nr);
}

- (BOOL)cartridgeAttached
{
    return [self eport]->getCartridgeTraits().type != CRT_NONE;
}
 
- (void)attachCartridge:(MediaFileProxy *)c reset:(BOOL)reset exception:(ExceptionWrapper *)ex
{
    try { [self eport]->attachCartridge(*(MediaFile *)c->obj, reset); }
    catch (Error &err) { [ex save:err]; }
}

- (void)attachReuCartridge:(NSInteger)capacity
{
    [self eport]->attachReu(capacity);
}

- (void)attachGeoRamCartridge:(NSInteger)capacity
{
    [self eport]->attachGeoRam(capacity);
}

- (void)attachIsepicCartridge
{
    [self eport]->attachIsepicCartridge();
}

- (void)detachCartridge
{
    [self eport]->detachCartridge();
}

@end


//
// User port
//

@implementation UserPortProxy

@synthesize rs232;

- (instancetype) initWith:(void *)ref emu:(VirtualC64 *)emuref
{
    if (self = [super initWith:ref emu:emuref]) {

        UserPortAPI *port = (UserPortAPI *)obj;
        rs232 = [[RS232Proxy alloc] initWith:&port->rs232];
    }
    return self;
}

- (RS232API *)port
{
    return (RS232API *)obj;
}

@end


//
// DiskAnalyzer
//

@implementation DiskAnalyzerProxy

- (DiskAnalyzer *)analyzer
{
    return (DiskAnalyzer *)obj;
}

- (instancetype) initWithDrive:(DriveProxy *)drive
{
    NSLog(@"DiskAnalyzerProxy::initWithDrive");

    if (!(self = [super init])) return self;
    auto drv = (DriveAPI *)drive->obj;
    obj = new DiskAnalyzer(*drv->drive);

    return self;
}

- (void)dealloc
{
    NSLog(@"DiskAnalyzerProxy::dealloc");
    
    if (obj) delete (DiskAnalyzer *)obj;
}

- (NSInteger)lengthOfTrack:(Track)t
{
    return [self analyzer]->lengthOfTrack(t);
}

- (NSInteger)lengthOfHalftrack:(Halftrack)ht
{
    return [self analyzer]->lengthOfHalftrack(ht);
}

- (NSString *)getLogbook:(Halftrack)ht
{
    string s = [self analyzer]->getLogbook(ht);
    return [NSString stringWithUTF8String:s.c_str()];
}

- (SectorInfo)sectorInfo:(Halftrack)ht sector:(Sector)s
{
    return [self analyzer]->sectorLayout(ht, s);
}

- (const char *)trackBitsAsString:(Halftrack)ht
{
    return [self analyzer]->trackBitsAsString(ht);
}

- (const char *)diskNameAsString
{
    return [self analyzer]->diskNameAsString();
}

- (const char *)sectorHeaderBytesAsString:(Halftrack)ht sector:(Sector)nr hex:(BOOL)hex
{
    return [self analyzer]->sectorHeaderBytesAsString(ht, nr, hex);
}

- (const char *)sectorDataBytesAsString:(Halftrack)ht sector:(Sector)nr hex:(BOOL)hex
{
    return [self analyzer]->sectorDataBytesAsString(ht, nr, hex);
}

@end


//
// Drive
//

@implementation DriveProxy

- (DriveAPI *)drive
{
    return (DriveAPI *)obj;
}

- (VirtualC64 *)emu
{
    return (VirtualC64 *)emu;
}

- (DriveConfig)config
{
    return [self drive]->getConfig();
}

- (DriveInfo)info
{
    return [self drive]->getInfo();
}

- (DriveInfo)cachedInfo
{
    return [self drive]->getCachedInfo();
}

- (void)insertMedia:(MediaFileProxy *)proxy protected:(BOOL)wp
{
    [self drive]->insertMedia(*(MediaFile *)proxy->obj, wp);
}

- (void)insertFileSystem:(FileSystemProxy *)proxy protected:(BOOL)wp
{
    [self drive]->insertFileSystem(*(FileSystem *)proxy->obj, wp);
}

- (void)insertBlankDisk:(DOSType)fsType name:(NSString *)name
{
    [self drive]->insertBlankDisk(fsType, [name UTF8String]);
}

- (void)ejectDisk
{
    [self drive]->ejectDisk();
}

@end


//
// Datasette
//

@implementation DatasetteProxy

- (DatasetteAPI *)datasette
{
    return (DatasetteAPI *)obj;
}

- (VirtualC64 *)emu
{
    return (VirtualC64 *)emu;
}

- (DatasetteInfo)info
{
    return [self datasette]->getInfo();
}

- (DatasetteInfo)cachedInfo
{
    return [self datasette]->getCachedInfo();
}

- (void)pressPlay
{
    [self emu]->put(CMD_DATASETTE_PLAY);
}

- (void)pressStop
{
    [self emu]->put(CMD_DATASETTE_STOP);
}

- (void)rewind
{
    [self emu]->put(CMD_DATASETTE_REWIND);
}

- (void)insertTape:(MediaFileProxy *)proxy
{
    [self emu]->datasette.insertTape(*(MediaFile *)proxy->obj);
}

- (void)ejectTape
{
    [self emu]->datasette.ejectTape();
}

@end


//
// RS232 proxy
//

@implementation RS232Proxy

- (RS232API *)rs232
{
    return (RS232API *)obj;
}

- (NSInteger)readIncomingPrintableByte
{
    return [self rs232]->readIncomingPrintableByte();
}

- (NSInteger)readOutgoingPrintableByte
{
    return [self rs232]->readOutgoingPrintableByte();
}

@end



//
// Mouse proxy
//

@implementation MouseProxy

- (MouseAPI *)mouse
{
    return (MouseAPI *)obj;
}

- (BOOL)detectShakeAbs:(NSPoint)pos
{
    return [self mouse]->detectShakeXY(pos.x, pos.y);
}

- (BOOL)detectShakeRel:(NSPoint)pos
{
    return [self mouse]->detectShakeDxDy(pos.x, pos.y);
}

@end


//
// Joystick proxy
//

@implementation JoystickProxy

- (JoystickAPI *)joystick
{
    return (JoystickAPI *)obj;
}

@end


//
// Recorder
//

@implementation RecorderProxy

- (RecorderAPI *)recorder
{
    return (RecorderAPI *)obj;
}

- (RecorderConfig)config
{
    return [self recorder]->getConfig();
}

- (RecorderInfo)info
{
    return [self recorder]->getInfo();
}

- (RecorderInfo)cachedInfo
{
    return [self recorder]->getCachedInfo();
}

- (NSString *)path
{
    auto path = FFmpeg::getExecPath();
    return @(path.c_str());
}

- (void)setPath:(NSString *)path
{
    if ([path length] == 0) {
        FFmpeg::setExecPath("");
    } else {
        FFmpeg::setExecPath(string([path fileSystemRepresentation]));
    }
}

- (NSString *)findFFmpeg:(NSInteger)nr
{
    if (nr < (NSInteger)FFmpeg::paths.size()) {
        return @(FFmpeg::paths[nr].c_str());
    } else {
        return nil;
    }
}

- (BOOL)hasFFmpeg
{
    return FFmpeg::available();
}

- (BOOL)recording
{
    return [self recorder]->getInfo().state != REC_STATE_WAIT;
}

- (void)startRecording:(NSRect)rect exception:(ExceptionWrapper *)ex
{
    auto x1 = isize(rect.origin.x);
    auto y1 = isize(rect.origin.y);
    auto x2 = isize(x1 + (int)rect.size.width);
    auto y2 = isize(y1 + (int)rect.size.height);

    try { return [self recorder]->startRecording(x1, y1, x2, y2); }
    catch (Error &error) { [ex save:error]; }
}

- (void)stopRecording
{
    [self recorder]->stopRecording();
}

- (BOOL)exportAs:(NSString *)path
{
    return [self recorder]->exportAs(string([path fileSystemRepresentation]));
}

@end


//
// RemoteManager proxy
//

@implementation RemoteManagerProxy

- (RemoteManagerAPI *)manager
{
    return (RemoteManagerAPI *)obj;
}

+ (instancetype)make:(RemoteManagerAPI *)manager
{
    if (manager == nullptr) { return nil; }

    RemoteManagerProxy *proxy = [[self alloc] initWith: manager];
    return proxy;
}

- (RemoteManagerInfo)info
{
    return [self manager]->getInfo();
}

@end


//
// RetroShell proxy
//

@implementation RetroShellProxy

- (RetroShellAPI *)shell
{
    return (RetroShellAPI *)obj;
}

+ (instancetype)make:(RetroShellAPI *)shell
{
    if (shell == nullptr) { return nil; }
    
    RetroShellProxy *proxy = [[self alloc] initWith: shell];
    return proxy;
}

-(NSInteger)cursorRel
{
	return [self shell]->cursorRel();
}

-(NSString *)getText
{
	const char *str = [self shell]->text();
	return str ? @(str) : nullptr;
}

- (void)pressKey:(char)c
{
	[self shell]->press(c);
}

- (void)pressSpecialKey:(RetroShellKey)key
{
    [self shell]->press(key);
}

- (void)pressSpecialKey:(RetroShellKey)key shift:(BOOL)shift
{
    [self shell]->press(key, shift);
}

- (void)executeScript:(MediaFileProxy *)file
{
    [self shell]->execScript(*(MediaFile *)file->obj);
}

- (void)executeString:(NSString *)str
{
    [self shell]->execScript(std::string([str UTF8String]));
}

@end


//
// MediaFile
//

@implementation MediaFileProxy

- (MediaFile *)file
{
    return (MediaFile *)obj;
}

+ (FileType)typeOfUrl:(NSURL *)url
{
    return MediaFile::type([url fileSystemRepresentation]);
}

+ (instancetype)make:(void *)file
{
    return file ? [[self alloc] initWith:file] : nil;
}

+ (instancetype)makeWithFile:(NSString *)path
                   exception:(ExceptionWrapper *)ex
{
    try { return [self make: MediaFile::make([path fileSystemRepresentation])]; }
    catch (Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithFile:(NSString *)path
                        type:(FileType)type
                   exception:(ExceptionWrapper *)ex
{
    try { return [self make: MediaFile::make([path fileSystemRepresentation], type)]; }
    catch (Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len
                          type:(FileType)type
                     exception:(ExceptionWrapper *)ex
{
    try { return [self make: MediaFile::make((u8 *)buf, len, type)]; }
    catch (Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithC64:(EmulatorProxy *)proxy
{
    auto vc64 = (VirtualC64 *)proxy->obj;
    return [self make:vc64->c64.takeSnapshot()];
}

+ (instancetype)makeWithDrive:(DriveProxy *)proxy
                        type:(FileType)type
                   exception:(ExceptionWrapper *)ex
{
    auto drive = (DriveAPI *)proxy->obj;
    try { return [self make: MediaFile::make(*drive, type)]; }
    catch (Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithFileSystem:(FileSystemProxy *)proxy
                              type:(FileType)type
                         exception:(ExceptionWrapper *)ex
{
    auto fs = (FileSystem *)proxy->obj;
    try { return [self make: MediaFile::make(*fs, type)]; }
    catch (Error &error) { [ex save:error]; return nil; }
}

- (FileType)type
{
    return [self file]->type();
}

- (u64)fnv
{
    return [self file]->fnv64();
}

- (NSInteger)size
{
    return [self file]->getSize();
}

- (BOOL)compressed
{
    return [self file]->isCompressed();
}

- (void)writeToFile:(NSString *)path exception:(ExceptionWrapper *)ex
{
    try { [self file]->writeToFile(string([path fileSystemRepresentation])); }
    catch (Error &err) { [ex save:err]; }
}

- (NSImage *)previewImage
{
    // Return cached image (if any)
    if (preview) { return preview; }

    // Get dimensions and data
    auto size = [self file]->previewImageSize();
    auto data = (unsigned char *)[self file]->previewImageData();

    // Create preview image
    if (data) {

        NSBitmapImageRep *rep = [[NSBitmapImageRep alloc]
                                 initWithBitmapDataPlanes: &data
                                 pixelsWide:size.first
                                 pixelsHigh:size.second
                                 bitsPerSample:8
                                 samplesPerPixel:4
                                 hasAlpha:true
                                 isPlanar:false
                                 colorSpaceName:NSCalibratedRGBColorSpace
                                 bytesPerRow:4*size.first
                                 bitsPerPixel:32];

        preview = [[NSImage alloc] initWithSize:[rep size]];
        [preview addRepresentation:rep];

        // image.makeGlossy()
    }
    return preview;
}

- (time_t)timeStamp
{
    return [self file]->timestamp();
}

@end


//
// FileSystem
//

@implementation FileSystemProxy

- (FileSystem *)fs
{
    return (FileSystem *)obj;
}

+ (instancetype)make:(FileSystem *)fs
{
    return fs ? [[self alloc] initWith: fs] : nil;
}

+ (instancetype)makeWithDrive:(DriveProxy *)proxy exception:(ExceptionWrapper *)ex
{
    auto *disk = [proxy drive]->disk.get();
    try { return [self make: new FileSystem(*disk)]; }
    catch (Error &err) { [ex save:err]; return nil; }
}

+ (instancetype)makeWithDiskType:(DiskType)diskType dosType:(DOSType)dosType
{
    return [self make: new FileSystem(diskType, dosType)];
}

+ (instancetype)makeWithMediaFile:(MediaFileProxy *)proxy exception:(ExceptionWrapper *)ex
{
    try { return [self make: new FileSystem(*(MediaFile *)proxy->obj)]; }
    catch (Error &err) { [ex save:err]; return nil; }
}

- (NSString *)name
{
    auto str = [self fs]->getName();
    return @(str.c_str());
}

- (void)setName:(NSString *)name
{
    auto str = string([name UTF8String]);
    [self fs]->setName(PETName<16>(str));
}

- (NSString *)idString
{
    auto str = [self fs]->getID();
    return @(str.c_str());
}

- (NSString *)capacityString
{
    auto str = util::byteCountAsString([self fs]->getNumBytes());
    return @(str.c_str());
}

- (NSString *)fillLevelString
{
    auto str = util::fillLevelAsString([self fs]->fillLevel());
    return @(str.c_str());
}

- (DOSType)dos
{
    return [self fs]->dos();
}

- (NSInteger)numCyls
{
    return [self fs]->getNumCyls();
}

- (NSInteger)numHeads
{
    return [self fs]->getNumHeads();
}

- (NSInteger)numTracks
{
    return [self fs]->getNumTracks();
}

- (NSInteger)numSectors:(NSInteger)track
{
    return [self fs]->getNumSectors((Track)track);
}

- (NSInteger)numBlocks
{
    return [self fs]->getNumBlocks();
}

- (NSInteger)freeBlocks
{
    return [self fs]->freeBlocks();
}

- (NSInteger)usedBlocks
{
    return [self fs]->usedBlocks();
}

- (NSInteger)numFiles
{
    return [self fs]->numFiles();
}

- (NSInteger)cylNr:(NSInteger)t
{
    return [self fs]->layout.cylNr((Track)t);
}

- (NSInteger)headNr:(NSInteger)t
{
    return [self fs]->layout.headNr((Track)t);
}

- (NSInteger)trackNr:(NSInteger)c head:(NSInteger)h
{
    return [self fs]->layout.trackNr((Cylinder)c, (Head)h);
}

- (TSLink)tsLink:(NSInteger)b
{
    return [self fs]->layout.tsLink((Block)b);
}

- (NSInteger)trackNr:(NSInteger)b
{
    return (NSInteger)[self tsLink:b].t;
}

- (NSInteger)sectorNr:(NSInteger)b
{
    return (NSInteger)[self tsLink:b].s;
}

- (NSInteger)blockNr:(TSLink)ts
{
    return [self fs]->layout.blockNr(ts);
}

- (NSInteger)blockNr:(NSInteger)t sector:(NSInteger)s
{
    return [self fs]->layout.blockNr((Track)t, (Sector)s);
}

- (NSInteger)blockNr:(NSInteger)c head:(NSInteger)h sector:(NSInteger)s
{
    return [self fs]->layout.blockNr((Cylinder)c, (Head)h, (Sector)s);
}

- (FSBlockType)blockType:(NSInteger)blockNr
{
    return [self fs]->blockType((u32)blockNr);
}

- (FSUsage)itemType:(NSInteger)blockNr pos:(NSInteger)pos
{
    return [self fs]->usage((u32)blockNr, (u32)pos);
}

- (FSErrorReport)check:(BOOL)strict
{
    return [self fs]->check(strict);
}

- (ErrorCode)check:(NSInteger)blockNr
               pos:(NSInteger)pos
          expected:(unsigned char *)exp
            strict:(BOOL)strict
{
    return [self fs]->check((u32)blockNr, (u32)pos, exp, strict);
}

- (BOOL)isCorrupted:(NSInteger)blockNr
{
    return [self fs]->isCorrupted((u32)blockNr);
}

- (NSInteger)getCorrupted:(NSInteger)blockNr
{
    return [self fs]->getCorrupted((u32)blockNr);
}

- (NSInteger)nextCorrupted:(NSInteger)blockNr
{
    return [self fs]->nextCorrupted((u32)blockNr);
}

- (NSInteger)prevCorrupted:(NSInteger)blockNr
{
    return [self fs]->prevCorrupted((u32)blockNr);
}

- (void)printDirectory
{
    return [self fs]->printDirectory();
}

- (NSInteger)readByte:(NSInteger)block offset:(NSInteger)offset
{
    return [self fs]->readByte((u32)block, offset);
}

- (NSString *)ascii:(NSInteger)block offset:(NSInteger)offset length:(NSInteger)len
{
    return @([self fs]->ascii(Block(block), offset, len).c_str());
}

- (void)export:(NSString *)path exception:(ExceptionWrapper *)e
{
    try { [self fs]->exportDirectory([path fileSystemRepresentation]); }
    catch (Error &error) { [e save:error]; }
}

- (void)info
{
    [self fs]->info();
}

- (BOOL)isFree:(NSInteger)blockNr
{
    return [self fs]->isFree(blockNr);
}

- (NSString *)fileName:(NSInteger)nr
{
    return @([self fs]->fileName((unsigned)nr).c_str());
}

- (FSFileType)fileType:(NSInteger)nr
{
    return [self fs]->fileType((unsigned)nr);
}

- (NSInteger)fileSize:(NSInteger)nr
{
    return [self fs]->fileSize((unsigned)nr);
}

- (NSInteger)fileBlocks:(NSInteger)nr
{
    return [self fs]->fileBlocks((unsigned)nr);
}

- (FSBlockType)getDisplayType:(NSInteger)column
{
    return [self fs]->getDisplayType(column);
}

- (NSInteger)diagnoseImageSlice:(NSInteger)column
{
    return [self fs]->diagnoseImageSlice(column);
}

- (NSInteger)nextBlockOfType:(FSBlockType)type after:(NSInteger)after
{
    return [self fs]->nextBlockOfType(type, after);
}

- (NSInteger)nextCorruptedBlock:(NSInteger)after
{
    return [self fs]->nextCorruptedBlock(after);
}

@end


//
// Constants
//

@implementation Constants

+ (NSInteger)texWidth { return Texture::width; }
+ (NSInteger)texHeight { return Texture::height; }

@end


//
// C64 proxy
//

@implementation C64Proxy

- (C64API *)c64
{
    return (C64API *)obj;
}

- (C64Info)info
{
    return [self c64]->getInfo();
}

- (C64Info)cachedInfo
{
    return [self c64]->getCachedInfo();
}

- (NSInteger)autoInspectionMask
{
    return (NSInteger)[self c64]->getAutoInspectionMask();
}

- (void)setAutoInspectionMask:(NSInteger)mask
{
    return [self c64]->setAutoInspectionMask(u64(mask));
}

- (EventSlotInfo)cachedSlotInfo:(NSInteger)slot
{
    // return [self c64]->getSlotInfo(slot);
    return [self c64]->getCachedInfo().slotInfo[slot];
}

- (RomTraits)getRomTraits:(RomType)type
{
    return [self c64]->getRomTraits(type);
}

- (RomTraits)basicRom
{
    return [self getRomTraits:ROM_TYPE_BASIC];
}

- (RomTraits)charRom
{
    return [self getRomTraits:ROM_TYPE_CHAR];
}

- (RomTraits)kernalRom
{
    return [self getRomTraits:ROM_TYPE_KERNAL];
}

- (RomTraits)vc1541Rom
{
    return [self getRomTraits:ROM_TYPE_VC1541];
}

- (void)hardReset
{
    [self c64]->hardReset();
}

- (void)softReset
{
    [self c64]->softReset();
}

- (MediaFileProxy *)takeSnapshot
{
    MediaFile *snapshot = [self c64]->takeSnapshot();
    return [MediaFileProxy make:snapshot];
}

@end

//
// Emulator
//

@implementation EmulatorProxy

@synthesize audioPort;
@synthesize c64;
@synthesize cia1;
@synthesize cia2;
@synthesize cpu;
@synthesize datasette;
@synthesize dmaDebugger;
@synthesize drive8;
@synthesize drive9;
@synthesize expansionport;
@synthesize userPort;
@synthesize iec;
@synthesize keyboard;
@synthesize mem;
@synthesize port1;
@synthesize port2;
@synthesize recorder;
@synthesize remoteManager;
@synthesize retroShell;
@synthesize sid;
@synthesize vic;
@synthesize videoPort;

- (instancetype) init
{
    if (!(self = [super init])) return self;
    
    // Create the emulator instance
    VirtualC64 *emu = new VirtualC64();
    obj = emu;

    // Create sub proxys
    audioPort = [[AudioPortProxy alloc] initWith:&emu->audioPort emu:emu];
    c64 = [[C64Proxy alloc] initWith:&emu->c64 emu:emu];
    cia1 = [[CIAProxy alloc] initWith:&emu->cia1 emu:emu];
    cia2 = [[CIAProxy alloc] initWith:&emu->cia2 emu:emu];
    cpu = [[CPUProxy alloc] initWith:&emu->cpu emu:emu];
    datasette = [[DatasetteProxy alloc] initWith:&emu->datasette emu:emu];
    dmaDebugger = [[DmaDebuggerProxy alloc] initWith:&emu->dmaDebugger];
    drive8 = [[DriveProxy alloc] initWith:&emu->drive8 emu:emu];
    drive9 = [[DriveProxy alloc] initWith:&emu->drive9 emu:emu];
    expansionport = [[ExpansionPortProxy alloc] initWith:&emu->expansionPort emu:emu];
    userPort = [[UserPortProxy alloc] initWith:&emu->userPort emu:emu];
    iec = [[SerialPortProxy alloc] initWith:&emu->serialPort emu:emu];
    keyboard = [[KeyboardProxy alloc] initWith:&emu->keyboard emu:emu];
    mem = [[MemoryProxy alloc] initWith:&emu->mem emu:emu];
    port1 = [[ControlPortProxy alloc] initWith:&emu->controlPort1 emu:emu];
    port2 = [[ControlPortProxy alloc] initWith:&emu->controlPort2 emu:emu];
    recorder = [[RecorderProxy alloc] initWith:&emu->recorder emu:emu];
    remoteManager = [[RemoteManagerProxy alloc] initWith:&emu->remoteManager emu:emu];
    retroShell = [[RetroShellProxy alloc] initWith:&emu->retroShell emu:emu];
    sid = [[SIDProxy alloc] initWith:&emu->sid emu:emu];
    vic = [[VICIIProxy alloc] initWith:&emu->vicii emu:emu];
    videoPort = [[VideoPortProxy alloc] initWith:&emu->videoPort emu:emu];

    return self;
}

- (VirtualC64 *)emu
{
    return (VirtualC64 *)obj;
}

+ (DefaultsProxy *) defaults
{
    return [[DefaultsProxy alloc] initWith:(void *)&VirtualC64::defaults];
}

- (void)dealloc
{

}

- (void)kill
{
    assert([self emu] != NULL);
    delete [self emu];
    obj = NULL;
}

+ (NSString *)build
{
    return @(VirtualC64::build().c_str());
}

+ (NSString *)version
{
    return @(VirtualC64::version().c_str());
}

- (BOOL)releaseBuild
{
    return releaseBuild;
}

- (BOOL)trackMode
{
    return [self emu]->isTracking();
}

- (void)launch:(const void *)listener function:(Callback *)func
{
    [self emu]->launch(listener, func);
}

- (void)isReady:(ExceptionWrapper *)ex
{
    try { [self emu]->isReady(); }
    catch (Error &error) { [ex save:error]; }
}

- (void)powerOn:(ExceptionWrapper *)ex
{
    try { [self emu]->powerOn(); }
    catch (Error &error) { [ex save:error]; }
}

- (void)powerOff
{
    [self emu]->powerOff();
}

- (EmulatorInfo)info
{
    return [self emu]->getInfo();
}

- (EmulatorInfo)cachedInfo
{
    return [self emu]->getCachedInfo();
}

- (EmulatorStats)stats
{
    return [self emu]->getStats();
}

- (BOOL)poweredOn
{
    return [self emu]->isPoweredOn();
}

- (BOOL)poweredOff
{
    return [self emu]->isPoweredOff();
}

- (BOOL)paused
{
    return [self emu]->isPaused();
}

- (BOOL)running
{
    return [self emu]->isRunning();
}

- (BOOL)suspended
{
    return [self emu]->isSuspended();
}

- (BOOL)halted
{
    return [self emu]->isHalted();
}

- (BOOL)warping
{
    return [self emu]->isWarping();
}

- (BOOL)tracking
{
    return [self emu]->isTracking();
}

- (void)run:(ExceptionWrapper *)e
{
    [self emu]->run();
}

- (void)pause
{
    [self emu]->pause();
}

- (void)halt
{
    [self emu]->halt();
}

- (void)suspend
{
    [self emu]->suspend();
}

- (void)resume
{
    [self emu]->resume();
}

- (void)warpOn
{
    [self emu]->warpOn();
}

- (void)warpOn:(NSInteger)source
{
    [self emu]->warpOn(source);
}

- (void)warpOff
{
    [self emu]->warpOff();
}

- (void)warpOff:(NSInteger)source
{
    [self emu]->warpOff(source);
}

- (void)trackOn
{
    [self emu]->trackOn();
}

- (void)trackOn:(NSInteger)source
{
    [self emu]->trackOn(source);
}

- (void)trackOff
{
    [self emu]->trackOff();
}

- (void)trackOff:(NSInteger)source
{
    [self emu]->trackOff(source);
}

- (NSInteger)get:(Option)opt
{
    return [self emu]->get(opt);
}

- (NSInteger)get:(Option)opt id:(NSInteger)id
{
    return [self emu]->get(opt, id);
}

- (NSInteger)get:(Option)opt drive:(NSInteger)id
{
    return [self emu]->get(opt, (long)id);
}

- (BOOL)set:(Option)opt value:(NSInteger)val
{
    try {
        [self emu]->set(opt, val);
        return true;
    } catch (Error &exception) {
        return false;
    }
}

- (BOOL)set:(Option)opt enable:(BOOL)val
{
    try {
        [self emu]->set(opt, val ? 1 : 0);
        return true;
    } catch (Error &exception) {
        return false;
    }
}

- (BOOL)set:(Option)opt id:(NSInteger)id value:(NSInteger)val
{
    try {
        [self emu]->set(opt, val, id);
        return true;
    } catch (Error &exception) {
        return false;
    }
}

- (BOOL)set:(Option)opt id:(NSInteger)id enable:(BOOL)val
{
    try {
        [self emu]->set(opt, val ? 1 : 0, id);
        return true;
    } catch (Error &exception) {
        return false;
    }
}

- (BOOL)set:(Option)opt drive:(NSInteger)id value:(NSInteger)val
{
    try {
        [self emu]->set(opt, val, (long)id);
        return true;
    } catch (Error &exception) {
        return false;
    }
}

- (BOOL)set:(Option)opt drive:(NSInteger)id enable:(BOOL)val
{
    try {
        [self emu]->set(opt, val ? 1 : 0, (long)id);
        return true;
    } catch (Error &exception) {
        return false;
    }
}

- (void)set:(C64Model)model
{
    [self emu]->set(model);
}

- (void)exportConfig:(NSURL *)url exception:(ExceptionWrapper *)ex
{
    try { [self emu]->exportConfig([url fileSystemRepresentation]); }
    catch (Error &error) { [ex save:error]; }
}

- (void)wakeUp
{
    [self emu]->wakeUp();
}

- (void)stepInto
{
    [self emu]->stepInto();
}

- (void)stepOver
{
    [self emu]->stepOver();
}

- (BOOL) isRom:(RomType)type url:(NSURL *)url
{
    auto fileType = MediaFile::type([url fileSystemRepresentation]);

    return 
    (fileType == FILETYPE_BASIC_ROM && type == ROM_TYPE_BASIC) ||
    (fileType == FILETYPE_CHAR_ROM && type == ROM_TYPE_CHAR) ||
    (fileType == FILETYPE_KERNAL_ROM && type == ROM_TYPE_KERNAL) ||
    (fileType == FILETYPE_VC1541_ROM && type == ROM_TYPE_VC1541);
}

- (void)installOpenRoms
{
    [self emu]->c64.installOpenRoms();
}

- (void) loadRom:(NSURL *)url exception:(ExceptionWrapper *)e
{
    try { [self emu]->c64.loadRom(string([url fileSystemRepresentation])); }
    catch (Error &error) { [e save:error]; }
}

- (void) loadRom:(MediaFileProxy *)proxy
{
    [self emu]->c64.loadRom(*(MediaFile *)proxy->obj);
}

- (void) saveRom:(RomType)type url:(NSURL *)url exception:(ExceptionWrapper *)e
{
    try { [self emu]->c64.saveRom(type, string([url fileSystemRepresentation])); }
    catch (Error &error) { [e save:error]; }
}

- (void) deleteRom:(RomType)type
{
    [self emu]->c64.deleteRom(type);
}

- (void)flash:(MediaFileProxy *)proxy exception:(ExceptionWrapper *)ex
{
    try { [self emu]->c64.flash(*(MediaFile *)proxy->obj); }
    catch (Error &error) { [ex save:error]; }
}

- (void)flash:(FileSystemProxy *)proxy item:(NSInteger)nr exception:(ExceptionWrapper *)ex
{
    try { [self emu]->c64.flash(*(FileSystem *)proxy->obj, (unsigned)nr); }
    catch (Error &error) { [ex save:error]; }
}

- (void)put:(CmdType)type
{
    [self emu]->put(type, 0);
}

- (void)put:(CmdType)type value:(NSInteger)value
{
    [self emu]->put(type, value);
}

- (void)put:(CmdType)type value:(NSInteger)value value2:(NSInteger)value2
{
    [self emu]->put(type, value, value2);
}

- (void)put:(CmdType)type key:(KeyCmd)cmd
{
    [self emu]->put(type, cmd);
}

- (void)put:(CmdType)type coord:(CoordCmd)cmd
{
    [self emu]->put(type, cmd);
}

- (void)put:(CmdType)type action:(GamePadCmd)cmd
{
    [self emu]->put(type, cmd);
}

@end
