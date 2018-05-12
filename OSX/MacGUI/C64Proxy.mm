/*
 * Author: Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#import "C64GUI.h"
#import "C64.h"
#import "VirtualC64-Swift.h"

struct C64Wrapper { C64 *c64; };
struct CpuWrapper { CPU *cpu; };
struct MemoryWrapper { Memory *mem; };
struct VicWrapper { VIC *vic; };
struct CiaWrapper { CIA *cia; };
struct KeyboardWrapper { Keyboard *keyboard; };
struct ControlPortWrapper { ControlPort *port; };
struct SidBridgeWrapper { SIDBridge *sid; };
struct IecWrapper { IEC *iec; };
struct ExpansionPortWrapper { ExpansionPort *expansionPort; };
struct Via6522Wrapper { VIA6522 *via; };
struct Disk525Wrapper { Disk525 *disk; };
struct Vc1541Wrapper { VC1541 *vc1541; };
struct DatasetteWrapper { Datasette *datasette; };
struct ContainerWrapper { Container *container; };

// DEPRECATED
struct SnapshotWrapper { Snapshot *snapshot; };
struct ArchiveWrapper { Archive *archive; };
struct TAPContainerWrapper { TAPContainer *tapcontainer; };
struct CRTContainerWrapper { CRTContainer *crtcontainer; };


// --------------------------------------------------------------------------
//                                    CPU
// --------------------------------------------------------------------------

@implementation CPUProxy

- (instancetype) initWithCPU:(CPU *)cpu
{
    if (self = [super init]) {
        wrapper = new CpuWrapper();
        wrapper->cpu = cpu;
    }
    return self;
}

- (void) dump { wrapper->cpu->dumpState(); }
- (bool) tracingEnabled { return wrapper->cpu->tracingEnabled(); }
- (void) setTraceMode:(bool)b {
    if (b) wrapper->cpu->startTracing(); else wrapper->cpu->stopTracing(); }

- (uint16_t) PC { return wrapper->cpu->getPC_at_cycle_0(); }
- (void) setPC:(uint16_t)pc { wrapper->cpu->setPC_at_cycle_0(pc); }
- (uint8_t) SP { return wrapper->cpu->getSP(); }
- (void) setSP:(uint8_t)sp { wrapper->cpu->setSP(sp); }
- (uint8_t) A { return wrapper->cpu->getA(); }
- (void) setA:(uint8_t)a { wrapper->cpu->setA(a); }
- (uint8_t) X { return wrapper->cpu->getX(); }
- (void) setX:(uint8_t)x { wrapper->cpu->setX(x); }
- (uint8_t) Y { return wrapper->cpu->getY(); }
- (void) setY:(uint8_t)y { wrapper->cpu->setY(y); }
- (bool) Nflag { return wrapper->cpu->getN(); }
- (void) setNflag:(bool)b { wrapper->cpu->setN(b); }
- (bool) Zflag { return wrapper->cpu->getZ(); }
- (void) setZflag:(bool)b { wrapper->cpu->setZ(b); }
- (bool) Cflag { return wrapper->cpu->getC(); }
- (void) setCflag:(bool)b { wrapper->cpu->setC(b); }
- (bool) Iflag { return wrapper->cpu->getI(); }
- (void) setIflag:(bool)b { wrapper->cpu->setI(b); }
- (bool) Bflag { return wrapper->cpu->getB(); }
- (void) setBflag:(bool)b { wrapper->cpu->setB(b); }
- (bool) Dflag { return wrapper->cpu->getD(); }
- (void) setDflag:(bool)b { wrapper->cpu->setD(b); }
- (bool) Vflag { return wrapper->cpu->getV(); }
- (void) setVflag:(bool)b { wrapper->cpu->setV(b); }

- (uint16_t) readPC {
    return wrapper->cpu->mem->read(wrapper->cpu->getPC_at_cycle_0()); }
- (uint8_t) lengthOfInstruction:(uint8_t)opcode {
    return wrapper->cpu->getLengthOfInstruction(opcode); }
 - (uint8_t) lengthOfInstructionAtAddress:(uint16_t)addr {
    return wrapper->cpu->getLengthOfInstructionAtAddress(addr); }
- (uint16_t) addressOfNextInstruction {
    return wrapper->cpu->getAddressOfNextInstruction(); }
- (DisassembledInstruction) disassemble:(uint16_t)addr hex:(BOOL)h; {
    return wrapper->cpu->disassemble(addr, 0, h);
}

- (int) topOfCallStack { return wrapper->cpu->getTopOfCallStack(); }
- (int) breakpoint:(uint16_t)addr { return wrapper->cpu->getBreakpoint(addr); }
- (void) setBreakpoint:(uint16_t)addr tag:(uint8_t)t { wrapper->cpu->setBreakpoint(addr, t); }
- (void) setHardBreakpoint:(uint16_t)addr { wrapper->cpu->setHardBreakpoint(addr); }
- (void) deleteHardBreakpoint:(uint16_t)addr { wrapper->cpu->deleteHardBreakpoint(addr); }
- (void) toggleHardBreakpoint:(uint16_t)addr { wrapper->cpu->toggleHardBreakpoint(addr); }
- (void) setSoftBreakpoint:(uint16_t)addr { wrapper->cpu->setSoftBreakpoint(addr); }
- (void) deleteSoftBreakpoint:(uint16_t)addr { wrapper->cpu->deleteSoftBreakpoint(addr); }
- (void) toggleSoftBreakpoint:(uint16_t)addr { wrapper->cpu->toggleSoftBreakpoint(addr); }

@end


// --------------------------------------------------------------------------
//                                   Memory
// --------------------------------------------------------------------------

@implementation MemoryProxy

- (instancetype) initWithMemory:(Memory *)mem
{
    if (self = [super init]) {
        wrapper = new MemoryWrapper();
        wrapper->mem = mem;
    }
    return self;
}

- (void) dump { wrapper->mem->dumpState(); }

- (uint8_t) read:(uint16_t)addr {
    return wrapper->mem->read(addr); }
- (uint16_t) readWord:(uint16_t)addr {
    return wrapper->mem->readWord(addr); }
- (uint8_t) readFrom:(uint16_t)addr memtype:(MemoryType)type {
    return wrapper->mem->readFrom(addr, type); }
- (void) poke:(uint16_t)addr value:(uint8_t)val {
    wrapper->mem->poke(addr, val); }
- (void) pokeTo:(uint16_t)addr value:(uint8_t)val memtype:(MemoryType)type {
    wrapper->mem->pokeTo(addr, val, type); }
- (bool) isValidAddr:(uint16_t)addr memtype:(MemoryType)type {
    return wrapper->mem->isValidAddr(addr, type); }

@end


// --------------------------------------------------------------------------
//                                    VIC
// --------------------------------------------------------------------------

@implementation VICProxy

- (instancetype) initWithVIC:(VIC *)vic
{
    if (self = [super init]) {
        wrapper = new VicWrapper();
        wrapper->vic = vic;
    }
    return self;
}

- (void) dump { wrapper->vic->dumpState(); }

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
- (NSInteger) colorScheme { return wrapper->vic->getColorScheme(); }
- (void) setColorScheme:(NSInteger)scheme { wrapper->vic->setColorScheme((ColorScheme)scheme); }

- (uint16_t) memoryBankAddr { return wrapper->vic->getMemoryBankAddr(); }
- (void) setMemoryBankAddr:(uint16_t)addr { wrapper->vic->setMemoryBankAddr(addr); }
- (uint16_t) screenMemoryAddr { return wrapper->vic->getScreenMemoryAddr(); }
- (void) setScreenMemoryAddr:(uint16_t)addr { wrapper->vic->setScreenMemoryAddr(addr); }
- (uint16_t) characterMemoryAddr { return wrapper->vic->getCharacterMemoryAddr(); }
- (void) setCharacterMemoryAddr:(uint16_t)addr { wrapper->vic->setCharacterMemoryAddr(addr); }

- (int) displayMode { return wrapper->vic->getDisplayMode(); }
- (void) setDisplayMode:(long)mode { wrapper->vic->setDisplayMode((DisplayMode)mode); }
- (int) screenGeometry { return (int)wrapper->vic->getScreenGeometry(); }
- (void) setScreenGeometry:(long)mode { wrapper->vic->setScreenGeometry((ScreenGeometry)mode); }
- (int) horizontalRasterScroll { return wrapper->vic->getHorizontalRasterScroll(); }
- (void) setHorizontalRasterScroll:(int)offset { wrapper->vic->setHorizontalRasterScroll(offset & 0x07); }
- (int) verticalRasterScroll { return wrapper->vic->getVerticalRasterScroll(); }
- (void) setVerticalRasterScroll:(int)offset { wrapper->vic->setVerticalRasterScroll(offset & 0x07); }

- (bool) spriteVisibilityFlag:(NSInteger)nr { return wrapper->vic->spriteIsEnabled(nr); }
- (void) setSpriteVisibilityFlag:(NSInteger)nr value:(bool)flag { wrapper->vic->setSpriteEnabled(nr, flag); }
- (void) toggleSpriteVisibilityFlag:(NSInteger)nr { wrapper->vic->toggleSpriteEnabled(nr); }

- (int) spriteX:(NSInteger)nr { return wrapper->vic->getSpriteX(nr); }
- (void) setSpriteX:(NSInteger)nr value:(int)x { wrapper->vic->setSpriteX(nr, x); }
- (int) spriteY:(NSInteger)nr { return wrapper->vic->getSpriteY(nr); }
- (void) setSpriteY:(NSInteger)nr value:(int)y { wrapper->vic->setSpriteY(nr, y); }

- (int) spriteColor:(NSInteger)nr { return wrapper->vic->getSpriteColor(nr); }
- (void) setSpriteColor:(NSInteger)nr value:(int)c { wrapper->vic->setSpriteColor(nr, c); }
- (bool) spriteMulticolorFlag:(NSInteger)nr { return wrapper->vic->spriteIsMulticolor((unsigned)nr); }
- (void) setSpriteMulticolorFlag:(NSInteger)nr value:(bool)flag { wrapper->vic->setSpriteMulticolor((unsigned)nr, flag); }
- (void) toggleSpriteMulticolorFlag:(NSInteger)nr { wrapper->vic->toggleMulticolorFlag((unsigned)nr); }

- (bool) spriteStretchXFlag:(NSInteger)nr { return wrapper->vic->spriteWidthIsDoubled((unsigned)nr); }
- (void) setSpriteStretchXFlag:(NSInteger)nr value:(bool)flag { wrapper->vic->setSpriteStretchX((unsigned)nr, flag); }
- (void) toggleSpriteStretchXFlag:(NSInteger)nr { wrapper->vic->spriteToggleStretchXFlag((unsigned)nr); }

- (bool) spriteStretchYFlag:(NSInteger)nr { return wrapper->vic->spriteHeightIsDoubled((unsigned)nr); }
- (void) setSpriteStretchYFlag:(NSInteger)nr value:(bool)flag { return wrapper->vic->setSpriteStretchY((unsigned)nr, flag); }
- (void) toggleSpriteStretchYFlag:(NSInteger)nr { wrapper->vic->spriteToggleStretchYFlag((unsigned)nr); }

- (bool) spriteSpriteCollisionFlag { return wrapper->vic->getSpriteSpriteCollisionFlag(); }
- (void) setSpriteSpriteCollisionFlag:(bool)flag { wrapper->vic->setSpriteSpriteCollisionFlag(flag); }
- (void) toggleSpriteSpriteCollisionFlag { wrapper->vic->toggleSpriteSpriteCollisionFlag(); }

- (bool) spriteBackgroundCollisionFlag { return wrapper->vic->getSpriteBackgroundCollisionFlag(); }
- (void) setSpriteBackgroundCollisionFlag:(bool)flag { wrapper->vic->setSpriteBackgroundCollisionFlag(flag); }
- (void) toggleSpriteBackgroundCollisionFlag { wrapper->vic->toggleSpriteBackgroundCollisionFlag(); }

- (uint16_t) rasterline { return wrapper->vic->getScanline(); }
- (void) setRasterline:(uint16_t)line { wrapper->vic->setScanline(line); }
- (uint16_t) rasterInterruptLine { return wrapper->vic->rasterInterruptLine(); }
- (void) setRasterInterruptLine:(uint16_t)line { wrapper->vic->setRasterInterruptLine(line); }
- (bool) rasterInterruptFlag { return wrapper->vic->rasterInterruptEnabled(); }
- (void) setRasterInterruptFlag:(bool)b { wrapper->vic->setRasterInterruptEnable(b); }
- (void) toggleRasterInterruptFlag { wrapper->vic->toggleRasterInterruptFlag(); }

- (bool) hideSprites { return wrapper->vic->hideSprites(); }
- (void) setHideSprites:(bool)b { wrapper->vic->setHideSprites(b); }
- (bool) showIrqLines { return wrapper->vic->showIrqLines(); }
- (void) setShowIrqLines:(bool)b { wrapper->vic->setShowIrqLines(b); }
- (bool) showDmaLines { return wrapper->vic->showDmaLines(); }
- (void) setShowDmaLines:(bool)b { wrapper->vic->setShowDmaLines(b); }

@end

// --------------------------------------------------------------------------
//                                     CIA
// --------------------------------------------------------------------------

@implementation CIAProxy

- (instancetype) initWithCIA:(CIA *)cia
{
    if (self = [super init]) {
        wrapper = new CiaWrapper();
        wrapper->cia = cia;
    }
    return self;
}

- (void) dump { wrapper->cia->dumpState(); }
- (void) setTraceMode:(bool)b {
    if (b) wrapper->cia->startTracing(b); else wrapper->cia->stopTracing(); }
- (CIAInfo) getInfo { return wrapper->cia->getInfo(); }

@end 

// --------------------------------------------------------------------------
//                                    Keyboard
// --------------------------------------------------------------------------

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

- (BOOL) shiftLockIsPressed { return wrapper->keyboard->shiftLockIsPressed(); }
- (void) lockShift { wrapper->keyboard->pressShiftLockKey(); }
- (void) unlockShift { wrapper->keyboard->releaseShiftLockKey(); }

@end


// --------------------------------------------------------------------------
//                                 Joystick
// -------------------------------------------------------------------------

@implementation ControlPortProxy

- (instancetype) initWithJoystick:(ControlPort *)port
{
    if (self = [super init]) {
        wrapper = new ControlPortWrapper();
        wrapper->port = port;
    }
    return self;
}

- (void) trigger:(JoystickEvent)event { wrapper->port->trigger(event); }
- (void) dump { wrapper->port->dumpState(); }
// - (NSInteger) potX { return wrapper->sid->getPotX(); }
// - (NSInteger) potY { return wrapper->sid->getPotY(); }
- (void) setMouseX:(NSInteger)value { wrapper->port->mouseX = (uint32_t)value; }
- (void) setMouseY:(NSInteger)value { wrapper->port->mouseY = (uint32_t)value; }
- (void) setMouseTargetX:(NSInteger)value { wrapper->port->mouseTargetX = (uint32_t)value; }
- (void) setMouseTargetY:(NSInteger)value { wrapper->port->mouseTargetY = (uint32_t)value; }


@end

// --------------------------------------------------------------------------
//                                    SID
// --------------------------------------------------------------------------

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
- (uint32_t) sampleRate { return wrapper->sid->getSampleRate(); }
- (void) setSampleRate:(uint32_t)rate { wrapper->sid->setSampleRate(rate); }
- (void) readMonoSamples:(float *)target size:(NSInteger)n {
    wrapper->sid->readMonoSamples(target, n);
}
- (void) readStereoSamples:(float *)target1 buffer2:(float *)target2 size:(NSInteger)n {
    wrapper->sid->readStereoSamples(target1, target2, n);
}
- (void) readStereoSamplesInterleaved:(float *)target size:(NSInteger)n {
    wrapper->sid->readStereoSamplesInterleaved(target, n);
}

@end

// --------------------------------------------------------------------------
//                                   IEC bus
// -------------------------------------------------------------------------

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
- (bool) tracingEnabled { return wrapper->iec->tracingEnabled(); }
- (void) setTraceMode:(bool)b {
    if (b) wrapper->iec->startTracing(); else wrapper->iec->stopTracing(); }
- (void) connectDrive { wrapper->iec->connectDrive(); }
- (void) disconnectDrive { wrapper->iec->disconnectDrive(); }
- (bool) isDriveConnected { return wrapper->iec->driveIsConnected(); }
- (BOOL) atnLine { return wrapper->iec->getAtnLine(); }
- (BOOL) clockLine { return wrapper->iec->getClockLine(); }
- (BOOL) dataLine { return wrapper->iec->getDataLine(); }

@end

// --------------------------------------------------------------------------
//                                 Expansion port
// -------------------------------------------------------------------------

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
- (bool) cartridgeAttached { return wrapper->expansionPort->getCartridgeAttached(); }
- (void) pressFirstButton { wrapper->expansionPort->pressFirstButton(); }
- (void) pressSecondButton { wrapper->expansionPort->pressSecondButton(); }

@end

// --------------------------------------------------------------------------
//                                     VIA
// -------------------------------------------------------------------------

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
- (bool) tracingEnabled { return wrapper->via->tracingEnabled(); }
- (void) setTraceMode:(bool)b {
    if (b) wrapper->via->startTracing(); else wrapper->via->stopTracing(); }

@end

// -------------------------------------------------------------------------
//                                5,25" diskette
// -------------------------------------------------------------------------

@implementation Disk525Proxy

- (instancetype) initWithDisk525:(Disk525 *)disk
{
    if (self = [super init]) {
        wrapper = new Disk525Wrapper();
        wrapper->disk = disk;
    }
    return self;
}

- (BOOL)isWriteProtected { return wrapper->disk->isWriteProtected(); }
- (void)setWriteProtection:(BOOL)b { wrapper->disk->setWriteProtection(b); }
- (BOOL)isModified { return wrapper->disk->isModified(); }
- (void)setModified:(BOOL)b { wrapper->disk->setModified(b); }
- (NSInteger)numTracks { return (NSInteger)wrapper->disk->numTracks; }

@end

// -------------------------------------------------------------------------
//                                    VC1541
// -------------------------------------------------------------------------

@implementation VC1541Proxy

@synthesize wrapper, cpu, mem, via1, via2, disk;

- (instancetype) initWithVC1541:(VC1541 *)vc1541
{
    if (self = [super init]) {
        wrapper = new Vc1541Wrapper();
        wrapper->vc1541 = vc1541;
        cpu = [[CPUProxy alloc] initWithCPU:&vc1541->cpu];
        mem = [[MemoryProxy alloc] initWithMemory:&vc1541->mem];
        via1 = [[VIAProxy alloc] initWithVIA:&vc1541->via1];
        via2 = [[VIAProxy alloc] initWithVIA:&vc1541->via2];
        disk = [[Disk525Proxy alloc] initWithDisk525:&vc1541->disk];
    }
    return self;
}

- (VIAProxy *) via:(int)num {
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

- (void) dump { wrapper->vc1541->dumpState(); }
- (bool) tracingEnabled { return wrapper->vc1541->tracingEnabled(); }
- (void) setTraceMode:(bool)b {
    if (b) wrapper->vc1541->startTracing(); else wrapper->vc1541->stopTracing(); }
- (bool) hasRedLED { return wrapper->vc1541->getRedLED(); }
- (bool) hasDisk { return wrapper->vc1541->hasDisk(); }
- (void) ejectDisk { wrapper->vc1541->ejectDisk(); }
- (bool) writeProtection { return wrapper->vc1541->disk.isWriteProtected(); }
- (void) setWriteProtection:(bool)b { wrapper->vc1541->disk.setWriteProtection(b); }
- (bool) DiskModified { return wrapper->vc1541->disk.isModified(); }
- (void) setDiskModified:(bool)b { wrapper->vc1541->disk.setModified(b); }
- (bool) soundMessagesEnabled { return wrapper->vc1541->soundMessagesEnabled(); }
- (void) setSendSoundMessages:(bool)b { wrapper->vc1541->setSendSoundMessages(b); }
- (NSInteger) halftrack { return wrapper->vc1541->getHalftrack(); }
- (void) setHalftrack:(NSInteger) value { wrapper->vc1541->setHalftrack((Halftrack)value); }
- (NSInteger) numberOfBits { return wrapper->vc1541->numberOfBits(); }
- (NSInteger) bitOffset { return wrapper->vc1541->getBitOffset(); }
- (void) setBitOffset:(NSInteger)value { wrapper->vc1541->setBitOffset((uint16_t)value); }
- (NSInteger) readBitFromHead { return wrapper->vc1541->readBitFromHead(); }
- (void) writeBitToHead:(NSInteger)value { wrapper->vc1541->writeBitToHead(value); }

- (void) moveHeadUp { wrapper->vc1541->moveHeadUp(); }
- (void) moveHeadDown { wrapper->vc1541->moveHeadDown(); }
- (void) rotateDisk { wrapper->vc1541->rotateDisk(); }
- (void) rotateBack { wrapper->vc1541->rotateBack(); }

- (const char *)dataAbs:(NSInteger)start {
    return wrapper->vc1541->dataAbs((int)start); }
- (const char *)dataAbs:(NSInteger)start length:(NSInteger)n {
    return wrapper->vc1541->dataAbs((int)start, (unsigned)n); }
- (const char *)dataRel:(NSInteger)start {
    return wrapper->vc1541->dataRel((int)start); }
- (const char *)dataRel:(NSInteger)start length:(NSInteger)n {
    return wrapper->vc1541->dataRel((int)start, (unsigned)n); }

- (bool) exportToD64:(NSString *)path { return wrapper->vc1541->exportToD64([path UTF8String]); }
- (void) playSound:(NSString *)name volume:(float)v
{
    NSSound *s = [NSSound soundNamed:name];
    [s setVolume:v];
    [s play];
}

@end

// --------------------------------------------------------------------------
//                                    Datasette
// -------------------------------------------------------------------------

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
- (bool) hasTape { return wrapper->datasette->hasTape(); }
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

// --------------------------------------------------------------------------
//                                     C64
// --------------------------------------------------------------------------

@implementation C64Proxy {
    AudioEngine *audioEngine;
}

@synthesize cpu, mem, vic, cia1, cia2, sid, keyboard, iec, expansionport, vc1541, datasette;
@synthesize port1, port2;
@synthesize iecBusIsBusy, tapeBusIsBusy;

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
    // cpu = [[CPUProxy alloc] initWithCPU:&c64->floppy->cpu];
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
	vc1541 = [[VC1541Proxy alloc] initWithVC1541:&c64->floppy];
    datasette = [[DatasetteProxy alloc] initWithDatasette:&c64->datasette];
    
    // Initialize audio interface
    audioEngine = [[AudioEngine alloc] initWithSID:sid];
    if (!audioEngine) {
        NSLog(@"WARNING: Failed to initialize AudioEngine");
    }

    return self;
}

/*
- (void) awakeFromNib
{
}
*/

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

- (bool) audioFilter { return wrapper->c64->getAudioFilter(); }
- (void) setAudioFilter:(bool)b { wrapper->c64->setAudioFilter(b); }
- (bool) reSID { return wrapper->c64->getReSID(); }
- (void) setReSID:(bool)b { wrapper->c64->setReSID(b); }
- (int) samplingMethod { return (int)(wrapper->c64->getSamplingMethod()); }
- (void) setSamplingMethod:(long)value { wrapper->c64->setSamplingMethod((SamplingMethod)value); }
- (int) chipModel { return (int)(wrapper->c64->getChipModel()); }
- (void) setChipModel:(long)value {wrapper->c64->setChipModel((SIDChipModel)value); }
- (void) rampUp { wrapper->c64->sid.rampUp(); }
- (void) rampUpFromZero { wrapper->c64->sid.rampUpFromZero(); }
- (void) rampDown { wrapper->c64->sid.rampDown(); }

// DEPRECATED
- (void) _loadFromSnapshotWrapper:(ContainerWrapper *)containerWrapper
{
    Snapshot *snapshot = (Snapshot *)(containerWrapper->container);
    wrapper->c64->loadFromSnapshotSafe(snapshot);
}

- (void) loadFromSnapshot:(SnapshotProxy *)snapshot
{
    [self _loadFromSnapshotWrapper:[snapshot wrapper]];
}

- (void) _saveToSnapshotWrapper:(ContainerWrapper *)containerWrapper
{
    Snapshot *snapshot = (Snapshot *)(containerWrapper->container);
    wrapper->c64->saveToSnapshotSafe(snapshot);
}

- (void) saveToSnapshot:(SnapshotProxy *)snapshot
{
    [self _saveToSnapshotWrapper:[snapshot wrapper]];
}

- (CIAProxy *) cia:(int)num { assert(num == 1 || num == 2); return (num == 1) ? [self cia1] : [self cia2]; }

- (void) dump { wrapper->c64->dumpState(); }
- (BOOL) developmentMode { return wrapper->c64->developmentMode(); }
- (VC64Message)message { return wrapper->c64->getMessage(); }
- (void) putMessage:(VC64Message)msg { wrapper->c64->putMessage(msg); }
- (void) setListener:(const void *)sender function:(void(*)(const void *, int))func {
    wrapper->c64->setListener(sender, func);
}

- (void) powerUp { wrapper->c64->powerUp(); }
// - (void) reset { wrapper->c64->reset(); }
- (void) ping { wrapper->c64->ping(); }
- (void) halt { wrapper->c64->halt(); }
- (void) step { wrapper->c64->step(); }
- (void) run { wrapper->c64->run(); }
- (void) suspend { wrapper->c64->suspend(); }
- (void) resume { wrapper->c64->resume(); }
- (bool) isHalted { return wrapper->c64->isHalted(); }
- (bool) isRunnable { return wrapper->c64->isRunnable(); }
- (bool) isRunning { return wrapper->c64->isRunning(); }
- (bool) isPAL { return wrapper->c64->isPAL(); }
- (bool) isNTSC { return wrapper->c64->isNTSC(); }
- (void) setPAL { wrapper->c64->setPAL(); }
- (void) setNTSC { wrapper->c64->setNTSC(); }
- (void) setNTSC:(BOOL)b { if (b) [self setNTSC]; else [self setPAL]; }

- (bool) isBasicRom:(NSURL *)url {
    return wrapper->c64->mem.isBasicRom([[url path] UTF8String]);
}
- (bool) loadBasicRom:(NSURL *)url {
    return [self isBasicRom:url] && wrapper->c64->loadRom([[url path] UTF8String]);
}
- (bool) isBasicRomLoaded {
    return wrapper->c64->mem.basicRomIsLoaded();
}
- (bool) isCharRom:(NSURL *)url {
    return wrapper->c64->mem.isCharRom([[url path] UTF8String]);
}
- (bool) loadCharRom:(NSURL *)url {
    return [self isCharRom:url] && wrapper->c64->loadRom([[url path] UTF8String]);
}
- (bool) isCharRomLoaded {
    return wrapper->c64->mem.charRomIsLoaded();
}
- (bool) isKernalRom:(NSURL *)url {
    return wrapper->c64->mem.isKernalRom([[url path] UTF8String]);
}
- (bool) loadKernalRom:(NSURL *)url {
    return [self isKernalRom:url] && wrapper->c64->loadRom([[url path] UTF8String]);
}
- (bool) isKernalRomLoaded {
    return wrapper->c64->mem.kernalRomIsLoaded();
}
- (bool) isVC1541Rom:(NSURL *)url {
    return wrapper->c64->floppy.mem.is1541Rom([[url path] UTF8String]);
}
- (bool) loadVC1541Rom:(NSURL *)url {
    return [self isVC1541Rom:url] && wrapper->c64->loadRom([[url path] UTF8String]);
}
- (bool) isVC1541RomLoaded {
    return wrapper->c64->floppy.mem.romIsLoaded();
}
- (bool) isRom:(NSURL *)url {
    return [self isBasicRom:url] || [self isCharRom:url] || [self isKernalRom:url] || [self isVC1541Rom:url];
}
- (bool) loadRom:(NSURL *)url {
    return [self loadBasicRom:url] || [self loadCharRom:url] || [self loadKernalRom:url] || [self loadVC1541Rom:url];
}

- (bool) attachCartridgeAndReset:(CRTProxy *)c {
    return wrapper->c64->attachCartridgeAndReset((CRTContainer *)([c wrapper]->container)); }
- (void) detachCartridgeAndReset { wrapper->c64->detachCartridgeAndReset(); }
- (bool) isCartridgeAttached { return wrapper->c64->isCartridgeAttached(); }
- (bool) insertDisk:(ArchiveProxy *)a {
    Archive *archive = (Archive *)([a wrapper]->container);
    return wrapper->c64->insertDisk(archive);
}
- (bool) flushArchive:(ArchiveProxy *)a item:(NSInteger)nr {
    Archive *archive = (Archive *)([a wrapper]->container);
    return wrapper->c64->flushArchive(archive, (int)nr);
}
- (bool) insertTape:(TAPProxy *)c {
    TAPContainer *container = (TAPContainer *)([c wrapper]->container);
    return wrapper->c64->insertTape(container);
}
- (bool) warp { return wrapper->c64->getWarp(); }
- (void) setWarp:(bool)b { wrapper->c64->setWarp(b); }
- (bool) alwaysWarp { return wrapper->c64->getAlwaysWarp(); }
- (void) setAlwaysWarp:(bool)b { wrapper->c64->setAlwaysWarp(b); }
- (bool) warpLoad { return wrapper->c64->getWarpLoad(); }
- (void) setWarpLoad:(bool)b { wrapper->c64->setWarpLoad(b); }

- (UInt64) cycles { return wrapper->c64->getCycles(); }
- (UInt64) frames { return wrapper->c64->getFrame(); }

// Snapshot storage
- (void) setAutoSaveSnapshots:(bool)b { wrapper->c64->autoSaveSnapshots = b; }
- (NSInteger) numAutoSnapshots { return wrapper->c64->numAutoSnapshots(); }
- (NSData *)autoSnapshotData:(NSInteger)nr {
    Snapshot *snapshot = wrapper->c64->autoSnapshot((unsigned)nr);
    return [NSData dataWithBytes: (void *)snapshot->header()
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
- (bool)restoreAutoSnapshot:(NSInteger)nr { return wrapper->c64->restoreAutoSnapshot((unsigned)nr); }
- (bool)restoreLatestAutoSnapshot { return wrapper->c64->restoreLatestAutoSnapshot(); }

- (NSInteger) numUserSnapshots { return wrapper->c64->numUserSnapshots(); }
- (NSData *)userSnapshotData:(NSInteger)nr {
    Snapshot *snapshot = wrapper->c64->userSnapshot((unsigned)nr);
    return [NSData dataWithBytes: (void *)snapshot->header()
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
- (bool)takeUserSnapshot { return wrapper->c64->takeUserSnapshot(); }
- (bool)restoreUserSnapshot:(NSInteger)nr { return wrapper->c64->restoreUserSnapshot((unsigned)nr); }
- (bool)restoreLatestUserSnapshot { return wrapper->c64->restoreLatestUserSnapshot(); }
- (void)deleteUserSnapshot:(NSInteger)nr { wrapper->c64->deleteUserSnapshot((unsigned)nr); }

// Audio hardware
- (BOOL) enableAudio {
    [self rampUpFromZero];
    return [audioEngine startPlayback];
}

- (void) disableAudio {
    [self rampDown];
    [audioEngine stopPlayback];
}

@end

// --------------------------------------------------------------------------
//                                Container
// --------------------------------------------------------------------------

@implementation ContainerProxy

- (instancetype) initWithContainer:(Container *)container
{
    // NSLog(@"ContainerProxy::initWithContainer");

    if (container == nil) {
        return nil;
    }
    if (self = [super init]) {
        wrapper = new ContainerWrapper();
        wrapper->container = container;
    }
    return self;
}

+ (ContainerProxy *) makeWithContainer:(Container *)container
{
    // NSLog(@"ContainerProxy::makeWithContainer");
    
    if (container == nil) {
        return nil;
    }
    return [[self alloc] initWithContainer:container];
}

- (ContainerWrapper *)wrapper { return wrapper; }
- (ContainerType)type { return wrapper->container->type(); }
- (NSInteger) sizeOnDisk { return wrapper->container->sizeOnDisk(); }

- (void) readFromBuffer:(const void *)buffer length:(NSInteger)length
{
    wrapper->container->readFromBuffer((const uint8_t *)buffer, length);
}

- (NSInteger) writeToBuffer:(void *)buffer
{
    return wrapper->container->writeToBuffer((uint8_t *)buffer);
}

- (void) dealloc
{
    // NSLog(@"ContainerProxy::dealloc");
    
    if (wrapper) {
        if (wrapper->container) delete wrapper->container;
        delete wrapper;
    }
}
@end

// --------------------------------------------------------------------------
//                              SnapshotProxy
// --------------------------------------------------------------------------

@implementation SnapshotProxy

/*
+ (BOOL) isSnapshotFile:(NSString *)path {
    return Snapshot::isSnapshotFile([path UTF8String]);
}
 */ 
+ (BOOL)isSupportedSnapshot:(const void *)buffer length:(NSInteger)length {
    return Snapshot::isSupportedSnapshot((uint8_t *)buffer, length);
}
+ (BOOL)isUnsupportedSnapshot:(const void *)buffer length:(NSInteger)length {
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
    Snapshot *snapshot = Snapshot::makeSnapshotWithBuffer((uint8_t *)buffer, length);
    return [self make:snapshot];
}

+ (instancetype) makeWithFile:(NSString *)path
{
    Snapshot *snapshot = Snapshot::makeSnapshotWithFile([path UTF8String]);
    return [self make:snapshot];
}

+ (instancetype) makeWithC64:(C64Proxy *)c64proxy
{
    C64Wrapper *wrapper = [c64proxy wrapper];
    C64 *c64 = wrapper->c64;
    Snapshot *snapshot = c64->takeSnapshotSafe();
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

// --------------------------------------------------------------------------
//                                CRTProxy
// --------------------------------------------------------------------------

@implementation CRTProxy

+ (BOOL) isCRTFile:(NSString *)path
{
    return CRTContainer::isValidCRTFile([path UTF8String]);
}

+ (instancetype) make:(CRTContainer *)container
{
    if (container == NULL) {
        return nil;
    }
    return [[self alloc] initWithContainer:container];
}

+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    CRTContainer *container = CRTContainer::makeCRTContainerWithBuffer((const uint8_t *)buffer, length);
    return [self make: container];
}

+ (instancetype) makeWithFile:(NSString *)path
{
    CRTContainer *container = CRTContainer::makeCRTContainerWithFile([path UTF8String]);
    return [self make: container];
}

- (NSString *)cartridgeName
{
    CRTContainer *c = (CRTContainer *)wrapper->container;
    return [NSString stringWithUTF8String:c->cartridgeName()];
}

- (CartridgeType)cartridgeType {
    CRTContainer *c = (CRTContainer *)wrapper->container;
    return c->cartridgeType();
}

- (NSString *)cartridgeTypeName {
    CRTContainer *c = (CRTContainer *)wrapper->container;
    return [NSString stringWithUTF8String:c->cartridgeTypeName()];
}

- (BOOL) isSupported {
    return Cartridge::isSupportedType([self cartridgeType]);
}

- (NSInteger)exromLine {
    CRTContainer *c = (CRTContainer *)wrapper->container;
    return c->exromLine();
}

- (NSInteger)gameLine {
    CRTContainer *c = (CRTContainer *)wrapper->container;
    return c->gameLine();
}

- (NSInteger)chipCount {
    CRTContainer *c = (CRTContainer *)wrapper->container;
    return c->chipCount();
}

- (NSInteger)typeOfChip:(NSInteger)nr; {
    CRTContainer *c = (CRTContainer *)wrapper->container;
    return c->chipType((unsigned)nr);
}

- (NSInteger)loadAddrOfChip:(NSInteger)nr; {
    CRTContainer *c = (CRTContainer *)wrapper->container;
    return c->chipAddr((unsigned)nr);
}

- (NSInteger)sizeOfChip:(NSInteger)nr; {
    CRTContainer *c = (CRTContainer *)wrapper->container;
    return c->chipSize((unsigned)nr);
}
@end

// --------------------------------------------------------------------------
//                                  TAPProxy
// --------------------------------------------------------------------------

@implementation TAPProxy

+ (BOOL) isTAPFile:(NSString *)path
{
    return TAPContainer::isTAPFile([path UTF8String]);
}

+ (instancetype) make:(TAPContainer *)container
{
    if (container == NULL) {
        return nil;
    }
    return [[self alloc] initWithContainer:container];
}

+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    TAPContainer *container = TAPContainer::makeTAPContainerWithBuffer((const uint8_t *)buffer, length);
    return [self make: container];
}

+ (instancetype) makeWithFile:(NSString *)path
{
    TAPContainer *container = TAPContainer::makeTAPContainerWithFile([path UTF8String]);
    return [self make: container];
}

- (NSInteger)TAPversion {
    TAPContainer *container = (TAPContainer *)wrapper->container;
    return (NSInteger)container->TAPversion();
}
@end

// --------------------------------------------------------------------------
//                                  Archive
// --------------------------------------------------------------------------

@implementation ArchiveProxy

+ (instancetype) make:(Archive *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithContainer:archive];
}

+ (instancetype) make
{
    Archive *archive = new Archive();
    return [self make: archive];
}

+ (instancetype) makeWithFile:(NSString *)path
{
    Archive *archive = Archive::makeArchiveWithFile([path UTF8String]);
    return [self make: archive];
}

- (NSInteger)numberOfItems {
    Archive *archive = (Archive *)([self wrapper]->container);
    return (NSInteger)archive->getNumberOfItems();
}
- (NSString *)nameOfItem:(NSInteger)item {
    Archive *archive = (Archive *)([self wrapper]->container);
    return [NSString stringWithUTF8String:archive->getNameOfItem((int)item)];
}
- (NSString *)unicodeNameOfItem:(NSInteger)item maxChars:(NSInteger)max {
    Archive *archive = (Archive *)([self wrapper]->container);
    const unsigned short *name = archive->getUnicodeNameOfItem((int)item, max);
    
    if (name == NULL)
        return NULL;
    
    unsigned numChars;
    for (numChars = 0; name[numChars] != 0; numChars++);
    
    return [NSString stringWithCharacters:name length:numChars];
}

- (NSInteger)sizeOfItem:(NSInteger)item
{
    Archive *archive = (Archive *)([self wrapper]->container);
    return archive->getSizeOfItem((int)item);
}
- (NSInteger)sizeOfItemInBlocks:(NSInteger)item
{
    Archive *archive = (Archive *)([self wrapper]->container);
    return archive->getSizeOfItemInBlocks((int)item);
}
- (NSString *)typeOfItem:(NSInteger)item
{
    Archive *archive = (Archive *)([self wrapper]->container);
    return [NSString stringWithUTF8String:archive->getTypeOfItem((int)item)];
}
- (NSString *)byteStream:(NSInteger)n offset:(NSInteger)offset num:(NSInteger)num
{
    Archive *archive = (Archive *)([self wrapper]->container);
    return [NSString stringWithUTF8String:archive->byteStream((unsigned)n, offset, num)];
}
@end

// --------------------------------------------------------------------------
//                                 T64Proxy
// --------------------------------------------------------------------------

@implementation T64Proxy

+ (BOOL)isT64File:(NSString *)filename
{
    return T64Archive::isT64File([filename UTF8String]);
}
+ (instancetype) make:(T64Archive *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithContainer:archive];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    T64Archive *archive = T64Archive::makeT64ArchiveWithBuffer((const uint8_t *)buffer, length);
    return [self make: archive];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    T64Archive *archive = T64Archive::makeT64ArchiveWithFile([path UTF8String]);
    return [self make: archive];
}
+ (instancetype) makeWithAnyArchive:(ArchiveProxy *)otherArchive
{
    Archive *other = (Archive *)([otherArchive wrapper]->container);
    T64Archive *archive = T64Archive::makeT64ArchiveWithAnyArchive(other);
    return [self make: archive];
}
@end

// --------------------------------------------------------------------------
//                                 PRGProxy
// --------------------------------------------------------------------------

@implementation PRGProxy

+ (BOOL)isPRGFile:(NSString *)filename
{
    return PRGArchive::isPRGFile([filename UTF8String]);
}
+ (instancetype) make:(PRGArchive *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithContainer:archive];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    PRGArchive *archive = PRGArchive::makePRGArchiveWithBuffer((const uint8_t *)buffer, length);
    return [self make: archive];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    PRGArchive *archive = PRGArchive::makePRGArchiveWithFile([path UTF8String]);
    return [self make: archive];
}
+ (instancetype) makeWithAnyArchive:(ArchiveProxy *)otherArchive
{
    Archive *other = (Archive *)([otherArchive wrapper]->container);
    PRGArchive *archive = PRGArchive::makePRGArchiveWithAnyArchive(other);
    return [self make: archive];
}
@end

// --------------------------------------------------------------------------
//                                 P00Proxy
// --------------------------------------------------------------------------

@implementation P00Proxy

+ (BOOL)isP00File:(NSString *)filename
{
    return P00Archive::isP00File([filename UTF8String]);
}
+ (instancetype) make:(P00Archive *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithContainer:archive];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    P00Archive *archive = P00Archive::makeP00ArchiveWithBuffer((const uint8_t *)buffer, length);
    return [self make: archive];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    P00Archive *archive = P00Archive::makeP00ArchiveWithFile([path UTF8String]);
    return [self make: archive];
}
+ (instancetype) makeWithAnyArchive:(ArchiveProxy *)otherArchive
{
    Archive *other = (Archive *)([otherArchive wrapper]->container);
    P00Archive *archive = P00Archive::makeP00ArchiveWithAnyArchive(other);
    return [self make: archive];
}
@end

// --------------------------------------------------------------------------
//                                 D64Proxy
// --------------------------------------------------------------------------

@implementation D64Proxy

+ (BOOL)isD64File:(NSString *)filename
{
    return D64Archive::isD64File([filename UTF8String]);
}
+ (instancetype) make:(D64Archive *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithContainer:archive];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    D64Archive *archive = D64Archive::makeD64ArchiveWithBuffer((const uint8_t *)buffer, length);
    return [self make: archive];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    D64Archive *archive = D64Archive::makeD64ArchiveWithFile([path UTF8String]);
    return [self make: archive];
}
+ (instancetype) makeWithAnyArchive:(ArchiveProxy *)otherArchive
{
    Archive *other = (Archive *)([otherArchive wrapper]->container);
    D64Archive *archive = D64Archive::makeD64ArchiveWithAnyArchive(other);
    return [self make: archive];
}
+ (instancetype) makeWithVC1541:(VC1541Proxy *)vc1541
{
    D64Archive *archive = [vc1541 wrapper]->vc1541->convertToD64();
    return [self make: archive];
}
@end

// --------------------------------------------------------------------------
//                                 PG64Proxy
// --------------------------------------------------------------------------

@implementation G64Proxy

+ (BOOL)isG64File:(NSString *)filename
{
    return G64Archive::isG64File([filename UTF8String]);
}
+ (instancetype) make:(G64Archive *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithContainer:archive];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    G64Archive *archive = G64Archive::makeG64ArchiveWithBuffer((const uint8_t *)buffer, length);
    return [self make: archive];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    G64Archive *archive = G64Archive::makeG64ArchiveWithFile([path UTF8String]);
    return [self make: archive];
}
@end

// --------------------------------------------------------------------------
//                                 NIBProxy
// --------------------------------------------------------------------------

@implementation NIBProxy

+ (BOOL)isNIBFile:(NSString *)filename
{
    return NIBArchive::isNIBFile([filename UTF8String]);
}
+ (instancetype) make:(NIBArchive *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithContainer:archive];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    NIBArchive *archive = NIBArchive::makeNIBArchiveWithBuffer((const uint8_t *)buffer, length);
    return [self make: archive];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    NIBArchive *archive = NIBArchive::makeNIBArchiveWithFile([path UTF8String]);
    return [self make: archive];
}
@end

// --------------------------------------------------------------------------
//                                 FileProxy
// --------------------------------------------------------------------------

@implementation FileProxy

+ (instancetype) make:(FileArchive *)archive
{
    if (archive == NULL) return nil;
    return [[self alloc] initWithContainer:archive];
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length
{
    FileArchive *archive = FileArchive::makeFileArchiveWithBuffer((const uint8_t *)buffer, length);
    return [self make: archive];
}
+ (instancetype) makeWithFile:(NSString *)path
{
    FileArchive *archive = FileArchive::makeFileArchiveWithFile([path UTF8String]);
    return [self make: archive];
}
@end

