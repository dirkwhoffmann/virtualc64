// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#import "config.h"
#import "C64Proxy.h"
#import "C64.h"
#import "C64Key.h"
#import "VirtualC64-Swift.h"
#import "Script.h"

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
// C64Component proxy
//

@implementation C64ComponentProxy

-(C64Component *)component
{
    return (C64Component *)obj;
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

- (Guards *)guards
{
    return (Guards *)obj;
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

- (CPU<C64Memory> *)cpu
{
    return (CPU<C64Memory> *)obj;
}

- (CPUInfo)info
{
    return [self cpu]->getInfo();
}

- (i64)clock
{
    return (i64)[self cpu]->clock;
}

- (u16)pc
{
    return [self cpu]->getPC0();
}

- (NSInteger)loggedInstructions
{
    return [self cpu]->debugger.loggedInstructions();
}

- (NSInteger)loggedPCRel:(NSInteger)nr
{
    return [self cpu]->debugger.loggedPC0Rel((int)nr);
}

- (NSInteger)loggedPCAbs:(NSInteger)nr
{
    return [self cpu]->debugger.loggedPC0Abs((int)nr);
}

- (RecordedInstruction)getRecordedInstruction:(NSInteger)index
{
    return [self cpu]->debugger.logEntryAbs((int)index);
}

- (void)clearLog
{
    [self cpu]->debugger.clearLog();
}

- (BOOL)isJammed
{
    return [self cpu]->isJammed();
}

- (void)setHex
{
    [self cpu]->debugger.hex = true;
}

- (void)setDec
{
    [self cpu]->debugger.hex = false;
}

- (NSString *)disassembleRecordedInstr:(NSInteger)i length:(NSInteger *)len
{
    const char *str = [self cpu]->debugger.disassembleRecordedInstr((int)i, len);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)disassembleRecordedBytes:(NSInteger)i
{
    const char *str = [self cpu]->debugger.disassembleRecordedBytes((int)i);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)disassembleRecordedFlags:(NSInteger)i
{
    const char *str = [self cpu]->debugger.disassembleRecordedFlags((int)i);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)disassembleRecordedPC:(NSInteger)i
{
    const char *str = [self cpu]->debugger.disassembleRecordedPC((int)i);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)disassembleInstr:(NSInteger)addr length:(NSInteger *)len
{
    const char *str = [self cpu]->debugger.disassembleInstr((u16)addr, len);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)disassembleBytes:(NSInteger)addr
{
    const char *str = [self cpu]->debugger.disassembleBytes((u16)addr);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)disassembleAddr:(NSInteger)addr
{
    const char *str = [self cpu]->debugger.disassembleAddr((u16)addr);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

@end


//
// Memory proxy
//

@implementation MemoryProxy

- (C64Memory *)mem
{
    return (C64Memory *)obj;
}

- (MemInfo)getInfo
{
    return [self mem]->getInfo();
}

- (MemoryType)peekSource:(u16)addr
{
    return [self mem]->getPeekSource(addr);
}

- (MemoryType)pokeTarget:(u16)addr
{
    return [self mem]->getPokeTarget(addr);
}

- (u8)spypeek:(u16)addr source:(MemoryType)source
{
    return [self mem]->spypeek(addr, source);
}

- (u8)spypeek:(u16)addr
{
    return [self mem]->spypeek(addr);
}

- (u8)spypeekIO:(u16)addr
{
    return [self mem]->spypeekIO(addr);
}

- (u8)spypeekColor:(u16)addr
{
    return [self mem]->spypeekColor(addr);
}

- (NSString *)memdump:(NSInteger)addr num:(NSInteger)num hex:(BOOL)hex src:(MemoryType)src
{
    return @([self mem]->memdump((u16)addr, num, hex, src).c_str());
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

- (CIA *)cia
{
    return (CIA *)obj;
}

- (CIAInfo)getInfo
{
    return [self cia]->getInfo();
}

@end

//
// VICII
//

@implementation VICProxy

- (VICII *)vicii
{
    return (VICII *)obj;
}

- (NSInteger)hPixels
{
    return [self vicii]->getCyclesPerLine() * 8;
}

- (NSInteger)vPixels
{
    return [self vicii]->getLinesPerFrame();
}

- (VICIIConfig)getConfig
{
    return [self vicii]->getConfig();
}

- (BOOL)isPAL
{
    return [self vicii]->pal();
}

- (VICIIInfo)getInfo {
    return [self vicii]->getInfo();
}

- (SpriteInfo)getSpriteInfo:(NSInteger)sprite
{
    return [self vicii]->getSpriteInfo((unsigned)sprite);
}

- (u32 *)stableEmuTexture
{
    return [self vicii]->stableEmuTexture();
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

- (u32 *)noise
{
    return [self vicii]->getNoise();
}

@end

//
// Dma Debugger
//

@implementation DmaDebuggerProxy

- (DmaDebugger *)debugger
{
    return (DmaDebugger *)obj;
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

- (Muxer *)bridge
{
    return (Muxer *)obj;
}

- (SIDInfo)getInfo:(NSInteger)nr
{
    return [self bridge]->getInfo((unsigned)nr);
}

- (VoiceInfo)getVoiceInfo:(NSInteger)nr voice:(NSInteger)voice
{
    return [self bridge]->getVoiceInfo((unsigned)nr, (unsigned)voice);
}

- (SIDStats)getStats
{
    return [self bridge]->getStats();
}

- (double)sampleRate
{
    return [self bridge]->getSampleRate();
}

- (void)setSampleRate:(double)rate
{
    [self bridge]->setSampleRate(rate);
}

- (NSInteger)ringbufferSize
{
    return [self bridge]->stream.cap();
}

- (void)ringbufferData:(NSInteger)offset left:(float *)l right:(float *)r
{
    [self bridge]->ringbufferData(offset, l, r);
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
    [self bridge]->rampUpFromZero();
}

- (void)rampDown
{
    [self bridge]->rampDown();
}

@end

//
// IEC bus
//

@implementation IECProxy

- (IEC *)iec
{
    return (IEC *)obj;
}

- (BOOL)transferring
{
    return [self iec]->isTransferring();
}

@end

//
// Keyboard
//

@implementation KeyboardProxy

- (Keyboard *)kb
{
    return (Keyboard *)obj;
}

- (BOOL)keyIsPressed:(NSInteger)nr
{
    return [self kb]->isPressed(nr);
}

- (BOOL)keyIsPressedAtRow:(NSInteger)row col:(NSInteger)col
{
    return [self kb]->isPressed(C64Key(row, col));
}

- (BOOL)controlIsPressed
{
    return [self kb]->ctrlIsPressed();
}

- (BOOL)commodoreIsPressed
{
    return [self kb]->commodoreIsPressed();
}

- (BOOL)leftShiftIsPressed
{
    return [self kb]->leftShiftIsPressed();
}

- (BOOL)rightShiftIsPressed
{
    return [self kb]->rightShiftIsPressed();
}

- (BOOL)shiftLockIsPressed
{
    return [self kb]->shiftLockIsPressed();
}

- (void)pressKey:(NSInteger)nr
{
    [self kb]->press(nr);
}

- (void)pressKeyAtRow:(NSInteger)row col:(NSInteger)col
{
    [self kb]->press(C64Key(row, col));
}

- (void)pressShiftLock
{
    [self kb]->pressShiftLock();
}

- (void)releaseKey:(NSInteger)nr
{
    [self kb]->release(nr);
}

- (void)releaseKeyAtRow:(NSInteger)row col:(NSInteger)col
{
    [self kb]->release(C64Key(row, col));
}

- (void)releaseShiftLock
{
    [self kb]->releaseShiftLock();
}

- (void)releaseAll
{
    [self kb]->releaseAll();
}

- (void)toggleKey:(NSInteger)nr
{
    [self kb]->toggle(nr);
}

- (void)toggleKeyAtRow:(NSInteger)row col:(NSInteger)col
{
    [self kb]->toggle(C64Key(row, col));
}

- (void)toggleShiftLock
{
    [self kb]->toggleShiftLock();
}

- (void)scheduleKeyPress:(NSInteger)nr delay:(NSInteger)delay
{
    [self kb]->scheduleKeyPress(C64Key(nr), delay);
}

- (void)scheduleKeyPressAtRow:(NSInteger)row col:(NSInteger)col delay:(NSInteger)delay
{
    [self kb]->scheduleKeyPress(C64Key(row, col), delay);
}

- (void)scheduleKeyRelease:(NSInteger)nr delay:(NSInteger)delay
{
    [self kb]->scheduleKeyRelease(C64Key(nr), delay);
}

- (void)scheduleKeyReleaseAtRow:(NSInteger)row col:(NSInteger)col delay:(NSInteger)delay
{
    [self kb]->scheduleKeyRelease(C64Key(row, col), delay);
}

- (void)scheduleKeyReleaseAll:(NSInteger)delay
{
    [self kb]->scheduleKeyReleaseAll(delay);
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
        
        ControlPort *port = (ControlPort *)ref;
        obj = ref;
        joystick = [[JoystickProxy alloc] initWith:&port->joystick];
        mouse = [[MouseProxy alloc] initWith:&port->mouse];
    }
    return self;
}

- (ControlPort *)port
{
    return (ControlPort *)obj;
}

@end

//
// Expansion port
//

@implementation ExpansionPortProxy

- (ExpansionPort *)eport
{
    return (ExpansionPort *)obj;
}

- (CartridgeType)cartridgeType
{
    return [self eport]->getCartridgeType();
}

- (BOOL)cartridgeAttached
{
    return [self eport]->getCartridgeAttached();
}
 
- (void)attachCartridge:(CRTFileProxy *)c reset:(BOOL)reset exception:(ExceptionWrapper *)ex
{
    try { [self eport]->attachCartridge((CRTFile *)c->obj, reset); }
    catch (VC64Error &err) { [ex save:err]; }
}

- (void)attachGeoRamCartridge:(NSInteger)capacity
{
    [self eport]->attachGeoRamCartridge(capacity);
}

- (void)attachIsepicCartridge
{
    [self eport]->attachIsepicCartridge();
}

- (void)detachCartridgeAndReset
{
    [self eport]->detachCartridgeAndReset();
}

- (NSInteger)numButtons
{
    return [self eport]->numButtons();
}

- (NSString *)getButtonTitle:(NSInteger)nr
{
    return @([self eport]->getButtonTitle((unsigned)nr).c_str());
}

- (void)pressButton:(NSInteger)nr
{
    [self eport]->pressButton((unsigned)nr);
}

- (void)releaseButton:(NSInteger)nr
{
    [self eport]->releaseButton((unsigned)nr);
}

- (BOOL)hasSwitch
{
    return [self eport]->hasSwitch();
}

- (NSInteger)switchPosition
{
    return [self eport]->getSwitch();
}

- (NSString *)switchDescription:(NSInteger)pos
{
    return @([self eport]->getSwitchDescription(pos).c_str());
}

- (NSString *)currentSwitchDescription
{
    return @([self eport]->getSwitchDescription().c_str());
}

- (BOOL)validSwitchPosition:(NSInteger)pos
{
    return [self eport]->validSwitchPosition(pos);
}

- (BOOL)switchIsNeutral
{
    return [self eport]->switchIsNeutral();
}

- (BOOL)switchIsLeft
{
    return [self eport]->switchIsLeft();
}

- (BOOL)switchIsRight
{
    return [self eport]->switchIsRight();
}

- (void)setSwitchPosition:(NSInteger)pos
{
    [self eport]->setSwitch(pos);
}

- (BOOL)hasLed
{
    return [self eport]->hasLED();
}

- (BOOL)led
{
    return [self eport]->getLED();
}

- (void)setLed:(BOOL)value
{
    [self eport]->setLED(value);
}

- (NSInteger)ramCapacity
{
    return (NSInteger)[self eport]->getRamCapacity();
}

- (BOOL)hasBattery
{
    return [self eport]->hasBattery();
}

- (void)setBattery:(BOOL)value
{
    [self eport]->setBattery(value);
}

@end

//
// Disk
//

@implementation DiskProxy

- (Drive *)drive
{
    return (Drive *)obj;
}

- (Disk *)disk
{
    return [self drive]->disk.get();
}

- (BOOL)writeProtected
{
    return [self disk] ? [self disk]->isWriteProtected() : false;
}

- (void)setWriteProtected:(BOOL)b
{
    if ([self disk]) { [self disk]->setWriteProtection(b); }
}

- (void)toggleWriteProtection
{
    if ([self disk]) { [self disk]->toggleWriteProtection(); }
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
    obj = new DiskAnalyzer(*[disk disk]);
    
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
// VIA
//

@implementation VIAProxy

- (VIA6522 *)via
{
    return (VIA6522 *)obj;
}

@end

//
// Drive
//

@implementation DriveProxy

@synthesize via1, via2;

- (instancetype)initWithVC1541:(Drive *)drive
{    
    if ([self initWith:drive]) {
        via1 = [[VIAProxy alloc] initWith:&drive->via1];
        via2 = [[VIAProxy alloc] initWith:&drive->via2];
        disk = [[DiskProxy alloc] initWith:drive];
    }
    return self;
}

- (Drive *)drive
{
    return (Drive *)obj;
}

- (DiskProxy *)disk
{
    return [self drive]->hasDisk() ? disk : NULL;
}

- (DriveID)id
{
    return [self drive]->getDeviceNr();
}

- (DriveConfig)getConfig
{
    return [self drive]->getConfig();
}

- (VIAProxy *)via:(NSInteger)num {
	switch (num) {
		case 1:
			return [self via1];
		case 2:
			return [self via2];
		default:
			assert(false);
			return NULL;
	}
}

- (BOOL)isConnected
{
    return [self drive]->getConfigItem(OPT_DRV_CONNECT) != 0;
}

- (BOOL)isSwitchedOn
{
    return [self drive]->getConfigItem(OPT_DRV_POWER_SWITCH) != 0;
}

- (BOOL)readMode
{
    return [self drive]->readMode();
}

- (BOOL)writeMode
{
    return [self drive]->writeMode();
}

- (BOOL)redLED
{
    return [self drive]->getRedLED();
}

- (BOOL)hasDisk
{
    return [self drive]->hasDisk();
}

- (BOOL)hasModifiedDisk
{
    return [self drive]->hasModifiedDisk();
}

- (BOOL)hasProtectedDisk
{
    return [self drive]->hasProtectedDisk();
}

- (BOOL)hasUnmodifiedDisk
{
    return [self drive]->hasUnmodifiedDisk();
}

- (BOOL)hasUnprotectedDisk
{
    return [self drive]->hasUnprotectedDisk();
}

- (void)setModificationFlag:(BOOL)value
{
    [self drive]->setModificationFlag(value);
}

/*
- (void)setProtectionFlag:(BOOL)value
{
    [self drive]->setProtectionFlag(value);
}
*/

- (void)markDiskAsModified
{
    [self drive]->markDiskAsModified();
}

- (void)markDiskAsUnmodified
{
    [self drive]->markDiskAsUnmodified();
}

/*
- (void)toggleWriteProtection
{
    [self drive]->toggleWriteProtection();
}
*/

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

- (void)insertNewDisk:(DOSType)fsType name:(NSString *)name
{
    [self drive]->insertNewDisk(fsType, PETName<16>([name UTF8String]));
}

- (void)ejectDisk
{
    [self drive]->ejectDisk();
}

- (Track)track
{
    return [self drive]->getTrack();
}

- (Halftrack)halftrack
{
    return [self drive]->getHalftrack();
}

- (NSInteger)sizeOfHalftrack:(Halftrack)ht
{
    return [self drive]->sizeOfHalftrack(ht);
}

- (NSInteger)sizeOfCurrentHalftrack
{
    return [self drive]->sizeOfCurrentHalftrack();
}

- (NSInteger)offset
{
    return [self drive]->getOffset();
}

- (u8)readBitFromHead
{
    return [self drive]->readBitFromHead();
}

- (BOOL)isRotating
{
    return [self drive]->isRotating();
}

@end


//
// ParCable
//

@implementation ParCableProxy

- (ParCable *)cable
{
    return (ParCable *)obj;
}

@end


//
// Datasette
//

@implementation DatasetteProxy

- (Datasette *)datasette
{
    return (Datasette *)obj;
}

- (BOOL)hasTape
{
    return [self datasette]->hasTape();
}

- (NSInteger)type
{
    return [self datasette]->getType();
}

- (BOOL)motor
{
    return [self datasette]->getMotor();
}

- (BOOL)playKey
{
    return [self datasette]->getPlayKey();
}

- (NSInteger)counter
{
    return [self datasette]->getCounter();
}

- (void)pressPlay
{
    [self datasette]->pressPlay();
}

- (void)pressStop
{
    [self datasette]->pressStop();
}

- (void)rewind
{
    [self datasette]->rewind();
}

- (void)insertTape:(TAPFileProxy *)proxy
{
    [self datasette]->insertTape(*(TAPFile *)proxy->obj);
}

- (void)ejectTape
{
    [self datasette]->ejectTape();
}

@end


//
// Mouse proxy
//

@implementation MouseProxy

- (Mouse *)mouse
{
    return (Mouse *)obj;
}

- (BOOL)detectShakeAbs:(NSPoint)pos
{
    return [self mouse]->detectShakeXY(pos.x, pos.y);
}

- (BOOL)detectShakeRel:(NSPoint)pos
{
    return [self mouse]->detectShakeDxDy(pos.x, pos.y);
}

- (void)setXY:(NSPoint)pos
{
    [self mouse]->setXY((double)pos.x, (double)pos.y);
}

- (void)setDxDy:(NSPoint)pos
{
    [self mouse]->setDxDy((double)pos.x, (double)pos.y);
}

- (void)trigger:(GamePadAction)event
{
    [self mouse]->trigger(event);
}

@end

//
// Joystick proxy
//

@implementation JoystickProxy

- (Joystick *)joystick
{
    return (Joystick *)obj;
}

- (void)trigger:(GamePadAction)event
{
    [self joystick]->trigger(event);
}

@end

//
// Recorder
//

@implementation RecorderProxy

- (Recorder *)recorder
{
    return (Recorder *)obj;
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

- (double)duration
{
    return [self recorder]->getDuration().asSeconds();
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

- (RetroShell *)shell
{
    return (RetroShell *)obj;
}

+ (instancetype)make:(RetroShell *)shell
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

- (void)pressUp
{
	[self shell]->press(RSKEY_UP);
}

- (void)pressDown
{
	[self shell]->press(RSKEY_DOWN);
}

- (void)pressLeft
{
	[self shell]->press(RSKEY_LEFT);
}

- (void)pressRight
{
	[self shell]->press(RSKEY_RIGHT);
}

- (void)pressHome
{
	[self shell]->press(RSKEY_HOME);
}

- (void)pressEnd
{
	[self shell]->press(RSKEY_END);
}

- (void)pressBackspace
{
	[self shell]->press(RSKEY_BACKSPACE);
}

- (void)pressDelete
{
	[self shell]->press(RSKEY_DEL);
}

- (void)pressReturn
{
	[self shell]->press(RSKEY_RETURN);
}

- (void)pressTab
{
	[self shell]->press(RSKEY_TAB);
}

- (void)pressKey:(char)c
{
	[self shell]->press(c);
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

+ (instancetype)makeWithC64:(C64Proxy *)proxy
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

- (void)execute:(C64Proxy *)proxy
{
    C64 *c64 = (C64 *)proxy->obj;
    
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
// C64
//

@implementation C64Proxy

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
@synthesize parCable;
@synthesize port1;
@synthesize port2;
@synthesize recorder;
@synthesize retroShell;
@synthesize sid;
@synthesize vic;
@synthesize watchpoints;

- (instancetype) init
{
    NSLog(@"C64Proxy::init");
    
    if (!(self = [super init])) return self;
    
    C64 *c64 = new C64();
    obj = c64;
    
    breakpoints = [[GuardsProxy alloc] initWith:&c64->cpu.debugger.breakpoints];
    cia1 = [[CIAProxy alloc] initWith:&c64->cia1];
    cia2 = [[CIAProxy alloc] initWith:&c64->cia2];
    cpu = [[CPUProxy alloc] initWith:&c64->cpu];
    datasette = [[DatasetteProxy alloc] initWith:&c64->datasette];
    dmaDebugger = [[DmaDebuggerProxy alloc] initWith:&c64->vic.dmaDebugger];
    drive8 = [[DriveProxy alloc] initWithVC1541:&c64->drive8];
    drive9 = [[DriveProxy alloc] initWithVC1541:&c64->drive9];
    expansionport = [[ExpansionPortProxy alloc] initWith:&c64->expansionport];
    iec = [[IECProxy alloc] initWith:&c64->iec];
    keyboard = [[KeyboardProxy alloc] initWith:&c64->keyboard];
    mem = [[MemoryProxy alloc] initWith:&c64->mem];
    parCable = [[ParCableProxy alloc] initWith:&c64->parCable];
    port1 = [[ControlPortProxy alloc] initWith:&c64->port1];
    port2 = [[ControlPortProxy alloc] initWith:&c64->port2];
    recorder = [[RecorderProxy alloc] initWith:&c64->recorder];
    retroShell = [[RetroShellProxy alloc] initWith:&c64->retroShell];
    sid = [[SIDProxy alloc] initWith:&c64->muxer];
    vic = [[VICProxy alloc] initWith:&c64->vic];
    watchpoints = [[GuardsProxy alloc] initWith:&c64->cpu.debugger.watchpoints];

    return self;
}

- (C64 *)c64
{
    return (C64 *)obj;
}

+ (DefaultsProxy *) defaults
{
    return [[DefaultsProxy alloc] initWith:&C64::defaults];
}

- (void)dealloc
{
    NSLog(@"C64 dealloc");
}

- (void)kill
{
    NSLog(@"kill");
    
    assert([self c64] != NULL);
    delete [self c64];
    obj = NULL;
}

- (BOOL)releaseBuild
{
    return releaseBuild;
}

- (BOOL)warpMode
{
    return [self c64]->inWarpMode();
}

- (void)setWarpMode:(BOOL)enable
{
    enable ? [self c64]->warpOn() : [self c64]->warpOff();
}

- (BOOL)debugMode
{
    return [self c64]->inDebugMode();
}

- (void)setDebugMode:(BOOL)enable
{
    enable ? [self c64]->debugOn() : [self c64]->debugOff();
}

- (NSInteger)cpuLoad
{
    double load = [self c64]->getCpuLoad();
    return (NSInteger)(100 * load);
}

- (InspectionTarget)inspectionTarget
{
    return [self c64]->getInspectionTarget();
}

- (void)setInspectionTarget:(InspectionTarget)target
{
    [self c64]->setInspectionTarget(target);
}

- (void) removeInspectionTarget
{
    [self c64]->removeInspectionTarget();
}

- (void)inspect
{
    [self c64]->inspect();
}

- (void)hardReset
{
    [self c64]->hardReset();
}

- (void)softReset
{
    [self c64]->softReset();
}

- (void)isReady:(ExceptionWrapper *)ex
{
    try { [self c64]->isReady(); }
    catch (VC64Error &error) { [ex save:error]; }
}

- (void)powerOn:(ExceptionWrapper *)ex
{
    try { [self c64]->powerOn(); }
    catch (VC64Error &error) { [ex save:error]; }
}

- (void)powerOff
{
    [self c64]->powerOff();
}

- (BOOL)poweredOn
{
    return [self c64]->isPoweredOn();
}

- (BOOL)poweredOff
{
    return [self c64]->isPoweredOff();
}

- (BOOL)running
{
    return [self c64]->isRunning();
}

- (BOOL)paused
{
    return [self c64]->isPaused();
}

- (void)run:(ExceptionWrapper *)e
{
    try { [self c64]->run(); }
    catch (VC64Error &error) { [e save:error]; }
}

- (void)pause
{
    [self c64]->pause();
}

- (void)halt
{
    [self c64]->halt();
}

- (void)suspend
{
    [self c64]->suspend();
}

- (void)resume
{
    [self c64]->resume();
}

- (void)continueScript
{
    [self c64]->retroShell.continueScript();
}

- (void)requestAutoSnapshot
{
    [self c64]->requestAutoSnapshot();
}

- (void)requestUserSnapshot
{
    [self c64]->requestUserSnapshot();
}

- (SnapshotProxy *)latestAutoSnapshot
{
    Snapshot *snapshot = [self c64]->latestAutoSnapshot();
    return [SnapshotProxy make:snapshot];
}

- (SnapshotProxy *)latestUserSnapshot
{
    Snapshot *snapshot = [self c64]->latestUserSnapshot();
    return [SnapshotProxy make:snapshot];
}

- (NSInteger)getConfig:(Option)opt
{
    return [self c64]->getConfigItem(opt);
}

- (NSInteger)getConfig:(Option)opt id:(NSInteger)id
{
    return [self c64]->getConfigItem(opt, id);
}

- (NSInteger)getConfig:(Option)opt drive:(DriveID)id
{
    return [self c64]->getConfigItem(opt, (long)id);
}

- (BOOL)configure:(Option)opt value:(NSInteger)val
{
    try {
        [self c64]->configure(opt, val);
        return true;
    } catch (VC64Error &exception) {
        return false;
    }
}

- (BOOL)configure:(Option)opt enable:(BOOL)val
{
    try {
        [self c64]->configure(opt, val ? 1 : 0);
        return true;
    } catch (VC64Error &exception) {
        return false;
    }
}

- (BOOL)configure:(Option)opt id:(NSInteger)id value:(NSInteger)val
{
    try {
        [self c64]->configure(opt, id, val);
        return true;
    } catch (VC64Error &exception) {
        return false;
    }
}

- (BOOL)configure:(Option)opt id:(NSInteger)id enable:(BOOL)val
{
    try {
        [self c64]->configure(opt, id, val ? 1 : 0);
        return true;
    } catch (VC64Error &exception) {
        return false;
    }
}

- (BOOL)configure:(Option)opt drive:(DriveID)id value:(NSInteger)val
{
    try {
        [self c64]->configure(opt, (long)id, val);
        return true;
    } catch (VC64Error &exception) {
        return false;
    }
}

- (BOOL)configure:(Option)opt drive:(DriveID)id enable:(BOOL)val
{
    try {
        [self c64]->configure(opt, (long)id, val ? 1 : 0);
        return true;
    } catch (VC64Error &exception) {
        return false;
    }
}

- (void)configure:(C64Model)model
{
    [self c64]->configure(model);
}

- (void)setListener:(const void *)sender function:(Callback *)func
{
    [self c64]->msgQueue.setListener(sender, func);
}

- (void)stopAndGo
{
    [self c64]->stopAndGo();
}

- (void)stepInto
{
    [self c64]->stepInto();
}

- (void)stepOver
{
    [self c64]->stepOver();
}

/*
- (NSInteger)breakpointPC
{
    return [self c64]->cpu.debugger.breakpointPC;
}

- (NSInteger)watchpointPC
{
    return [self c64]->cpu.debugger.watchpointPC;
}
*/

- (BOOL) hasRom:(RomType)type
{
    return [self c64]->hasRom(type);
}

- (BOOL)hasMega65Rom:(RomType)type
{
    return [self c64]->hasMega65Rom(type);
}

- (BOOL) isRom:(RomType)type url:(NSURL *)url
{
    return RomFile::isRomFile(type, [url fileSystemRepresentation]);
}

- (void) loadRom:(NSURL *)url exception:(ExceptionWrapper *)e
{
    try { [self c64]->loadRom(string([url fileSystemRepresentation])); }
    catch (VC64Error &error) { [e save:error]; }
}

- (void) loadRom:(RomFileProxy *)proxy
{
    [self c64]->loadRom(*(RomFile *)proxy->obj);
}

- (void) saveRom:(RomType)type url:(NSURL *)url exception:(ExceptionWrapper *)e
{
    try { [self c64]->saveRom(type, string([url fileSystemRepresentation])); }
    catch (VC64Error &error) { [e save:error]; }
}

- (void) deleteRom:(RomType)type
{
    [self c64]->deleteRom(type);
}

- (RomIdentifier)romIdentifier:(RomType)type
{
    return [self c64]->romIdentifier(type);
}

- (NSString *)romTitle:(RomType)type
{
    return @([self c64]->romTitle(type).c_str());
}

- (NSString *)basicRomTitle
{
    return @([self c64]->romTitle(ROM_TYPE_BASIC).c_str());
}

- (NSString *)charRomTitle
{
    return @([self c64]->romTitle(ROM_TYPE_CHAR).c_str());
}

- (NSString *)kernalRomTitle
{
    return @([self c64]->romTitle(ROM_TYPE_KERNAL).c_str());
}

- (NSString *)vc1541RomTitle
{
    return @([self c64]->romTitle(ROM_TYPE_VC1541).c_str());
}

- (NSString *)romSubTitle:(RomType)type
{
    return @([self c64]->romSubTitle(type).c_str());
}

- (NSString *)basicRomSubTitle
{
    return @([self c64]->romSubTitle(ROM_TYPE_BASIC).c_str());
}

- (NSString *)charRomSubTitle
{
    return @([self c64]->romSubTitle(ROM_TYPE_CHAR).c_str());
}

- (NSString *)kernalRomSubTitle
{
    return @([self c64]->romSubTitle(ROM_TYPE_KERNAL).c_str());
}

- (NSString *)vc1541RomSubTitle
{
    return @([self c64]->romSubTitle(ROM_TYPE_VC1541).c_str());
}

- (NSString *)romRevision:(RomType)type
{
    return @([self c64]->romRevision(type).c_str());
}

- (NSString *)basicRomRevision
{
    return @([self c64]->romRevision(ROM_TYPE_BASIC).c_str());
}

- (NSString *)charRomRevision
{
    return @([self c64]->romRevision(ROM_TYPE_CHAR).c_str());
}

- (NSString *)kernalRomRevision
{
    return @([self c64]->romRevision(ROM_TYPE_KERNAL).c_str());
}

- (NSString *)vc1541RomRevision
{
    return @([self c64]->romRevision(ROM_TYPE_VC1541).c_str());
}

- (BOOL)isCommodoreRom:(RomIdentifier)rev
{
    return RomFile::isCommodoreRom(rev);
}

- (BOOL)isPatchedRom:(RomIdentifier)rev
{
    return RomFile::isPatchedRom(rev);
}

- (void)flash:(AnyFileProxy *)proxy exception:(ExceptionWrapper *)ex
{
    try { [self c64]->flash(*(AnyFile *)proxy->obj); }
    catch (VC64Error &error) { [ex save:error]; }
}

- (void)flash:(FileSystemProxy *)proxy item:(NSInteger)nr exception:(ExceptionWrapper *)ex
{
    try { [self c64]->flash(*(FileSystem *)proxy->obj, (unsigned)nr); }
    catch (VC64Error &error) { [ex save:error]; }
}

@end
