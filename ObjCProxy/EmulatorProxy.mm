// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#import "EmulatorProxy.h"
#import "VirtualC64.h"
#import "DiskAnalyzer.h"
#import "Drive.h"
// #import "Images/DiskImage.h"
#import "Media/RomFile.h"
#import "Images/FloppyDiskImage.h"
#import "FileSystems/CBM/FileSystem.h"
#import "Texture.h"
#import "MediaError.h"

using namespace vc64;

using retro::vault::DiskImage;
using retro::vault::FloppyDiskImage;
using retro::vault::cbm::FileSystem;

NSString *EventSlotName(EventSlot slot)
{
    return @(EventSlotEnum::help(slot));
}

//
// Exception wrapper
//

@implementation ExceptionWrapper

@synthesize fault;
@synthesize key;
@synthesize what;

- (instancetype)init {

    if (self = [super init]) {
        
        self.fault = NULL;
        self.key   = @"";
        self.what  = @"";
    }
    return self;
}

- (void)save:(const std::exception &)exception
{
    if (const auto *error = dynamic_cast<const Error *>(&exception)) {

        self.fault = @(error->payload);
        self.key   = @(error->errstr());
        self.what  = @(error->what());
        
    } else if (const auto *error = dynamic_cast<const AppError *>(&exception)) {
        
        self.fault = @(error->data);
        self.key   = @("");
        self.what  = @(error->what());
        
    } else {

        self.fault = @(0);
        self.key   = @("");
        self.what  = @(error->what());
    }
}

/*
- (void)save:(const std::exception &)exception
{
    if (const auto *error = dynamic_cast<const Error *>(&exception)) {
        
        fault = Fault(error->payload);
        key = @(error->errstr());
        what = @(error->what());
        
    } else if (const auto *error = dynamic_cast<const AppError *>(&exception)) {
        
        // DEPRECATED
        fault = Fault(error->data);
        key = @("");
        what = @(error->what());
        
    } else {

        fault = Fault(0);
        key = @("");
        what = @(exception.what());
    }
}
*/

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
    catch (std::exception &stdex) { [ex save:stdex]; }
}

- (void)save:(NSURL *)url exception:(ExceptionWrapper *)ex
{
    try { return [self props]->save([url fileSystemRepresentation]); }
    catch (std::exception &stdex) { [ex save:stdex]; }
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

- (NSInteger)getOpt:(Opt)option
{
    return [self props]->get(option);
}

- (NSInteger)getOpt:(Opt)option nr:(NSInteger)nr
{
    return [self props]->get(option, nr);
}

- (void)setKey:(NSString *)key value:(NSString *)value
{
    [self props]->set(string([key UTF8String]), string([value UTF8String]));
}

- (void)setOpt:(Opt)option value:(NSInteger)value
{
    [self props]->set(option, value);
}

- (void)setOpt:(Opt)option nr:(NSInteger)nr value:(NSInteger)value
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

- (void)remove:(Opt)option
{
    [self props]->remove(option);
}

- (void)remove:(Opt) option nr:(NSInteger)nr
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

// DEPPRECATED
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

- (NSString *)memdump:(NSInteger)addr num:(NSInteger)num hex:(BOOL)hex src:(MemType)src
{
    return @([self mem]->memdump((u16)addr, num, hex, hex ? 2 : 1, src).c_str());
}

- (NSString *)txtdump:(NSInteger)addr num:(NSInteger)num src:(MemType)src
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

- (void)lockTexture
{
    [self port]->lockTexture();
}

- (void)unlockTexture
{
    [self port]->unlockTexture();
}

/*
- (u32 *)oldTexture
{
    return [self port]->oldGetTexture();
}

- (u32 *)oldDmaTexture
{
    return [self port]->oldGetDmaTexture();
}
*/

- (void)texture:(const u32 **)ptr nr:(NSInteger *)nr
{
    isize frame, width, height;

    *ptr = [self port]->getTexture(&frame, &width, &height);
    *nr = frame;
}

- (void)dmaTexture:(const u32 **)ptr nr:(NSInteger *)nr
{
    isize frame, width, height;

    *ptr = [self port]->getDmaTexture(&frame, &width, &height);
    *nr = frame;
}

- (void)innerArea:(NSInteger *)x1 x2:(NSInteger *)x2 y1:(NSInteger *)y1 y2:(NSInteger *)y2
{
    isize xx1, xx2, yy1, yy2;
    [self port]->findInnerArea(xx1, xx2, yy1, yy2);

    *x1 = (NSInteger)xx1;
    *x2 = (NSInteger)xx2;
    *y1 = (NSInteger)yy1;
    *y2 = (NSInteger)yy2;
}

- (void)innerAreaNormalized:(double *)x1 x2:(double *)x2 y1:(double *)y1 y2:(double *)y2
{
    double xx1, xx2, yy1, yy2;
    [self port]->findInnerAreaNormalized(xx1, xx2, yy1, yy2);

    *x1 = (double)xx1;
    *x2 = (double)xx2;
    *y1 = (double)yy1;
    *y2 = (double)yy2;
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
    return [self eport]->getCartridgeTraits().type != CartridgeType::NONE;
}
 
- (void)attachCartridge:(NSURL *)url reset:(BOOL)reset exception:(ExceptionWrapper *)ex
{
    try { [self eport]->attachCartridge(fs::path(url.fileSystemRepresentation), reset); }
    catch (std::exception &stdex) { [ex save:stdex]; }
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

- (void)writeToFile:(NSURL *)url exception:(ExceptionWrapper *)ex
{
    try { [self eport]->exportCRT(fs::path(url.fileSystemRepresentation)); }
    catch (std::exception &stdex) { [ex save:stdex]; }
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

- (SectorLayout)sectorInfo:(Halftrack)ht sector:(Sector)s
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

- (void)insert:(NSURL *)url protected:(BOOL)wp exception:(ExceptionWrapper *)ex
{
    try { [self drive]->insert(fs::path(url.fileSystemRepresentation), wp); }
    catch (std::exception &stdex) { [ex save:stdex]; }
}

- (void)insertBlankDisk:(FSFormat)fsType name:(NSString *)name
{
    [self drive]->insertBlankDisk(fsType, [name UTF8String]);
}

- (void)ejectDisk
{
    [self drive]->ejectDisk();
}

- (void)writeToFile:(NSURL *)path exception:(ExceptionWrapper *)ex
{
    try { [self drive]->writeToFile(fs::path(path.fileSystemRepresentation)); }
    catch (std::exception &stdex) { [ex save:stdex]; }
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
    [self emu]->put(Cmd::DATASETTE_PLAY);
}

- (void)pressStop
{
    [self emu]->put(Cmd::DATASETTE_STOP);
}

- (void)rewind
{
    [self emu]->put(Cmd::DATASETTE_REWIND);
}

- (void)ejectTape
{
    [self emu]->datasette.ejectTape();
}

- (void)insertTape:(NSURL *)url exception:(ExceptionWrapper *)ex
{
    try { [self emu]->datasette.insertTape(fs::path(url.fileSystemRepresentation)); }
    catch (std::exception &stdex) { [ex save:stdex]; }
}

- (void)exportTape:(NSURL *)url exception:(ExceptionWrapper *)ex
{
    try { [self emu]->datasette.exportTape(fs::path(url.fileSystemRepresentation)); }
    catch (std::exception &stdex) { [ex save:stdex]; }
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

- (void)setXY:(NSPoint)pos
{
    [self mouse]->setXY(pos.x, pos.y);
}

- (void)setDxDy:(NSPoint)pos
{
    [self mouse]->setDxDy((double)pos.x, (double)pos.y);
}

- (void)trigger:(GamePadAction)event
{
    [self mouse]->trigger(event);
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

- (JoystickInfo)info
{
    return [self joystick]->getInfo();
}

- (JoystickInfo)cachedInfo
{
    return [self joystick]->getCachedInfo();
}

- (void)trigger:(GamePadAction)event
{
    [self joystick]->trigger(event);
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

-(RetroShellInfo)info
{
    return [self shell]->getInfo();
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

- (void)pressSpecialKey:(RSKey)key
{
    [self shell]->press(key);
}

- (void)pressSpecialKey:(RSKey)key shift:(BOOL)shift
{
    [self shell]->press(key, shift);
}

- (void)executeScript:(NSURL *)url exception:(ExceptionWrapper *)ex
{
    try { [self shell]->execScript(fs::path(url.fileSystemRepresentation)); }
    catch (std::exception &stdex) { [ex save:stdex]; }
    // [self shell]->execScript(fs::path(url.fileSystemRepresentation));
}

- (void)executeString:(NSString *)str
{
    [self shell]->execScript(std::string([str UTF8String]));
}

@end

//
// AnyFile proxy
//

@implementation AnyFileProxy

- (AnyFile *)file
{
    return (AnyFile *)obj;
}

+ (instancetype)make:(AnyFile *)file
{
    return file ? [[self alloc] initWith:file] : nil;
}

- (NSURL *)path
{
    auto nsPath = @([self file]->path.c_str());
    return [NSURL fileURLWithPath:nsPath];
}

- (NSInteger)size
{
    return [self file]->getSize();
}

- (u64)fnv
{
    return [self file]->fnv64();
}

- (void)setPath:(NSURL *)path
{
    [self file]->path = [path fileSystemRepresentation];
}

- (NSInteger)writeToFile:(NSURL *)path exception:(ExceptionWrapper *)ex
{
    try { return [self file]->writeToFile([path fileSystemRepresentation]); }
    catch(Error &error) { [ex save:error]; return 0; }
}

- (void)dealloc
{
    delete (AnyFile *)obj;
}

@end


//
// Snapshot
//

@implementation SnapshotProxy

+ (instancetype)makeWithC64:(EmulatorProxy *)proxy compressor:(Compressor)c
{
    auto c64 = (VirtualC64 *)proxy->obj;
    auto snap = c64->c64.takeSnapshot(c);

    //Transfer ownership to ObjC via release
    return [self make:snap.release()];
}

- (Snapshot *)file
{
    return (Snapshot *)obj;
}

- (NSInteger)size
{
    return [self file]->getSize();
}

- (u64)fnv
{
    return [self file]->fnv64();
}

- (Compressor)compressor
{
    return [self file]->compressor();
}

- (BOOL)compressed
{
    return [self file]->isCompressed();
}

- (u8 *)data
{
    return [self file]->getData()+ sizeof(SnapshotHeader);
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
// DiskImageProxy
//

@implementation DiskImageProxy

+ (ImageInfo)about:(NSURL *)url
{
    if (auto about = DiskImage::about([url fileSystemRepresentation])) {
        return *about;
    } else {
        return { ImageType::UNKNOWN, ImageFormat::UNKNOWN };
    }
}

- (DiskImage *)file
{
    return (DiskImage *)obj;
}

- (NSArray<NSString *> *)describe
{
    const auto vec = [self file]->describeImage();

    NSMutableArray<NSString *> *result =
        [NSMutableArray arrayWithCapacity:vec.size()];

    for (const auto &s : vec) {
        [result addObject:[NSString stringWithUTF8String:s.c_str()]];
    }

    return result;
}

- (NSURL *)path
{
    auto nsPath = @([self file]->path.c_str());
    return [NSURL fileURLWithPath:nsPath];
}

- (NSInteger)size
{
    return [self file]->getSize();
}

- (u64)fnv
{
    return [self file]->fnv64();
}

- (NSInteger)writeToFile:(NSURL *)path exception:(ExceptionWrapper *)ex
{
    try { return [self file]->writeToFile([path fileSystemRepresentation]); }
    catch(Error &error) { [ex save:error]; return 0; }
}

- (ImageType)type
{
    return [self file]->type();
}

- (ImageFormat)format
{
    return [self file]->format();
}

-(ImageInfo)info
{
    return [self file]->info();
}

- (NSInteger)bsize
{
    return [self file]->bsize();
}

- (NSInteger)numCyls
{
    return [self file]->numCyls();
}

- (NSInteger)numHeads
{
    return [self file]->numHeads();
}

- (NSInteger)numTracks
{
    return [self file]->numTracks();
}

- (NSInteger)numSectors
{
    return [self file]->numSectors(0);
}

- (NSInteger)numBlocks
{
    return [self file]->numBlocks();
}

- (NSInteger)numBytes
{
    return [self file]->numBytes();
}

- (NSInteger)readByte:(NSInteger)b offset:(NSInteger)offset
{
    return [self file]->readByte(b * [self bsize] + offset);
}

- (NSString *)asciidump:(NSInteger)b offset:(NSInteger)offset len:(NSInteger)len
{
    string result;
    auto p = [self file]->data.ptr + b * [self bsize] + offset;

    for (isize i = 0; i < len; i++) {
        result += isprint(int(p[i])) ? char(p[i]) : '.';
    }

    return @(result.c_str());
}

@end


//
// DiskFileProxy
//

@implementation FloppyDiskImageProxy

+ (ImageInfo)about:(NSURL *)url
{
    if (auto about = FloppyDiskImage::about([url fileSystemRepresentation])) {
        return *about;
    } else {
        return { ImageType::UNKNOWN, ImageFormat::UNKNOWN };
    }
}

- (FloppyDiskImage *)image
{
    return (FloppyDiskImage *)obj;
}

+ (instancetype)make:(void *)file
{
    return file ? [[self alloc] initWith:file] : nil;
}

+ (instancetype)makeWithDrive:(DriveProxy *)proxy
                       format:(ImageFormat)fmt
                    exception:(ExceptionWrapper *)ex
{
    /*
    auto drive = (DriveAPI *)proxy->obj;
    try { return [self make: drive->drive->exportDisk(fmt).release()]; }
    catch(Error &error) { [ex save:error]; return nil; }
    */
    return nil;
}

- (Diameter)diameter
{
    return [self image]->getDiameter();
}

- (Density)density
{
    return [self image]->getDensity();
}

- (BOOL)isSD
{
    return [self image]->isSD();
}

- (BOOL)isDD
{
    return [self image]->isDD();
}

- (BOOL)isHD
{
    return [self image]->isHD();
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

- (NSString *)stateString
{
    std::stringstream ss;
    [self c64]->dump(Category::Trace, ss);
    return @(ss.str().c_str());
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
    return [self getRomTraits:RomType::BASIC];
}

- (RomTraits)charRom
{
    return [self getRomTraits:RomType::CHAR];
}

- (RomTraits)kernalRom
{
    return [self getRomTraits:RomType::KERNAL];
}

- (RomTraits)vc1541Rom
{
    return [self getRomTraits:RomType::VC1541];
}

- (SnapshotProxy *)takeSnapshot:(Compressor)compressor
{
    try {

        auto snap = [self c64]->takeSnapshot(compressor);

        //Transfer ownership to ObjC via release
        return [SnapshotProxy make:snap.release()];

    } catch (std::exception &stdex) {

        return nil;
    }
}

- (void)loadSnapshot:(SnapshotProxy *)proxy exception:(ExceptionWrapper *)ex
{
    try { [self c64]->loadSnapshot(*[proxy file]); }
    catch (std::exception &stdex) { [ex save:stdex]; }
}

- (void)loadSnapshotFromUrl:(NSURL *)url exception:(ExceptionWrapper *)ex
{
    try { [self c64]->loadSnapshot([url fileSystemRepresentation]); }
    catch (std::exception &stdex) { [ex save:stdex]; }
}

- (void)saveSnapshotToUrl:(NSURL *)url compressor:(Compressor)c exception:(ExceptionWrapper *)ex
{
    try { [self c64]->saveSnapshot([url fileSystemRepresentation], c); }
    catch (std::exception &stdex) { [ex save:stdex]; }
}

- (void)loadWorkspace:(NSURL *)url exception:(ExceptionWrapper *)ex
{
    try { [self c64]->loadWorkspace([url fileSystemRepresentation]); }
    catch (std::exception &stdex) { [ex save:stdex]; }
}

- (void)saveWorkspace:(NSURL *)url exception:(ExceptionWrapper *)ex
{
    try { [self c64]->saveWorkspace([url fileSystemRepresentation]); }
    catch (std::exception &stdex) { [ex save:stdex]; }
}

- (BOOL)getMessage:(Message *)msg
{
    return [self c64]->getMsg(*msg);
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

- (void)launch:(ExceptionWrapper *)ex
{
    try { [self emu]->launch(); }
    catch (std::exception &stdex) { [ex save:stdex]; }
}

- (void)launch:(const void *)listener function:(Callback *)func exception:(ExceptionWrapper *)ex
{
    try { [self emu]->launch(listener, func); }
    catch (std::exception &stdex) { [ex save:stdex]; }
}

- (void)hardReset
{
    [self emu]->hardReset();
}

- (void)softReset
{
    [self emu]->softReset();
}

- (void)isReady:(ExceptionWrapper *)ex
{
    try { [self emu]->isReady(); }
    catch (std::exception &stdex) { [ex save:stdex]; }
}

- (void)powerOn:(ExceptionWrapper *)ex
{
    try { [self emu]->powerOn(); }
    catch (std::exception &stdex) { [ex save:stdex]; }
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

- (void)run:(ExceptionWrapper *)ex
{
    try { [self emu]->run(); }
    catch (std::exception &stdex) { [ex save:stdex]; }
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

- (NSInteger)get:(Opt)opt
{
    return [self emu]->get(opt);
}

- (NSInteger)get:(Opt)opt id:(NSInteger)id
{
    return [self emu]->get(opt, id);
}

- (NSInteger)get:(Opt)opt drive:(NSInteger)id
{
    return [self emu]->get(opt, (long)id);
}

- (BOOL)set:(Opt)opt value:(NSInteger)val
{
    try {
        [self emu]->set(opt, val);
        return true;
    } catch (std::exception &stdex) {
        return false;
    }
}

- (BOOL)set:(Opt)opt enable:(BOOL)val
{
    try {
        [self emu]->set(opt, val ? 1 : 0);
        return true;
    } catch (std::exception &stdex) {
        return false;
    }
}

- (BOOL)set:(Opt)opt id:(NSInteger)id value:(NSInteger)val
{
    try {
        [self emu]->set(opt, val, id);
        return true;
    } catch (std::exception &stdex) {
        return false;
    }
}

- (BOOL)set:(Opt)opt id:(NSInteger)id enable:(BOOL)val
{
    try {
        [self emu]->set(opt, val ? 1 : 0, id);
        return true;
    } catch (std::exception &stdex) {
        return false;
    }
}

- (BOOL)set:(Opt)opt drive:(NSInteger)id value:(NSInteger)val
{
    try {
        [self emu]->set(opt, val, (long)id);
        return true;
    } catch (std::exception &stdex) {
        return false;
    }
}

- (BOOL)set:(Opt)opt drive:(NSInteger)id enable:(BOOL)val
{
    try {
        [self emu]->set(opt, val ? 1 : 0, (long)id);
        return true;
    } catch (std::exception &stdex) {
        return false;
    }
}

- (void)set:(ConfigScheme)model
{
    [self emu]->set(model);
}

- (void)exportConfig:(NSURL *)url exception:(ExceptionWrapper *)ex
{
    try { [self emu]->exportConfig([url fileSystemRepresentation]); }
    catch (std::exception &stdex) { [ex save:stdex]; }
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

- (void)stepCycle
{
    [self emu]->stepCycle();
}

- (void)finishLine
{
    [self emu]->finishLine();
}

- (void)finishFrame
{
    [self emu]->finishFrame();
}

- (BOOL) isRom:(RomType)type url:(NSURL *)url
{
    auto fileType = MediaFile::type([url fileSystemRepresentation]);

    return 
    (fileType == FileType::BASIC_ROM && type == RomType::BASIC) ||
    (fileType == FileType::CHAR_ROM && type == RomType::CHAR) ||
    (fileType == FileType::KERNAL_ROM && type == RomType::KERNAL) ||
    (fileType == FileType::VC1541_ROM && type == RomType::VC1541);
}

- (void)installOpenRoms
{
    [self emu]->c64.installOpenRoms();
}

- (RomType *)romType:(NSURL *)url
{
    if (url) {
        if (auto type = RomFile::romType(fs::path(url.fileSystemRepresentation)))
            return new RomType(*type);
    }
    return nil;
}

- (void) loadRom:(NSURL *)url exception:(ExceptionWrapper *)ex
{
    try { [self emu]->c64.loadRom(string([url fileSystemRepresentation])); }
    catch (std::exception &stdex) { [ex save:stdex]; }
}

- (void)loadRom:(NSURL *)url exception:(ExceptionWrapper *)ex type:(RomType)type
{
    try { [self emu]->c64.loadRom(string([url fileSystemRepresentation]), type); }
    catch (std::exception &stdex) { [ex save:stdex]; }
}

- (void) saveRom:(RomType)type url:(NSURL *)url exception:(ExceptionWrapper *)ex
{
    try { [self emu]->c64.saveRom(string([url fileSystemRepresentation]), type); }
    catch (std::exception &stdex) { [ex save:stdex]; }
}

- (void) deleteRom:(RomType)type
{
    [self emu]->c64.deleteRom(type);
}

- (void)flashFile:(NSURL *)url exception:(ExceptionWrapper *)ex
{
    try { [self emu]->c64.flash(fs::path(url.fileSystemRepresentation)); }
    catch (std::exception &stdex) { [ex save:stdex]; }
}

- (void)put:(Cmd)type
{
    [self emu]->put(type, 0);
}

- (void)put:(Cmd)type value:(NSInteger)value
{
    [self emu]->put(type, value);
}

- (void)put:(Cmd)type value:(NSInteger)value value2:(NSInteger)value2
{
    [self emu]->put(type, value, value2);
}

- (void)put:(Cmd)type key:(KeyCmd)cmd
{
    [self emu]->put(type, cmd);
}

- (void)put:(Cmd)type coord:(CoordCmd)cmd
{
    [self emu]->put(type, cmd);
}

- (void)put:(Cmd)type action:(GamePadCmd)cmd
{
    [self emu]->put(type, cmd);
}

@end
