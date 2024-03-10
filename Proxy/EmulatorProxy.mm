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
#import "VirtualC64-Swift.h"

using namespace vc64;

//
// Exception wrapper
//

@implementation ExceptionWrapper

@synthesize errorCode;
@synthesize what;

- (instancetype)init {

    if (self = [super init]) {
        
        errorCode = ERROR_OK;
        what = @"";
    }
    return self;
}

- (void)save:(const VC64Error &)exception
{
    errorCode = exception.data;
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
// CoreComponent proxy
//

@implementation CoreComponentProxy

@end

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

- (Defaults *)props
{
    return (Defaults *)obj;
}

- (void)load:(NSURL *)url exception:(ExceptionWrapper *)ex
{
    try { return [self props]->load([url fileSystemRepresentation]); }
    catch (VC64Error &error) { [ex save:error]; }
}

- (void)save:(NSURL *)url exception:(ExceptionWrapper *)ex
{
    try { return [self props]->save([url fileSystemRepresentation]); }
    catch (VC64Error &error) { [ex save:error]; }
}

- (void)register:(NSString *)key value:(NSString *)value
{
    [self props]->setFallback(string([key UTF8String]), string([value UTF8String]));
}

- (NSString *)getString:(NSString *)key
{
    auto result = [self props]->getString([key UTF8String]);
    return @(result.c_str());
}

- (NSInteger)getInt:(NSString *)key
{
    return [self props]->getInt([key UTF8String]);
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
    [self props]->setString(string([key UTF8String]), string([value UTF8String]));
}

- (void)setOpt:(Option)option value:(NSInteger)value
{
    [self props]->set(option, value);
}

- (void)setOpt:(Option)option nr:(NSInteger)nr value:(NSInteger)value
{
    [self props]->set(option, nr, value);
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
    [self props]->remove(option, nr);
}

@end


//
// Guards (Breakpoints, Watchpoints)
//

@implementation GuardsProxy

- (VirtualC64::GuardAPI *)guards
{
    return (VirtualC64::GuardAPI *)obj;
}

- (NSInteger)count
{
    return [self guards]->elements();
}

- (NSInteger)addr:(NSInteger)nr
{
    return [self guards]->guardAddr(nr);
}

- (BOOL)isEnabled:(NSInteger)nr
{
    return [self guards]->isEnabled(nr);
}

- (BOOL)isDisabled:(NSInteger)nr
{
    return [self guards]->isDisabled(nr);
}

- (void)enable:(NSInteger)nr
{
    [self guards]->enable(nr);
}

- (void)disable:(NSInteger)nr
{
    [self guards]->disable(nr);
}

- (void)remove:(NSInteger)nr
{
    return [self guards]->remove(nr);
}

- (void)replace:(NSInteger)nr addr:(NSInteger)addr
{
    [self guards]->replace(nr, (u32)addr);
}

- (BOOL)isSetAt:(NSInteger)addr
{
    return [self guards]->isSetAt((u32)addr);
}

- (BOOL)isSetAndEnabledAt:(NSInteger)addr
{
    return [self guards]->isSetAndEnabledAt((u32)addr);
}

- (BOOL)isSetAndDisabledAt:(NSInteger)addr
{
    return [self guards]->isSetAndDisabledAt((u32)addr);
}

- (void)enableAt:(NSInteger)addr
{
    [self guards]->enableAt((u32)addr);
}

- (void)disableAt:(NSInteger)addr
{
    [self guards]->disableAt((u32)addr);
}

- (void)addAt:(NSInteger)addr
{
    [self guards]->addAt((u32)addr);
}

- (void)removeAt:(NSInteger)addr
{
    [self guards]->removeAt((u32)addr);
}

@end


//
// CPU proxy
//

@implementation CPUProxy

- (VirtualC64::CPUAPI *)cpu
{
    return (VirtualC64::CPUAPI *)obj;
}

- (CPUInfo)info
{
    return [self cpu]->getInfo();
}

- (i64)clock
{
    return (i64)[self cpu]->clock();
}

- (u16)pc
{
    return [self cpu]->getPC0();
}

- (NSInteger)loggedInstructions
{
    return [self cpu]->loggedInstructions();
}

- (NSInteger)loggedPCRel:(NSInteger)nr
{
    return [self cpu]->loggedPC0Rel((int)nr);
}

- (NSInteger)loggedPCAbs:(NSInteger)nr
{
    return [self cpu]->loggedPC0Abs((int)nr);
}

- (RecordedInstruction)getRecordedInstruction:(NSInteger)index
{
    return [self cpu]->logEntryAbs((int)index);
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

- (NSString *)disassembleRecordedInstr:(NSInteger)i length:(NSInteger *)len
{
    char result[32];

    (void)[self cpu]->disassembleRecordedInstr(i, result);
    return @(result);
}

- (NSString *)disassembleRecordedBytes:(NSInteger)i
{
    char result[16];

    (void)[self cpu]->disassembleRecordedBytes(i, result);
    return @(result);
}

- (NSString *)disassembleRecordedFlags:(NSInteger)i
{
    char result[16];

    (void)[self cpu]->disassembleRecordedFlags(i, result);
    return @(result);
}

- (NSString *)disassembleRecordedPC:(NSInteger)i
{
    char result[16];

    (void)[self cpu]->disassembleRecordedPC(i, result);
    return @(result);
}

- (NSString *)disassembleInstr:(NSInteger)addr length:(NSInteger *)len
{
    char result[32];

    auto length = [self cpu]->disassemble(result, u16(addr));

    *len = (NSInteger)length;
    return @(result);
}

- (NSString *)disassembleBytes:(NSInteger)addr
{
    char result[32];

    auto length = [self cpu]->getLengthOfInstructionAt(u16(addr));
    [self cpu]->dumpBytes(result, u16(addr), length);
    return @(result);
}

- (NSString *)disassembleAddr:(NSInteger)addr
{
    char result[32];

    [self cpu]->dumpWord(result, u16(addr));
    return @(result);
}

@end


//
// Memory proxy
//

@implementation MemoryProxy

- (VirtualC64::MemoryAPI *)mem
{
    return (VirtualC64::MemoryAPI *)obj;
}

- (MemConfig)config
{
    return [self mem]->getConfig();
}

- (MemInfo)info
{
    return [self mem]->getInfo();
}

- (NSString *)memdump:(NSInteger)addr num:(NSInteger)num hex:(BOOL)hex src:(MemoryType)src
{
    return @([self mem]->memdump((u16)addr, num, hex, hex ? 2 : 1, src).c_str());
}

- (NSString *)txtdump:(NSInteger)addr num:(NSInteger)num src:(MemoryType)src
{
    return @([self mem]->txtdump((u16)addr, num, src).c_str());
}

@end


//
// CIA
//

@implementation CIAProxy

- (VirtualC64::CIAAPI *)cia
{
    return (VirtualC64::CIAAPI *)obj;
}

- (CIAConfig)config
{
    return [self cia]->getConfig();
}

- (CIAInfo)info
{
    return [self cia]->getInfo();
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

- (VirtualC64::VICIIAPI *)vicii
{
    return (VirtualC64::VICIIAPI *)obj;
}

- (NSInteger)hPixels
{
    return [self vicii]->getCyclesPerLine() * 8;
}

- (NSInteger)vPixels
{
    return [self vicii]->getLinesPerFrame();
}

- (VICIIConfig)config
{
    return [self vicii]->getConfig();
}

- (VICIIInfo)info 
{
    return [self vicii]->getInfo();
}

- (BOOL)isPAL
{
    return [self vicii]->pal();
}

- (SpriteInfo)getSpriteInfo:(NSInteger)sprite
{
    return [self vicii]->getSpriteInfo((unsigned)sprite);
}

/*
- (u32 *)texture
{
    return [self vicii]->getTexture();
}
*/

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

- (VirtualC64::DmaDebuggerAPI *)debugger
{
    return (VirtualC64::DmaDebuggerAPI *)obj;
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

- (VirtualC64::SIDAPI *)bridge
{
    return (VirtualC64::SIDAPI *)obj;
}

- (SIDInfo)getInfo:(NSInteger)nr
{
    return [self bridge]->getInfo((unsigned)nr);
}

- (VoiceInfo)getVoiceInfo:(NSInteger)nr voice:(NSInteger)voice
{
    return [self bridge]->getVoiceInfo((unsigned)nr, (unsigned)voice);
}

- (MuxerStats)stats
{
    return [self bridge]->getStats();
}

- (void)copyMono:(float *)target size:(NSInteger)n
{
    [self bridge]->copyMono(target, n);
}

- (void)copyStereo:(float *)target1 buffer2:(float *)target2 size:(NSInteger)n
{
    [self bridge]->copyStereo(target1, target2, n);
}

- (void)copyInterleaved:(float *)target size:(NSInteger)n
{
    [self bridge]->copyInterleaved(target, n);
}

- (void)rampUp
{
    [self bridge]->rampUp();
}

- (void)rampUpFromZero
{
    [self bridge]->rampUp(0);
}

- (void)rampDown
{
    [self bridge]->rampDown();
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
// IEC bus
//

@implementation IECProxy

- (VirtualC64::IECAPI *)iec
{
    return (VirtualC64::IECAPI *)obj;
}

/*
- (BOOL)transferring
{
    return [self iec]->isTransferring();
}
*/

@end


//
// Keyboard
//

@implementation KeyboardProxy

- (VirtualC64::KeyboardAPI *)kb
{
    return (VirtualC64::KeyboardAPI *)obj;
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
    [self emu]->put(CMD_KEY_PRESS, KeyCmd { .keycode = (u8)nr });
}

/*
- (void)pressKeyAtRow:(NSInteger)row col:(NSInteger)col
{
    [self emu]->put(CMD_KEY_PRESS, KeyCmd { .keycode = (u8)C64Key(row, col).nr, .delay = 0.0 });
}
*/

- (void)releaseKey:(NSInteger)nr
{
    // [self kb]->release(C64Key(nr));
    [self emu]->put(CMD_KEY_RELEASE, KeyCmd { .keycode = u8(nr) });
}

- (void)releaseKey:(NSInteger)nr delay:(double)delay
{
    [self emu]->put(CMD_KEY_RELEASE, KeyCmd { .keycode = (u8)nr, .delay = delay });
}

- (void)releaseAll
{
    [self emu]->put(CMD_KEY_RELEASE_ALL);
}

- (void)releaseAllWithDelay:(double)delay
{
    [self emu]->put(CMD_KEY_RELEASE_ALL, KeyCmd { .delay = delay });
}

- (void)toggleKey:(NSInteger)nr
{
    [self emu]->put(CMD_KEY_TOGGLE, KeyCmd { .keycode = u8(nr) });
}

- (void)toggleKeyAtRow:(NSInteger)row col:(NSInteger)col
{
    [self emu]->put(CMD_KEY_TOGGLE, KeyCmd { .keycode = u8(C64Key(row, col).nr) });
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

- (instancetype) initWith:(void *)ref
{
    if (self = [super init]) {
        
        VirtualC64::ControlPortAPI *port = (VirtualC64::ControlPortAPI *)ref;
        obj = ref;
        joystick = [[JoystickProxy alloc] initWith:&port->joystick];
        mouse = [[MouseProxy alloc] initWith:&port->mouse];
    }
    return self;
}

- (VirtualC64::ControlPortAPI *)port
{
    return (VirtualC64::ControlPortAPI *)obj;
}

@end


//
// Expansion port
//

@implementation ExpansionPortProxy

- (VirtualC64::ExpansionPortAPI *)eport
{
    return (VirtualC64::ExpansionPortAPI *)obj;
}

- (VirtualC64 *)emu
{
    return (VirtualC64 *)emu;
}

- (CartridgeTraits)traits
{
    return [self eport]->getTraits();
}

- (CartridgeInfo)info
{
    return [self eport]->getInfo();
}

- (CartridgeRomInfo)getRomInfo:(NSInteger)nr
{
    return [self eport]->getRomInfo(nr);
}

- (BOOL)cartridgeAttached
{
    return [self eport]->getTraits().type != CRT_NONE;
}
 
- (void)attachCartridge:(CRTFileProxy *)c reset:(BOOL)reset exception:(ExceptionWrapper *)ex
{
    try { [self eport]->attachCartridge(*(CRTFile *)c->obj, reset); }
    catch (VC64Error &err) { [ex save:err]; }
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
// Disk
//

@implementation DiskProxy

- (VirtualC64::DriveAPI *)drive
{
    return (VirtualC64::DriveAPI *)obj;
}

- (VirtualC64::DiskAPI *)disk
{
    return &[self drive]->disk;
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

- (instancetype) initWithDisk:(DiskProxy *)disk
{
    NSLog(@"DiskAnalyzerProxy::initWithDisk");
    
    if (!(self = [super init])) return self;
    auto dsk = [disk disk]->drive.disk.get();
    obj = new DiskAnalyzer(*dsk);

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

- (NSInteger)numErrors:(Halftrack)ht
{
    return [self analyzer]->numErrors(ht);
}

- (NSString *)errorMessage:(Halftrack)ht nr:(NSInteger)nr
{
    string s = [self analyzer]->errorMessage(ht, nr);
    return [NSString stringWithUTF8String:s.c_str()];
}

- (NSInteger)firstErroneousBit:(Halftrack)ht nr:(NSInteger)nr
{
    return [self analyzer]->firstErroneousBit(ht, nr);
}

- (NSInteger)lastErroneousBit:(Halftrack)ht nr:(NSInteger)nr
{
    return [self analyzer]->lastErroneousBit(ht, nr);
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

- (instancetype)initWithVC1541:(VirtualC64::DriveAPI *)drive emu:(VirtualC64 *)emuref
{
    if ([self initWith:drive emu:emuref]) {
        disk = [[DiskProxy alloc] initWith:drive];
    }
    return self;
}

- (VirtualC64::DriveAPI *)drive
{
    return (VirtualC64::DriveAPI *)obj;
}

- (VirtualC64 *)emu
{
    return (VirtualC64 *)emu;
}

- (DiskProxy *)disk
{
    return [self drive]->getInfo().hasDisk ? disk : NULL;
}

/*
- (NSInteger)id
{
    return [self drive]->getDeviceNr();
}
*/

- (DriveConfig)config
{
    return [self drive]->getConfig();
}

- (DriveInfo)info
{
    return [self drive]->getInfo();
}

- (void)insertD64:(D64FileProxy *)proxy protected:(BOOL)wp
{
    [self drive]->insertD64(*(D64File *)proxy->obj, wp);
}

- (void)insertG64:(G64FileProxy *)proxy protected:(BOOL)wp
{
    [self drive]->insertG64(*(G64File *)proxy->obj, wp);
}

- (void)insertCollection:(AnyCollectionProxy *)proxy protected:(BOOL)wp
{
    [self drive]->insertCollection(*(AnyCollection *)proxy->obj, wp);
}

- (void)insertFileSystem:(FileSystemProxy *)proxy protected:(BOOL)wp
{
    [self drive]->insertFileSystem(*(FileSystem *)proxy->obj, wp);
}

- (void)insertBlankDisk:(DOSType)fsType name:(NSString *)name
{
    [self drive]->insertBlankDisk(fsType, PETName<16>([name UTF8String]));
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

- (VirtualC64::DatasetteAPI *)datasette
{
    return (VirtualC64::DatasetteAPI *)obj;
}

- (VirtualC64 *)emu
{
    return (VirtualC64 *)emu;
}

- (DatasetteInfo)info
{
    return [self datasette]->getInfo();
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

- (void)insertTape:(TAPFileProxy *)proxy
{
    [self emu]->datasette.insertTape(*(TAPFile *)proxy->obj);
}

- (void)ejectTape
{
    [self emu]->datasette.ejectTape();
}

@end


//
// Mouse proxy
//

@implementation MouseProxy

- (VirtualC64::MouseAPI *)mouse
{
    return (VirtualC64::MouseAPI *)obj;
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

- (VirtualC64::JoystickAPI *)joystick
{
    return (VirtualC64::JoystickAPI *)obj;
}

@end


//
// Recorder
//

@implementation RecorderProxy

- (VirtualC64::RecorderAPI *)recorder
{
    return (VirtualC64::RecorderAPI *)obj;
}

- (RecorderInfo)info
{
    return [self recorder]->getState();
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
    return [self recorder]->isRecording();
}

- (NSInteger)frameRate
{
    return [self recorder]->getFrameRate();
}

- (NSInteger)bitRate
{
    return [self recorder]->getBitRate();
}

- (NSInteger)sampleRate
{
    return [self recorder]->getSampleRate();
}

- (void)startRecording:(NSRect)rect
               bitRate:(NSInteger)rate
               aspectX:(NSInteger)aspectX
               aspectY:(NSInteger)aspectY
             exception:(ExceptionWrapper *)ex
{
    auto x1 = isize(rect.origin.x);
    auto y1 = isize(rect.origin.y);
    auto x2 = isize(x1 + (int)rect.size.width);
    auto y2 = isize(y1 + (int)rect.size.height);

    try { return [self recorder]->startRecording(x1, y1, x2, y2, rate, aspectX, aspectY); }
    catch (VC64Error &error) { [ex save:error]; }
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
// RetroShell proxy
//

@implementation RetroShellProxy

- (VirtualC64::RetroShellAPI *)shell
{
    return (VirtualC64::RetroShellAPI *)obj;
}

+ (instancetype)make:(VirtualC64::RetroShellAPI *)shell
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

@end


//
// AnyFile
//

@implementation AnyFileProxy

+ (AnyFileProxy *)makeWithFile:(AnyFile *)file
{
    return file ? [[self alloc] initWith:file] : nil;
}

- (AnyFile *)file
{
    return (AnyFile *)obj;
}

+ (FileType)typeOfUrl:(NSURL *)url
{
    return AnyFile::type([url fileSystemRepresentation]);
}

- (FileType)type
{
    return [self file]->type();
}

- (NSString *)name
{
    return [NSString stringWithUTF8String:[self file]->getName().c_str()];
}

- (u64)fnv
{
    return [self file]->fnv();
}

- (void)setPath:(NSString *)path
{
    [self file]->path = [path fileSystemRepresentation];
}

- (void)writeToFile:(NSString *)path exception:(ExceptionWrapper *)ex
{
    try { [self file]->writeToFile(string([path fileSystemRepresentation])); }
    catch (VC64Error &err) { [ex save:err]; }    
}

@end


//
// Snapshot proxy
//

@implementation SnapshotProxy

- (Snapshot *)snapshot
{
    return (Snapshot *)obj;
}

+ (instancetype)make:(Snapshot *)snapshot
{
    return snapshot ? [[self alloc] initWith:snapshot] : nil;
}

+ (instancetype)makeWithFile:(NSString *)path
                   exception:(ExceptionWrapper *)ex
{
    try { return [self make: new Snapshot([path fileSystemRepresentation])]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len
                     exception:(ExceptionWrapper *)ex
{
    try { return [self make: new Snapshot((u8 *)buf, len)]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithC64:(EmulatorProxy *)proxy
{
    [proxy suspend];
    Snapshot *snapshot = new Snapshot(*(C64 *)proxy->obj);
    [proxy resume];

    return [self make:snapshot];
}

- (NSImage *)previewImage
{
    // Return cached image (if any)
    if (preview) { return preview; }
    
    // Create preview image
    const Thumbnail &thumbnail = [self snapshot]->getThumbnail();
    unsigned char *data = (unsigned char *)thumbnail.screen;
    
    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc]
                             initWithBitmapDataPlanes: &data
                             pixelsWide:thumbnail.width
                             pixelsHigh:thumbnail.height
                             bitsPerSample:8
                             samplesPerPixel:4
                             hasAlpha:true
                             isPlanar:false
                             colorSpaceName:NSCalibratedRGBColorSpace
                             bytesPerRow:4*thumbnail.width
                             bitsPerPixel:32];
    
    preview = [[NSImage alloc] initWithSize:[rep size]];
    [preview addRepresentation:rep];
    
    // image.makeGlossy()
    
    return preview;
}

- (time_t)timeStamp
{
    return [self snapshot]->getThumbnail().timestamp;
}

@end


//
// Script proxy
//

@implementation ScriptProxy

- (Script *)script
{
    return (Script *)obj;
}

+ (instancetype)make:(Script *)file
{
    return file ? [[self alloc] initWith:file] : nil;
}

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex
{
    try { return [self make: new Script([path fileSystemRepresentation])]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex
{
    try { return [self make: new Script((u8 *)buf, len)]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }    
}

- (void)execute:(EmulatorProxy *)proxy
{
    VirtualC64 *c64 = (VirtualC64 *)proxy->obj;

    [self script]->execute(*c64);
}

@end


//
// RomFile proxy
//

@implementation RomFileProxy

+ (instancetype)make:(RomFile *)file
{
    return file ? [[self alloc] initWith:file] : nil;
}

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex
{
    try { return [self make: new RomFile([path fileSystemRepresentation])]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex
{
    try { return [self make: new RomFile((const u8 *)buf, len)]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

@end


//
// CRT proxy
//

@implementation CRTFileProxy

+ (instancetype)make:(CRTFile *)container
{
    return container ? [[self alloc] initWith:container] : nil;
}

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex
{
    try { return [self make: new CRTFile([path fileSystemRepresentation])]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex
{
    try { return [self make: new CRTFile((const u8 *)buf, len)]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

- (CRTFile *)crt
{
    return (CRTFile *)obj;
}

- (CartridgeType)cartridgeType
{
    return [self crt]->cartridgeType();
}

- (BOOL)isSupported
{
    return [self crt]->isSupported();
}

- (NSInteger)initialExromLine
{
    return [self crt]->initialExromLine();
}

- (NSInteger)initialGameLine
{
    return [self crt]->initialGameLine();
}

- (NSInteger)chipCount
{
    return [self crt]->chipCount();
}

@end


//
// TAP
//

@implementation TAPFileProxy

+ (instancetype)make:(TAPFile *)container
{
    return container ? [[self alloc] initWith:container] : nil;
}

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex
{
    try { return [self make: new TAPFile([path fileSystemRepresentation])]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex
{
    try { return [self make: new TAPFile((const u8 *)buf, len)]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

- (TAPFile *)tap
{
    return (TAPFile *)obj;
}

- (TAPVersion)version
{
    return [self tap]->version();
}

@end


//
// AnyCollection
//

@implementation AnyCollectionProxy

- (AnyCollection *)unwrap
{
    return (AnyCollection *)obj;
}

@end


//
// T64
//

@implementation T64FileProxy

+ (instancetype)make:(T64File *)archive
{
    return archive ? [[self alloc] initWith:archive] : nil;
}

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex
{
    try { return [self make: new T64File([path fileSystemRepresentation])]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex
{
    try { return [self make: new T64File((const u8 *)buf, len)]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithFileSystem:(FileSystemProxy *)proxy exception:(ExceptionWrapper *)ex
{
    try { return [self make: new T64File(*(FileSystem *)proxy->obj)]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

@end


//
// PRG
//

@implementation PRGFileProxy

+ (instancetype)make:(PRGFile *)archive
{
    return archive ? [[self alloc] initWith:archive] : nil;
}

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex
{
    try { return [self make: new PRGFile([path fileSystemRepresentation])]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex
{
    try { return [self make: new PRGFile((const u8 *)buf, len)]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithFileSystem:(FileSystemProxy *)proxy exception:(ExceptionWrapper *)ex
{
    try { return [self make: new PRGFile(*(FileSystem *)proxy->obj)]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

@end


//
// P00 proxy
//

@implementation P00FileProxy

+ (instancetype)make:(P00File *)archive
{
    return archive ? [[self alloc] initWith:archive] : nil;
}

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex
{
    try { return [self make: new P00File([path fileSystemRepresentation])]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex
{
    try { return [self make: new P00File((const u8 *)buf, len)]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithFileSystem:(FileSystemProxy *)proxy exception:(ExceptionWrapper *)ex
{
    try { return [self make: new P00File(*(FileSystem *)proxy->obj)]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

@end


//
// D64 proxy
//

@implementation D64FileProxy

- (D64File *)d64
{
    return (D64File *)obj;
}

+ (instancetype)make:(D64File *)archive
{
    return archive ? [[self alloc] initWith:archive] : nil;
}

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex
{
    try { return [self make: new D64File([path fileSystemRepresentation])]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex
{
    try { return [self make: new D64File((const u8 *)buf, len)]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithFileSystem:(FileSystemProxy *)proxy exception:(ExceptionWrapper *)ex
{
    try { return [self make: new D64File(*(FileSystem *)proxy->obj)]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

@end


//
// G64 proxy
//

@implementation G64FileProxy

+ (instancetype)make:(G64File *)archive
{
    return archive ? [[self alloc] initWith:archive] : nil;
}

+ (instancetype)makeWithFile:(NSString *)path exception:(ExceptionWrapper *)ex
{
    try { return [self make: new G64File([path fileSystemRepresentation])]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len exception:(ExceptionWrapper *)ex
{
    try { return [self make: new G64File((const u8 *)buf, len)]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
}

+ (instancetype)makeWithDisk:(DiskProxy *)proxy exception:(ExceptionWrapper *)ex
{
    try { return [self make: new G64File(*(Disk *)proxy->obj)]; }
    catch (VC64Error &error) { [ex save:error]; return nil; }
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

+ (instancetype)makeWithDisk:(DiskProxy *)proxy exception:(ExceptionWrapper *)ex
{
    Drive *drv = (Drive *)proxy->obj;
    try { return [self make: new FileSystem(*drv->disk)]; }
    catch (VC64Error &err) { [ex save:err]; return nil; }
}

+ (instancetype)makeWithCollection:(AnyCollectionProxy *)proxy exception:(ExceptionWrapper *)ex
{
    try { return [self make: new FileSystem(*(AnyCollection *)proxy->obj)]; }
    catch (VC64Error &err) { [ex save:err]; return nil; }
}

+ (instancetype)makeWithD64:(D64FileProxy *)proxy exception:(ExceptionWrapper *)ex
{
    try { return [self make: new FileSystem(*(D64File *)proxy->obj)]; }
    catch (VC64Error &err) { [ex save:err]; return nil; }
}

+ (instancetype)makeWithDiskType:(DiskType)diskType dosType:(DOSType)dosType
{
    return [self make: new FileSystem(diskType, dosType)];
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
    catch (VC64Error &error) { [e save:error]; }
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
// Folder proxy
//

@implementation FolderProxy

+ (instancetype)make:(Folder *)folder
{
    return folder ? [[self alloc] initWith:folder] : nil;
}

+ (instancetype)makeWithFolder:(NSString *)path exception:(ExceptionWrapper *)ex
{
    try { return [self make: new Folder([path fileSystemRepresentation])]; }
    catch (VC64Error &err) { [ex save:err]; return nil; }    
}

- (Folder *)folder
{
    return (Folder *)obj;
}

- (FileSystemProxy *)fileSystem
{
    return [FileSystemProxy make:[self folder]->getFS()];
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
// Emulator
//

@implementation EmulatorProxy

@synthesize breakpoints;
@synthesize cia1;
@synthesize cia2;
@synthesize cpu;
@synthesize datasette;
@synthesize dmaDebugger;
@synthesize drive8;
@synthesize drive9;
@synthesize expansionport;
@synthesize iec;
@synthesize keyboard;
@synthesize mem;
@synthesize port1;
@synthesize port2;
@synthesize recorder;
@synthesize retroShell;
@synthesize sid;
@synthesize vic;
@synthesize watchpoints;

- (instancetype) init
{
    if (!(self = [super init])) return self;
    
    // Create the emulator instance
    VirtualC64 *emu = new VirtualC64();
    obj = emu;

    // Create sub proxys
    breakpoints = [[GuardsProxy alloc] initWith:&emu->cpu.breakpoints];
    cia1 = [[CIAProxy alloc] initWith:&emu->cia1];
    cia2 = [[CIAProxy alloc] initWith:&emu->cia2];
    cpu = [[CPUProxy alloc] initWith:&emu->cpu];
    datasette = [[DatasetteProxy alloc] initWith:&emu->datasette emu:emu];
    dmaDebugger = [[DmaDebuggerProxy alloc] initWith:&emu->dmaDebugger];
    drive8 = [[DriveProxy alloc] initWithVC1541:&emu->drive8 emu:emu];
    drive9 = [[DriveProxy alloc] initWithVC1541:&emu->drive9 emu:emu];
    expansionport = [[ExpansionPortProxy alloc] initWith:&emu->expansionport emu:emu];
    iec = [[IECProxy alloc] initWith:&emu->iec];
    keyboard = [[KeyboardProxy alloc] initWith:&emu->keyboard emu:emu];
    mem = [[MemoryProxy alloc] initWith:&emu->mem];
    port1 = [[ControlPortProxy alloc] initWith:&emu->port1];
    port2 = [[ControlPortProxy alloc] initWith:&emu->port2];
    recorder = [[RecorderProxy alloc] initWith:&emu->recorder];
    retroShell = [[RetroShellProxy alloc] initWith:&emu->retroShell];
    sid = [[SIDProxy alloc] initWith:&emu->muxer];
    vic = [[VICIIProxy alloc] initWith:&emu->vicii];
    watchpoints = [[GuardsProxy alloc] initWith:&emu->cpu.watchpoints];

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

- (BOOL)releaseBuild
{
    return releaseBuild;
}

- (BOOL)trackMode
{
    return [self emu]->isTracking();
}

- (void)setTrackMode:(BOOL)value
{
    if (value) {
        [self emu]->trackOn();
    } else {
        [self emu]->trackOff();
    }
}

- (InspectionTarget)inspectionTarget
{
    return [self emu]->c64.getInspectionTarget();
}

- (void)setInspectionTarget:(InspectionTarget)target
{
    [self emu]->c64.setInspectionTarget(target);
}

- (void) removeInspectionTarget
{
    [self emu]->c64.removeInspectionTarget();
}

- (C64Info)eventInfo
{
    return [self emu]->c64.getInfo();
}

- (EventSlotInfo)getEventSlotInfo:(NSInteger)slot
{
    return [self emu]->c64.getSlotInfo(slot);
}

- (void)launch:(const void *)listener function:(Callback *)func
{
    [self emu]->launch(listener, func);
}

- (void)hardReset
{
    [self emu]->c64.hardReset();
}

- (void)softReset
{
    [self emu]->c64.softReset();
}

- (void)isReady:(ExceptionWrapper *)ex
{
    try { [self emu]->c64.isReady(); }
    catch (VC64Error &error) { [ex save:error]; }
}

- (void)powerOn:(ExceptionWrapper *)ex
{
    [self emu]->powerOn();
}

- (void)powerOff
{
    [self emu]->powerOff();
}

- (EmulatorInfo)info
{
    return [self emu]->getState();
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

- (u32 *)texture
{
    return [self emu]->getTexture();
}

- (u32 *)noise
{
    return [self emu]->getNoise();
}

- (void)requestAutoSnapshot
{
    [self emu]->put(CMD_SNAPSHOT_AUTO);
}

- (void)requestUserSnapshot
{
    [self emu]->put(CMD_SNAPSHOT_USER);
}

- (SnapshotProxy *)latestAutoSnapshot
{
    Snapshot *snapshot = [self emu]->c64.latestAutoSnapshot();
    return [SnapshotProxy make:snapshot];
}

- (SnapshotProxy *)latestUserSnapshot
{
    Snapshot *snapshot = [self emu]->c64.latestUserSnapshot();
    return [SnapshotProxy make:snapshot];
}

- (NSInteger)getConfig:(Option)opt
{
    return [self emu]->get(opt);
}

- (NSInteger)getConfig:(Option)opt id:(NSInteger)id
{
    return [self emu]->get(opt, id);
}

- (NSInteger)getConfig:(Option)opt drive:(NSInteger)id
{
    return [self emu]->get(opt, (long)id);
}

- (BOOL)configure:(Option)opt value:(NSInteger)val
{
    try {
        [self emu]->set(opt, val);
        return true;
    } catch (VC64Error &exception) {
        return false;
    }
}

- (BOOL)configure:(Option)opt enable:(BOOL)val
{
    try {
        [self emu]->set(opt, val ? 1 : 0);
        return true;
    } catch (VC64Error &exception) {
        return false;
    }
}

- (BOOL)configure:(Option)opt id:(NSInteger)id value:(NSInteger)val
{
    try {
        [self emu]->set(opt, id, val);
        return true;
    } catch (VC64Error &exception) {
        return false;
    }
}

- (BOOL)configure:(Option)opt id:(NSInteger)id enable:(BOOL)val
{
    try {
        [self emu]->set(opt, id, val ? 1 : 0);
        return true;
    } catch (VC64Error &exception) {
        return false;
    }
}

- (BOOL)configure:(Option)opt drive:(NSInteger)id value:(NSInteger)val
{
    try {
        [self emu]->set(opt, (long)id, val);
        return true;
    } catch (VC64Error &exception) {
        return false;
    }
}

- (BOOL)configure:(Option)opt drive:(NSInteger)id enable:(BOOL)val
{
    try {
        [self emu]->set(opt, (long)id, val ? 1 : 0);
        return true;
    } catch (VC64Error &exception) {
        return false;
    }
}

- (void)configure:(C64Model)model
{
    [self emu]->set(model);
}

- (void)wakeUp
{
    [self emu]->wakeUp();
}

- (void)stopAndGo
{
    [self emu]->stopAndGo();
}

- (void)stepInto
{
    [self emu]->stepInto();
}

- (void)stepOver
{
    [self emu]->stepOver();
}

- (RomInfo)getRomInfo:(RomType)type
{
    return [self emu]->c64.getRomInfo(type);
}

- (RomInfo)basicRom
{
    return [self emu]->c64.getRomInfo(ROM_TYPE_BASIC);
}

- (RomInfo)charRom
{
    return [self emu]->c64.getRomInfo(ROM_TYPE_CHAR);
}

- (RomInfo)kernalRom
{
    return [self emu]->c64.getRomInfo(ROM_TYPE_KERNAL);
}

- (RomInfo)vc1541Rom
{
    return [self emu]->c64.getRomInfo(ROM_TYPE_VC1541);
}
/*
- (BOOL) hasRom:(RomType)type
{
    return [self emu]->c64.hasRom(type);
}

- (BOOL)hasMega65Rom:(RomType)type
{
    return [self emu]->c64.hasMega65Rom(type);
}
*/

- (BOOL) isRom:(RomType)type url:(NSURL *)url
{
    return RomFile::isRomFile(type, [url fileSystemRepresentation]);
}

- (void) loadRom:(NSURL *)url exception:(ExceptionWrapper *)e
{
    try { [self emu]->c64.loadRom(string([url fileSystemRepresentation])); }
    catch (VC64Error &error) { [e save:error]; }
}

- (void) loadRom:(RomFileProxy *)proxy
{
    [self emu]->c64.loadRom(*(RomFile *)proxy->obj);
}

- (void) saveRom:(RomType)type url:(NSURL *)url exception:(ExceptionWrapper *)e
{
    try { [self emu]->c64.saveRom(type, string([url fileSystemRepresentation])); }
    catch (VC64Error &error) { [e save:error]; }
}

- (void) deleteRom:(RomType)type
{
    [self emu]->c64.deleteRom(type);
}

- (void)flash:(AnyFileProxy *)proxy exception:(ExceptionWrapper *)ex
{
    try { [self emu]->c64.flash(*(AnyFile *)proxy->obj); }
    catch (VC64Error &error) { [ex save:error]; }
}

- (void)flash:(FileSystemProxy *)proxy item:(NSInteger)nr exception:(ExceptionWrapper *)ex
{
    try { [self emu]->c64.flash(*(FileSystem *)proxy->obj, (unsigned)nr); }
    catch (VC64Error &error) { [ex save:error]; }
}

- (void)send:(CmdType)type
{
    [self emu]->put(type, 0);
}

- (void)send:(CmdType)type value:(NSInteger)value
{
    [self emu]->put(type, value);
}

- (void)send:(CmdType)type key:(KeyCmd)cmd
{
    [self emu]->put(type, cmd);
}

- (void)send:(CmdType)type coord:(CoordCmd)cmd
{
    [self emu]->put(type, cmd);
}

- (void)send:(CmdType)type action:(GamePadCmd)cmd
{
    [self emu]->put(type, cmd);
}

@end
