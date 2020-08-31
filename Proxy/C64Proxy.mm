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
struct ViaWrapper { VIA6522 *via; };
struct DiskWrapper { Disk *disk; };
struct DriveWrapper { Drive *drive; };
struct DatasetteWrapper { Datasette *datasette; };
struct MouseWrapper { Mouse *mouse; };
struct AnyC64FileWrapper { AnyFile *file; };

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
- (BOOL) tracing
{
    return wrapper->cpu->tracingEnabled();
}
- (void) setTracing:(BOOL)b
{
    b ? wrapper->cpu->startTracing() : wrapper->cpu->stopTracing();
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
- (BOOL) tracing
{
    return wrapper->cia->tracingEnabled();
}
- (void) setTracing:(BOOL)b
{
    b ? wrapper->cia->startTracing() : wrapper->cia->stopTracing();
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
- (void *) screenBuffer
{
    return wrapper->vic->screenBuffer();
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
- (SIDInfo) getInfo
{
    return wrapper->sid->getInfo();
}
- (VoiceInfo) getVoiceInfo:(NSInteger)voice
{
    return wrapper->sid->getVoiceInfo((unsigned)voice);
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
    return wrapper->sid->ringbufferSize();
}
- (float) ringbufferData:(NSInteger)offset
{
    return wrapper->sid->ringbufferData(offset);
}
- (double) fillLevel
{
    return wrapper->sid->fillLevel();
}
- (NSInteger) bufferUnderflows
{
    return wrapper->sid->bufferUnderflows;
}
- (NSInteger) bufferOverflows
{
    return wrapper->sid->bufferOverflows;
}
- (void) readMonoSamples:(float *)target size:(NSInteger)n
{
    wrapper->sid->readMonoSamples(target, n);
}
- (void) readStereoSamples:(float *)target1 buffer2:(float *)target2 size:(NSInteger)n
{
    wrapper->sid->readStereoSamples(target1, target2, n);
}
- (void) readStereoSamplesInterleaved:(float *)target size:(NSInteger)n
{
    wrapper->sid->readStereoSamplesInterleaved(target, n);
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
- (BOOL) tracing
{
    return wrapper->iec->tracingEnabled();
}
- (void) setTracing:(BOOL)b
{
    b ? wrapper->iec->startTracing() : wrapper->iec->stopTracing();
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
- (void) pressKeyAtRow:(NSInteger)row col:(NSInteger)col
{
    wrapper->keyboard->pressKey(row, col);
}
- (void) pressRestoreKey {
    wrapper->keyboard->pressRestoreKey();
}
- (void) releaseKeyAtRow:(NSInteger)row col:(NSInteger)col
{
    wrapper->keyboard->releaseKey(row, col);
}
- (void) releaseRestoreKey
{
    wrapper->keyboard->releaseRestoreKey();
}
- (void) releaseAll
{
    wrapper->keyboard->releaseAll();
}
- (BOOL) leftShiftIsPressed
{
    return wrapper->keyboard->leftShiftIsPressed();
}
- (BOOL) rightShiftIsPressed
{
    return wrapper->keyboard->rightShiftIsPressed();
}
- (BOOL) controlIsPressed
{
    return wrapper->keyboard->ctrlIsPressed();
}
- (BOOL) commodoreIsPressed
{
    return wrapper->keyboard->commodoreIsPressed();
}
- (BOOL) shiftLockIsHoldDown
{
    return wrapper->keyboard->shiftLockIsHoldDown();
}
- (void) lockShift
{
    wrapper->keyboard->pressShiftLockKey();
}
- (void) unlockShift
{
    wrapper->keyboard->releaseShiftLockKey();
}
- (BOOL) inUpperCaseMode
{
    return wrapper->keyboard->inUpperCaseMode();
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
- (void) attachCartridgeAndReset:(CRTFileProxy *)c
{
    CRTFile *file = (CRTFile *)([c wrapper]->file);
    wrapper->expansionPort->attachCartridgeAndReset(file);
}
- (BOOL) attachGeoRamCartridge:(NSInteger)capacity
{
    return wrapper->expansionPort->attachGeoRamCartridge((u32)capacity);
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
- (BOOL) tracing
{
    return wrapper->via->tracingEnabled();
}
- (void) setTracing:(BOOL)b
{
    b ? wrapper->via->startTracing() : wrapper->via->stopTracing();
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
- (BOOL) tracing
{
    return wrapper->drive->tracingEnabled();
}
- (void) setTracing:(BOOL)b
{
    b ? wrapper->drive->startTracing() : wrapper->drive->stopTracing();
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
- (void) prepareToInsert
{
    wrapper->drive->prepareToInsert();
}
- (void) insertDisk:(AnyArchiveProxy *)disk
{
    AnyArchive *archive = (AnyArchive *)([disk wrapper]->file);
    wrapper->drive->insertDisk(archive);
}
- (void) prepareToEject
{
    wrapper->drive->prepareToEject();
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

- (NSInteger) model
{
    return (NSInteger)wrapper->mouse->getModel();
}
- (void) setModel:(NSInteger)model
{
    wrapper->mouse->setModel((MouseModel)model);
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
// AnyC64File
//

@implementation AnyC64FileProxy

- (instancetype) initWithFile:(AnyFile *)file
{
    if (file == nil) {
        return nil;
    }
    if (self = [super init]) {
        wrapper = new AnyC64FileWrapper();
        wrapper->file = file;
    }
    return self;
}
+ (AnyC64FileProxy *) makeWithFile:(AnyFile *)file
{
    if (file == nil) {
        return nil;
    }
    return [[self alloc] initWithFile:file];
}

- (void)setPath:(NSString *)path
{
    AnyFile *file = (AnyFile *)([self wrapper]->file);
    file->setPath([path UTF8String]);
}
- (AnyC64FileWrapper *)wrapper
{
    return wrapper;
}
- (C64FileType)type
{
    return wrapper->file->type();
}
- (NSString *)typeAsString
{
    return [NSString stringWithUTF8String:wrapper->file->typeAsString()];
}
- (NSString *)name
{
    return [NSString stringWithUTF8String:wrapper->file->getName()];
}
- (NSInteger) sizeOnDisk
{
    return wrapper->file->sizeOnDisk();
}
- (u64) fnv
{
    return wrapper->file->fnv();
}
- (void) readFromBuffer:(const void *)buffer length:(NSInteger)length
{
    wrapper->file->readFromBuffer((const u8 *)buffer, length);
}
- (NSInteger) writeToBuffer:(void *)buffer
{
    return wrapper->file->writeToBuffer((u8 *)buffer);
}

- (void) dealloc
{
    // NSLog(@"AnyC64FileProxy::dealloc");
    
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

+ (BOOL) isSupportedSnapshot:(const void *)buffer length:(NSInteger)length
{
    return Snapshot::isSupportedSnapshot((u8 *)buffer, length);
}
+ (BOOL) isUnsupportedSnapshot:(const void *)buffer length:(NSInteger)length
{
    return Snapshot::isUnsupportedSnapshot((u8 *)buffer, length);
}
+ (BOOL) isSupportedSnapshotFile:(NSString *)path
{
    return Snapshot::isSupportedSnapshotFile([path UTF8String]);
}
+ (BOOL) isUnsupportedSnapshotFile:(NSString *)path
{
    return Snapshot::isUnsupportedSnapshotFile([path UTF8String]);
}
+ (instancetype) make:(Snapshot *)snapshot
{
    if (snapshot == NULL) {
        return nil;
    }
    return [[self alloc] initWithFile:snapshot];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    Snapshot *snapshot = Snapshot::makeWithBuffer((u8 *)buffer, length);
    return [self make:snapshot];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    Snapshot *snapshot = Snapshot::makeWithFile([path UTF8String]);
    return [self make:snapshot];
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
    
    NSInteger width = snapshot->getImageWidth();
    NSInteger height = snapshot->getImageHeight();
    unsigned char *data = snapshot->getImageData();
    
    
    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc]
                             initWithBitmapDataPlanes:&data
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
    return ((Snapshot *)wrapper->file)->getTimestamp();
}
- (NSData *)data
{
    Snapshot *snapshot = (Snapshot *)wrapper->file;
    return [NSData dataWithBytes: (void *)snapshot->getHeader()
                          length: snapshot->sizeOnDisk()];
}

@end

//
// CRT
//

@implementation CRTFileProxy

+ (CartridgeType) typeOfCRTBuffer:(const void *)buffer length:(NSInteger)length
{
    return CRTFile::typeOfCRTBuffer((u8 *)buffer, length);
}
+ (NSString *) typeNameOfCRTBuffer:(const void *)buffer length:(NSInteger)length
{
    const char *str = CRTFile::typeNameOfCRTBuffer((u8 *)buffer, length);
    return [NSString stringWithUTF8String: str];
}
+ (BOOL) isSupportedCRTBuffer:(const void *)buffer length:(NSInteger)length
{
    return CRTFile::isSupportedCRTBuffer((u8 *)buffer, length);
}
+ (BOOL) isUnsupportedCRTBuffer:(const void *)buffer length:(NSInteger)length
{
    return CRTFile::isUnsupportedCRTBuffer((u8 *)buffer, length);
}
+ (BOOL) isCRTFile:(NSString *)path
{
    return CRTFile::isCRTFile([path UTF8String]);
}
+ (instancetype) make:(CRTFile *)container
{
    return container ? [[self alloc] initWithFile:container] : nil;
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    CRTFile *container = CRTFile::makeWithBuffer((const u8 *)buffer, length);
    return [self make: container];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    CRTFile *container = CRTFile::makeWithFile([path UTF8String]);
    return [self make: container];
}

- (CRTFile *)unwrap
{
    return (CRTFile *)wrapper->file;
}
- (CartridgeType)cartridgeType
{
    return [self unwrap]->cartridgeType();
}
- (NSString *)cartridgeTypeName
{
    return [NSString stringWithUTF8String:[self unwrap]->cartridgeTypeName()];
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
- (NSInteger)chipType:(NSInteger)nr;
{
    return [self unwrap]->chipType((unsigned)nr);
}
- (NSInteger)chipAddr:(NSInteger)nr;
{
    return [self unwrap]->chipAddr((unsigned)nr);
}
- (NSInteger)chipSize:(NSInteger)nr;
{
    return [self unwrap]->chipSize((unsigned)nr);
}

@end

//
// TAP
//

@implementation TAPFileProxy

+ (BOOL) isTAPFile:(NSString *)path
{
    return TAPFile::isTAPFile([path UTF8String]);
}
+ (instancetype) make:(TAPFile *)container
{
    return container ? [[self alloc] initWithFile:container] : nil;
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    TAPFile *container = TAPFile::makeWithBuffer((const u8 *)buffer, length);
    return [self make: container];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    TAPFile *container = TAPFile::makeWithFile([path UTF8String]);
    return [self make: container];
}
- (TAPFile *)unwrap
{
    return (TAPFile *)wrapper->file;
}
- (NSInteger)tapVersion
{
    return [self unwrap]->TAPversion();
}

@end

//
// AnyArchive
//

@implementation AnyArchiveProxy

+ (instancetype) make:(AnyArchive *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithFile:archive];
}
+ (instancetype) make
{
    AnyArchive *archive = new AnyArchive();
    return [self make: archive];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    AnyArchive *archive = AnyArchive::makeWithFile([path UTF8String]);
    return [self make: archive];
}

- (AnyArchive *)unwrap
{
    return (AnyArchive *)([self wrapper]->file);
}
- (void)selectItem:(NSInteger)item
{
    return [self unwrap]->selectItem((unsigned)item);
}
- (NSInteger)numberOfItems
{
    return [self unwrap]->numberOfItems();
}
- (NSString *)nameOfItem
{
    const char *chars = [self unwrap]->getNameOfItem();
    return [NSString stringWithUTF8String:chars];
}
- (NSString *)unicodeNameOfItem
{
    const unsigned short *unichars = [self unwrap]->getUnicodeNameOfItem();
    return [NSString stringWithCharacters:unichars length:strlen16(unichars)];
}
- (NSInteger)sizeOfItem
{
    return [self unwrap]->getSizeOfItem();
}
- (NSInteger)sizeOfItemInBlocks
{
    return [self unwrap]->getSizeOfItemInBlocks();
}
- (void)seekItem:(NSInteger)offset
{
    [self unwrap]->seekItem(offset);
}
- (NSString *)typeOfItem
{
    const char *chars = [self unwrap]->getTypeOfItem();
    return [NSString stringWithUTF8String:chars];
}
- (NSInteger)destinationAddrOfItem
{
    return [self unwrap]->getDestinationAddrOfItem();
}
- (NSString *)readItemHex:(NSInteger)num
{
    const char *chars = [self unwrap]->readItemHex(num);
    return [NSString stringWithUTF8String:chars];
}

@end

//
// T64
//

@implementation T64FileProxy

+ (BOOL)isT64File:(NSString *)filename
{
    return T64File::isT64File([filename UTF8String]);
}
+ (instancetype) make:(T64File *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithFile:archive];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    T64File *archive = T64File::makeWithBuffer((const u8 *)buffer, length);
    return [self make: archive];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    T64File *archive = T64File::makeWithFile([path UTF8String]);
    return [self make: archive];
}
+ (instancetype) makeWithAnyArchive:(AnyArchiveProxy *)otherArchive
{
    AnyArchive *other = (AnyArchive *)([otherArchive wrapper]->file);
    T64File *archive = T64File::makeT64ArchiveWithAnyArchive(other);
    return [self make: archive];
}

@end

//
// PRG
//

@implementation PRGFileProxy

+ (BOOL)isPRGFile:(NSString *)filename
{
    return PRGFile::isPRGFile([filename UTF8String]);
}
+ (instancetype) make:(PRGFile *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithFile:archive];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    PRGFile *archive = PRGFile::makeWithBuffer((const u8 *)buffer, length);
    return [self make: archive];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    PRGFile *archive = PRGFile::makeWithFile([path UTF8String]);
    return [self make: archive];
}
+ (instancetype) makeWithAnyArchive:(AnyArchiveProxy *)otherArchive
{
    AnyArchive *other = (AnyArchive *)([otherArchive wrapper]->file);
    PRGFile *archive = PRGFile::makeWithAnyArchive(other);
    return [self make: archive];
}

@end

//
// P00
//

@implementation P00FileProxy

+ (BOOL)isP00File:(NSString *)filename
{
    return P00File::isP00File([filename UTF8String]);
}
+ (instancetype) make:(P00File *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithFile:archive];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    P00File *archive = P00File::makeWithBuffer((const u8 *)buffer, length);
    return [self make: archive];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    P00File *archive = P00File::makeWithFile([path UTF8String]);
    return [self make: archive];
}
+ (instancetype) makeWithAnyArchive:(AnyArchiveProxy *)otherArchive
{
    AnyArchive *other = (AnyArchive *)([otherArchive wrapper]->file);
    P00File *archive = P00File::makeWithAnyArchive(other);
    return [self make: archive];
}

@end

//
// AnyDisk
//

@implementation AnyDiskProxy

+ (instancetype) make:(AnyDisk *)disk
{
    if (disk == NULL) return nil;
    return [[self alloc] initWithFile:disk];
}
+ (instancetype) make
{
    AnyDisk *disk = new AnyDisk();
    return [self make: disk];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    AnyDisk *disk = AnyDisk::makeWithFile([path UTF8String]);
    return [self make: disk];
}

- (NSInteger) numberOfHalftracks
{
    AnyDisk *disk = (AnyDisk *)([self wrapper]->file);
    return disk->numberOfHalftracks();
}
- (void) selectHalftrack:(NSInteger)ht
{
    AnyDisk *disk = (AnyDisk *)([self wrapper]->file);
    disk->selectHalftrack((unsigned)ht);
}
- (NSInteger) sizeOfHalftrack
{
    AnyDisk *disk = (AnyDisk *)([self wrapper]->file);
    return disk->getSizeOfHalftrack();
}
- (void)seekHalftrack:(NSInteger)offset
{
    AnyDisk *disk = (AnyDisk *)([self wrapper]->file);
    return disk->seekHalftrack(offset);
}
- (NSString *)readHalftrackHex:(NSInteger)num
{
    AnyDisk *disk = (AnyDisk *)([self wrapper]->file);
    return [NSString stringWithUTF8String:disk->readHalftrackHex(num)];
}

@end

//
// D64
//

@implementation D64FileProxy

+ (BOOL)isD64File:(NSString *)filename
{
    return D64File::isD64File([filename UTF8String]);
}
+ (instancetype) make:(D64File *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithFile:archive];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    D64File *archive = D64File::makeWithBuffer((const u8 *)buffer, length);
    return [self make: archive];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    D64File *archive = D64File::makeWithFile([path UTF8String]);
    return [self make: archive];
}
+ (instancetype) makeWithAnyArchive:(AnyArchiveProxy *)otherArchive
{
    AnyArchive *other = (AnyArchive *)([otherArchive wrapper]->file);
    D64File *archive = D64File::makeWithAnyArchive(other);
    return [self make: archive];
}
+ (instancetype) makeWithDisk:(DiskProxy *)disk
{
    Disk *d = (Disk *)([disk wrapper]->disk);
    D64File *archive = D64File::makeWithDisk(d);
    return archive ? [self make: archive] : NULL; 
}

@end

//
// G64
//

@implementation G64FileProxy

+ (BOOL)isG64File:(NSString *)filename
{
    return G64File::isG64File([filename UTF8String]);
}
+ (instancetype) make:(G64File *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithFile:archive];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    G64File *archive = G64File::makeWithBuffer((const u8 *)buffer, length);
    return [self make: archive];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    G64File *archive = G64File::makeWithFile([path UTF8String]);
    return [self make: archive];
}
+ (instancetype) makeWithDisk:(DiskProxy *)diskProxy
{
    Disk *disk = [diskProxy wrapper]->disk;
    G64File *archive = G64File::makeWithDisk(disk);
    return [self make: archive];
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
- (BOOL) isReady:(ErrorCode *)error
{
    return wrapper->c64->isReady(error);
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
- (void) requestSnapshot
{
    wrapper->c64->requestSnapshot();
}
- (SnapshotProxy *) latestSnapshot
{
    Snapshot *snapshot = wrapper->c64->latestSnapshot();
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
- (NSInteger) getConfig:(ConfigOption)opt
{
    return wrapper->c64->getConfigItem(opt);
}
- (NSInteger) getConfig:(ConfigOption)opt drive:(DriveID)id
{
    return wrapper->c64->getConfigItem(id, opt);
}
- (BOOL) configure:(ConfigOption)opt value:(NSInteger)val
{
    return wrapper->c64->configure(opt, val);
}
- (BOOL) configure:(ConfigOption)opt enable:(BOOL)val
{
    return wrapper->c64->configure(opt, val ? 1 : 0);
}
- (BOOL) configure:(ConfigOption)opt drive:(DriveID)id value:(NSInteger)val
{
    return wrapper->c64->configure(id, opt, val);
}
- (BOOL) configure:(ConfigOption)opt drive:(DriveID)id enable:(BOOL)val
{
    return wrapper->c64->configure(id, opt, val ? 1 : 0);
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
    return wrapper->c64->hasRom(ROM_BASIC);
}
- (BOOL) hasCharRom
{
    return wrapper->c64->hasRom(ROM_CHAR);
}
- (BOOL) hasKernalRom
{
    return wrapper->c64->hasRom(ROM_KERNAL);
}
- (BOOL) hasVC1541Rom
{
    return wrapper->c64->hasRom(ROM_VC1541);
}
- (BOOL) hasMega65BasicRom
{
    return wrapper->c64->hasMega65Rom(ROM_BASIC);
}
- (BOOL) hasMega65CharRom
{
    return wrapper->c64->hasMega65Rom(ROM_CHAR);
}
- (BOOL) hasMega65KernelRom
{
    return wrapper->c64->hasMega65Rom(ROM_KERNAL);
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
- (BOOL) loadBasicRomFromFile:(NSURL *)url
{
    return wrapper->c64->loadRomFromFile(ROM_BASIC, [[url path] UTF8String]);
}
- (BOOL) loadCharRomFromFile:(NSURL *)url
{
    return wrapper->c64->loadRomFromFile(ROM_CHAR, [[url path] UTF8String]);
}
- (BOOL) loadKernalRomFromFile:(NSURL *)url
{
    return wrapper->c64->loadRomFromFile(ROM_KERNAL, [[url path] UTF8String]);
}
- (BOOL) loadVC1541RomFromFile:(NSURL *)url
{
    return wrapper->c64->loadRomFromFile(ROM_VC1541, [[url path] UTF8String]);
}
- (BOOL) loadBasicRomFromBuffer:(NSData *)data
{
    if (data == NULL) return NO;
    const u8 *bytes = (const u8 *)[data bytes];
    return wrapper->c64->loadRomFromBuffer(ROM_BASIC, bytes, [data length]);
}
- (BOOL) loadCharRomFromBuffer:(NSData *)data
{
    if (data == NULL) return NO;
    const u8 *bytes = (const u8 *)[data bytes];
    return wrapper->c64->loadRomFromBuffer(ROM_CHAR, bytes, [data length]);
}
- (BOOL) loadKernalRomFromBuffer:(NSData *)data
{
    if (data == NULL) return NO;
    const u8 *bytes = (const u8 *)[data bytes];
    return wrapper->c64->loadRomFromBuffer(ROM_KERNAL, bytes, [data length]);
}
- (BOOL) loadVC1541RomFromBuffer:(NSData *)data
{
    if (data == NULL) return NO;
    const u8 *bytes = (const u8 *)[data bytes];
    return wrapper->c64->loadRomFromBuffer(ROM_VC1541, bytes, [data length]);
}
- (BOOL) saveBasicRom:(NSURL *)url
{
    return wrapper->c64->saveRom(ROM_BASIC, [[url path] UTF8String]);
}
- (BOOL) saveCharRom:(NSURL *)url
{
    return wrapper->c64->saveRom(ROM_CHAR, [[url path] UTF8String]);
}
- (BOOL) saveKernalRom:(NSURL *)url
{
    return wrapper->c64->saveRom(ROM_KERNAL, [[url path] UTF8String]);
}
- (BOOL) saveVC1541Rom:(NSURL *)url
{
    return wrapper->c64->saveRom(ROM_VC1541, [[url path] UTF8String]);
}
- (void) deleteBasicRom
{
    wrapper->c64->deleteRom(ROM_BASIC);
}
- (void) deleteKernalRom
{
    wrapper->c64->deleteRom(ROM_KERNAL);
}
- (void) deleteCharRom
{
    wrapper->c64->deleteRom(ROM_CHAR);
}
- (void) deleteVC1541Rom
{
    wrapper->c64->deleteRom(ROM_VC1541);
}
- (RomIdentifier) basicRomIdentifier
{
    return wrapper->c64->romIdentifier(ROM_BASIC);
}
- (RomIdentifier) kernalRomIdentifier
{
    return wrapper->c64->romIdentifier(ROM_KERNAL);
}
- (RomIdentifier) charRomIdentifier
{
    return wrapper->c64->romIdentifier(ROM_CHAR);
}
- (RomIdentifier) vc1541RomIdentifier
{
    return wrapper->c64->romIdentifier(ROM_VC1541);
}
- (NSString *) basicRomTitle
{
    const char *str = wrapper->c64->romTitle(ROM_BASIC);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) charRomTitle
{
    const char *str = wrapper->c64->romTitle(ROM_CHAR);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) kernalRomTitle
{
    const char *str = wrapper->c64->romTitle(ROM_KERNAL);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) vc1541RomTitle
{
    const char *str = wrapper->c64->romTitle(ROM_VC1541);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) basicRomSubTitle
{
    const char *str = wrapper->c64->romSubTitle(ROM_BASIC);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) charRomSubTitle
{
    const char *str = wrapper->c64->romSubTitle(ROM_CHAR);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) kernalRomSubTitle
{
    const char *str = wrapper->c64->romSubTitle(ROM_KERNAL);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) vc1541RomSubTitle
{
    const char *str = wrapper->c64->romSubTitle(ROM_VC1541);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) basicRomRevision
{
    const char *str = wrapper->c64->romRevision(ROM_BASIC);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) charRomRevision
{
    const char *str = wrapper->c64->romRevision(ROM_CHAR);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) kernalRomRevision
{
    const char *str = wrapper->c64->romRevision(ROM_KERNAL);
    return str ? [NSString stringWithUTF8String:str] : NULL;
}
- (NSString *) vc1541RomRevision
{
    const char *str = wrapper->c64->romRevision(ROM_VC1541);
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
- (BOOL) isRom:(NSURL *)url
{
    return
    [self isBasicRom:url] ||
    [self isCharRom:url] ||
    [self isKernalRom:url] ||
    [self isVC1541Rom:url];
}

- (BOOL) loadRom:(NSURL *)url
{
    return
    [self loadBasicRomFromFile:url] ||
    [self loadCharRomFromFile:url] ||
    [self loadKernalRomFromFile:url] ||
    [self loadVC1541RomFromFile:url];
}

// Flashing files
- (BOOL)flash:(AnyC64FileProxy *)file
{
    return wrapper->c64->flash([file wrapper]->file);
}
- (BOOL)flash:(AnyArchiveProxy *)archive item:(NSInteger)nr;
{
    AnyArchive *a = (AnyArchive *)([archive wrapper]->file);
    return wrapper->c64->flash(a, (unsigned)nr);
}

@end
