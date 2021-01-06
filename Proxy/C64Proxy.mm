//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//


#import "C64Proxy.h"
#import "C64.h"
#import "VirtualC64-Swift.h"

struct C64Wrapper { C64 *c64; };
struct CpuWrapper { CPU<C64Memory> *cpu; };
struct GuardsWrapper { Guards *guards; };
struct MemoryWrapper { C64Memory *mem; };
struct VicWrapper { VICII *vic; };
struct CiaWrapper { CIA *cia; };
struct KeyboardWrapper { Keyboard *keyboard; };
struct ControlPortWrapper { ControlPort *port; };
struct SidBridgeWrapper { SIDBridge *sid; };
struct IecWrapper { IEC *iec; };
struct ExpansionPortWrapper { ExpansionPort *expansionPort; };
struct FSDeviceWrapper { FSDevice *device; };
struct ViaWrapper { VIA6522 *via; };
struct DiskWrapper { Disk *disk; };
struct DriveWrapper { Drive *drive; };
struct DatasetteWrapper { Datasette *datasette; };
struct MouseWrapper { Mouse *mouse; };
struct AnyFileWrapper { AnyFile *file; };

//
// Guards (Breakpoints, Watchpoints)
//

@implementation GuardsProxy

- (instancetype) initWithGuards:(Guards *)guards
{
    if (self = [super init]) {
        wrapper = new GuardsWrapper();
        wrapper->guards = guards;
    }
    return self;
}
- (NSInteger) count
{
    return wrapper->guards->elements();
}
- (NSInteger) addr:(NSInteger)nr
{
    return wrapper->guards->guardAddr(nr);
}
- (BOOL) isEnabled:(NSInteger)nr
{
    return wrapper->guards->isEnabled(nr);
}
- (BOOL) isDisabled:(NSInteger)nr
{
    return wrapper->guards->isDisabled(nr);
}
- (void) enable:(NSInteger)nr
{
    wrapper->guards->enable(nr);
}
- (void) disable:(NSInteger)nr
{
    wrapper->guards->disable(nr);
}
- (void) remove:(NSInteger)nr
{
    return wrapper->guards->remove(nr);
}
- (void) replace:(NSInteger)nr addr:(NSInteger)addr
{
    wrapper->guards->replace(nr, (u32)addr);
}
- (BOOL) isSetAt:(NSInteger)addr
{
    return wrapper->guards->isSetAt((u32)addr);
}
- (BOOL) isSetAndEnabledAt:(NSInteger)addr
{
    return wrapper->guards->isSetAndEnabledAt((u32)addr);
}
- (BOOL) isSetAndDisabledAt:(NSInteger)addr
{
    return wrapper->guards->isSetAndDisabledAt((u32)addr);
}
- (void) enableAt:(NSInteger)addr
{
    wrapper->guards->enableAt((u32)addr);
}
- (void) disableAt:(NSInteger)addr
{
    wrapper->guards->disableAt((u32)addr);
}
- (void) addAt:(NSInteger)addr
{
    wrapper->guards->addAt((u32)addr);
}
- (void) removeAt:(NSInteger)addr
{
    wrapper->guards->removeAt((u32)addr);
}

@end

//
// CPU
//

@implementation CPUProxy

// Constructing
- (instancetype) initWithCPU:(CPU<C64Memory> *)cpu
{
    if (self = [super init]) {
        wrapper = new CpuWrapper();
        wrapper->cpu = cpu;
    }
    return self;
}

// Proxy methods
- (CPUInfo) getInfo
{
    return wrapper->cpu->getInfo();
}
- (NSInteger) loggedInstructions
{
    return wrapper->cpu->debugger.loggedInstructions();
}
- (NSInteger) loggedPCRel:(NSInteger)nr
{
    return wrapper->cpu->debugger.loggedPC0Rel((int)nr);
}
- (NSInteger) loggedPCAbs:(NSInteger)nr
{
    return wrapper->cpu->debugger.loggedPC0Abs((int)nr);
}
- (RecordedInstruction) getRecordedInstruction:(NSInteger)index
{
    return wrapper->cpu->debugger.logEntryAbs((int)index);
}
- (void) clearLog
{
    wrapper->cpu->debugger.clearLog();
}
- (void) dump
{
    wrapper->cpu->dump();
}
- (bool) isJammed
{
    return wrapper->cpu->isJammed();
}
- (void) setHex
{
    wrapper->cpu->debugger.hex = true;
}
- (void) setDec
{
    wrapper->cpu->debugger.hex = false;
}
- (i64) cycle
{
    return (i64)wrapper->cpu->cycle;
}
- (u16) pc
{
    return wrapper->cpu->getPC0();
}
- (NSString *) disassembleRecordedInstr:(NSInteger)i length:(NSInteger *)len
{
    const char *str = wrapper->cpu->debugger.disassembleRecordedInstr((int)i, len);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) disassembleRecordedBytes:(NSInteger)i
{
    const char *str = wrapper->cpu->debugger.disassembleRecordedBytes((int)i);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) disassembleRecordedFlags:(NSInteger)i
{
    const char *str = wrapper->cpu->debugger.disassembleRecordedFlags((int)i);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) disassembleRecordedPC:(NSInteger)i
{
    const char *str = wrapper->cpu->debugger.disassembleRecordedPC((int)i);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) disassembleInstr:(NSInteger)addr length:(NSInteger *)len
{
    const char *str = wrapper->cpu->debugger.disassembleInstr(addr, len);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) disassembleBytes:(NSInteger)addr
{
    const char *str = wrapper->cpu->debugger.disassembleBytes(addr);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) disassembleAddr:(NSInteger)addr
{
    const char *str = wrapper->cpu->debugger.disassembleAddr(addr);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

@end


//
// Memory
//

@implementation MemoryProxy

- (instancetype) initWithMemory:(C64Memory *)mem
{
    if (self = [super init]) {
        wrapper = new MemoryWrapper();
        wrapper->mem = mem;
    }
    return self;
}
- (MemInfo) getInfo
{
    return wrapper->mem->getInfo();
}
- (void) dump
{
    wrapper->mem->dump();
}
- (MemoryType) peekSource:(u16)addr
{
    return wrapper->mem->getPeekSource(addr);
}
- (MemoryType) pokeTarget:(u16)addr
{
    return wrapper->mem->getPokeTarget(addr);
}
- (u8) spypeek:(u16)addr source:(MemoryType)source
{
    return wrapper->mem->spypeek(addr, source);
}
- (u8) spypeek:(u16)addr
{
    return wrapper->mem->spypeek(addr);
}
- (u8) spypeekIO:(u16)addr
{
    return wrapper->mem->spypeekIO(addr);
}
- (u8) spypeekColor:(u16)addr
{
    return wrapper->mem->spypeekColor(addr);
}
- (void) poke:(u16)addr value:(u8)value target:(MemoryType)target
{
    wrapper->mem->suspend();
    wrapper->mem->poke(addr, value, target);
    wrapper->mem->resume();
}
- (void) poke:(u16)addr value:(u8)value
{
    wrapper->mem->suspend();
    wrapper->mem->poke(addr, value);
    wrapper->mem->resume();
}
- (void) pokeIO:(u16)addr value:(u8)value
{
    wrapper->mem->suspend();
    wrapper->mem->pokeIO(addr, value);
    wrapper->mem->resume();
}
- (NSString *)memdump:(NSInteger)addr num:(NSInteger)num hex:(BOOL)hex src:(MemoryType)src
{
    const char *str = wrapper->mem->memdump(addr, num, hex, src);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *)txtdump:(NSInteger)addr num:(NSInteger)num src:(MemoryType)src
{
    const char *str = wrapper->mem->txtdump(addr, num, src);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}

@end

//
// CIA
//

@implementation CIAProxy

- (instancetype) initWithCIA:(CIA *)cia
{
    if (self = [super init]) {
        wrapper = new CiaWrapper();
        wrapper->cia = cia;
    }
    return self;
}
- (CIAInfo) getInfo
{
    return wrapper->cia->getInfo();
}
- (void) dump
{
    wrapper->cia->dump();
}
- (void) poke:(u16)addr value:(u8)value {
    wrapper->cia->suspend();
    wrapper->cia->poke(addr, value);
    wrapper->cia->resume();
}

@end

//
// VICII
//

@implementation VICProxy

- (instancetype) initWithVIC:(VICII *)vic
{
    if (self = [super init]) {
        wrapper = new VicWrapper();
        wrapper->vic = vic;
    }
    return self;
}
/*
- (NSInteger) videoPalette
{
    return (NSInteger)wrapper->vic->getPalette();
}
- (void) setVideoPalette:(NSInteger)value
{
    wrapper->vic->setPalette((Palette)value);
}
*/
- (BOOL) isPAL
{
    return wrapper->vic->isPAL();
}
- (VICIIInfo) getInfo {
    return wrapper->vic->getInfo();
}
- (void) dump
{
    wrapper->vic->dump();
}
- (SpriteInfo) getSpriteInfo:(NSInteger)sprite
{
    return wrapper->vic->getSpriteInfo((unsigned)sprite);
}
- (void *) stableEmuTexture
{
    return wrapper->vic->stableEmuTexture();
}
- (NSColor *) color:(NSInteger)nr
{
    assert (0 <= nr && nr < 16);
    
    u32 color = wrapper->vic->getColor((unsigned)nr);
    u8 r = color & 0xFF;
    u8 g = (color >> 8) & 0xFF;
    u8 b = (color >> 16) & 0xFF;
    
	return [NSColor colorWithCalibratedRed:(float)r/255.0
                                     green:(float)g/255.0
                                      blue:(float)b/255.0
                                     alpha:1.0];
}
- (UInt32) rgbaColor:(NSInteger)nr palette:(Palette)palette
{
    assert (0 <= nr && nr < 16);
    return wrapper->vic->getColor((unsigned)nr, palette);
}
- (double)brightness
{
    return wrapper->vic->getBrightness();
}
- (void)setBrightness:(double)value
{
    wrapper->vic->setBrightness(value);
}
- (double)contrast
{
    return wrapper->vic->getContrast();
}
- (void)setContrast:(double)value
{
    wrapper->vic->setContrast(value);
}
- (double)saturation
{
    return wrapper->vic->getSaturation();
}
- (void)setSaturation:(double)value
{
    wrapper->vic->setSaturation(value);
}
/*
- (BOOL) hideSprites
{
    return wrapper->vic->hideSprites;
}
- (void) setHideSprites:(BOOL)b
{
    wrapper->vic->setHideSprites(b);
}
- (BOOL) showIrqLines
{
    return wrapper->vic->markIRQLines;
}
- (void) setShowIrqLines:(BOOL)b
{
    wrapper->vic->setShowIrqLines(b);
}
- (BOOL) showDmaLines
{
    return wrapper->vic->markDMALines;
}
- (void) setShowDmaLines:(BOOL)b
{
    wrapper->vic->setShowDmaLines(b);
}
*/
- (u32 *) noise
{
    return wrapper->vic->getNoise();
}

@end

//
// SID
//

@implementation SIDProxy

- (instancetype) initWithSID:(SIDBridge *)sid
{
    if (self = [super init]) {
        wrapper = new SidBridgeWrapper();
        wrapper->sid = sid;
    }
    return self;
}
- (void) dump
{
    wrapper->sid->dump();
}
- (SIDInfo) getInfo:(NSInteger)nr
{
    return wrapper->sid->getInfo((unsigned)nr);
}
- (VoiceInfo) getVoiceInfo:(NSInteger)nr voice:(NSInteger)voice
{
    return wrapper->sid->getVoiceInfo((unsigned)nr, (unsigned)voice);
} 
- (double) sampleRate
{
    return wrapper->sid->getSampleRate();
}
- (void) setSampleRate:(double)rate
{
    wrapper->sid->setSampleRate(rate);
}
- (NSInteger) ringbufferSize
{
    return wrapper->sid->stream.cap();
}
- (void) ringbufferData:(NSInteger)offset left:(float *)l right:(float *)r
{
    wrapper->sid->ringbufferData(offset, l, r);
}
- (double) fillLevel
{
    return wrapper->sid->stream.fillLevel();
}
- (NSInteger) bufferUnderflows
{
    return wrapper->sid->bufferUnderflows;
}
- (NSInteger) bufferOverflows
{
    return wrapper->sid->bufferOverflows;
}
- (void) copyMono:(float *)target size:(NSInteger)n
{
    wrapper->sid->copyMono(target, n);
}
- (void) copyStereo:(float *)target1 buffer2:(float *)target2 size:(NSInteger)n
{
    wrapper->sid->copyStereo(target1, target2, n);
}
- (void) copyInterleaved:(float *)target size:(NSInteger)n
{
    wrapper->sid->copyInterleaved(target, n);
}
- (void) rampUp
{
    wrapper->sid->rampUp();
}
- (void) rampUpFromZero
{
    wrapper->sid->rampUpFromZero();
}
- (void) rampDown
{
    wrapper->sid->rampDown();
}

@end

//
// IEC bus
//

@implementation IECProxy

- (instancetype) initWithIEC:(IEC *)iec
{
    if (self = [super init]) {
        wrapper = new IecWrapper();
        wrapper->iec = iec;
    }
    return self;
}

- (void) dump
{
    wrapper->iec->dump();
}
- (BOOL) busy
{
    return wrapper->iec->isBusy();
}

@end


//
// Keyboard
//

@implementation KeyboardProxy

- (instancetype) initWithKeyboard:(Keyboard *)keyboard
{
    if (self = [super init]) {
        wrapper = new KeyboardWrapper();
        wrapper->keyboard = keyboard;
    }
    return self;
}

- (void) dump
{
    wrapper->keyboard->dump();
}

- (BOOL) keyIsPressed:(NSInteger)nr
{
    return wrapper->keyboard->isPressed(nr);
}

- (BOOL) keyIsPressedAtRow:(NSInteger)row col:(NSInteger)col
{
    return wrapper->keyboard->isPressed(row, col);
}

- (BOOL) controlIsPressed
{
    return wrapper->keyboard->ctrlIsPressed();
}

- (BOOL) commodoreIsPressed
{
    return wrapper->keyboard->commodoreIsPressed();
}

- (BOOL) leftShiftIsPressed
{
    return wrapper->keyboard->leftShiftIsPressed();
}

- (BOOL) rightShiftIsPressed
{
    return wrapper->keyboard->rightShiftIsPressed();
}

- (BOOL) shiftLockIsPressed
{
    return wrapper->keyboard->shiftLockIsPressed();
}

- (void) pressKey:(NSInteger)nr
{
    wrapper->keyboard->press(nr);
}

- (void) pressKeyAtRow:(NSInteger)row col:(NSInteger)col
{
    wrapper->keyboard->pressRowCol(row, col);
}

- (void) pressShiftLock
{
    wrapper->keyboard->pressShiftLock();
}

- (void) releaseKey:(NSInteger)nr
{
    wrapper->keyboard->release(nr);
}

- (void) releaseKeyAtRow:(NSInteger)row col:(NSInteger)col
{
    wrapper->keyboard->releaseRowCol(row, col);
}

- (void) releaseShiftLock
{
    wrapper->keyboard->releaseShiftLock();
}

- (void) releaseAll
{
    wrapper->keyboard->releaseAll();
}

- (void) toggleKey:(NSInteger)nr
{
    wrapper->keyboard->toggle(nr);
}

- (void) toggleKeyAtRow:(NSInteger)row col:(NSInteger)col
{
    wrapper->keyboard->toggle(row, col);
}

- (void) toggleShiftLock
{
    wrapper->keyboard->toggleShiftLock();
}

- (void) scheduleKeyPress:(NSInteger)nr delay:(NSInteger)delay
{
    wrapper->keyboard->scheduleKeyPress(nr, delay);
}

- (void) scheduleKeyPressAtRow:(NSInteger)row col:(NSInteger)col delay:(NSInteger)delay
{
    wrapper->keyboard->scheduleKeyPress(row, col, delay);
}

- (void) scheduleKeyRelease:(NSInteger)nr delay:(NSInteger)delay
{
    wrapper->keyboard->scheduleKeyRelease(nr, delay);
}

- (void) scheduleKeyReleaseAtRow:(NSInteger)row col:(NSInteger)col delay:(NSInteger)delay
{
    wrapper->keyboard->scheduleKeyRelease(row, col, delay);
}

@end


//
// Control port
//

@implementation ControlPortProxy

- (instancetype) initWithJoystick:(ControlPort *)port
{
    if (self = [super init]) {
        wrapper = new ControlPortWrapper();
        wrapper->port = port;
    }
    return self;
}

- (void) dump
{
    wrapper->port->dump();
}
- (void) trigger:(GamePadAction)event
{
    wrapper->port->trigger(event);
}
- (BOOL) autofire
{
    return wrapper->port->getAutofire();
}
- (void) setAutofire:(BOOL)value
{
    return wrapper->port->setAutofire(value);
}
- (NSInteger) autofireBullets
{
    return (NSInteger)wrapper->port->getAutofireBullets();
}
- (void) setAutofireBullets:(NSInteger)value
{
    wrapper->port->setAutofireBullets((int)value);
}
- (float) autofireFrequency
{
    return wrapper->port->getAutofireFrequency();
}
- (void) setAutofireFrequency:(float)value
{
    wrapper->port->setAutofireFrequency(value);
}

@end

//
// Expansion port
//

@implementation ExpansionPortProxy

- (instancetype) initWithExpansionPort:(ExpansionPort *)expansionPort
{
    if (self = [super init]) {
        wrapper = new ExpansionPortWrapper();
        wrapper->expansionPort = expansionPort;
    }
    return self;
}
- (void) dump
{
    wrapper->expansionPort->dump();
}
- (CartridgeType) cartridgeType
{
    return wrapper->expansionPort->getCartridgeType();
}
- (BOOL) cartridgeAttached
{
    return wrapper->expansionPort->getCartridgeAttached();
}
- (BOOL) attachCartridgeAndReset:(CRTFileProxy *)c
{
    CRTFile *file = (CRTFile *)([c wrapper]->file);
    return wrapper->expansionPort->attachCartridgeAndReset(file);
}
- (void) attachGeoRamCartridge:(NSInteger)capacity
{
    wrapper->expansionPort->attachGeoRamCartridge((u32)capacity);
}
- (void) attachIsepicCartridge
{
    wrapper->expansionPort->attachIsepicCartridge();
}
- (void) detachCartridgeAndReset
{
    wrapper->expansionPort->detachCartridgeAndReset();
}
- (NSInteger) numButtons
{
    return wrapper->expansionPort->numButtons();
}
- (NSString *) getButtonTitle:(NSInteger)nr
{
    const char *str = wrapper->expansionPort->getButtonTitle((unsigned)nr);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (void) pressButton:(NSInteger)nr
{
    wrapper->expansionPort->pressButton((unsigned)nr);
}
- (void) releaseButton:(NSInteger)nr
{
    wrapper->expansionPort->releaseButton((unsigned)nr);
}
- (BOOL) hasSwitch
{
    return wrapper->expansionPort->hasSwitch();
}
- (NSInteger) switchPosition
{
    return wrapper->expansionPort->getSwitch();
}
- (NSString *) switchDescription:(NSInteger)pos
{
    const char *str = wrapper->expansionPort->getSwitchDescription(pos);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) currentSwitchDescription
{
    const char *str = wrapper->expansionPort->getSwitchDescription();
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (BOOL) validSwitchPosition:(NSInteger)pos
{
    return wrapper->expansionPort->validSwitchPosition(pos);
}
- (BOOL) switchIsNeutral
{
    return wrapper->expansionPort->switchIsNeutral();
}
- (BOOL) switchIsLeft
{
    return wrapper->expansionPort->switchIsLeft();
}
- (BOOL) switchIsRight
{
    return wrapper->expansionPort->switchIsRight();
}
- (void) setSwitchPosition:(NSInteger)pos
{
    wrapper->expansionPort->setSwitch(pos);
}
- (BOOL) hasLed
{
    return wrapper->expansionPort->hasLED();
}
- (BOOL) led
{
    return wrapper->expansionPort->getLED();
}
- (void) setLed:(BOOL)value
{
    wrapper->expansionPort->setLED(value);
}
- (BOOL) hasBattery
{
    return wrapper->expansionPort->hasBattery();
}
- (void) setBattery:(BOOL)value
{
    wrapper->expansionPort->setBattery(value);
}

@end

//
// Disk
//

@implementation DiskProxy

@synthesize wrapper;

- (instancetype) initWithDisk525:(Disk *)disk
{
    if (self = [super init]) {
        wrapper = new DiskWrapper();
        wrapper->disk = disk;
    }
    return self;
}
- (void) dump
{
    wrapper->disk->dump();
}
- (BOOL)writeProtected
{
    return wrapper->disk->isWriteProtected();
}
- (void)setWriteProtection:(BOOL)b
{
    wrapper->disk->setWriteProtection(b);
}
- (void)toggleWriteProtection
{
    wrapper->disk->toggleWriteProtection();
}
- (NSInteger)nonemptyHalftracks
{
    return (NSInteger)wrapper->disk->nonemptyHalftracks();
}
- (void)analyzeTrack:(Track)t
{
    wrapper->disk->analyzeTrack(t);
}
- (void)analyzeHalftrack:(Halftrack)ht
{
    wrapper->disk->analyzeHalftrack(ht);
}
- (NSInteger)numErrors
{
    return wrapper->disk->numErrors();
}
- (NSString *)errorMessage:(NSInteger)nr
{
    std::string s = wrapper->disk->errorMessage((unsigned)nr);
    return [NSString stringWithUTF8String:s.c_str()];
}
- (NSInteger)firstErroneousBit:(NSInteger)nr
{
    return wrapper->disk->firstErroneousBit((unsigned)nr);
}
- (NSInteger)lastErroneousBit:(NSInteger)nr
{
    return wrapper->disk->lastErroneousBit((unsigned)nr);
}
- (SectorInfo)sectorInfo:(Sector)s
{
    return wrapper->disk->sectorLayout(s);
}
- (const char *)trackBitsAsString
{
    return wrapper->disk->trackBitsAsString();
}
- (const char *)diskNameAsString
{
    return wrapper->disk->diskNameAsString();
}
- (const char *)sectorHeaderBytesAsString:(Sector)nr hex:(BOOL)hex
{
    return wrapper->disk->sectorHeaderBytesAsString(nr, hex);
}
- (const char *)sectorDataBytesAsString:(Sector)nr hex:(BOOL)hex
{
    return wrapper->disk->sectorDataBytesAsString(nr, hex);
}

@end

//
// FSDevice
//

@implementation FSDeviceProxy

@synthesize wrapper;

- (instancetype) initWithDevice:(FSDevice *)volume
{
    if (self = [super init]) {
        wrapper = new FSDeviceWrapper();
        wrapper->device = volume;
    }
    return self;
}

+ (instancetype) make:(FSDevice *)volume
{
    if (volume == NULL) { return nil; }
    
    FSDeviceProxy *proxy = [[self alloc] initWithDevice: volume];
    return proxy;
}

+ (instancetype) makeWithD64:(D64FileProxy *)proxy
{
    AnyFile *file = [proxy wrapper]->file;

    ErrorCode err;
    FSDevice *volume = FSDevice::makeWithD64((D64File *)file, &err);
    return [self make:volume];
}

+ (instancetype)makeWithDisk:(DiskProxy *)proxy error:(ErrorCode *)err;
{
    Disk *disk = [proxy wrapper]->disk;

    FSDevice *volume = FSDevice::makeWithDisk(disk, err);
    return [self make:volume];
}

+ (instancetype)makeWithCollection:(AnyCollectionProxy *)proxy
{
    AnyFile *file = [proxy wrapper]->file;

    ErrorCode err;
    FSDevice *volume = FSDevice::makeWithCollection((AnyCollection *)file, &err);
    return [self make:volume];
}

- (DOSType) dos
{
    return wrapper->device->dos();
}

- (NSInteger) numCyls
{
    return wrapper->device->getNumCyls();
}

- (NSInteger) numHeads
{
    return wrapper->device->getNumHeads();
}

- (NSInteger) numTracks
{
    return wrapper->device->getNumTracks();
}

- (NSInteger) numSectors:(NSInteger)track
{
    return wrapper->device->getNumSectors((Track)track);
}

- (NSInteger) numBlocks
{
    return wrapper->device->getNumBlocks();
}

- (NSInteger) numFreeBlocks
{
    return wrapper->device->numFreeBlocks();
}

- (NSInteger) numUsedBlocks
{
    return wrapper->device->numUsedBlocks();
}

- (NSInteger) numFiles
{
    return wrapper->device->numFiles();
}

- (NSInteger)cylNr:(NSInteger)t
{
    return (NSInteger)wrapper->device->layout.cylNr((Track)t);
}

- (NSInteger)headNr:(NSInteger)t
{
    return (NSInteger)wrapper->device->layout.headNr((Track)t);
}

- (NSInteger)trackNr:(NSInteger)c head:(NSInteger)h
{
    return (NSInteger)wrapper->device->layout.trackNr((Cylinder)c, (Head)h);
}

- (TSLink)tsLink:(NSInteger)b
{
    return wrapper->device->layout.tsLink((Block)b);
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
    return (NSInteger)wrapper->device->layout.blockNr(ts);
}

- (NSInteger)blockNr:(NSInteger)t sector:(NSInteger)s
{
    return (NSInteger)wrapper->device->layout.blockNr((Track)t, (Sector)s);
}

- (NSInteger)blockNr:(NSInteger)c head:(NSInteger)h sector:(NSInteger)s
{
    return (NSInteger)wrapper->device->layout.blockNr((Cylinder)c, (Head)h, (Sector)s);
}

- (FSBlockType) blockType:(NSInteger)blockNr
{
    return wrapper->device->blockType((u32)blockNr);
}

- (FSUsage) itemType:(NSInteger)blockNr pos:(NSInteger)pos
{
    return wrapper->device->usage((u32)blockNr, (u32)pos);
}

- (FSErrorReport) check:(BOOL)strict
{
    return wrapper->device->check(strict);
}

- (ErrorCode) check:(NSInteger)blockNr
                pos:(NSInteger)pos
           expected:(unsigned char *)exp
             strict:(BOOL)strict
{
    return wrapper->device->check((u32)blockNr, (u32)pos, exp, strict);
}

- (BOOL) isCorrupted:(NSInteger)blockNr
{
    return wrapper->device->isCorrupted((u32)blockNr);
}

- (NSInteger) getCorrupted:(NSInteger)blockNr
{
    return wrapper->device->getCorrupted((u32)blockNr);
}

- (NSInteger) nextCorrupted:(NSInteger)blockNr
{
    return wrapper->device->nextCorrupted((u32)blockNr);
}

- (NSInteger) prevCorrupted:(NSInteger)blockNr
{
    return wrapper->device->prevCorrupted((u32)blockNr);
}

- (void) printDirectory
{
    return wrapper->device->printDirectory();
}

- (NSInteger) readByte:(NSInteger)block offset:(NSInteger)offset
{
    return wrapper->device->readByte((u32)block, (u32)offset);
}

- (BOOL) exportDirectory:(NSString *)path error:(ErrorCode *)err
{
    return wrapper->device->exportDirectory([path fileSystemRepresentation], err);
}

- (void) dump
{
    wrapper->device->dump();
}

- (void) info
{
    wrapper->device->info();
}

- (NSString *)fileName:(NSInteger)nr
{
    return @(wrapper->device->fileName((unsigned)nr).c_str());
}

- (FSFileType)fileType:(NSInteger)nr
{
    return wrapper->device->fileType((unsigned)nr);
}

- (NSInteger)fileSize:(NSInteger)nr
{
    return wrapper->device->fileSize((unsigned)nr);
}

- (NSInteger)fileBlocks:(NSInteger)nr
{
    return wrapper->device->fileBlocks((unsigned)nr);
}

@end

//
// VIA
//

@implementation VIAProxy

- (instancetype) initWithVIA:(VIA6522 *)via
{
    if (self = [super init]) {
        wrapper = new ViaWrapper();
        wrapper->via = via;
    }
    return self;
}
- (void) dump
{
    wrapper->via->dump();
}

@end

//
// VC1541
//

@implementation DriveProxy

@synthesize wrapper, via1, via2, disk;

- (instancetype) initWithVC1541:(Drive *)drive
{
    if (self = [super init]) {
        wrapper = new DriveWrapper();
        wrapper->drive = drive;
        via1 = [[VIAProxy alloc] initWithVIA:&drive->via1];
        via2 = [[VIAProxy alloc] initWithVIA:&drive->via2];
        disk = [[DiskProxy alloc] initWithDisk525:&drive->disk];
    }
    return self;
}

- (VIAProxy *) via:(NSInteger)num {
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

- (DriveConfig) getConfig
{
    return wrapper->drive->getConfig();
}

- (void) dump
{
    wrapper->drive->dump();
}

- (BOOL) isConnected
{
    return wrapper->drive->getConfigItem(OPT_DRIVE_CONNECT) != 0;
}

- (BOOL) isSwitchedOn
{
    return wrapper->drive->getConfigItem(OPT_DRIVE_POWER_SWITCH) != 0;
}

- (BOOL) readMode
{
    return wrapper->drive->readMode();
}

- (BOOL) writeMode
{
    return wrapper->drive->writeMode();
}

- (BOOL) redLED
{
    return wrapper->drive->getRedLED();
}

- (BOOL) hasDisk
{
    return wrapper->drive->hasDisk();
}

- (BOOL) hasModifiedDisk
{
    return wrapper->drive->hasModifiedDisk();
}

- (void) setModifiedDisk:(BOOL)b
{
    wrapper->drive->setModifiedDisk(b);
}

- (void) insertD64:(D64FileProxy *)proxy
{
    D64File *d64 = (D64File *)([proxy wrapper]->file);
    wrapper->drive->insertD64(d64);
}

- (void) insertG64:(G64FileProxy *)proxy
{
    G64File *g64 = (G64File *)([proxy wrapper]->file);
    wrapper->drive->insertG64(g64);
}

- (void) insertFileSystem:(FSDeviceProxy *)proxy
{
    FSDevice *device = (FSDevice *)([proxy wrapper]->device);
    wrapper->drive->insertFileSystem(device);
}

- (void) insertCollection:(AnyCollectionProxy *)disk
{
    AnyCollection *collection = (AnyCollection *)([disk wrapper]->file);
    wrapper->drive->insertDisk(collection);
}

- (void) insertNewDisk:(DOSType)fsType
{
    wrapper->drive->insertNewDisk(fsType);
}

- (void) ejectDisk
{
    wrapper->drive->ejectDisk();
}

- (BOOL) writeProtected
{
    return wrapper->drive->disk.isWriteProtected();
}

- (void) setWriteProtection:(BOOL)b
{
    wrapper->drive->disk.setWriteProtection(b);
}

- (BOOL) hasWriteProtectedDisk
{
    return wrapper->drive->hasWriteProtectedDisk();
}

- (Track) track
{
    return wrapper->drive->getTrack();
}

- (Halftrack) halftrack
{
    return wrapper->drive->getHalftrack();
}

- (u16) sizeOfHalftrack:(Halftrack)ht
{
    return wrapper->drive->sizeOfHalftrack(ht);
}

- (u16) sizeOfCurrentHalftrack
{
    return wrapper->drive->sizeOfCurrentHalftrack();
}

- (u16) offset
{
    return wrapper->drive->getOffset();
}

- (u8) readBitFromHead
{
    return wrapper->drive->readBitFromHead();
}

- (BOOL) isRotating
{
    return wrapper->drive->isRotating();
}

@end


//
// Datasette
//

@implementation DatasetteProxy

- (instancetype) initWithDatasette:(Datasette *)datasette
{
    if (self = [super init]) {
        wrapper = new DatasetteWrapper();
        wrapper->datasette = datasette;
    }
    return self;
}

- (void) dump
{
    wrapper->datasette->dump();
}
- (BOOL) hasTape
{
    return wrapper->datasette->hasTape();
}
- (void) pressPlay
{
    wrapper->datasette->pressPlay();
}
- (void) pressStop
{
    wrapper->datasette->pressStop();
}
- (void) rewind
{
    wrapper->datasette->rewind();
}
- (BOOL) insertTape:(TAPFileProxy *)tape
{
    TAPFile *file = (TAPFile *)([tape wrapper]->file);
    return wrapper->datasette->insertTape(file);
}
- (void) ejectTape
{
    wrapper->datasette->ejectTape();
}
- (NSInteger) getType
{
    return wrapper->datasette->getType();
}
- (long) durationInCycles
{
    return wrapper->datasette->getDurationInCycles();
}
- (int) durationInSeconds
{
    return wrapper->datasette->getDurationInSeconds();
}
- (NSInteger) head
{
    return wrapper->datasette->getHead();
}
- (NSInteger) headInCycles
{
    return wrapper->datasette->getHeadInCycles();
}
- (int) headInSeconds
{
    return wrapper->datasette->getHeadInSeconds();
}
- (void) setHeadInCycles:(long)value
{
    wrapper->datasette->setHeadInCycles(value);
}
- (BOOL) motor
{
    return wrapper->datasette->getMotor();
}
- (BOOL) playKey
{
    return wrapper->datasette->getPlayKey();
}
@end


//
// Mouse
//

@implementation MouseProxy

- (instancetype) initWithMouse:(Mouse *)mouse
{
    if (self = [super init]) {
        wrapper = new MouseWrapper();
        wrapper->mouse = mouse;
    }
    return self;
}

- (MouseModel) model
{
    return wrapper->mouse->getModel();
}
- (void) setModel:(MouseModel)model
{
    wrapper->mouse->setModel(model);
}
- (NSInteger) port
{
    return wrapper->mouse->getPort();
}
- (void) connect:(NSInteger)toPort
{
    wrapper->mouse->connectMouse((unsigned)toPort);
}
- (void) disconnect
{
    wrapper->mouse->disconnectMouse();
}
- (void) setXY:(NSPoint)pos
{
    wrapper->mouse->setXY((i64)pos.x, (i64)pos.y);
}
- (void) setLeftButton:(BOOL)pressed
{
    wrapper->mouse->setLeftButton(pressed);
}
- (void) setRightButton:(BOOL)pressed
{
    wrapper->mouse->setRightButton(pressed);
}
@end


//
// AnyFile
//

@implementation AnyFileProxy

- (instancetype) initWithFile:(AnyFile *)file
{
    if (file == nil) {
        return nil;
    }
    if (self = [super init]) {
        wrapper = new AnyFileWrapper();
        wrapper->file = file;
    }
    return self;
}

+ (AnyFileProxy *) makeWithFile:(AnyFile *)file
{
    if (file == nil) {
        return nil;
    }
    return [[self alloc] initWithFile:file];
}

- (void)setPath:(NSString *)path
{
    AnyFile *file = (AnyFile *)([self wrapper]->file);
    file->path = [path UTF8String];
}

- (AnyFileWrapper *)wrapper
{
    return wrapper;
}

- (FileType)type
{
    return wrapper->file->type();
}

- (NSString *)name
{
    return [NSString stringWithUTF8String:wrapper->file->getName().c_str()];
}

- (u64) fnv
{
    return wrapper->file->fnv();
}

- (NSInteger)writeToFile:(NSString *)path error:(ErrorCode *)err
{
    NSInteger result = 0;
    
    try { result = wrapper->file->writeToFile([path fileSystemRepresentation]); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
    
    return result;
}

- (void) dealloc
{
    // NSLog(@"AnyFileProxy::dealloc");
    
    if (wrapper) {
        if (wrapper->file) delete wrapper->file;
        delete wrapper;
    }
}

@end

//
// Snapshot
//

@implementation SnapshotProxy

+ (instancetype) make:(Snapshot *)snapshot
{
    if (snapshot == NULL) {
        return nil;
    }
    return [[self alloc] initWithFile:snapshot];
}

+ (instancetype) makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err
{
    Snapshot *file = nil;
    
    try { file = AnyFile::make <Snapshot> ((const u8 *)buf, len); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
    
    return [self make: file];
}

+ (instancetype) makeWithFile:(NSString *)path error:(ErrorCode *)err
{
    Snapshot *file = nil;
    
    try { file = AnyFile::make <Snapshot> ([path fileSystemRepresentation]); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
    
    return [self make: file];
}

+ (instancetype) makeWithC64:(C64Proxy *)c64proxy
{
    C64 *c64 = [c64proxy wrapper]->c64;
    c64->suspend();
    Snapshot *snapshot = Snapshot::makeWithC64(c64);
    c64->resume();
    return [self make:snapshot];
}

- (NSImage *)previewImage
{
    if (preview != NULL) { return preview; }
    
    // Create preview image
    Snapshot *snapshot = (Snapshot *)wrapper->file;
    
    NSInteger width = snapshot->imageWidth();
    NSInteger height = snapshot->imageHeight();
    unsigned char *data = snapshot->imageData();
    
    
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
    
    // image.makeGlossy()

    return preview;
}
- (time_t)timeStamp
{
    return ((Snapshot *)wrapper->file)->timeStamp();
}

@end

//
// CRT
//

@implementation CRTFileProxy

+ (instancetype) make:(CRTFile *)container
{
    return container ? [[self alloc] initWithFile:container] : nil;
}

+ (instancetype) makeWithFile:(NSString *)path error:(ErrorCode *)err
{
    CRTFile *file = nil;
    
    try { file = AnyFile::make <CRTFile> ([path fileSystemRepresentation]); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
    
    return [self make: file];
}

+ (instancetype) makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err
{
    CRTFile *file = nil;
    
    try { file = AnyFile::make <CRTFile> ((const u8 *)buf, len); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
        
    return [self make: file];
}

- (CRTFile *)unwrap
{
    return (CRTFile *)wrapper->file;
}

- (CartridgeType)cartridgeType
{
    return [self unwrap]->cartridgeType();
}

- (BOOL)isSupported
{
    return [self unwrap]->isSupported();
}

- (NSInteger)initialExromLine
{
    return [self unwrap]->initialExromLine();
}

- (NSInteger)initialGameLine
{
    return [self unwrap]->initialGameLine();
}

- (NSInteger)chipCount
{
    return [self unwrap]->chipCount();
}

@end

//
// TAP
//

@implementation TAPFileProxy

+ (instancetype)make:(TAPFile *)container
{
    return container ? [[self alloc] initWithFile:container] : nil;
}

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err
{
    TAPFile *file = nil;
    
    try { file = AnyFile::make <TAPFile> ([path fileSystemRepresentation]); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
    
    return [self make: file];
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err
{
    TAPFile *file = nil;
    
    try { file = AnyFile::make <TAPFile> ((const u8 *)buf, len); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
    
    return [self make: file];
}

- (TAPFile *)unwrap
{
    return (TAPFile *)wrapper->file;
}

- (TAPVersion)version
{
    return [self unwrap]->version();
}

@end

//
// AnyCollection
//

@implementation AnyCollectionProxy

- (AnyCollection *)unwrap
{
    return (AnyCollection *)([self wrapper]->file);
}

- (NSInteger)itemSize:(NSInteger)nr
{
    return [self unwrap]->itemSize((unsigned)nr);
}

@end


//
// T64
//

@implementation T64FileProxy

+ (instancetype)make:(T64File *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithFile:archive];
}

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err
{
    T64File *file = nil;
    
    try { file = AnyFile::make <T64File> ([path fileSystemRepresentation]); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
    
    return [self make: file];
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err
{
    T64File *file = nil;
    
    try { file = AnyFile::make <T64File> ((const u8 *)buf, len); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
        
    return [self make: file];
}

+ (instancetype)makeWithFileSystem:(FSDeviceProxy *)proxy error:(ErrorCode *)err;
{
    FSDevice *fs = [proxy wrapper]->device;
    T64File *archive = T64File::makeWithFileSystem(fs);
    return [self make: archive];
}

@end

//
// PRG
//

@implementation PRGFileProxy

+ (instancetype)make:(PRGFile *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithFile:archive];
}

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err
{
    PRGFile *file = nil;
    
    try { file = AnyFile::make <PRGFile> ([path fileSystemRepresentation]); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
    
    return [self make: file];
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err
{
    PRGFile *file = nil;
    
    try { file = AnyFile::make <PRGFile> ((const u8 *)buf, len); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
        
    return [self make: file];
}

+ (instancetype)makeWithFileSystem:(FSDeviceProxy *)proxy error:(ErrorCode *)err
{
    FSDevice *fs = [proxy wrapper]->device;
    PRGFile *archive = PRGFile::makeWithFileSystem(fs);
    return [self make: archive];
}

@end

//
// Folder
//

@implementation FolderProxy

+ (instancetype)make:(Folder *)folder
{
    if (folder == NULL) return nil;
    return [[self alloc] initWithFile:folder];
}

+ (instancetype)makeWithFolder:(NSString *)path error:(ErrorCode *)err
{
    Folder *folder = nil;
    
    try { folder = Folder::makeWithFolder([path fileSystemRepresentation]); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
        
    return [self make: folder];
}

- (FSDeviceProxy *)fileSystem
{
    Folder *folder = (Folder *)([self wrapper]->file);
    return [FSDeviceProxy make:folder->getFS()];
}

@end

//
// P00
//

@implementation P00FileProxy

+ (instancetype)make:(P00File *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithFile:archive];
}

+ (instancetype)makeWithFile:(NSString *)path error:(ErrorCode *)err
{
    P00File *file = nil;
    
    try { file = AnyFile::make <P00File> ([path fileSystemRepresentation]); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
    
    return [self make: file];
}

+ (instancetype)makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err
{
    P00File *file = nil;
    
    try { file = AnyFile::make <P00File> ((const u8 *)buf, len); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
        
    return [self make: file];
}

+ (instancetype)makeWithFileSystem:(FSDeviceProxy *)proxy error:(ErrorCode *)err
{
    FSDevice *fs = [proxy wrapper]->device;
    P00File *archive = P00File::makeWithFileSystem(fs);
    return [self make: archive];    
}

@end

//
// D64
//

@implementation D64FileProxy

- (D64File *)unwrap
{
    return (D64File *)wrapper->file;
}

+ (instancetype) make:(D64File *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithFile:archive];
}

+ (instancetype) makeWithFile:(NSString *)path error:(ErrorCode *)err
{
    D64File *file = nil;
    
    try { file = AnyFile::make <D64File> ([path fileSystemRepresentation]); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
    
    return [self make: file];
}

+ (instancetype) makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err
{
    D64File *file = nil;
    
    try { file = AnyFile::make <D64File> ((const u8 *)buf, len); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
        
    return [self make: file];
}

+ (instancetype) makeWithDisk:(DiskProxy *)proxy error:(ErrorCode *)err
{
    D64File *file = nil;
    Disk *disk = (Disk *)([proxy wrapper]->disk);

    try { file = D64File::makeWithDisk(disk);
    } catch (VC64Error &exception) { *err = exception.errorCode; }
    
    return [self make: file];
}

+ (instancetype) makeWithDrive:(DriveProxy *)proxy
{
    Drive *drive = (Drive *)([proxy wrapper]->drive);
    D64File *archive = D64File::makeWithDrive(drive);
    return archive ? [self make: archive] : nullptr;
}

+ (instancetype)makeWithFileSystem:(FSDeviceProxy *)proxy error:(ErrorCode *)err
{
    FSDevice *device = (FSDevice *)([proxy wrapper]->device);
    
    D64File *archive = D64File::makeWithVolume(*device, err);
    return archive ? [self make: archive] : nullptr;
}

- (NSInteger)numTracks
{
    D64File *disk = (D64File *)([self wrapper]->file);
    return disk->numTracks();
}

- (NSInteger) numHalftracks
{
    D64File *disk = (D64File *)([self wrapper]->file);
    return disk->numHalftracks();
}

@end

//
// G64
//

@implementation G64FileProxy

+ (instancetype) make:(G64File *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithFile:archive];
}

+ (instancetype) makeWithFile:(NSString *)path error:(ErrorCode *)err
{
    G64File *file = nil;
    
    try { file = AnyFile::make <G64File> ([path fileSystemRepresentation]); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
    
    return [self make: file];
}

+ (instancetype) makeWithBuffer:(const void *)buf length:(NSInteger)len error:(ErrorCode *)err
{
    G64File *file = nil;
    
    try { file = AnyFile::make <G64File> ((const u8 *)buf, len); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
        
    return [self make: file];
}

+ (instancetype) makeWithDisk:(DiskProxy *)proxy error:(ErrorCode *)err
{
    G64File *file = nil;
    Disk *disk = (Disk *)([proxy wrapper]->disk);

    try { file = G64File::makeWithDisk(disk);
    } catch (VC64Error &exception) { *err = exception.errorCode; }
    
    return [self make: file];
}

@end

//
// C64
//

@implementation C64Proxy

@synthesize wrapper;
@synthesize mem, cpu, breakpoints, watchpoints, vic, cia1, cia2, sid;
@synthesize keyboard, port1, port2, iec;
@synthesize expansionport, drive8, drive9, datasette, mouse;

- (instancetype) init
{
    NSLog(@"C64Proxy::init");
    
    if (!(self = [super init]))
        return self;
    
    C64 *c64 = new C64();
    wrapper = new C64Wrapper();
    wrapper->c64 = c64;
    
    // Create sub proxys
    mem = [[MemoryProxy alloc] initWithMemory:&c64->mem];
    cpu = [[CPUProxy alloc] initWithCPU:&c64->cpu];
    breakpoints = [[GuardsProxy alloc] initWithGuards:&c64->cpu.debugger.breakpoints];
    watchpoints = [[GuardsProxy alloc] initWithGuards:&c64->cpu.debugger.watchpoints];
    vic = [[VICProxy alloc] initWithVIC:&c64->vic];
    cia1 = [[CIAProxy alloc] initWithCIA:&c64->cia1];
    cia2 = [[CIAProxy alloc] initWithCIA:&c64->cia2];
    sid = [[SIDProxy alloc] initWithSID:&c64->sid];
    keyboard = [[KeyboardProxy alloc] initWithKeyboard:&c64->keyboard];
    port1 = [[ControlPortProxy alloc] initWithJoystick:&c64->port1];
    port2 = [[ControlPortProxy alloc] initWithJoystick:&c64->port2];
    iec = [[IECProxy alloc] initWithIEC:&c64->iec];
    expansionport = [[ExpansionPortProxy alloc] initWithExpansionPort:&c64->expansionport];
    drive8 = [[DriveProxy alloc] initWithVC1541:&c64->drive8];
    drive9 = [[DriveProxy alloc] initWithVC1541:&c64->drive9];
    datasette = [[DatasetteProxy alloc] initWithDatasette:&c64->datasette];
    mouse = [[MouseProxy alloc] initWithMouse:&c64->mouse];

    return self;
}

- (DriveProxy *) drive:(DriveID)id
{
    switch (id) {
        case DRIVE8:  return drive8;
        case DRIVE9:  return drive9;
        default:      return NULL;
    }
}

- (void) dealloc
{
    NSLog(@"dealloc");
}

- (void) kill
{
    assert(wrapper->c64 != NULL);
    NSLog(@"C64Proxy::kill");
    
    // Kill the emulator
    delete wrapper->c64;
    wrapper->c64 = NULL;
}

- (BOOL) releaseBuild
{
    return releaseBuild();
}
- (BOOL) debugMode
{
    return wrapper->c64->inDebugMode();
}
- (void) enableDebugging
{
    wrapper->c64->enableDebugMode();
}
- (void) disableDebugging
{
    wrapper->c64->disableDebugMode();
}
- (void) setInspectionTarget:(InspectionTarget)target
{
    wrapper->c64->setInspectionTarget(target);
}
- (void) clearInspectionTarget
{
    wrapper->c64->clearInspectionTarget();
}
- (BOOL) isReady:(ErrorCode *)err
{
    return wrapper->c64->isReady(err);
}
- (BOOL) isReady
{
    return wrapper->c64->isReady();
}
- (void) powerOn
{
    wrapper->c64->powerOn();
}
- (void) powerOff
{
    wrapper->c64->powerOff();
}
- (void) inspect
{
    wrapper->c64->inspect();
}
- (void) reset
{
    wrapper->c64->reset();
}
- (void) dump
{
    wrapper->c64->dump();
}
- (BOOL) isPoweredOn
{
    return wrapper->c64->isPoweredOn();
}
- (BOOL) isPoweredOff
{
    return wrapper->c64->isPoweredOff();
}
- (BOOL) isRunning
{
    return wrapper->c64->isRunning();
}
- (BOOL) isPaused
{
    return wrapper->c64->isPaused();
}
- (void) run
{
    wrapper->c64->run();
}
- (void) pause
{
    wrapper->c64->pause();
}
- (void) suspend
{
    wrapper->c64->suspend();
}
- (void) resume
{
    wrapper->c64->resume();
}
- (void) requestAutoSnapshot
{
    wrapper->c64->requestAutoSnapshot();
}
- (void) requestUserSnapshot
{
    wrapper->c64->requestUserSnapshot();
}
- (SnapshotProxy *) latestAutoSnapshot
{
    Snapshot *snapshot = wrapper->c64->latestAutoSnapshot();
    return [SnapshotProxy make:snapshot];
}
- (SnapshotProxy *) latestUserSnapshot
{
    Snapshot *snapshot = wrapper->c64->latestUserSnapshot();
    return [SnapshotProxy make:snapshot];
}
- (void) loadFromSnapshot:(SnapshotProxy *)proxy
{
    Snapshot *snapshot = (Snapshot *)([proxy wrapper]->file);
    wrapper->c64->loadFromSnapshot(snapshot);
}
- (C64Configuration) config
{
    return wrapper->c64->getConfig();
}
- (NSInteger) getConfig:(Option)opt
{
    return wrapper->c64->getConfigItem(opt);
}
- (NSInteger) getConfig:(Option)opt id:(NSInteger)id
{
    return wrapper->c64->getConfigItem(opt, id);
}
- (NSInteger) getConfig:(Option)opt drive:(DriveID)id
{
    return wrapper->c64->getConfigItem(opt, (long)id);
}
- (BOOL) configure:(Option)opt value:(NSInteger)val
{
    return wrapper->c64->configure(opt, val);
}
- (BOOL) configure:(Option)opt enable:(BOOL)val
{
    return wrapper->c64->configure(opt, val ? 1 : 0);
}
- (BOOL) configure:(Option)opt id:(NSInteger)id value:(NSInteger)val
{
    return wrapper->c64->configure(opt, id, val);
}
- (BOOL) configure:(Option)opt id:(NSInteger)id enable:(BOOL)val
{
    return wrapper->c64->configure(opt, id, val ? 1 : 0);
}
- (BOOL) configure:(Option)opt drive:(DriveID)id value:(NSInteger)val
{
    return wrapper->c64->configure(opt, (long)id, val);
}
- (BOOL) configure:(Option)opt drive:(DriveID)id enable:(BOOL)val
{
    return wrapper->c64->configure(opt, (long)id, val ? 1 : 0);
}
- (void) configure:(C64Model)model
{
    wrapper->c64->configure(model);
}
- (C64Model) model
{
    return wrapper->c64->getModel();
}
- (Message)message
{
    return wrapper->c64->getMessage();
}
- (void) addListener:(const void *)sender function:(Callback *)func
{
    wrapper->c64->addListener(sender, func);
}
- (void) removeListener:(const void *)sender
{
    wrapper->c64->removeListener(sender);
}
- (void) stopAndGo
{
    wrapper->c64->stopAndGo();
}
- (void) stepInto
{
    wrapper->c64->stepInto();
}
- (void) stepOver
{
    wrapper->c64->stepOver();
}
- (BOOL) warp
{
    return wrapper->c64->inWarpMode();
}
- (void) warpOn
{
    wrapper->c64->enableWarpMode();
}
- (void) warpOff
{
    wrapper->c64->disableWarpMode();
}
- (BOOL) hasBasicRom
{
    return wrapper->c64->hasRom(ROM_TYPE_BASIC);
}
- (BOOL) hasCharRom
{
    return wrapper->c64->hasRom(ROM_TYPE_CHAR);
}
- (BOOL) hasKernalRom
{
    return wrapper->c64->hasRom(ROM_TYPE_KERNAL);
}
- (BOOL) hasVC1541Rom
{
    return wrapper->c64->hasRom(ROM_TYPE_VC1541);
}
- (BOOL) hasMega65BasicRom
{
    return wrapper->c64->hasMega65Rom(ROM_TYPE_BASIC);
}
- (BOOL) hasMega65CharRom
{
    return wrapper->c64->hasMega65Rom(ROM_TYPE_CHAR);
}
- (BOOL) hasMega65KernelRom
{
    return wrapper->c64->hasMega65Rom(ROM_TYPE_KERNAL);
}
- (BOOL) isBasicRom:(NSURL *)url
{
    return RomFile::isBasicRomFile([[url path] UTF8String]);
}
- (BOOL) isCharRom:(NSURL *)url
{
    return RomFile::isCharRomFile([[url path] UTF8String]);
}
- (BOOL) isKernalRom:(NSURL *)url
{
    return RomFile::isKernalRomFile([[url path] UTF8String]);
}
- (BOOL) isVC1541Rom:(NSURL *)url
{
    return RomFile::isVC1541RomFile([[url path] UTF8String]);
}

- (BOOL) loadRom:(RomType)type url:(NSURL *)url error:(ErrorCode *)err;
{
    try { wrapper->c64->loadRomFromFile(type, [[url path] UTF8String]); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
    return YES;
}
/*
- (BOOL) loadBasicRomFromFile:(NSURL *)url;
- (BOOL) loadCharRomFromFile:(NSURL *)url;
- (BOOL) loadKernalRomFromFile:(NSURL *)url;
- (BOOL) loadVC1541RomFromFile:(NSURL *)url;
*/
- (BOOL) loadRom:(RomType)type data:(NSData *)data error:(ErrorCode *)err;
{
    if (data == nil) return NO;
    const u8 *bytes = (const u8 *)[data bytes];
    return wrapper->c64->loadRomFromBuffer(type, bytes, [data length]);
}/*
- (BOOL) loadBasicRomFromFile:(NSURL *)url
{
    return wrapper->c64->loadRomFromFile(ROM_TYPE_BASIC, [[url path] UTF8String]);
}
- (BOOL) loadCharRomFromFile:(NSURL *)url
{
    return wrapper->c64->loadRomFromFile(ROM_TYPE_CHAR, [[url path] UTF8String]);
}
- (BOOL) loadKernalRomFromFile:(NSURL *)url
{
    return wrapper->c64->loadRomFromFile(ROM_TYPE_KERNAL, [[url path] UTF8String]);
}
- (BOOL) loadVC1541RomFromFile:(NSURL *)url
{
    return wrapper->c64->loadRomFromFile(ROM_TYPE_VC1541, [[url path] UTF8String]);
}
- (BOOL) loadBasicRomFromBuffer:(NSData *)data
{
    if (data == NULL) return NO;
    const u8 *bytes = (const u8 *)[data bytes];
    return wrapper->c64->loadRomFromBuffer(ROM_TYPE_BASIC, bytes, [data length]);
}
- (BOOL) loadCharRomFromBuffer:(NSData *)data
{
    if (data == NULL) return NO;
    const u8 *bytes = (const u8 *)[data bytes];
    return wrapper->c64->loadRomFromBuffer(ROM_TYPE_CHAR, bytes, [data length]);
}
- (BOOL) loadKernalRomFromBuffer:(NSData *)data
{
    if (data == NULL) return NO;
    const u8 *bytes = (const u8 *)[data bytes];
    return wrapper->c64->loadRomFromBuffer(ROM_TYPE_KERNAL, bytes, [data length]);
}
- (BOOL) loadVC1541RomFromBuffer:(NSData *)data
{
    if (data == NULL) return NO;
    const u8 *bytes = (const u8 *)[data bytes];
    return wrapper->c64->loadRomFromBuffer(ROM_TYPE_VC1541, bytes, [data length]);
}
*/
- (BOOL) saveBasicRom:(NSURL *)url
{
    return wrapper->c64->saveRom(ROM_TYPE_BASIC, [[url path] UTF8String]);
}
- (BOOL) saveCharRom:(NSURL *)url
{
    return wrapper->c64->saveRom(ROM_TYPE_CHAR, [[url path] UTF8String]);
}
- (BOOL) saveKernalRom:(NSURL *)url
{
    return wrapper->c64->saveRom(ROM_TYPE_KERNAL, [[url path] UTF8String]);
}
- (BOOL) saveVC1541Rom:(NSURL *)url
{
    return wrapper->c64->saveRom(ROM_TYPE_VC1541, [[url path] UTF8String]);
}
- (void) deleteBasicRom
{
    wrapper->c64->deleteRom(ROM_TYPE_BASIC);
}
- (void) deleteKernalRom
{
    wrapper->c64->deleteRom(ROM_TYPE_KERNAL);
}
- (void) deleteCharRom
{
    wrapper->c64->deleteRom(ROM_TYPE_CHAR);
}
- (void) deleteVC1541Rom
{
    wrapper->c64->deleteRom(ROM_TYPE_VC1541);
}
- (RomIdentifier) basicRomIdentifier
{
    return wrapper->c64->romIdentifier(ROM_TYPE_BASIC);
}
- (RomIdentifier) kernalRomIdentifier
{
    return wrapper->c64->romIdentifier(ROM_TYPE_KERNAL);
}
- (RomIdentifier) charRomIdentifier
{
    return wrapper->c64->romIdentifier(ROM_TYPE_CHAR);
}
- (RomIdentifier) vc1541RomIdentifier
{
    return wrapper->c64->romIdentifier(ROM_TYPE_VC1541);
}
- (NSString *) basicRomTitle
{
    const char *str = wrapper->c64->romTitle(ROM_TYPE_BASIC);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) charRomTitle
{
    const char *str = wrapper->c64->romTitle(ROM_TYPE_CHAR);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) kernalRomTitle
{
    const char *str = wrapper->c64->romTitle(ROM_TYPE_KERNAL);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) vc1541RomTitle
{
    const char *str = wrapper->c64->romTitle(ROM_TYPE_VC1541);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) basicRomSubTitle
{
    const char *str = wrapper->c64->romSubTitle(ROM_TYPE_BASIC);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) charRomSubTitle
{
    const char *str = wrapper->c64->romSubTitle(ROM_TYPE_CHAR);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) kernalRomSubTitle
{
    const char *str = wrapper->c64->romSubTitle(ROM_TYPE_KERNAL);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) vc1541RomSubTitle
{
    const char *str = wrapper->c64->romSubTitle(ROM_TYPE_VC1541);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) basicRomRevision
{
    const char *str = wrapper->c64->romRevision(ROM_TYPE_BASIC);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) charRomRevision
{
    const char *str = wrapper->c64->romRevision(ROM_TYPE_CHAR);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) kernalRomRevision
{
    const char *str = wrapper->c64->romRevision(ROM_TYPE_KERNAL);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) vc1541RomRevision
{
    const char *str = wrapper->c64->romRevision(ROM_TYPE_VC1541);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (BOOL) isCommodoreRom:(RomIdentifier)rev
{
    return RomFile::isCommodoreRom(rev);
}
- (BOOL) isPatchedRom:(RomIdentifier)rev;
{
    return RomFile::isPatchedRom(rev);
}
/*
- (BOOL) isRom:(NSURL *)url
{
    return
    [self isBasicRom:url] ||
    [self isCharRom:url] ||
    [self isKernalRom:url] ||
    [self isVC1541Rom:url];
}
*/
/*
- (BOOL) loadRom:(NSURL *)url
{
    return
    [self loadRom: ROM_TYPE_BASIC url: url] ||
    [self loadRom: ROM_TYPE_CHAR url: url] ||
    [self loadRom: ROM_TYPE_KERNAL url: url] ||
    [self loadRom: ROM_TYPE_VC1541 url: url];
}
*/

// Flashing files
- (BOOL)flash:(AnyFileProxy *)file
{
    return wrapper->c64->flash([file wrapper]->file);
}
- (BOOL)flash:(AnyCollectionProxy *)proxy item:(NSInteger)nr
{
    AnyCollection *collection = (AnyCollection *)([proxy wrapper]->file);
    return wrapper->c64->flash(collection, (unsigned)nr);
}

@end
