//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//


#import "C64GUI.h"
#import "C64.h"
#import "VirtualC64-Swift.h"

struct C64Wrapper { C64 *c64; };
struct CpuWrapper { CPU *cpu; };
struct MemoryWrapper { C64Memory *mem; };
struct VicWrapper { VIC *vic; };
struct CiaWrapper { CIA *cia; };
struct KeyboardWrapper { Keyboard *keyboard; };
struct ControlPortWrapper { ControlPort *port; };
struct SidBridgeWrapper { SIDBridge *sid; };
struct IecWrapper { IEC *iec; };
struct ExpansionPortWrapper { ExpansionPort *expansionPort; };
struct Via6522Wrapper { VIA6522 *via; };
struct DiskWrapper { Disk *disk; };
struct DriveWrapper { VC1541 *drive; };
struct DatasetteWrapper { Datasette *datasette; };
struct ContainerWrapper { AnyC64File *container; };

// DEPRECATED
struct SnapshotWrapper { Snapshot *snapshot; };
struct ArchiveWrapper { AnyArchive *archive; };
struct TAPContainerWrapper { TAPFile *tapcontainer; };
struct CRTContainerWrapper { CRTFile *crtcontainer; };


//
// CPU
//

@implementation CPUProxy

- (instancetype) initWithCPU:(CPU *)cpu
{
    if (self = [super init]) {
        wrapper = new CpuWrapper();
        wrapper->cpu = cpu;
    }
    return self;
}

- (CPUInfo) getInfo { return wrapper->cpu->getInfo(); }
- (void) dump { wrapper->cpu->dumpState(); }

- (BOOL) tracing { return wrapper->cpu->tracingEnabled(); }
- (void) setTracing:(BOOL)b {
    if (b) wrapper->cpu->startTracing(); else wrapper->cpu->stopTracing(); }

- (uint16_t) pc { return wrapper->cpu->getPC_at_cycle_0(); }
- (void) setPC:(uint16_t)pc { wrapper->cpu->setPC_at_cycle_0(pc); }
- (void) setSP:(uint8_t)sp { wrapper->cpu->setSP(sp); }
- (void) setA:(uint8_t)a { wrapper->cpu->setA(a); }
- (void) setX:(uint8_t)x { wrapper->cpu->setX(x); }
- (void) setY:(uint8_t)y { wrapper->cpu->setY(y); }
- (void) setNflag:(BOOL)b { wrapper->cpu->setN(b); }
- (void) setZflag:(BOOL)b { wrapper->cpu->setZ(b); }
- (void) setCflag:(BOOL)b { wrapper->cpu->setC(b); }
- (void) setIflag:(BOOL)b { wrapper->cpu->setI(b); }
- (void) setBflag:(BOOL)b { wrapper->cpu->setB(b); }
- (void) setDflag:(BOOL)b { wrapper->cpu->setD(b); }
- (void) setVflag:(BOOL)b { wrapper->cpu->setV(b); }

- (BOOL) breakpoint:(uint16_t)addr { return wrapper->cpu->hardBreakpoint(addr); }
- (void) setBreakpoint:(uint16_t)addr { wrapper->cpu->setHardBreakpoint(addr); }
- (void) deleteBreakpoint:(uint16_t)addr { wrapper->cpu->deleteHardBreakpoint(addr); }
- (void) toggleBreakpoint:(uint16_t)addr { wrapper->cpu->toggleHardBreakpoint(addr); }

- (NSInteger) recordedInstructions { return
    wrapper->cpu->recordedInstructions(); }
- (RecordedInstruction) readRecordedInstruction {
    return wrapper->cpu->readRecordedInstruction(); }
- (RecordedInstruction) readRecordedInstruction:(NSInteger)previous {
    return wrapper->cpu->readRecordedInstruction((unsigned)previous); }

- (DisassembledInstruction) disassemble:(uint16_t)addr hex:(BOOL)h; {
    return wrapper->cpu->disassemble(addr, h); }
- (DisassembledInstruction) disassembleRecordedInstr:(RecordedInstruction)instr hex:(BOOL)h; {
    return wrapper->cpu->disassemble(instr, h); }

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

- (void) dump { wrapper->mem->dumpState(); }

- (NSInteger) ramInitPattern { return wrapper->mem->getRamInitPattern(); }
- (void) setRamInitPattern:(NSInteger)pattern {
    wrapper->mem->setRamInitPattern((RamInitPattern)pattern); }
- (void) eraseWithPattern:(NSInteger)pattern {
    wrapper->mem->eraseWithPattern((RamInitPattern)pattern); }

- (MemoryType) peekSource:(uint16_t)addr { return wrapper->mem->getPeekSource(addr); }
- (MemoryType) pokeTarget:(uint16_t)addr { return wrapper->mem->getPokeTarget(addr); }

- (uint8_t) spypeek:(uint16_t)addr source:(MemoryType)source {
    return wrapper->mem->spypeek(addr, source); }
- (uint8_t) spypeek:(uint16_t)addr { return wrapper->mem->spypeek(addr); }
- (uint8_t) spypeekIO:(uint16_t)addr { return wrapper->mem->spypeekIO(addr); }

- (void) poke:(uint16_t)addr value:(uint8_t)value target:(MemoryType)target {
    wrapper->mem->c64->suspend();
    wrapper->mem->poke(addr, value, target);
    wrapper->mem->c64->resume(); }
- (void) poke:(uint16_t)addr value:(uint8_t)value {
    wrapper->mem->c64->suspend();
    wrapper->mem->poke(addr, value);
    wrapper->mem->c64->resume(); }
- (void) pokeIO:(uint16_t)addr value:(uint8_t)value {
    wrapper->mem->c64->suspend();
    wrapper->mem->pokeIO(addr, value);
    wrapper->mem->c64->resume(); }

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

- (CIAInfo) getInfo { return wrapper->cia->getInfo(); }
- (void) dump { wrapper->cia->dumpState(); }
- (BOOL) tracing { return wrapper->cia->tracingEnabled(); }
- (void) setTracing:(BOOL)b { b ? wrapper->cia->startTracing() : wrapper->cia->stopTracing(); }

- (NSInteger) chipModel { return (NSInteger)wrapper->cia->getChipModel(); }
- (void) setChipModel:(NSInteger)value { wrapper->cia->setChipModel((CIAChipModel)value); }
- (BOOL) emulateTimerBBug { return wrapper->cia->getEmulateTimerBBug(); }
- (void) setEmulateTimerBBug:(BOOL)value { wrapper->cia->setEmulateTimerBBug(value); }

- (void) poke:(uint16_t)addr value:(uint8_t)value {
    wrapper->cia->c64->suspend();
    wrapper->cia->poke(addr, value);
    wrapper->cia->c64->resume();
}

@end


//
// VIC
//

@implementation VICProxy

- (instancetype) initWithVIC:(VIC *)vic
{
    if (self = [super init]) {
        wrapper = new VicWrapper();
        wrapper->vic = vic;
    }
    return self;
}

- (NSInteger) chipModel { return (NSInteger)wrapper->vic->getChipModel(); }
- (void) setChipModel:(NSInteger)value { wrapper->vic->setChipModel((VICChipModel)value); }
- (NSInteger) videoPalette { return (NSInteger)wrapper->vic->videoPalette(); }
- (void) setVideoPalette:(NSInteger)value { wrapper->vic->setVideoPalette((VICPalette)value); }
- (NSInteger) glueLogic { return (NSInteger)wrapper->vic->getGlueLogic(); }
- (void) setGlueLogic:(NSInteger)value { wrapper->vic->setGlueLogic((GlueLogic)value); }
- (BOOL) hasGrayDotBug { return wrapper->vic->hasGrayDotBug(); }
- (BOOL) emulateGrayDotBug { return wrapper->vic->emulateGrayDotBug; }
- (void) setEmulateGrayDotBug:(BOOL)value { wrapper->vic->emulateGrayDotBug = value; }

- (BOOL) isPAL { return wrapper->vic->isPAL(); }
- (BOOL) isNTSC { return wrapper->vic->isNTSC(); }

- (VICInfo) getInfo { return wrapper->vic->getInfo(); }
- (void) dump { wrapper->vic->dumpState(); }
- (SpriteInfo) getSpriteInfo:(NSInteger)sprite { return wrapper->vic->getSpriteInfo((unsigned)sprite); }

- (void *) screenBuffer { return wrapper->vic->screenBuffer(); }
- (NSColor *) color:(NSInteger)nr
{
    assert (0 <= nr && nr < 16);
    
    uint32_t color = wrapper->vic->getColor((unsigned)nr);
    uint8_t r = color & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = (color >> 16) & 0xFF;
    
	return [NSColor colorWithCalibratedRed:(float)r/255.0
                                     green:(float)g/255.0
                                      blue:(float)b/255.0
                                     alpha:1.0];
}
- (double)brightness { return wrapper->vic->getBrightness(); }
- (void)setBrightness:(double)value { wrapper->vic->setBrightness(value); }
- (double)contrast { return wrapper->vic->getContrast(); }
- (void)setContrast:(double)value  { wrapper->vic->setContrast(value); }
- (double)saturation { return wrapper->vic->getSaturation(); }
- (void)setSaturation:(double)value  { wrapper->vic->setSaturation(value); }

- (void) setMemoryBankAddr:(uint16_t)addr { wrapper->vic->setMemoryBankAddr(addr); }
- (void) setScreenMemoryAddr:(uint16_t)addr { wrapper->vic->setScreenMemoryAddr(addr); }
- (void) setCharacterMemoryAddr:(uint16_t)addr { wrapper->vic->setCharacterMemoryAddr(addr); }

- (void) setDisplayMode:(DisplayMode)mode { wrapper->vic->setDisplayMode(mode); }
- (void) setScreenGeometry:(ScreenGeometry)mode { wrapper->vic->setScreenGeometry(mode); }
- (void) setHorizontalRasterScroll:(uint8_t)offset { wrapper->vic->setHorizontalRasterScroll(offset & 0x07); }
- (void) setVerticalRasterScroll:(uint8_t)offset { wrapper->vic->setVerticalRasterScroll(offset & 0x07); }

- (void) setSpriteEnabled:(NSInteger)nr value:(BOOL)flag { wrapper->vic->setSpriteEnabled(nr, flag); }
- (void) toggleSpriteEnabled:(NSInteger)nr { wrapper->vic->toggleSpriteEnabled(nr); }
- (void) setSpriteX:(NSInteger)nr value:(NSInteger)x {
    wrapper->vic->setSpriteX((unsigned)nr, (uint16_t)x); }
- (void) setSpriteY:(NSInteger)nr value:(NSInteger)y {
    wrapper->vic->setSpriteY((unsigned)nr, (uint8_t)y); }
- (void) setSpriteStretchX:(NSInteger)nr value:(BOOL)flag {
    wrapper->vic->setSpriteStretchX((unsigned)nr, flag); }
- (void) toggleSpriteStretchX:(NSInteger)nr {
    wrapper->vic->spriteToggleStretchXFlag((unsigned)nr); }
- (void) setSpriteStretchY:(NSInteger)nr value:(BOOL)flag {
    return wrapper->vic->setSpriteStretchY((unsigned)nr, flag); }
- (void) toggleSpriteStretchY:(NSInteger)nr {
    wrapper->vic->spriteToggleStretchYFlag((unsigned)nr); }
- (void) setSpriteColor:(NSInteger)nr value:(int)c {
    wrapper->vic->setSpriteColor((unsigned)nr, c); }
- (void) setSpritePriority:(NSInteger)nr value:(BOOL)flag {
    wrapper->vic->setSpritePriority((unsigned)nr, flag); }
- (void) toggleSpritePriority:(NSInteger)nr {
    wrapper->vic->toggleSpritePriority((unsigned)nr); }
- (void) setSpriteMulticolor:(NSInteger)nr value:(BOOL)flag {
    wrapper->vic->setSpriteMulticolor((unsigned)nr, flag); }
- (void) toggleSpriteMulticolor:(NSInteger)nr {
    wrapper->vic->toggleMulticolorFlag((unsigned)nr); }

- (void) setIrqOnSpriteSpriteCollision:(BOOL)value {
    wrapper->vic->setIrqOnSpriteSpriteCollision(value); }
- (void) toggleIrqOnSpriteSpriteCollision {
    wrapper->vic-> toggleIrqOnSpriteSpriteCollision(); }
- (void) setIrqOnSpriteBackgroundCollision:(BOOL)value {
    wrapper->vic->setIrqOnSpriteBackgroundCollision(value); }
- (void) toggleIrqOnSpriteBackgroundCollision {
    wrapper->vic->toggleIrqOnSpriteBackgroundCollision(); }

- (void) setRasterInterruptLine:(uint16_t)line {
    wrapper->vic->setRasterInterruptLine(line); }
- (void) setRasterInterruptEnabled:(BOOL)b {
    wrapper->vic->setRasterInterruptEnable(b); }
- (void) toggleRasterInterruptFlag {
    wrapper->vic->toggleRasterInterruptFlag(); }

- (BOOL) hideSprites { return wrapper->vic->hideSprites; }
- (void) setHideSprites:(BOOL)b { wrapper->vic->setHideSprites(b); }
- (BOOL) showIrqLines { return wrapper->vic->markIRQLines; }
- (void) setShowIrqLines:(BOOL)b { wrapper->vic->setShowIrqLines(b); }
- (BOOL) showDmaLines { return wrapper->vic->markDMALines; }
- (void) setShowDmaLines:(BOOL)b { wrapper->vic->setShowDmaLines(b); }

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

- (void) dump { wrapper->sid->dumpState(); }
- (SIDInfo) getInfo { return wrapper->sid->getInfo(); }
- (VoiceInfo) getVoiceInfo:(NSInteger)voice {
    return wrapper->sid->getVoiceInfo((unsigned)voice); }

- (BOOL) reSID { return wrapper->sid->getReSID(); }
- (void) setReSID:(BOOL)b { wrapper->sid->setReSID(b); }
- (BOOL) audioFilter { return wrapper->sid->getAudioFilter(); }
- (void) setAudioFilter:(BOOL)b { wrapper->sid->setAudioFilter(b); }
- (NSInteger) samplingMethod { return (NSInteger)(wrapper->sid->getSamplingMethod()); }
- (void) setSamplingMethod:(NSInteger)value { wrapper->sid->setSamplingMethod((SamplingMethod)value); }
- (NSInteger) chipModel { return (int)(wrapper->sid->getChipModel()); }
- (void) setChipModel:(NSInteger)value {wrapper->sid->setChipModel((SIDChipModel)value); }
- (uint32_t) sampleRate { return wrapper->sid->getSampleRate(); }
- (void) setSampleRate:(uint32_t)rate { wrapper->sid->setSampleRate(rate); }

- (NSInteger) ringbufferSize { return wrapper->sid->ringbufferSize(); }
- (float) ringbufferData:(NSInteger)offset {
    return wrapper->sid->ringbufferData(offset); }
- (double) fillLevel { return wrapper->sid->fillLevel(); }
- (NSInteger) bufferUnderflows { return wrapper->sid->bufferUnderflows; }
- (NSInteger) bufferOverflows { return wrapper->sid->bufferOverflows; }

- (void) readMonoSamples:(float *)target size:(NSInteger)n {
    wrapper->sid->readMonoSamples(target, n); }
- (void) readStereoSamples:(float *)target1 buffer2:(float *)target2 size:(NSInteger)n {
    wrapper->sid->readStereoSamples(target1, target2, n); }
- (void) readStereoSamplesInterleaved:(float *)target size:(NSInteger)n {
    wrapper->sid->readStereoSamplesInterleaved(target, n); }

- (void) rampUp { wrapper->sid->rampUp(); }
- (void) rampUpFromZero { wrapper->sid->rampUpFromZero(); }
- (void) rampDown { wrapper->sid->rampDown(); }

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

- (void) dump { wrapper->iec->dumpState(); }
- (BOOL) tracing { return wrapper->iec->tracingEnabled(); }
- (void) setTracing:(BOOL)b { b ? wrapper->iec->startTracing() : wrapper->iec->stopTracing(); }

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

- (void) dump { wrapper->keyboard->dumpState(); }

- (void) pressKeyAtRow:(NSInteger)row col:(NSInteger)col {
    wrapper->keyboard->pressKey(row, col); }
- (void) pressRestoreKey {
    wrapper->keyboard->pressRestoreKey(); }

- (void) releaseKeyAtRow:(NSInteger)row col:(NSInteger)col {
    wrapper->keyboard->releaseKey(row, col); }
- (void) releaseRestoreKey {
    wrapper->keyboard->releaseRestoreKey(); }
- (void) releaseAll { wrapper->keyboard->releaseAll(); }

- (BOOL) leftShiftIsPressed { return wrapper->keyboard->leftShiftIsPressed(); }
- (BOOL) rightShiftIsPressed { return wrapper->keyboard->rightShiftIsPressed(); }
- (BOOL) controlIsPressed { return wrapper->keyboard->ctrlIsPressed(); }
- (BOOL) commodoreIsPressed { return wrapper->keyboard->commodoreIsPressed(); }

- (BOOL) shiftLockIsHoldDown { return wrapper->keyboard->shiftLockIsHoldDown(); }
- (void) lockShift { wrapper->keyboard->pressShiftLockKey(); }
- (void) unlockShift { wrapper->keyboard->releaseShiftLockKey(); }

- (BOOL) inUpperCaseMode { return wrapper->keyboard->inUpperCaseMode(); }
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

- (void) dump { wrapper->port->dumpState(); }
- (BOOL) autofire { return wrapper->port->getAutofire(); }
- (void) setAutofire:(BOOL)value { return wrapper->port->setAutofire(value); }
- (NSInteger) autofireBullets { return (NSInteger)wrapper->port->getAutofireBullets(); }
- (void) setAutofireBullets:(NSInteger)value { wrapper->port->setAutofireBullets((int)value); }
- (float) autofireFrequency { return wrapper->port->getAutofireFrequency(); }
- (void) setAutofireFrequency:(float)value { wrapper->port->setAutofireFrequency(value); }
- (void) trigger:(JoystickEvent)event { wrapper->port->trigger(event); }

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

- (void) dump { wrapper->expansionPort->dumpState(); }
- (CartridgeType) cartridgeType { return wrapper->expansionPort->getCartridgeType(); }
- (BOOL) cartridgeAttached { return wrapper->expansionPort->getCartridgeAttached(); }
- (BOOL) hasBattery { return wrapper->expansionPort->hasBattery(); }
- (void) setBattery:(BOOL)value { wrapper->expansionPort->setBattery(value); }
- (BOOL) attachGeoRamCartridge:(NSInteger)capacity { return wrapper->expansionPort->attachGeoRamCartridge((uint32_t)capacity); }
- (BOOL) hasFreezeButton { return wrapper->expansionPort->hasFreezeButton(); }
- (void) pressFreezeButton { wrapper->expansionPort->pressFreezeButton(); }
- (void) releaseFreezeButton { wrapper->expansionPort->releaseFreezeButton(); }
- (BOOL) hasResetButton { return wrapper->expansionPort->hasResetButton(); }
- (void) pressResetButton { wrapper->expansionPort->pressResetButton(); }
- (void) releaseResetButton { wrapper->expansionPort->releaseResetButton(); }

@end


//
// 5,25" diskette
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

- (void) dump { wrapper->disk->dumpState(); }
- (BOOL)writeProtected { return wrapper->disk->isWriteProtected(); }
- (void)setWriteProtection:(BOOL)b { wrapper->disk->setWriteProtection(b); }
- (void)toggleWriteProtection { wrapper->disk->toggleWriteProtection(); }
- (NSInteger)nonemptyHalftracks { return (NSInteger)wrapper->disk->nonemptyHalftracks(); }
- (void)analyzeTrack:(Track)t { wrapper->disk->analyzeTrack(t); }
- (void)analyzeHalftrack:(Halftrack)ht { wrapper->disk->analyzeHalftrack(ht); }
- (NSInteger)numErrors { return wrapper->disk->numErrors(); }
- (NSString *)errorMessage:(NSInteger)nr {
    std::string s = wrapper->disk->errorMessage((unsigned)nr);
    return [NSString stringWithUTF8String:s.c_str()]; }
- (NSInteger)firstErroneousBit:(NSInteger)nr {
    return wrapper->disk->firstErroneousBit((unsigned)nr); }
- (NSInteger)lastErroneousBit:(NSInteger)nr {
    return wrapper->disk->lastErroneousBit((unsigned)nr); }
- (SectorInfo)sectorInfo:(Sector)s { return wrapper->disk->sectorLayout(s); }
- (const char *)trackDataAsString { return wrapper->disk->trackDataAsString(); }
- (const char *)diskNameAsString { return wrapper->disk->diskNameAsString(); }
- (const char *)sectorHeaderAsString:(Sector)nr {
    return wrapper->disk->sectorHeaderAsString(nr); }
- (const char *)sectorDataAsString:(Sector)nr {
    return wrapper->disk->sectorDataAsString(nr); }

@end


//
// VIA
//

@implementation VIAProxy

- (instancetype) initWithVIA:(VIA6522 *)via
{
    if (self = [super init]) {
        wrapper = new Via6522Wrapper();
        wrapper->via = via;
    }
    return self;
}

- (void) dump { wrapper->via->dumpState(); }
- (BOOL) tracing { return wrapper->via->tracingEnabled(); }
- (void) setTracing:(BOOL)b { b ? wrapper->via->startTracing() : wrapper->via->stopTracing(); }

@end


//
// VC1541
//

@implementation DriveProxy

@synthesize wrapper, cpu, via1, via2, disk;

- (instancetype) initWithVC1541:(VC1541 *)drive
{
    if (self = [super init]) {
        wrapper = new DriveWrapper();
        wrapper->drive = drive;
        cpu = [[CPUProxy alloc] initWithCPU:&drive->cpu];
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
			assert(0);
			return NULL;
	}
}

- (void) dump { wrapper->drive->dumpState(); }
- (BOOL) tracing { return wrapper->drive->tracingEnabled(); }
- (void) setTracing:(BOOL)b { b ? wrapper->drive->startTracing() : wrapper->drive->stopTracing(); }

- (BOOL) isPoweredOn { return wrapper->drive->isPoweredOn(); }
- (void) powerOn { wrapper->drive->powerOn(); }
- (void) powerOff { wrapper->drive->powerOff(); }
- (void) togglePowerSwitch { wrapper->drive->togglePowerSwitch(); }

- (BOOL) redLED { return wrapper->drive->getRedLED(); }
- (BOOL) hasDisk { return wrapper->drive->hasDisk(); }
- (BOOL) hasModifiedDisk {return wrapper->drive->hasModifiedDisk(); }
- (void) setModifiedDisk:(BOOL)b { wrapper->drive->setModifiedDisk(b); }
- (void) prepareToInsert { wrapper->drive->prepareToInsert(); }
- (void) insertDisk:(ArchiveProxy *)disk {
    AnyArchive *archive = (AnyArchive *)([disk wrapper]->container);
    wrapper->drive->insertDisk(archive);
}
- (void) prepareToEject { wrapper->drive->prepareToEject(); }
- (void) ejectDisk { wrapper->drive->ejectDisk(); }
- (BOOL) writeProtected { return wrapper->drive->disk.isWriteProtected(); }
- (void) setWriteProtection:(BOOL)b { wrapper->drive->disk.setWriteProtection(b); }
- (BOOL) hasWriteProtectedDisk { return wrapper->drive->hasWriteProtectedDisk(); }
- (BOOL) sendSoundMessages { return wrapper->drive->soundMessagesEnabled(); }
- (void) setSendSoundMessages:(BOOL)b { wrapper->drive->setSendSoundMessages(b); }
- (Halftrack) halftrack { return wrapper->drive->getHalftrack(); }
- (void) setTrack:(Track)t { wrapper->drive->setTrack(t); }
- (void) setHalftrack:(Halftrack)ht { wrapper->drive->setHalftrack(ht); }
- (uint16_t) sizeOfCurrentHalftrack { return wrapper->drive->sizeOfCurrentHalftrack(); }
- (uint16_t) offset { return wrapper->drive->getOffset(); }
- (void) setOffset:(uint16_t)value { wrapper->drive->setOffset(value); }
- (uint8_t) readBitFromHead { return wrapper->drive->readBitFromHead(); }
- (void) writeBitToHead:(uint8_t)value { wrapper->drive->writeBitToHead(value); }

- (void) moveHeadUp { wrapper->drive->moveHeadUp(); }
- (void) moveHeadDown { wrapper->drive->moveHeadDown(); }
- (BOOL) isRotating { return wrapper->drive->isRotating(); }
- (void) rotateDisk { wrapper->drive->rotateDisk(); }
- (void) rotateBack { wrapper->drive->rotateBack(); }

// - (BOOL) exportToD64:(NSString *)path { return wrapper->drive->exportToD64([path UTF8String]); }

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

- (void) dump { wrapper->datasette->dumpState(); }
- (BOOL) hasTape { return wrapper->datasette->hasTape(); }
- (void) pressPlay { wrapper->datasette->pressPlay(); }
- (void) pressStop { wrapper->datasette->pressStop(); }
- (void) rewind { wrapper->datasette->rewind(); }
- (void) ejectTape { wrapper->datasette->ejectTape(); }
- (NSInteger) getType { return wrapper->datasette->getType(); }
- (long) durationInCycles { return wrapper->datasette->getDurationInCycles(); }
- (int) durationInSeconds { return wrapper->datasette->getDurationInSeconds(); }
- (NSInteger) head { return wrapper->datasette->getHead(); }
- (NSInteger) headInCycles { return wrapper->datasette->getHeadInCycles(); }
- (int) headInSeconds { return wrapper->datasette->getHeadInSeconds(); }
- (void) setHeadInCycles:(long)value { wrapper->datasette->setHeadInCycles(value); }
- (BOOL) motor { return wrapper->datasette->getMotor(); }
- (BOOL) playKey { return wrapper->datasette->getPlayKey(); }
@end


//
// C64
//

@implementation C64Proxy {
    AudioEngine *audioEngine;
}

@synthesize cpu, mem, vic, cia1, cia2, sid, keyboard, iec;
@synthesize drive1, drive2, expansionport, datasette;
@synthesize port1, port2;

- (instancetype) init
{
	NSLog(@"C64Proxy::init");
	
    if (!(self = [super init]))
        return self;
    
    C64 *c64 = new C64();
    wrapper = new C64Wrapper();
    wrapper->c64 = c64;
	
    // Create sub proxys
    cpu = [[CPUProxy alloc] initWithCPU:&c64->cpu];
    mem = [[MemoryProxy alloc] initWithMemory:&c64->mem];
    vic = [[VICProxy alloc] initWithVIC:&c64->vic];
	cia1 = [[CIAProxy alloc] initWithCIA:&c64->cia1];
	cia2 = [[CIAProxy alloc] initWithCIA:&c64->cia2];
	sid = [[SIDProxy alloc] initWithSID:&c64->sid];
	keyboard = [[KeyboardProxy alloc] initWithKeyboard:&c64->keyboard];
    port1 = [[ControlPortProxy alloc] initWithJoystick:&c64->port1];
    port2 = [[ControlPortProxy alloc] initWithJoystick:&c64->port2];
    iec = [[IECProxy alloc] initWithIEC:&c64->iec];
    expansionport = [[ExpansionPortProxy alloc] initWithExpansionPort:&c64->expansionport];
	drive1 = [[DriveProxy alloc] initWithVC1541:&c64->drive1];
    drive2 = [[DriveProxy alloc] initWithVC1541:&c64->drive2];
    datasette = [[DatasetteProxy alloc] initWithDatasette:&c64->datasette];
    
    // Initialize audio interface
    audioEngine = [[AudioEngine alloc] initWithSID:sid];
    if (!audioEngine) {
        NSLog(@"WARNING: Failed to initialize AudioEngine");
    }

    return self;
}

- (struct C64Wrapper *)wrapper
{
    return wrapper;
}

- (void) kill
{
	assert(wrapper->c64 != NULL);
	NSLog(@"C64Proxy::kill");

	// Stop sound device
	[self disableAudio];
	
    // Delete emulator
    delete wrapper->c64;
	wrapper->c64 = NULL;
}

- (void) dump { wrapper->c64->dumpState(); }
- (BOOL) developmentMode { return wrapper->c64->developmentMode(); }

- (NSInteger) model { return wrapper->c64->getModel(); }
- (void) setModel:(NSInteger)value { wrapper->c64->setModel((C64Model)value); }

- (DriveProxy *) drive:(NSInteger)num {
    switch (num) {
        case 1:
            return [self drive1];
        case 2:
            return [self drive2];
        default:
            assert(false);
            return NULL;
    }
}

/*
- (BOOL)mount:(ContainerProxy *)container {
    return wrapper->c64->mount([container wrapper]->container); }
*/
- (BOOL)flash:(ContainerProxy *)container {
    return wrapper->c64->flash([container wrapper]->container); }
- (BOOL)flash:(ArchiveProxy *)archive item:(NSInteger)item; {
    AnyArchive *a = (AnyArchive *)([archive wrapper]->container);
    return wrapper->c64->flash(a, (unsigned)item); }
- (BOOL) isBasicRom:(NSURL *)url {
    return ROMFile::isBasicRomFile([[url path] UTF8String]); }
- (BOOL) loadBasicRom:(NSURL *)url {
    return [self isBasicRom:url] && wrapper->c64->loadRom([[url path] UTF8String]); }
- (BOOL) isBasicRomLoaded {
    return wrapper->c64->mem.basicRomIsLoaded(); }
- (BOOL) isCharRom:(NSURL *)url {
    return ROMFile::isCharRomFile([[url path] UTF8String]); }
- (BOOL) loadCharRom:(NSURL *)url {
    return [self isCharRom:url] && wrapper->c64->loadRom([[url path] UTF8String]); }
- (BOOL) isCharRomLoaded {
    return wrapper->c64->mem.charRomIsLoaded(); }
- (BOOL) isKernalRom:(NSURL *)url {
    return ROMFile::isKernalRomFile([[url path] UTF8String]); }
- (BOOL) loadKernalRom:(NSURL *)url {
    return [self isKernalRom:url] && wrapper->c64->loadRom([[url path] UTF8String]); }
- (BOOL) isKernalRomLoaded {
    return wrapper->c64->mem.kernalRomIsLoaded(); }
- (BOOL) isVC1541Rom:(NSURL *)url {
    return ROMFile::isVC1541RomFile([[url path] UTF8String]); }
- (BOOL) loadVC1541Rom:(NSURL *)url {
    return [self isVC1541Rom:url] && wrapper->c64->loadRom([[url path] UTF8String]); }
- (BOOL) isVC1541RomLoaded {
    return wrapper->c64->drive1.mem.romIsLoaded() && wrapper->c64->drive2.mem.romIsLoaded(); }
- (BOOL) isRom:(NSURL *)url {
    return [self isBasicRom:url] || [self isCharRom:url] || [self isKernalRom:url] || [self isVC1541Rom:url]; }
- (BOOL) loadRom:(NSURL *)url {
    return [self loadBasicRom:url] || [self loadCharRom:url] || [self loadKernalRom:url] || [self loadVC1541Rom:url]; }

- (Message)message { return wrapper->c64->getMessage(); }
- (void) addListener:(const void *)sender function:(Callback *)func {
    wrapper->c64->addListener(sender, func);
}
- (void) removeListener:(const void *)sender {
    wrapper->c64->removeListener(sender);
}

- (void) powerUp { wrapper->c64->powerUp(); }
- (void) ping { wrapper->c64->ping(); }

- (BOOL) isRunnable { return wrapper->c64->isRunnable(); }
- (BOOL) isRunning { return wrapper->c64->isRunning(); }
- (BOOL) isHalted { return wrapper->c64->isHalted(); }
- (void) suspend { wrapper->c64->suspend(); }
- (void) resume { wrapper->c64->resume(); }
- (void) run { wrapper->c64->run(); }
- (void) halt { wrapper->c64->halt(); }

- (void) step { wrapper->c64->step(); }
- (void) stepOver { wrapper->c64->stepOver(); }

- (BOOL) attachCartridgeAndReset:(CRTProxy *)c {
    return wrapper->c64->attachCartridgeAndReset((CRTFile *)([c wrapper]->container)); }
- (void) detachCartridgeAndReset { wrapper->c64->detachCartridgeAndReset(); }
- (BOOL) isCartridgeAttached { return wrapper->c64->isCartridgeAttached(); }

/* USE mount INSTEAD
- (BOOL) insertDisk:(ArchiveProxy *)a {
    AnyArchive *archive = (AnyArchive *)([a wrapper]->container);
    return wrapper->c64->insertDisk(archive);
}
 */
- (BOOL) insertTape:(TAPProxy *)c {
    TAPFile *container = (TAPFile *)([c wrapper]->container);
    return wrapper->c64->insertTape(container);
}
- (NSInteger) mouseModel { return (NSInteger)wrapper->c64->getMouseModel(); }
- (void) setMouseModel:(NSInteger)model { wrapper->c64->setMouseModel((MouseModel)model); }
- (void) connectMouse:(NSInteger)toPort { wrapper->c64->connectMouse((unsigned)toPort); }
- (void) disconnectMouse { wrapper->c64->connectMouse(0); }
- (void) setMouseXY:(NSPoint)pos {
    wrapper->c64->mouse->setXY((int64_t)pos.x, (int64_t)pos.y);
}
- (void) setMouseLeftButton:(BOOL)pressed { wrapper->c64->mouse->leftButton = pressed; }
- (void) setMouseRightButton:(BOOL)pressed { wrapper->c64->mouse->rightButton = pressed;  }

- (BOOL) warp { return wrapper->c64->getWarp(); }
- (BOOL) alwaysWarp { return wrapper->c64->getAlwaysWarp(); }
- (void) setAlwaysWarp:(BOOL)b { wrapper->c64->setAlwaysWarp(b); }
- (BOOL) warpLoad { return wrapper->c64->getWarpLoad(); }
- (void) setWarpLoad:(BOOL)b { wrapper->c64->setWarpLoad(b); }

- (UInt64) cycles { return wrapper->c64->cycle(); }

// Snapshot storage
- (void) disableAutoSnapshots { wrapper->c64->disableAutoSnapshots(); }
- (void) enableAutoSnapshots { wrapper->c64->enableAutoSnapshots(); }
- (void) suspendAutoSnapshots { wrapper->c64->suspendAutoSnapshots(); }
- (void) resumeAutoSnapshots { wrapper->c64->resumeAutoSnapshots(); }
- (NSInteger) snapshotInterval { return wrapper->c64->getSnapshotInterval(); }
- (void) setSnapshotInterval:(NSInteger)value { wrapper->c64->setSnapshotInterval(value); }
- (NSInteger) numAutoSnapshots { return wrapper->c64->numAutoSnapshots(); }
- (NSData *)autoSnapshotData:(NSInteger)nr {
    Snapshot *snapshot = wrapper->c64->autoSnapshot((unsigned)nr);
    return [NSData dataWithBytes: (void *)snapshot->getHeader()
                          length: snapshot->sizeOnDisk()];
}
- (unsigned char *)autoSnapshotImageData:(NSInteger)nr {
    Snapshot *s = wrapper->c64->autoSnapshot((int)nr); return s ? s->getImageData() : NULL; }
- (NSInteger)autoSnapshotImageWidth:(NSInteger)nr {
    Snapshot *s = wrapper->c64->autoSnapshot((int)nr); return s ? s->getImageWidth() : 0; }
- (NSInteger)autoSnapshotImageHeight:(NSInteger)nr {
    Snapshot *s = wrapper->c64->autoSnapshot((int)nr); return s ? s->getImageHeight() : 0; }
- (time_t)autoSnapshotTimestamp:(NSInteger)nr {
    Snapshot *s = wrapper->c64->autoSnapshot((int)nr); return s ? s->getTimestamp() : 0; }
- (BOOL)restoreAutoSnapshot:(NSInteger)nr { return wrapper->c64->restoreAutoSnapshot((unsigned)nr); }
- (BOOL)restoreLatestAutoSnapshot { return wrapper->c64->restoreLatestAutoSnapshot(); }
- (void)deleteAutoSnapshot:(NSInteger)nr { wrapper->c64->deleteAutoSnapshot((unsigned)nr); }

- (NSInteger) numUserSnapshots { return wrapper->c64->numUserSnapshots(); }
- (NSData *)userSnapshotData:(NSInteger)nr {
    Snapshot *snapshot = wrapper->c64->userSnapshot((unsigned)nr);
    return [NSData dataWithBytes: (void *)snapshot->getHeader()
                          length: snapshot->sizeOnDisk()];
}
- (unsigned char *)userSnapshotImageData:(NSInteger)nr {
    Snapshot *s = wrapper->c64->userSnapshot((int)nr); return s ? s->getImageData() : NULL; }
- (NSInteger)userSnapshotImageWidth:(NSInteger)nr {
    Snapshot *s = wrapper->c64->userSnapshot((int)nr); return s ? s->getImageWidth() : 0; }
- (NSInteger)userSnapshotImageHeight:(NSInteger)nr {
    Snapshot *s = wrapper->c64->userSnapshot((int)nr); return s ? s->getImageHeight() : 0; }
- (time_t)userSnapshotTimestamp:(NSInteger)nr {
    Snapshot *s = wrapper->c64->userSnapshot((int)nr); return s ? s->getTimestamp() : 0; }
- (void)takeUserSnapshot { wrapper->c64->takeUserSnapshotSafe(); }
- (BOOL)restoreUserSnapshot:(NSInteger)nr { return wrapper->c64->restoreUserSnapshot((unsigned)nr); }
- (BOOL)restoreLatestUserSnapshot { return wrapper->c64->restoreLatestUserSnapshot(); }
- (void)deleteUserSnapshot:(NSInteger)nr { wrapper->c64->deleteUserSnapshot((unsigned)nr); }

// Audio hardware
- (BOOL) enableAudio {
    [sid rampUpFromZero];
    return [audioEngine startPlayback];
}

- (void) disableAudio {
    [sid rampDown];
    [audioEngine stopPlayback];
}

@end


//
// Container
//

@implementation ContainerProxy

- (instancetype) initWithContainer:(AnyC64File *)container
{
    if (container == nil) {
        return nil;
    }
    if (self = [super init]) {
        wrapper = new ContainerWrapper();
        wrapper->container = container;
    }
    return self;
}

+ (ContainerProxy *) makeWithContainer:(AnyC64File *)container
{
    if (container == nil) {
        return nil;
    }
    return [[self alloc] initWithContainer:container];
}

- (void)setPath:(NSString *)path {
    AnyC64File *file = (AnyC64File *)([self wrapper]->container);
    file->setPath([path UTF8String]);
}
- (ContainerWrapper *)wrapper { return wrapper; }
- (C64FileType)type { return wrapper->container->type(); }
- (NSString *)name { return [NSString stringWithUTF8String:wrapper->container->getName()]; }
- (NSInteger) sizeOnDisk { return wrapper->container->sizeOnDisk(); }
// - (void)seek:(NSInteger)offset { wrapper->container->seek(offset); }

- (void) readFromBuffer:(const void *)buffer length:(NSInteger)length
{
    wrapper->container->readFromBuffer((const uint8_t *)buffer, length);
}

- (NSInteger) writeToBuffer:(void *)buffer
{
    return wrapper->container->writeToBuffer((uint8_t *)buffer);
}

/*
- (NSString *)readHex:(NSInteger)num
{
    AnyC64File *file = (AnyC64File *)([self wrapper]->container);
    return [NSString stringWithUTF8String:file->readHex(num)];
}
*/

- (void) dealloc
{
    // NSLog(@"ContainerProxy::dealloc");
    
    if (wrapper) {
        if (wrapper->container) delete wrapper->container;
        delete wrapper;
    }
}
@end


//
// SnapshotProxy
//

@implementation SnapshotProxy

+ (BOOL) isSupportedSnapshot:(const void *)buffer length:(NSInteger)length {
    return Snapshot::isSupportedSnapshot((uint8_t *)buffer, length);
}
+ (BOOL) isUnsupportedSnapshot:(const void *)buffer length:(NSInteger)length {
    return Snapshot::isUnsupportedSnapshot((uint8_t *)buffer, length);
}
+ (BOOL) isSupportedSnapshotFile:(NSString *)path {
    return Snapshot::isSupportedSnapshotFile([path UTF8String]);
}
+ (BOOL) isUnsupportedSnapshotFile:(NSString *)path {
    return Snapshot::isUnsupportedSnapshotFile([path UTF8String]);
}

+ (instancetype) make:(Snapshot *)snapshot
{
    if (snapshot == NULL) {
        return nil;
    }
    return [[self alloc] initWithContainer:snapshot];
}

+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    Snapshot *snapshot = Snapshot::makeWithBuffer((uint8_t *)buffer, length);
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
    Snapshot *snapshot = Snapshot::makeWithC64(c64);
    return [self make:snapshot];
}

- (NSInteger)imageWidth
{
    Snapshot *snapshot = (Snapshot *)wrapper->container;
    return snapshot->getImageWidth();
}
- (NSInteger)imageHeight
{
    Snapshot *snapshot = (Snapshot *)wrapper->container;
    return snapshot->getImageHeight();
}
- (uint8_t *)imageData
{
    Snapshot *snapshot = (Snapshot *)wrapper->container;
    return snapshot->getImageData();
}

@end


//
// CRTProxy
//

@implementation CRTProxy

+ (CartridgeType) typeOfCRTBuffer:(const void *)buffer length:(NSInteger)length {
    return CRTFile::typeOfCRTBuffer((uint8_t *)buffer, length); }
+ (NSString *) typeNameOfCRTBuffer:(const void *)buffer length:(NSInteger)length {
    const char *str = CRTFile::typeNameOfCRTBuffer((uint8_t *)buffer, length);
    return [NSString stringWithUTF8String: str]; }
+ (BOOL) isSupportedCRTBuffer:(const void *)buffer length:(NSInteger)length {
    return CRTFile::isSupportedCRTBuffer((uint8_t *)buffer, length); }
+ (BOOL) isUnsupportedCRTBuffer:(const void *)buffer length:(NSInteger)length {
    return CRTFile::isUnsupportedCRTBuffer((uint8_t *)buffer, length); }
+ (BOOL) isCRTFile:(NSString *)path {
    return CRTFile::isCRTFile([path UTF8String]); }
+ (instancetype) make:(CRTFile *)container
{
    if (container == NULL) {
        return nil;
    }
    return [[self alloc] initWithContainer:container];
}

+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    CRTFile *container = CRTFile::makeWithBuffer((const uint8_t *)buffer, length);
    return [self make: container];
}

+ (instancetype) makeWithFile:(NSString *)path
{
    CRTFile *container = CRTFile::makeWithFile([path UTF8String]);
    return [self make: container];
}

- (NSString *)cartridgeName
{
    CRTFile *c = (CRTFile *)wrapper->container;
    return [NSString stringWithUTF8String:c->getName()];
}

- (CartridgeType)cartridgeType {
    CRTFile *c = (CRTFile *)wrapper->container;
    return c->cartridgeType();
}

- (NSString *)cartridgeTypeName {
    CRTFile *c = (CRTFile *)wrapper->container;
    return [NSString stringWithUTF8String:c->cartridgeTypeName()];
}

- (BOOL) isSupported {
    return Cartridge::isSupportedType([self cartridgeType]);
}

- (NSInteger)initialExromLine {
    CRTFile *c = (CRTFile *)wrapper->container;
    return c->initialExromLine();
}

- (NSInteger)initialGameLine {
    CRTFile *c = (CRTFile *)wrapper->container;
    return c->initialGameLine();
}

- (NSInteger)chipCount {
    CRTFile *c = (CRTFile *)wrapper->container;
    return c->chipCount();
}

- (NSInteger)typeOfChip:(NSInteger)nr; {
    CRTFile *c = (CRTFile *)wrapper->container;
    return c->chipType((unsigned)nr);
}

- (NSInteger)loadAddrOfChip:(NSInteger)nr; {
    CRTFile *c = (CRTFile *)wrapper->container;
    return c->chipAddr((unsigned)nr);
}

- (NSInteger)sizeOfChip:(NSInteger)nr; {
    CRTFile *c = (CRTFile *)wrapper->container;
    return c->chipSize((unsigned)nr);
}
@end


//
// TAPProxy
//

@implementation TAPProxy

+ (BOOL) isTAPFile:(NSString *)path
{
    return TAPFile::isTAPFile([path UTF8String]);
}

+ (instancetype) make:(TAPFile *)container
{
    if (container == NULL) {
        return nil;
    }
    return [[self alloc] initWithContainer:container];
}

+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    TAPFile *container = TAPFile::makeWithBuffer((const uint8_t *)buffer, length);
    return [self make: container];
}

+ (instancetype) makeWithFile:(NSString *)path
{
    TAPFile *container = TAPFile::makeWithFile([path UTF8String]);
    return [self make: container];
}

- (NSInteger)TAPversion {
    TAPFile *container = (TAPFile *)wrapper->container;
    return (NSInteger)container->TAPversion();
}
@end


//
// AnyArchive
//

@implementation ArchiveProxy

+ (instancetype) make:(AnyArchive *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithContainer:archive];
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

- (void)selectItem:(NSInteger)item {
    AnyArchive *archive = (AnyArchive *)([self wrapper]->container);
    archive->selectItem((unsigned)item);
}

- (NSInteger)numberOfItems {
    AnyArchive *archive = (AnyArchive *)([self wrapper]->container);
    return (NSInteger)archive->numberOfItems();
}

- (NSString *)nameOfItem {
    AnyArchive *archive = (AnyArchive *)([self wrapper]->container);
    return [NSString stringWithUTF8String:archive->getNameOfItem()];
}

- (NSString *)unicodeNameOfItem {
    AnyArchive *archive = (AnyArchive *)([self wrapper]->container);
    const unsigned short *unichars = archive->getUnicodeNameOfItem();
    return [NSString stringWithCharacters:unichars length:strlen16(unichars)];
}

- (NSInteger)sizeOfItem
{
    AnyArchive *archive = (AnyArchive *)([self wrapper]->container);
    return archive->getSizeOfItem();
}

- (NSInteger)sizeOfItemInBlocks
{
    AnyArchive *archive = (AnyArchive *)([self wrapper]->container);
    return archive->getSizeOfItemInBlocks();
}

- (void)seekItem:(NSInteger)offset
{
    AnyArchive *archive = (AnyArchive *)([self wrapper]->container);
    return archive->seekItem(offset);
}

- (NSString *)typeOfItem
{
    AnyArchive *archive = (AnyArchive *)([self wrapper]->container);
    return [NSString stringWithUTF8String:archive->getTypeOfItemAsString()];
}

- (NSInteger)destinationAddrOfItem
{
    AnyArchive *archive = (AnyArchive *)([self wrapper]->container);
    return archive->getDestinationAddrOfItem();
}

- (NSString *)readItemHex:(NSInteger)num
{
    AnyArchive *archive = (AnyArchive *)([self wrapper]->container);
    return [NSString stringWithUTF8String:archive->readItemHex(num)];
}

@end


//
// T64Proxy
//

@implementation T64Proxy

+ (BOOL)isT64File:(NSString *)filename
{
    return T64File::isT64File([filename UTF8String]);
}
+ (instancetype) make:(T64File *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithContainer:archive];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    T64File *archive = T64File::makeWithBuffer((const uint8_t *)buffer, length);
    return [self make: archive];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    T64File *archive = T64File::makeWithFile([path UTF8String]);
    return [self make: archive];
}
+ (instancetype) makeWithAnyArchive:(ArchiveProxy *)otherArchive
{
    AnyArchive *other = (AnyArchive *)([otherArchive wrapper]->container);
    T64File *archive = T64File::makeT64ArchiveWithAnyArchive(other);
    return [self make: archive];
}
@end


//
// PRGProxy
//

@implementation PRGProxy

+ (BOOL)isPRGFile:(NSString *)filename
{
    return PRGFile::isPRGFile([filename UTF8String]);
}
+ (instancetype) make:(PRGFile *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithContainer:archive];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    PRGFile *archive = PRGFile::makeWithBuffer((const uint8_t *)buffer, length);
    return [self make: archive];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    PRGFile *archive = PRGFile::makeWithFile([path UTF8String]);
    return [self make: archive];
}
+ (instancetype) makeWithAnyArchive:(ArchiveProxy *)otherArchive
{
    AnyArchive *other = (AnyArchive *)([otherArchive wrapper]->container);
    PRGFile *archive = PRGFile::makeWithAnyArchive(other);
    return [self make: archive];
}
@end


//
// P00Proxy
//

@implementation P00Proxy

+ (BOOL)isP00File:(NSString *)filename
{
    return P00File::isP00File([filename UTF8String]);
}
+ (instancetype) make:(P00File *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithContainer:archive];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    P00File *archive = P00File::makeWithBuffer((const uint8_t *)buffer, length);
    return [self make: archive];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    P00File *archive = P00File::makeWithFile([path UTF8String]);
    return [self make: archive];
}
+ (instancetype) makeWithAnyArchive:(ArchiveProxy *)otherArchive
{
    AnyArchive *other = (AnyArchive *)([otherArchive wrapper]->container);
    P00File *archive = P00File::makeWithAnyArchive(other);
    return [self make: archive];
}
@end

//
// AnyDiskProxy
//

@implementation AnyDiskProxy

+ (instancetype) make:(AnyDisk *)disk
{
    if (disk == NULL) return nil;
    return [[self alloc] initWithContainer:disk];
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

- (NSInteger) numberOfHalftracks {
    AnyDisk *disk = (AnyDisk *)([self wrapper]->container);
    return disk->numberOfHalftracks();
}

- (void) selectHalftrack:(NSInteger)ht {
    AnyDisk *disk = (AnyDisk *)([self wrapper]->container);
    disk->selectHalftrack((unsigned)ht);
}

- (NSInteger) sizeOfHalftrack {
    AnyDisk *disk = (AnyDisk *)([self wrapper]->container);
    return disk->getSizeOfHalftrack();
}

- (void)seekHalftrack:(NSInteger)offset {
    AnyDisk *disk = (AnyDisk *)([self wrapper]->container);
    return disk->seekHalftrack(offset);
}

- (NSString *)readHalftrackHex:(NSInteger)num {
    AnyDisk *disk = (AnyDisk *)([self wrapper]->container);
    return [NSString stringWithUTF8String:disk->readHalftrackHex(num)];
}


@end

//
// D64Proxy
//

@implementation D64Proxy

+ (BOOL)isD64File:(NSString *)filename
{
    return D64File::isD64File([filename UTF8String]);
}
+ (instancetype) make:(D64File *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithContainer:archive];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    D64File *archive = D64File::makeWithBuffer((const uint8_t *)buffer, length);
    return [self make: archive];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    D64File *archive = D64File::makeWithFile([path UTF8String]);
    return [self make: archive];
}
+ (instancetype) makeWithAnyArchive:(ArchiveProxy *)otherArchive
{
    AnyArchive *other = (AnyArchive *)([otherArchive wrapper]->container);
    D64File *archive = D64File::makeWithAnyArchive(other);
    return [self make: archive];
}
+ (instancetype) makeWithDisk:(DiskProxy *)disk
{
    Disk *d = (Disk *)([disk wrapper]->disk);
    D64File *archive = D64File::makeWithDisk(d);
    return [self make: archive];
}
/*
+ (instancetype) makeWithDrive:(DriveProxy *)drive
{
    D64File *archive = [drive wrapper]->drive->convertToD64();
    return [self make: archive];
}
*/
@end


//
// PG64Proxy
//

@implementation G64Proxy

+ (BOOL)isG64File:(NSString *)filename
{
    return G64File::isG64File([filename UTF8String]);
}
+ (instancetype) make:(G64File *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithContainer:archive];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    G64File *archive = G64File::makeWithBuffer((const uint8_t *)buffer, length);
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
