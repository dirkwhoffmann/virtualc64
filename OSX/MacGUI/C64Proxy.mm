/*
 * (C) 2006 - 2010 Dirk W. Hoffmann. All rights reserved.
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

// --------------------------------------------------------------------------
//                                    CPU
// --------------------------------------------------------------------------

struct CpuWrapper { CPU *cpu; };

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
- (void) setTraceMode:(bool)b { wrapper->cpu->setTraceMode(b); }

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
- (bool) N { return wrapper->cpu->getN(); }
- (void) setN:(bool)b { wrapper->cpu->setN(b); }
- (bool) Z { return wrapper->cpu->getZ(); }
- (void) setZ:(bool)b { wrapper->cpu->setZ(b); }
- (bool) C { return wrapper->cpu->getC(); }
- (void) setC:(bool)b { wrapper->cpu->setC(b); }
- (bool) I { return wrapper->cpu->getI(); }
- (void) setI:(bool)b { wrapper->cpu->setI(b); }
- (bool) B { return wrapper->cpu->getB(); }
- (void) setB:(bool)b { wrapper->cpu->setB(b); }
- (bool) D { return wrapper->cpu->getD(); }
- (void) setD:(bool)b { wrapper->cpu->setD(b); }
- (bool) V { return wrapper->cpu->getV(); }
- (void) setV:(bool)b { wrapper->cpu->setV(b); }

- (uint16_t) peekPC {
    return wrapper->cpu->mem->peek(wrapper->cpu->getPC_at_cycle_0()); }
- (uint8_t) lengthOfInstruction:(uint8_t)opcode {
    return wrapper->cpu->getLengthOfInstruction(opcode); }
- (uint8_t) lengthOfInstructionAtAddress:(uint16_t)addr {
    return wrapper->cpu->getLengthOfInstructionAtAddress(addr); }
- (uint8_t) lengthOfCurrentInstruction {
    return wrapper->cpu->getLengthOfCurrentInstruction(); }
- (uint16_t) addressOfNextInstruction {
    return wrapper->cpu->getAddressOfNextInstruction(); }
- (const char *) mnemonic:(uint8_t)opcode {
    return wrapper->cpu->getMnemonic(opcode); }
- (CPU::AddressingMode) addressingMode:(uint8_t)opcode {
    return wrapper->cpu->getAddressingMode(opcode); }

- (int) topOfCallStack { return wrapper->cpu->getTopOfCallStack(); }
- (int) breakpoint:(int)addr { return wrapper->cpu->getBreakpoint(addr); }
- (void) setBreakpoint:(int)addr tag:(uint8_t)t { wrapper->cpu->setBreakpoint(addr, t); }
- (void) setHardBreakpoint:(int)addr { wrapper->cpu->setHardBreakpoint(addr); }
- (void) deleteHardBreakpoint:(int)addr { wrapper->cpu->deleteHardBreakpoint(addr); }
- (void) toggleHardBreakpoint:(int)addr { wrapper->cpu->toggleHardBreakpoint(addr); }
- (void) setSoftBreakpoint:(int)addr { wrapper->cpu->setSoftBreakpoint(addr); }
- (void) deleteSoftBreakpoint:(int)addr { wrapper->cpu->deleteSoftBreakpoint(addr); }
- (void) toggleSoftBreakpoint:(int)addr { wrapper->cpu->toggleSoftBreakpoint(addr); }

@end


// --------------------------------------------------------------------------
//                                   Memory
// --------------------------------------------------------------------------

struct MemoryWrapper { Memory *mem; };

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

- (uint8_t) peek:(uint16_t)addr {
    return wrapper->mem->peek(addr); }
- (uint16_t) peekWord:(uint16_t)addr {
    return wrapper->mem->peekWord(addr); }
- (uint8_t) peekFrom:(uint16_t)addr memtype:(Memory::MemoryType)type {
    return wrapper->mem->peekFrom(addr, type); }
- (void) poke:(uint16_t)addr value:(uint8_t)val {
    wrapper->mem->poke(addr, val); }
- (void) pokeTo:(uint16_t)addr value:(uint8_t)val memtype:(Memory::MemoryType)type {
    wrapper->mem->pokeTo(addr, val, type); }
- (bool) isValidAddr:(uint16_t)addr memtype:(Memory::MemoryType)type {
    return wrapper->mem->isValidAddr(addr, type); }

@end


// --------------------------------------------------------------------------
//                                    VIC
// --------------------------------------------------------------------------

struct VicWrapper { VIC *vic; };

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

- (NSColor *) color:(int)nr
{
    assert (0 <= nr && nr < 16);
    
    uint32_t color = wrapper->vic->getColor(nr);
    uint8_t r = color & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = (color >> 16) & 0xFF;
    
	return [NSColor colorWithCalibratedRed:(float)r/255.0 green:(float)g/255.0 blue:(float)b/255.0 alpha:1.0];
}

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

- (int) spriteColor:(NSInteger)nr { return wrapper->vic->spriteColor(nr); }
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

struct CiaWrapper { CIA *cia; };

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
- (bool) tracingEnabled { return wrapper->cia->tracingEnabled(); }
- (void) setTraceMode:(bool)b { wrapper->cia->setTraceMode(b); }

- (uint8_t) dataPortA { return wrapper->cia->getDataPortA(); }
- (void) setDataPortA:(uint8_t)v { wrapper->cia->setDataPortA(v); }
- (uint8_t) dataPortDirectionA { return wrapper->cia->getDataPortDirectionA(); }
- (void) setDataPortDirectionA:(uint8_t)v { wrapper->cia->setDataPortDirectionA(v); }
- (uint16_t) timerA { return wrapper->cia->getCounterA(); }
- (void) setTimerA:(uint16_t)v { wrapper->cia->setCounterA(v); }
- (uint16_t) timerLatchA { return wrapper->cia->getLatchA(); }
- (void) setTimerLatchA:(uint16_t)v { wrapper->cia->setLatchA(v); }
- (bool) startFlagA { return wrapper->cia->isStartedA(); }
- (void) setStartFlagA:(bool)b { wrapper->cia->setStartedA(b); }
- (void) toggleStartFlagA { wrapper->cia->toggleStartFlagA(); }
- (bool) oneShotFlagA { return wrapper->cia->isOneShotA(); }
- (void) setOneShotFlagA:(bool)b { wrapper->cia->setOneShotA(b); }
- (void) toggleOneShotFlagA { wrapper->cia->toggleOneShotFlagA(); }
- (bool) underflowFlagA { return wrapper->cia->willIndicateUnderflowA(); }
- (void) setUnderflowFlagA:(bool)b { return wrapper->cia->setIndicateUnderflowA(b); }
- (void) toggleUnderflowFlagA { wrapper->cia->toggleUnderflowFlagA(); }
- (bool) pendingSignalFlagA {return wrapper->cia->isSignalPendingA(); }
- (void) setPendingSignalFlagA:(bool)b { wrapper->cia->setSignalPendingA(b); }
- (void) togglePendingSignalFlagA { wrapper->cia->togglePendingSignalFlagA(); }
- (bool) interruptEnableFlagA { return wrapper->cia->isInterruptEnabledA(); }
- (void) setInterruptEnableFlagA:(bool)b { wrapper->cia->setInterruptEnabledA(b); }
- (void) toggleInterruptEnableFlagA { wrapper->cia->toggleInterruptEnableFlagA(); }

- (uint8_t) dataPortB { return wrapper->cia->getDataPortB(); }
- (void) setDataPortB:(uint8_t)v { wrapper->cia->setDataPortB(v); }
- (uint8_t) dataPortDirectionB { return wrapper->cia->getDataPortDirectionB(); }
- (void) setDataPortDirectionB:(uint8_t)v { wrapper->cia->setDataPortDirectionB(v); }
- (uint16_t) timerB { return wrapper->cia->getCounterB(); }
- (void) setTimerB:(uint16_t)v { wrapper->cia->setCounterB(v); }
- (uint16_t) timerLatchB { return wrapper->cia->getLatchB(); }
- (void) setTimerLatchB:(uint16_t)v { wrapper->cia->setLatchB(v); }
- (bool) startFlagB { return wrapper->cia->isStartedB(); }
- (void) setStartFlagB:(bool)b { wrapper->cia->setStartedB(b); }
- (void) toggleStartFlagB { wrapper->cia->toggleStartFlagB(); }
- (bool) oneShotFlagB { return wrapper->cia->isOneShotB(); }
- (void) setOneShotFlagB:(bool)b { wrapper->cia->setOneShotB(b); }
- (void) toggleOneShotFlagB { wrapper->cia->toggleOneShotFlagB(); }
- (bool) underflowFlagB { return wrapper->cia->willIndicateUnderflowB(); }
- (void) setUnderflowFlagB:(bool)b { return wrapper->cia->setIndicateUnderflowB(b); }
- (void) toggleUnderflowFlagB { wrapper->cia->toggleUnderflowFlagB(); }
- (bool) pendingSignalFlagB {return wrapper->cia->isSignalPendingB(); }
- (void) setPendingSignalFlagB:(bool)b { wrapper->cia->setSignalPendingB(b); }
- (void) togglePendingSignalFlagB { wrapper->cia->togglePendingSignalFlagB(); }
- (bool) interruptEnableFlagB { return wrapper->cia->isInterruptEnabledB(); }
- (void) setInterruptEnableFlagB:(bool)b { wrapper->cia->setInterruptEnabledB(b); }
- (void) toggleInterruptEnableFlagB { wrapper->cia->toggleInterruptEnableFlagB(); }

- (uint8_t) todHours { return wrapper->cia->tod.getTodHours(); }
- (void) setTodHours:(uint8_t)value { wrapper->cia->tod.setTodHours(value); }
- (uint8_t) todMinutes { return wrapper->cia->tod.getTodMinutes(); }
- (void) setTodMinutes:(uint8_t)value { wrapper->cia->tod.setTodMinutes(value); }
- (uint8_t) todSeconds { return wrapper->cia->tod.getTodSeconds(); }
- (void) setTodSeconds:(uint8_t)value { wrapper->cia->tod.setTodSeconds(value); }
- (uint8_t) todTenth { return wrapper->cia->tod.getTodTenth(); }
- (void) setTodTenth:(uint8_t)value { wrapper->cia->tod.setTodTenth(value); }

- (uint8_t) alarmHours { return wrapper->cia->tod.getAlarmHours(); }
- (void) setAlarmHours:(uint8_t)value { wrapper->cia->tod.setAlarmHours(value); }
- (uint8_t) alarmMinutes { return wrapper->cia->tod.getAlarmMinutes(); }
- (void) setAlarmMinutes:(uint8_t)value { wrapper->cia->tod.setAlarmMinutes(value); }
- (uint8_t) alarmSeconds { return wrapper->cia->tod.getAlarmSeconds(); }
- (void) setAlarmSeconds:(uint8_t)value { wrapper->cia->tod.setAlarmSeconds(value); }
- (uint8_t) alarmTenth { return wrapper->cia->tod.getAlarmTenth(); }
- (void) setAlarmTenth:(uint8_t)value { wrapper->cia->tod.setAlarmTenth(value); }
- (bool) isTodInterruptEnabled { return wrapper->cia->isInterruptEnabledTOD(); }
- (void) setTodInterruptEnabled:(bool)b { wrapper->cia->setInterruptEnabledTOD(b); }

@end 

// --------------------------------------------------------------------------
//                                    Keyboard
// --------------------------------------------------------------------------

@implementation KeyboardProxy

- (instancetype) initWithKeyboard:(Keyboard *)kb
{
    self = [super init];	
	keyboard = kb;	
	return self;
}

- (void) dump { keyboard->dumpState(); }
- (void) pressKey:(int)c { keyboard->pressKey(c); }
- (void) releaseKey:(int)c { keyboard->releaseKey(c); }
- (void) pressRunstopKey { keyboard->pressRunstopKey(); }
- (void) releaseRunstopKey { keyboard->releaseRunstopKey(); }
- (void) pressShiftRunstopKey { keyboard->pressShiftRunstopKey(); }
- (void) releaseShiftRunstopKey { keyboard->releaseShiftRunstopKey(); }
- (void) pressRestoreKey { keyboard->pressRestoreKey(); }
- (void) releaseRestoreKey { keyboard->releaseRestoreKey(); }
- (void) pressCommodoreKey { keyboard->pressCommodoreKey(); }
- (void) releaseCommodoreKey { keyboard->releaseCommodoreKey(); }
- (void) pressClearKey { keyboard->pressClearKey(); }
- (void) releaseClearKey { keyboard->releaseClearKey(); }
- (void) pressHomeKey { keyboard->pressHomeKey(); }
- (void) releaseHomeKey { keyboard->releaseHomeKey(); }
- (void) pressInsertKey { keyboard->pressInsertKey(); }
- (void) releaseInsertKey { keyboard->releaseInsertKey(); }

- (void)typeText:(NSString *)text
{
    [self typeText:text withDelay:0];
}

- (void)typeText:(NSString *)text withDelay:(int)delay
{    
    dispatch_async(dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
                   ^{ [self _typeText:text withDelay:delay]; });
}

- (void)_typeText:(NSString *)text withDelay:(int)delay
{
    const unsigned MAXCHARS = 256;
    const unsigned KEYDELAY = 27500;
    unsigned i;
    
    fprintf(stderr, "Typing: ");

    usleep(delay);
    for (i = 0; i < [text length] && i < MAXCHARS; i++) {
            
        unichar uc = [text characterAtIndex:i];
        char c = (char)uc;
            
        if (isupper(c))
            c = tolower(c);
            
        fprintf(stderr, "%c",c);
            
        usleep(KEYDELAY);
        [self pressKey:c];
        usleep(KEYDELAY);
        [self releaseKey:c];
    }
        
    if (i != [text length]) {
        // Abbreviate text by three dots
        for (i = 0; i < 3; i++) {
            [self pressKey:'.'];
            usleep(KEYDELAY);
            [self releaseKey:'.'];
            usleep(KEYDELAY);
        }
    }
        
    fprintf(stderr,"\n");
}


@end

// --------------------------------------------------------------------------
//                                 Joystick
// -------------------------------------------------------------------------

#if 0
@implementation JoystickManagerProxy

- (instancetype) initWithC64:(C64Proxy *)c64
{
    self = [super init];
    manager = new JoystickManager(c64);
    if (!manager->initialize()) {
        NSLog(@"WARNING: Failed to initialize joystick manager.");
        self = nil;
    }

    return self;
    return nil;
}

@end
#endif

@implementation JoystickProxy

- (instancetype) initWithJoystick:(Joystick *)joy
{
    self = [super init];
    joystick = joy;
    return self;
}

- (void) setButtonPressed:(BOOL)pressed { joystick->setButtonPressed(pressed); }
- (void) setAxisX:(JoystickDirection)state { joystick->setAxisX(state); }
- (void) setAxisY:(JoystickDirection)state {joystick->setAxisY(state); }

- (void) dump { joystick->dumpState(); }

@end

// --------------------------------------------------------------------------
//                                    SID
// --------------------------------------------------------------------------

@implementation SIDProxy

- (instancetype) initWithSID:(SIDWrapper *)s
{
    self = [super init];	
	sid = s;	
	return self;
}

- (void) dump { sid->dumpState(); }

@end

// --------------------------------------------------------------------------
//                                   IEC bus
// -------------------------------------------------------------------------

@implementation IECProxy

- (instancetype) initWithIEC:(IEC *)bus
{
    self = [super init];	
	iec = bus;	
	return self;
}

- (void) dump { iec->dumpState(); }
- (bool) tracingEnabled { return iec->tracingEnabled(); }
- (void) setTraceMode:(bool)b { iec->setTraceMode(b); }
- (void) connectDrive { iec->connectDrive(); }
- (void) disconnectDrive { iec->disconnectDrive(); }
- (bool) isDriveConnected { return iec->driveIsConnected(); }

@end

// --------------------------------------------------------------------------
//                                 Expansion port
// -------------------------------------------------------------------------

@implementation ExpansionPortProxy

- (instancetype) initWithExpansionPort:(ExpansionPort *)port
{
    self = [super init];
    expansionPort = port;
    return self;
}

- (void) dump { expansionPort->dumpState(); }
- (int) cartridgeType { return (int)expansionPort->getCartridgeType(); }
- (bool) cartridgeAttached { return expansionPort->getCartridgeAttached(); }
- (unsigned) numberOfChips { return expansionPort->numberOfChips(); }
- (unsigned) numberOfBytes { return expansionPort->numberOfBytes(); }

@end

// --------------------------------------------------------------------------
//                                     VIA
// -------------------------------------------------------------------------

@implementation VIAProxy

- (instancetype) initWithVIA:(VIA6522 *)v
{
    self = [super init];	
	via = v;
	return self;
}

- (void) dump { via->dumpState(); }
- (bool) tracingEnabled { return via->tracingEnabled(); }
- (void) setTraceMode:(bool)b { via->setTraceMode(b); }

@end

// -------------------------------------------------------------------------
//                                5,25" diskette
// -------------------------------------------------------------------------

@implementation Disk525Proxy

- (instancetype) initWithDisk525:(Disk525 *)d
{
    self = [super init];
    disk = d;
    return self;
}

- (BOOL)isWriteProtected { return disk->isWriteProtected(); }
- (void)setWriteProtection:(BOOL)b { disk->setWriteProtection(b); }
- (BOOL)isModified { return disk->isModified(); }
- (void)setModified:(BOOL)b { disk->setModified(b); }
- (NSInteger)numTracks { return (NSInteger)disk->numTracks; }


@end

// -------------------------------------------------------------------------
//                                    VC1541
// -------------------------------------------------------------------------

@implementation VC1541Proxy

@synthesize cpu, mem, via1, via2, disk;

- (instancetype) initWithVC1541:(VC1541 *)vc
{
    self = [super init];	
	vc1541 = vc;
	cpu = [[CPUProxy alloc] initWithCPU:&vc->cpu];
	mem = [[MemoryProxy alloc] initWithMemory:&vc->mem];
	via1 = [[VIAProxy alloc] initWithVIA:&vc->via1];
	via2 = [[VIAProxy alloc] initWithVIA:&vc->via2];
    disk = [[Disk525Proxy alloc] initWithDisk525:&vc->disk];
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

- (void) dump { vc1541->dumpState(); }
- (bool) tracingEnabled { return vc1541->tracingEnabled(); }
- (void) setTraceMode:(bool)b { vc1541->setTraceMode(b); }
- (bool) hasRedLED { return vc1541->getRedLED(); }
- (bool) hasDisk { return vc1541->hasDisk(); }
- (void) ejectDisk { vc1541->ejectDisk(); }
- (bool) writeProtection { return vc1541->disk.isWriteProtected(); }
- (void) setWriteProtection:(bool)b { vc1541->disk.setWriteProtection(b); }
- (bool) DiskModified { return vc1541->disk.isModified(); }
- (void) setDiskModified:(bool)b { vc1541->disk.setModified(b); }
- (bool) bitAccuracy { return vc1541->getBitAccuracy(); }
- (void) setBitAccuracy:(bool)b { vc1541->setBitAccuracy(b); }
- (bool) soundMessagesEnabled { return vc1541->soundMessagesEnabled(); }
- (void) setSendSoundMessages:(bool)b { vc1541->setSendSoundMessages(b); }
- (bool) exportToD64:(NSString *)path { return vc1541->exportToD64([path UTF8String]); }

- (D64ArchiveProxy *) convertToD64
{
    D64Archive *archive = vc1541->convertToD64();
    return archive ? [[D64ArchiveProxy alloc] initWithArchive:archive] : nil;
}

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

- (instancetype) initWithDatasette:(Datasette *)ds
{
    self = [super init];
    datasette = ds;
    return self;
}

- (void) dump { datasette->dumpState(); }
- (bool) hasTape { return datasette->hasTape(); }
- (void) pressPlay { datasette->pressPlay(); }
- (void) pressStop { datasette->pressStop(); }
- (void) pressRewind { datasette->rewind(); }
- (void) ejectTape { datasette->ejectTape(); }
- (NSInteger) getType { return datasette->getType(); }
- (long) durationInCycles { return datasette->getDurationInCycles(); }
- (int) durationInSeconds { return datasette->getDurationInSeconds(); }
- (int) head { return datasette->getHead(); }
- (long) headInCycles { return datasette->getHeadInCycles(); }
- (int) headInSeconds { return datasette->getHeadInSeconds(); }
- (void) setHeadInCycles:(long)value { datasette->setHeadInCycles(value); }
- (BOOL) motor { return datasette->getMotor(); }
- (BOOL) playKey { return datasette->getPlayKey(); }
@end

// --------------------------------------------------------------------------
//                                     C64
// --------------------------------------------------------------------------

@implementation C64Proxy

@synthesize cpu, mem, vic, cia1, cia2, sid, keyboard, iec, expansionport, vc1541, datasette;
@synthesize joystickA, joystickB; 
@synthesize iecBusIsBusy, tapeBusIsBusy;

- (instancetype) init
{
	NSLog(@"C64Proxy::init");
	
    self = [super init];
	
	// Create virtual machine and initialize references
	c64 = new C64();
	
	// Create sub proxys
	cpu = [[CPUProxy alloc] initWithCPU:&c64->cpu];
    // cpu = [[CPUProxy alloc] initWithCPU:&c64->floppy->cpu];
	mem = [[MemoryProxy alloc] initWithMemory:&c64->mem];
	vic = [[VICProxy alloc] initWithVIC:&c64->vic];
	cia1 = [[CIAProxy alloc] initWithCIA:&c64->cia1];
	cia2 = [[CIAProxy alloc] initWithCIA:&c64->cia2];
	sid = [[SIDProxy alloc] initWithSID:&c64->sid];
	keyboard = [[KeyboardProxy alloc] initWithKeyboard:&c64->keyboard];
    joystickA = [[JoystickProxy alloc] initWithJoystick:&c64->joystickA];
    joystickB = [[JoystickProxy alloc] initWithJoystick:&c64->joystickB];
    iec = [[IECProxy alloc] initWithIEC:&c64->iec];
    expansionport = [[ExpansionPortProxy alloc] initWithExpansionPort:&c64->expansionport];
	vc1541 = [[VC1541Proxy alloc] initWithVC1541:&c64->floppy];
    datasette = [[DatasetteProxy alloc] initWithDatasette:&c64->datasette];

    // Initialize Joystick HID interface
    if (!(joystickManager = new JoystickManager(self))) {
        NSLog(@"WARNING: Couldn't initialize HID interface.");
    }
    joystickManager->initialize(); 

	// Initialize CoreAudio sound interface
	if (!(audioDevice = [[AudioDevice alloc] initWithC64:c64])) {
		NSLog(@"WARNING: Couldn't initialize CoreAudio interface. Sound disabled.");
	}
		
    return self;
}

- (void) awakeFromNib
{
}

- (void) kill
{
	assert(c64 != NULL);
	NSLog(@"C64Proxy::kill");

	// Delete sound device
	[self disableAudio];
	audioDevice = nil;
	
    // Delete HDI interface
    delete joystickManager;
    joystickManager = NULL;
    
    // Delete emulator
    delete c64;
	c64 = NULL;
}

- (bool) audioFilter { return c64->getAudioFilter(); }
- (void) setAudioFilter:(bool)b { c64->setAudioFilter(b); }
- (bool) reSID { return c64->getReSID(); }
- (void) setReSID:(bool)b { c64->setReSID(b); }
- (int) samplingMethod { return (int)(c64->getSamplingMethod()); }
- (void) setSamplingMethod:(long)value { c64->setSamplingMethod((sampling_method)value); }
- (int) chipModel { return (chip_model)(c64->getChipModel()); }
- (void) setChipModel:(long)value {c64->setChipModel((chip_model)value); }
- (void) rampUp { c64->sid.rampUp(); }
- (void) rampUpFromZero { c64->sid.rampUpFromZero(); }
- (void) rampDown { c64->sid.rampDown(); }

- (void) _loadFromSnapshot:(Snapshot *)snapshot
{
    c64->suspend();
    c64->loadFromSnapshot(snapshot);
    c64->resume();
}

- (void) loadFromSnapshot:(SnapshotProxy *)snapshot
{
    [self _loadFromSnapshot:[snapshot snapshot]];
}

- (void) _saveToSnapshot:(Snapshot *)snapshot
{
    c64->suspend();
    c64->saveToSnapshot(snapshot);
    c64->resume();
}

- (void) saveToSnapshot:(SnapshotProxy *)snapshot
{
    [self _saveToSnapshot:[snapshot snapshot]];
}

- (CIAProxy *) cia:(int)num { assert(num == 1 || num == 2); return (num == 1) ? [self cia1] : [self cia2]; }

- (void) dump { c64->dumpState(); }

- (MessageProxy *)message { return [MessageProxy messageFromMessage:c64->getMessage()]; }
- (void) putMessage:(int)msg { c64->putMessage(msg); }
- (void) reset { c64->reset(); }
- (void) ping { c64->ping(); }
- (void) halt { c64->halt(); }
- (void) step { c64->step(); }
- (void) run { c64->run(); }
- (void) suspend { c64->suspend(); }
- (void) resume { c64->resume(); }
- (bool) isHalted { return c64->isHalted(); }
- (bool) isRunnable { return c64->isRunnable(); }
- (bool) isRunning { return c64->isRunning(); }
- (bool) isPAL { return c64->isPAL(); }
- (bool) isNTSC { return c64->isNTSC(); }
- (void) setPAL { c64->setPAL(); }
- (void) setNTSC { c64->setNTSC(); }

//- (int) numberOfMissingRoms { return c64->numberOfMissingRoms(); }
- (uint8_t) missingRoms { return c64->getMissingRoms(); }
- (bool) isBasicRom:(NSString *)filename { return c64->mem.isBasicRom([filename UTF8String]); }
- (bool) loadBasicRom:(NSString *)filename { return [self isBasicRom:filename] && c64->loadRom([filename UTF8String]); }
- (bool) isCharRom:(NSString *)filename { return c64->mem.isCharRom([filename UTF8String]); }
- (bool) loadCharRom:(NSString *)filename { return [self isCharRom:filename] && c64->loadRom([filename UTF8String]); }
- (bool) isKernelRom:(NSString *)filename { return c64->mem.isKernelRom([filename UTF8String]); }
- (bool) loadKernelRom:(NSString *)filename { return [self isKernelRom:filename] && c64->loadRom([filename UTF8String]); }
- (bool) isVC1541Rom:(NSString *)filename { return c64->floppy.mem.is1541Rom([filename UTF8String]); }
- (bool) loadVC1541Rom:(NSString *)filename { return [self isVC1541Rom:filename] && c64->loadRom([filename UTF8String]); }
- (bool) isRom:(NSString *)filename { return [self isBasicRom:filename] || [self isCharRom:filename] || [self isKernelRom:filename] || [self isVC1541Rom:filename]; }
- (bool) loadRom:(NSString *)filename { return [self loadBasicRom:filename] || [self loadCharRom:filename] || [self loadKernelRom:filename] || [self loadVC1541Rom:filename]; }

- (bool) attachCartridge:(Cartridge *)c { return c64->attachCartridge(c); }
- (void) detachCartridge { c64->detachCartridge(); }
- (bool) isCartridgeAttached { return c64->isCartridgeAttached(); }

- (bool) mountArchive:(ArchiveProxy *)a { return c64->mountArchive([a archive]); }
- (bool) flushArchive:(ArchiveProxy *)a item:(NSInteger)nr { return c64->flushArchive([a archive], (int)nr); }

- (bool) insertTape:(TAPContainerProxy *)c { return c64->insertTape([c container]); }

- (bool) warp { return c64->getWarp(); }
- (void) setWarp:(bool)b { c64->setWarp(b); }	
- (bool) alwaysWarp { return c64->getAlwaysWarp(); }
- (void) setAlwaysWarp:(bool)b { c64->setAlwaysWarp(b); }
- (bool) warpLoad { return c64->getWarpLoad(); }
- (void) setWarpLoad:(bool)b { c64->setWarpLoad(b); }

- (long) cycles { return (long)c64->getCycles(); }
- (long) frames { return (long)c64->getFrame(); }

// Cheatbox
- (int) historicSnapshots { return c64->numHistoricSnapshots(); }

- (int) historicSnapshotHeaderSize:(NSInteger)nr
    { Snapshot *s = c64->getHistoricSnapshot((int)nr); return s ? s->getHeaderSize() : 0; }

- (uint8_t *) historicSnapshotHeader:(NSInteger)nr
    { Snapshot *s = c64->getHistoricSnapshot((int)nr); return s ? s->getHeader() : NULL; }

- (int) historicSnapshotDataSize:(NSInteger)nr
    { Snapshot *s = c64->getHistoricSnapshot((int)nr); return s ? s->getDataSize() : 0; }

- (uint8_t *) historicSnapshotData:(NSInteger)nr
    { Snapshot *s = c64->getHistoricSnapshot((int)nr); return s ? s->getData() : NULL; }


- (unsigned char *)historicSnapshotImageData:(NSInteger)nr
    { Snapshot *s = c64->getHistoricSnapshot((int)nr); return s ? s->getImageData() : NULL; }
- (unsigned)historicSnapshotImageWidth:(NSInteger)nr
    { Snapshot *s = c64->getHistoricSnapshot((int)nr); return s ? s->getImageWidth() : 0; }
- (unsigned)historicSnapshotImageHeight:(NSInteger)nr
{ Snapshot *s = c64->getHistoricSnapshot((int)nr); return s ? s->getImageHeight() : 0; }
- (time_t)historicSnapshotTimestamp:(NSInteger)nr { Snapshot *s = c64->getHistoricSnapshot((int)nr); return s ? s->getTimestamp() : 0; }
- (bool)revertToHistoricSnapshot:(NSInteger)nr { Snapshot *s = c64->getHistoricSnapshot((int)nr); return s ? c64->loadFromSnapshot(s), true : false; }

// Joystick
- (BOOL)joystickIsPluggedIn:(int)nr { return joystickManager->joystickIsPluggedIn(nr); }
- (void)bindJoystickToPortA:(int)nr { joystickManager->bindJoystickToPortA(nr); }
- (void)bindJoystickToPortB:(int)nr { joystickManager->bindJoystickToPortB(nr); }
- (void)unbindJoysticksFromPortA { joystickManager->unbindJoysticksFromPortA(); }
- (void)unbindJoysticksFromPortB { joystickManager->unbindJoysticksFromPortB(); }

// Audio hardware
- (void) enableAudio { [self rampUpFromZero]; [audioDevice startPlayback]; }
- (void) disableAudio {	[self rampDown]; [audioDevice stopPlayback]; }

@end

// --------------------------------------------------------------------------
//                                Snapshot
// --------------------------------------------------------------------------

@implementation SnapshotProxy

@synthesize snapshot;

- (instancetype) init
{
	NSLog(@"V64Snapshot::init");
	
	if (!(self = [super init]))
		return nil;

	snapshot = new Snapshot;
	return self;
}

- (instancetype) initWithSnapshot:(Snapshot *)s
{
    NSLog(@"V64Snapshot::initWithSnapshot %p", s);
    
    if (s == nil)
        return nil;
    
    if (!(self = [super init]))
        return nil;
    
    snapshot = s;
    return self;
}

- (void) dealloc
{	
	NSLog(@"V64Snapshot::dealloc");

	if (snapshot)
		delete snapshot;
	
}

+ (instancetype) snapshotFromSnapshot:(Snapshot *)snapshot
{
    if (snapshot == NULL)
        return nil;
    
    SnapshotProxy *newSnapshot = [[self alloc] initWithSnapshot:snapshot];
    return newSnapshot;
}

+ (instancetype) snapshotFromFile:(NSString *)path
{
    return [self snapshotFromSnapshot:(Snapshot::snapshotFromFile([path UTF8String]))];
}

+ (instancetype) snapshotFromBuffer:(const void *)buffer length:(unsigned)length
{
    return [self snapshotFromSnapshot:(Snapshot::snapshotFromBuffer((uint8_t *)buffer, length))];
}

- (bool) readDataFromFile:(NSString *)path { return snapshot->readFromFile([path UTF8String]); }
- (bool) writeDataToFile:(NSString *)path { return snapshot->writeToFile([path UTF8String]); }

@end

// --------------------------------------------------------------------------
//                                Message
// --------------------------------------------------------------------------

@implementation MessageProxy;

@synthesize id;
@synthesize i;
@synthesize p;

- (instancetype)initWithMessage:(Message *)msg
{
    if (msg == nil)
        return nil;
    
    if (!(self = [super init]))
        return nil;
    
    id = msg->id;
    i = msg->i;
    p = msg->p;
    strncpy(c, msg->c, 127);
    
    return self;
}

+ (instancetype) messageFromMessage:(Message *)msg
{
    return msg ? [[MessageProxy alloc] initWithMessage:msg] : nil;
}

- (char *)c { return c; }

@end



// --------------------------------------------------------------------------
//                           Archive (incomplete)
// --------------------------------------------------------------------------

@implementation ArchiveProxy

@synthesize archive;

- (instancetype)initWithArchive:(Archive *)a
{
    NSLog(@"ArchiveProxy::initWithArchive %p", archive);

    if (a == nil)
        return nil;
    
    if (!(self = [super init]))
        return nil;
    
    archive = a;

    return self;
}

- (void)dealloc
{
    NSLog(@"ArchiveProxy %p deleted", archive);
    
    if (archive)
        delete archive;
}

- (NSString *)getPath { return [NSString stringWithUTF8String:archive->getPath()]; }
- (NSString *)getName { return [NSString stringWithUTF8String:archive->getName()]; }
- (NSInteger)getType { return (NSInteger)archive->getType(); }
- (NSInteger)getNumberOfItems { return (NSInteger)archive->getNumberOfItems(); }
- (BOOL)writeToFile:(NSString *)filename { return archive->writeToFile([filename UTF8String]); }

@end


@implementation T64ArchiveProxy

+ (BOOL)isT64File:(NSString *)filename
{
    return T64Archive::isT64File([filename UTF8String]);
}

+ (instancetype)archiveFromT64File:(NSString *)filename
{
    T64Archive *archive = T64Archive::archiveFromT64File([filename UTF8String]);
    return archive ? [[T64ArchiveProxy alloc] initWithArchive:archive] : nil;
}

+ (instancetype)archiveFromArchive:(ArchiveProxy *)otherArchive
{
    T64Archive *archive = T64Archive::archiveFromArchive([otherArchive archive]);
    return archive ? [[T64ArchiveProxy alloc] initWithArchive:archive] : nil;
}

@end


@implementation D64ArchiveProxy

+ (BOOL) isD64File:(NSString *)filename
{
   return D64Archive::isD64File([filename UTF8String]);
}

+ (instancetype) archiveFromD64File:(NSString *)filename
{
    D64Archive *archive = D64Archive::archiveFromD64File([filename UTF8String]);
    return archive ? [[D64ArchiveProxy alloc] initWithArchive:archive] : nil;
}

+ (instancetype) archiveFromArbitraryFile:(NSString *)filename
{
    D64Archive *archive = D64Archive::archiveFromArbitraryFile([filename UTF8String]);
    return archive ? [[D64ArchiveProxy alloc] initWithArchive:archive] : nil;
}

+ (instancetype) archiveFromD64Archive:(D64ArchiveProxy *)otherArchive
{
    D64Archive *archive = D64Archive::archiveFromD64Archive((D64Archive *)[otherArchive archive]);
    return archive ? [[D64ArchiveProxy alloc] initWithArchive:archive] : nil;
}

+ (instancetype) archiveFromArchive:(ArchiveProxy *)otherArchive
{
    D64Archive *archive = D64Archive::archiveFromArchive([otherArchive archive]);
    return archive ? [[D64ArchiveProxy alloc] initWithArchive:archive] : nil;
}

@end


@implementation PRGArchiveProxy

+ (BOOL)isPRGFile:(NSString *)filename
{
    return PRGArchive::isPRGFile([filename UTF8String]);
}

+ (instancetype)archiveFromPRGFile:(NSString *)filename
{
    PRGArchive *archive = PRGArchive::archiveFromPRGFile([filename UTF8String]);
    return archive ? [[PRGArchiveProxy alloc] initWithArchive:archive] : nil;
}

+ (instancetype)archiveFromArchive:(ArchiveProxy *)otherArchive
{
    PRGArchive *archive = PRGArchive::archiveFromArchive([otherArchive archive]);
    return archive ? [[PRGArchiveProxy alloc] initWithArchive:archive] : nil;
}

@end


@implementation P00ArchiveProxy

+ (BOOL)isP00File:(NSString *)filename
{
    return P00Archive::isP00File([filename UTF8String]);
}

+ (instancetype)archiveFromP00File:(NSString *)filename
{
    P00Archive *archive = P00Archive::archiveFromP00File([filename UTF8String]);
    return archive ? [[P00ArchiveProxy alloc] initWithArchive:archive] : nil;
}

+ (instancetype)archiveFromArchive:(ArchiveProxy *)otherArchive
{
    P00Archive *archive = P00Archive::archiveFromArchive([otherArchive archive]);
    return archive ? [[P00ArchiveProxy alloc] initWithArchive:archive] : nil;
}

@end


@implementation G64ArchiveProxy

+ (BOOL) isG64File:(NSString *)filename
{
    return G64Archive::isG64File([filename UTF8String]);
}

+ (instancetype) archiveFromG64File:(NSString *)filename
{
    G64Archive *archive = G64Archive::archiveFromG64File([filename UTF8String]);
    return archive ? [[G64ArchiveProxy alloc] initWithArchive:archive] : nil;
}

@end


@implementation NIBArchiveProxy

+ (BOOL) isNIBFile:(NSString *)filename
{
    return NIBArchive::isNIBFile([filename UTF8String]);
}

+ (instancetype) archiveFromNIBFile:(NSString *)filename
{
    NIBArchive *archive = NIBArchive::archiveFromNIBFile([filename UTF8String]);
    return archive ? [[NIBArchiveProxy alloc] initWithArchive:archive] : nil;
}

@end

@implementation TAPContainerProxy

@synthesize container;

- (instancetype) initWithTAPContainer:(TAPArchive *)c
{
    NSLog(@"TAPContainerProxy::initWithContainer");
    
    if (c == nil)
        return nil;
    
    if (!(self = [super init]))
        return nil;
    
    container = c;
    return self;
}

- (void)dealloc
{
    NSLog(@"TAPContainerProxy::dealloc");
    
    if (container)
        delete container;
}

+ (BOOL) isTAPFile:(NSString *)filename
{
    return TAPArchive::isTAPFile([filename UTF8String]);
}

+ (instancetype) containerFromTAPFile:(NSString *)filename
{
    TAPArchive *container = TAPArchive::archiveFromTAPFile([filename UTF8String]);
    return container ? [[TAPContainerProxy alloc] initWithTAPContainer:container] : nil;
}

- (NSString *)getPath { return [NSString stringWithUTF8String:container->getPath()]; }
- (NSString *)getName { return [NSString stringWithUTF8String:container->getName()]; }
- (NSInteger)getType { return (NSInteger)container->getType(); }
- (NSInteger)TAPversion { return (NSInteger)container->TAPversion(); }

@end

