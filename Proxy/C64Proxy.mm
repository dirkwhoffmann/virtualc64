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
#import "VirtualC64-Swift.h"

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
// HardwareComponent proxy
//

@implementation HardwareComponentProxy

-(HardwareComponent *)hwc
{
    return (HardwareComponent *)obj;
}

-(void)dump
{
    [self hwc]->dump();
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

- (CPUInfo)getInfo
{
    return [self cpu]->getInfo();
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

- (bool)isJammed
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

- (i64)cycle
{
    return (i64)[self cpu]->cycle;
}

- (u16)pc
{
    return [self cpu]->getPC0();
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
    const char *str = [self cpu]->debugger.disassembleInstr(addr, len);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)disassembleBytes:(NSInteger)addr
{
    const char *str = [self cpu]->debugger.disassembleBytes(addr);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)disassembleAddr:(NSInteger)addr
{
    const char *str = [self cpu]->debugger.disassembleAddr(addr);
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

/*
- (void)poke:(u16)addr value:(u8)value target:(MemoryType)target
{
    [self mem]->suspend();
    [self mem]->poke(addr, value, target);
    [self mem]->resume();
}

- (void)poke:(u16)addr value:(u8)value
{
    [self mem]->suspend();
    [self mem]->poke(addr, value);
    [self mem]->resume();
}

- (void)pokeIO:(u16)addr value:(u8)value
{
    [self mem]->suspend();
    [self mem]->pokeIO(addr, value);
    [self mem]->resume();
}
*/

- (NSString *)memdump:(NSInteger)addr num:(NSInteger)num hex:(BOOL)hex src:(MemoryType)src
{
    const char *str = [self mem]->memdump(addr, num, hex, src);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *)txtdump:(NSInteger)addr num:(NSInteger)num src:(MemoryType)src
{
    const char *str = [self mem]->txtdump(addr, num, src);
    return str ? [NSString stringWithUTF8String:str] : NULL;
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

- (BOOL)isPAL
{
    return [self vicii]->isPAL();
}

- (VICIIInfo)getInfo {
    return [self vicii]->getInfo();
}

- (SpriteInfo)getSpriteInfo:(NSInteger)sprite
{
    return [self vicii]->getSpriteInfo((unsigned)sprite);
}

- (void *)stableEmuTexture
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

- (double)brightness
{
    return [self vicii]->getBrightness();
}
- (void)setBrightness:(double)value
{
    [self vicii]->setBrightness(value);
}
- (double)contrast
{
    return [self vicii]->getContrast();
}
- (void)setContrast:(double)value
{
    [self vicii]->setContrast(value);
}
- (double)saturation
{
    return [self vicii]->getSaturation();
}
- (void)setSaturation:(double)value
{
    [self vicii]->setSaturation(value);
}
- (u32 *)noise
{
    return [self vicii]->getNoise();
}

@end

//
// SID
//

@implementation SIDProxy

- (SIDBridge *)bridge
{
    return (SIDBridge *)obj;
}

- (SIDInfo)getInfo:(NSInteger)nr
{
    return [self bridge]->getInfo((unsigned)nr);
}

- (VoiceInfo)getVoiceInfo:(NSInteger)nr voice:(NSInteger)voice
{
    return [self bridge]->getVoiceInfo((unsigned)nr, (unsigned)voice);
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

- (double)fillLevel
{
    return [self bridge]->stream.fillLevel();
}

- (NSInteger)bufferUnderflows
{
    return [self bridge]->bufferUnderflows;
}

- (NSInteger)bufferOverflows
{
    return [self bridge]->bufferOverflows;
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
    return [self kb]->isPressed(row, col);
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
    [self kb]->pressRowCol(row, col);
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
    [self kb]->releaseRowCol(row, col);
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
    [self kb]->toggle(row, col);
}

- (void)toggleShiftLock
{
    [self kb]->toggleShiftLock();
}

- (void)scheduleKeyPress:(NSInteger)nr delay:(NSInteger)delay
{
    [self kb]->scheduleKeyPress(nr, delay);
}

- (void)scheduleKeyPressAtRow:(NSInteger)row col:(NSInteger)col delay:(NSInteger)delay
{
    [self kb]->scheduleKeyPress(row, col, delay);
}

- (void)scheduleKeyRelease:(NSInteger)nr delay:(NSInteger)delay
{
    [self kb]->scheduleKeyRelease(nr, delay);
}

- (void)scheduleKeyReleaseAtRow:(NSInteger)row col:(NSInteger)col delay:(NSInteger)delay
{
    [self kb]->scheduleKeyRelease(row, col, delay);
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

- (BOOL)attachCartridge:(CRTFileProxy *)c reset:(BOOL)reset
{
    return [self eport]->attachCartridge((CRTFile *)c->obj, reset);
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
    const char *str = [self eport]->getButtonTitle((unsigned)nr);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (void) pressButton:(NSInteger)nr
{
    [self eport]->pressButton((unsigned)nr);
}

- (void) releaseButton:(NSInteger)nr
{
    [self eport]->releaseButton((unsigned)nr);
}

- (BOOL) hasSwitch
{
    return [self eport]->hasSwitch();
}

- (NSInteger) switchPosition
{
    return [self eport]->getSwitch();
}

- (NSString *) switchDescription:(NSInteger)pos
{
    const char *str = [self eport]->getSwitchDescription(pos);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *) currentSwitchDescription
{
    const char *str = [self eport]->getSwitchDescription();
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (BOOL) validSwitchPosition:(NSInteger)pos
{
    return [self eport]->validSwitchPosition(pos);
}

- (BOOL) switchIsNeutral
{
    return [self eport]->switchIsNeutral();
}

- (BOOL) switchIsLeft
{
    return [self eport]->switchIsLeft();
}

- (BOOL) switchIsRight
{
    return [self eport]->switchIsRight();
}

- (void) setSwitchPosition:(NSInteger)pos
{
    [self eport]->setSwitch(pos);
}

- (BOOL) hasLed
{
    return [self eport]->hasLED();
}

- (BOOL) led
{
    return [self eport]->getLED();
}

- (void) setLed:(BOOL)value
{
    [self eport]->setLED(value);
}

- (NSInteger) ramCapacity
{
    return (NSInteger)[self eport]->getRamCapacity();
}

- (BOOL) hasBattery
{
    return [self eport]->hasBattery();
}

- (void) setBattery:(BOOL)value
{
    [self eport]->setBattery(value);
}

@end

//
// Disk
//

@implementation DiskProxy

- (Disk *)disk
{
    return (Disk *)obj;
}

- (BOOL)writeProtected
{
    return [self disk]->isWriteProtected();
}

- (void)setWriteProtected:(BOOL)b
{
    [self disk]->setWriteProtection(b);
}

- (void)toggleWriteProtection
{
    [self disk]->toggleWriteProtection();
}

- (NSInteger)nonemptyHalftracks
{
    return [self disk]->nonemptyHalftracks();
}

- (void)analyzeTrack:(Track)t
{
    [self disk]->analyzeTrack(t);
}

- (void)analyzeHalftrack:(Halftrack)ht
{
    [self disk]->analyzeHalftrack(ht);
}

- (NSInteger)numErrors
{
    return [self disk]->numErrors();
}

- (NSString *)errorMessage:(NSInteger)nr
{
    std::string s = [self disk]->errorMessage((unsigned)nr);
    return [NSString stringWithUTF8String:s.c_str()];
}

- (NSInteger)firstErroneousBit:(NSInteger)nr
{
    return [self disk]->firstErroneousBit((unsigned)nr);
}

- (NSInteger)lastErroneousBit:(NSInteger)nr
{
    return [self disk]->lastErroneousBit((unsigned)nr);
}

- (SectorInfo)sectorInfo:(Sector)s
{
    return [self disk]->sectorLayout(s);
}

- (const char *)trackBitsAsString
{
    return [self disk]->trackBitsAsString();
}

- (const char *)diskNameAsString
{
    return [self disk]->diskNameAsString();
}

- (const char *)sectorHeaderBytesAsString:(Sector)nr hex:(BOOL)hex
{
    return [self disk]->sectorHeaderBytesAsString(nr, hex);
}

- (const char *)sectorDataBytesAsString:(Sector)nr hex:(BOOL)hex
{
    return [self disk]->sectorDataBytesAsString(nr, hex);
}

@end

//
// FSDevice
//

@implementation FSDeviceProxy

+ (instancetype)make:(FSDevice *)fs
{
    return fs ? [[self alloc] initWith: fs] : nil;
}

+ (instancetype)makeWithDisk:(DiskProxy *)proxy error:(ErrorCode *)err;
{
    FSDevice *fs = FSDevice::makeWithDisk(*(Disk *)proxy->obj, err);
    return [self make:fs];
}

+ (instancetype)makeWithCollection:(AnyCollectionProxy *)proxy error:(ErrorCode *)err;
{
    FSDevice *volume = FSDevice::makeWithCollection(*(AnyCollection *)proxy->obj, err);
    return [self make:volume];
}

+ (instancetype)makeWithD64:(D64FileProxy *)proxy error:(ErrorCode *)err;
{
    FSDevice *fs = FSDevice::makeWithD64(*(D64File *)proxy->obj, err);
    return [self make:fs];
}

- (FSDevice *)fs
{
    return (FSDevice *)obj;
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

- (NSInteger)numFreeBlocks
{
    return [self fs]->numFreeBlocks();
}

- (NSInteger)numUsedBlocks
{
    return [self fs]->numUsedBlocks();
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

- (FSBlockType) blockType:(NSInteger)blockNr
{
    return [self fs]->blockType((u32)blockNr);
}

- (FSUsage) itemType:(NSInteger)blockNr pos:(NSInteger)pos
{
    return [self fs]->usage((u32)blockNr, (u32)pos);
}

- (FSErrorReport) check:(BOOL)strict
{
    return [self fs]->check(strict);
}

- (ErrorCode) check:(NSInteger)blockNr
                pos:(NSInteger)pos
           expected:(unsigned char *)exp
             strict:(BOOL)strict
{
    return [self fs]->check((u32)blockNr, (u32)pos, exp, strict);
}

- (BOOL) isCorrupted:(NSInteger)blockNr
{
    return [self fs]->isCorrupted((u32)blockNr);
}

- (NSInteger) getCorrupted:(NSInteger)blockNr
{
    return [self fs]->getCorrupted((u32)blockNr);
}

- (NSInteger) nextCorrupted:(NSInteger)blockNr
{
    return [self fs]->nextCorrupted((u32)blockNr);
}

- (NSInteger) prevCorrupted:(NSInteger)blockNr
{
    return [self fs]->prevCorrupted((u32)blockNr);
}

- (void) printDirectory
{
    return [self fs]->printDirectory();
}

- (NSInteger) readByte:(NSInteger)block offset:(NSInteger)offset
{
    return [self fs]->readByte((u32)block, (u32)offset);
}

- (BOOL) exportDirectory:(NSString *)path error:(ErrorCode *)err
{
    return [self fs]->exportDirectory([path fileSystemRepresentation], err);
}

- (void) dump
{
    [self fs]->dump();
}

- (void) info
{
    [self fs]->info();
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
// VC1541
//

@implementation DriveProxy

@synthesize via1, via2, disk;

- (instancetype)initWithVC1541:(Drive *)drive
{
    if ([self initWith:drive]) {
        via1 = [[VIAProxy alloc] initWith:&drive->via1];
        via2 = [[VIAProxy alloc] initWith:&drive->via2];
        disk = [[DiskProxy alloc] initWith:&drive->disk];
    }
    return self;
}

- (Drive *)drive
{
    return (Drive *)obj;
}

- (DriveID)id
{
    return [self drive]->getDeviceNr();
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

- (DriveConfig)getConfig
{
    return [self drive]->getConfig();
}

- (void)dump
{
    [self drive]->dump();
}

- (BOOL)isConnected
{
    return [self drive]->getConfigItem(OPT_DRIVE_CONNECT) != 0;
}

- (BOOL)isSwitchedOn
{
    return [self drive]->getConfigItem(OPT_DRIVE_POWER_SWITCH) != 0;
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

- (void)setModifiedDisk:(BOOL)b
{
    [self drive]->setModifiedDisk(b);
}

- (void)insertG64:(G64FileProxy *)proxy
{
    [self drive]->insertG64((G64File *)proxy->obj);
}

- (void)insertFileSystem:(FSDeviceProxy *)proxy
{
    [self drive]->insertFileSystem((FSDevice *)proxy->obj);
}

/*
- (void)insertCollection:(AnyCollectionProxy *)proxy
{
    [self drive]->insertDisk((AnyCollection *)proxy->obj);
}
*/

- (void) insertNewDisk:(DOSType)fsType
{
    [self drive]->insertNewDisk(fsType);
}

- (void)ejectDisk
{
    [self drive]->ejectDisk();
}

- (BOOL)writeProtected
{
    return [self drive]->disk.isWriteProtected();
}

- (void)setWriteProtection:(BOOL)b
{
    [self drive]->disk.setWriteProtection(b);
}

- (BOOL)hasWriteProtectedDisk
{
    return [self drive]->hasWriteProtectedDisk();
}

- (Track)track
{
    return [self drive]->getTrack();
}

- (Halftrack)halftrack
{
    return [self drive]->getHalftrack();
}

- (u16)sizeOfHalftrack:(Halftrack)ht
{
    return [self drive]->sizeOfHalftrack(ht);
}

- (u16)sizeOfCurrentHalftrack
{
    return [self drive]->sizeOfCurrentHalftrack();
}

- (u16)offset
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

- (BOOL)insertTape:(TAPFileProxy *)proxy
{
    return [self datasette]->insertTape((TAPFile *)proxy->obj);
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

- (MouseModel)model
{
    return [self mouse]->getModel();
}

- (void)setModel:(MouseModel)model
{
    [self mouse]->setModel(model);
}

- (void)setXY:(NSPoint)pos
{
    [self mouse]->setXY((i64)pos.x, (i64)pos.y);
}

- (void)setLeftButton:(BOOL)pressed
{
    [self mouse]->setLeftButton(pressed);
}

- (void)setRightButton:(BOOL)pressed
{
    [self mouse]->setRightButton(pressed);
}

- (void) trigger:(GamePadAction)event
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

- (void) trigger:(GamePadAction)event
{
    [self joystick]->trigger(event);
}

- (BOOL) autofire
{
    return [self joystick]->getAutofire();
}

- (void) setAutofire:(BOOL)value
{
    return [self joystick]->setAutofire(value);
}

- (NSInteger) autofireBullets
{
    return (NSInteger)[self joystick]->getAutofireBullets();
}

- (void) setAutofireBullets:(NSInteger)value
{
    [self joystick]->setAutofireBullets((int)value);
}

- (float) autofireFrequency
{
    return [self joystick]->getAutofireFrequency();
}

- (void) setAutofireFrequency:(float)value
{
    [self joystick]->setAutofireFrequency(value);
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

-(NSInteger)cposRel
{
    return [self shell]->cposRel();
}

-(NSString *)getText
{
    const char *str = [self shell]->text();
    return str ? [NSString stringWithUTF8String:str] : nullptr;
}

- (void)pressUp
{
    [self shell]->pressUp();
}

- (void)pressDown
{
    [self shell]->pressDown();
}

- (void)pressLeft
{
    [self shell]->pressLeft();
}

- (void)pressRight
{
    [self shell]->pressRight();
}

- (void)pressHome
{
    [self shell]->pressHome();
}

- (void)pressEnd
{
    [self shell]->pressEnd();
}

- (void)pressBackspace
{
    [self shell]->pressBackspace();
}

- (void)pressDelete
{
    [self shell]->pressDelete();
}

- (void)pressReturn
{
    [self shell]->pressReturn();
}

- (void)pressTab
{
    [self shell]->pressTab();
}

- (void)pressKey:(char)c
{
    [self shell]->pressKey(c);
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

- (void)dealloc
{
    // NSLog(@"dealloc");
    delete (AnyFile *)obj;
}

- (AnyFile *)file
{
    return (AnyFile *)obj;
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
    [self file]->path = [path UTF8String];
}

- (NSInteger)writeToFile:(NSString *)path error:(ErrorCode *)err
{
    return [self file]->writeToFile([path fileSystemRepresentation], err);
}

@end

//
// Snapshot
//

@implementation SnapshotProxy

+ (instancetype)make:(Snapshot *)snapshot
{
    return snapshot ? [[self alloc] initWith:snapshot] : nil;
}

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err
{
    return [self make: AnyFile::make <Snapshot> ([path fileSystemRepresentation], err)];
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err
{
    return [self make: AnyFile::make <Snapshot> ((const u8 *)buf, len, err)];
}

+ (instancetype)makeWithC64:(C64Proxy *)c64proxy
{
    C64 *c64 = (C64 *)c64proxy->obj;
    c64->suspend();
    Snapshot *snapshot = Snapshot::makeWithC64(c64);
    c64->resume();
    return [self make:snapshot];
}

- (Snapshot *)snapshot
{
    return (Snapshot *)obj;
}

- (NSImage *)previewImage
{
    // Return cached image (if any)
    if (preview) { return preview; }
    
    // Create preview image
    NSInteger width = [self snapshot]->imageWidth();
    NSInteger height = [self snapshot]->imageHeight();
    unsigned char *data = [self snapshot]->imageData();
    
    
    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc]
                             initWithBitmapDataPlanes: &data
                             pixelsWide:width
                             pixelsHigh:height
                             bitsPerSample:8
                             samplesPerPixel:4
                             hasAlpha:true
                             isPlanar:false
                             colorSpaceName:NSCalibratedRGBColorSpace
                             bytesPerRow:4*width
                             bitsPerPixel:32];
    
    preview = [[NSImage alloc] initWithSize:[rep size]];
    [preview addRepresentation:rep];
    
    return preview;
}

- (time_t)timeStamp
{
    return [self snapshot]->timeStamp();
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

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err
{
    return [self make: AnyFile::make <RomFile> ([path fileSystemRepresentation], err)];
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err
{
    return [self make: AnyFile::make <RomFile> ((const u8 *)buf, len, err)];
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

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err
{
    return [self make: AnyFile::make <CRTFile> ([path fileSystemRepresentation], err)];
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err
{
    return [self make: AnyFile::make <CRTFile> ((const u8 *)buf, len, err)];
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

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err
{
    return [self make: AnyFile::make <TAPFile> ([path fileSystemRepresentation], err)];
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err
{
    return [self make: AnyFile::make <TAPFile> ((const u8 *)buf, len, err)];
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

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err
{
    return [self make: AnyFile::make <T64File> ([path fileSystemRepresentation], err)];
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err
{
    return [self make: AnyFile::make <T64File> ((const u8 *)buf, len, err)];
}

+ (instancetype)makeWithFileSystem:(FSDeviceProxy *)proxy error:(ErrorCode *)err;
{
    return [self make: T64File::makeWithFileSystem(*(FSDevice *)proxy->obj)];
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

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err
{
    return [self make: AnyFile::make <PRGFile> ([path fileSystemRepresentation], err)];
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err
{
    return [self make: AnyFile::make <PRGFile> ((const u8 *)buf, len, err)];
}

+ (instancetype)makeWithFileSystem:(FSDeviceProxy *)proxy error:(ErrorCode *)err
{
    return [self make: AnyFile::make <PRGFile> (*(FSDevice *)proxy->obj, err)];
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

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err
{
    return [self make: AnyFile::make <P00File> ([path fileSystemRepresentation], err)];
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err
{
    return [self make: AnyFile::make <P00File> ((const u8 *)buf, len, err)];
}

+ (instancetype)makeWithFileSystem:(FSDeviceProxy *)proxy error:(ErrorCode *)err
{
    return [self make: AnyFile::make <P00File> (*(FSDevice *)proxy->obj, err)];
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

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err
{
    return [self make: AnyFile::make <D64File> ([path fileSystemRepresentation], err)];
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err
{
    return [self make: AnyFile::make <D64File> ((const u8 *)buf, len, err)];
}

+ (instancetype)makeWithFileSystem:(FSDeviceProxy *)proxy error:(ErrorCode *)err
{
    return [self make: AnyFile::make <D64File> (*(FSDevice *)proxy->obj, err)];
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

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err
{
    return [self make: AnyFile::make <G64File> ([path fileSystemRepresentation], err)];
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err
{
    return [self make: AnyFile::make <G64File> ((const u8 *)buf, len, err)];
}

+ (instancetype)makeWithDisk:(DiskProxy *)proxy error:(ErrorCode *)err
{
    // Disk *disk = (Disk *)proxy->obj;
    return [self make: AnyFile::make <G64File> (*(Disk *)proxy->obj, err)];
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

+ (instancetype)makeWithFolder:(NSString *)path error:(ErrorCode *)err
{
    std::string str = string([path fileSystemRepresentation]);
    return [self make: Folder::makeWithFolder(str, err)];
}

- (Folder *)folder
{
    return (Folder *)obj;
}

- (FSDeviceProxy *)fileSystem
{
    return [FSDeviceProxy make:[self folder]->getFS()];
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
@synthesize drive8;
@synthesize drive9;
@synthesize expansionport;
@synthesize iec;
@synthesize keyboard;
@synthesize mem;
@synthesize port1;
@synthesize port2;
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
    drive8 = [[DriveProxy alloc] initWithVC1541:&c64->drive8];
    drive9 = [[DriveProxy alloc] initWithVC1541:&c64->drive9];
    expansionport = [[ExpansionPortProxy alloc] initWith:&c64->expansionport];
    iec = [[IECProxy alloc] initWith:&c64->iec];
    keyboard = [[KeyboardProxy alloc] initWith:&c64->keyboard];
    mem = [[MemoryProxy alloc] initWith:&c64->mem];
    port1 = [[ControlPortProxy alloc] initWith:&c64->port1];
    port2 = [[ControlPortProxy alloc] initWith:&c64->port2];
    retroShell = [[RetroShellProxy alloc] initWith:&c64->retroShell];
    sid = [[SIDProxy alloc] initWith:&c64->sid];
    vic = [[VICProxy alloc] initWith:&c64->vic];
    watchpoints = [[GuardsProxy alloc] initWith:&c64->cpu.debugger.watchpoints];

    return self;
}

- (C64 *)c64
{
    return (C64 *)obj;
}

- (DriveProxy *)drive:(DriveID)id
{
    switch (id) {
        case DRIVE8:  return drive8;
        case DRIVE9:  return drive9;
        default:      return NULL;
    }
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

- (BOOL)warp
{
    return [self c64]->inWarpMode();
}

- (void)setWarp:(BOOL)enable;
{
    [self c64]->setWarp(enable);
}

- (BOOL)debugMode
{
    return [self c64]->inDebugMode();
}

- (void)setDebugMode:(BOOL)enable
{
    [self c64]->setDebug(enable);
}

- (InspectionTarget)inspectionTarget
{
    return [self c64]->getInspectionTarget();
}

- (void)setInspectionTarget:(InspectionTarget)target
{
    [self c64]->setInspectionTarget(target);
}

- (BOOL)isReady:(ErrorCode *)err
{
    return [self c64]->isReady(err);
}

- (BOOL)isReady
{
    return [self c64]->isReady();
}

- (void)powerOn
{
    [self c64]->powerOn();
}

- (void)powerOff
{
    [self c64]->powerOff();
}

- (void)inspect
{
    [self c64]->inspect();
}

- (void)reset
{
    [self c64]->reset();
}

- (void)shutdown
{
    [self c64]->shutdown();
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

- (void)run
{
    [self c64]->run();
}

- (void)pause
{
    [self c64]->pause();
}

- (void)suspend
{
    [self c64]->suspend();
}

- (void)resume
{
    [self c64]->resume();
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

- (void)loadFromSnapshot:(SnapshotProxy *)proxy
{
    [self c64]->loadFromSnapshot((Snapshot *)proxy->obj);
}

/*
- (C64Config)config
{
    return [self c64]->getConfig();
}
*/

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
        return [self c64]->configure(opt, val);
    } catch (ConfigError &exception) {
        return false;
    }
}

- (BOOL)configure:(Option)opt enable:(BOOL)val
{
    try {
        return [self c64]->configure(opt, val ? 1 : 0);
    } catch (ConfigError &exception) {
        return false;
    }
}

- (BOOL)configure:(Option)opt id:(NSInteger)id value:(NSInteger)val
{
    try {
        return [self c64]->configure(opt, id, val);
    } catch (ConfigError &exception) {
        return false;
    }
}

- (BOOL)configure:(Option)opt id:(NSInteger)id enable:(BOOL)val
{
    try {
        return [self c64]->configure(opt, id, val ? 1 : 0);
    } catch (ConfigError &exception) {
        return false;
    }
}

- (BOOL)configure:(Option)opt drive:(DriveID)id value:(NSInteger)val
{
    try {
        return [self c64]->configure(opt, (long)id, val);
    } catch (ConfigError &exception) {
        return false;
    }
}

- (BOOL)configure:(Option)opt drive:(DriveID)id enable:(BOOL)val
{
    try {
        return [self c64]->configure(opt, (long)id, val ? 1 : 0);
    } catch (ConfigError &exception) {
        return false;
    }
}

- (void)configure:(C64Model)model
{
    [self c64]->configure(model);
}

- (Message)message
{
    return [self c64]->getMessage();
}

- (void)addListener:(const void *)sender function:(Callback *)func
{
    [self c64]->addListener(sender, func);
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
    return RomFile::isRomFile(type, [[url path] UTF8String]);
}

- (void) loadRom:(NSURL *)url error:(ErrorCode *)ec
{
    try {
        [self c64]->loadRom(string([[url path] UTF8String]));
        *ec = ERROR_OK;
    } catch (VC64Error &exception) {
        *ec = exception.data;
    }
}

- (void) loadRom:(RomFileProxy *)proxy
{
    [self c64]->loadRom((RomFile *)proxy->obj);
}

- (void) saveRom:(RomType)type url:(NSURL *)url error:(ErrorCode *)ec
{
    try {
        [self c64]->saveRom(type, [[url path] UTF8String]);
        *ec = ERROR_OK;
    } catch (VC64Error &exception) {
        *ec = exception.data;
    }
}

- (void) deleteRom:(RomType)type
{
    [self c64]->deleteRom(type);
}

- (RomIdentifier) romIdentifier:(RomType)type
{
    return [self c64]->romIdentifier(type);
}

- (NSString *) romTitle:(RomType)type
{
    const char *str = [self c64]->romTitle(type);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)basicRomTitle
{
    const char *str = [self c64]->romTitle(ROM_TYPE_BASIC);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)charRomTitle
{
    const char *str = [self c64]->romTitle(ROM_TYPE_CHAR);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)kernalRomTitle
{
    const char *str = [self c64]->romTitle(ROM_TYPE_KERNAL);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)vc1541RomTitle
{
    const char *str = [self c64]->romTitle(ROM_TYPE_VC1541);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)romSubTitle:(RomType)type
{
    const char *str = [self c64]->romSubTitle(type);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)basicRomSubTitle
{
    const char *str = [self c64]->romSubTitle(ROM_TYPE_BASIC);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)charRomSubTitle
{
    const char *str = [self c64]->romSubTitle(ROM_TYPE_CHAR);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)kernalRomSubTitle
{
    const char *str = [self c64]->romSubTitle(ROM_TYPE_KERNAL);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)vc1541RomSubTitle
{
    const char *str = [self c64]->romSubTitle(ROM_TYPE_VC1541);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)romRevision:(RomType)type
{
    const char *str = [self c64]->romRevision(type);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)basicRomRevision
{
    const char *str = [self c64]->romRevision(ROM_TYPE_BASIC);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)charRomRevision
{
    const char *str = [self c64]->romRevision(ROM_TYPE_CHAR);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)kernalRomRevision
{
    const char *str = [self c64]->romRevision(ROM_TYPE_KERNAL);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (NSString *)vc1541RomRevision
{
    const char *str = [self c64]->romRevision(ROM_TYPE_VC1541);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

- (BOOL)isCommodoreRom:(RomIdentifier)rev
{
    return RomFile::isCommodoreRom(rev);
}

- (BOOL)isPatchedRom:(RomIdentifier)rev;
{
    return RomFile::isPatchedRom(rev);
}

- (BOOL)flash:(AnyFileProxy *)proxy
{
    return [self c64]->flash((AnyFile *)proxy->obj);
}

- (BOOL)flash:(FSDeviceProxy *)proxy item:(NSInteger)nr
{
    return [self c64]->flash(*(FSDevice *)proxy->obj, (unsigned)nr);
}

@end
