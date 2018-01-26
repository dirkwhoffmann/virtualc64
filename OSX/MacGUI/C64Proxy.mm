/*
 * (C) 2006 - 2017 Dirk W. Hoffmann. All rights reserved.
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
struct JoystickWrapper { Joystick *joystick; };
struct SidWrapperWrapper { SIDWrapper *sid; };
struct IecWrapper { IEC *iec; };
struct ExpansionPortWrapper { ExpansionPort *expansionPort; };
struct Via6522Wrapper { VIA6522 *via; };
struct Disk525Wrapper { Disk525 *disk; };
struct Vc1541Wrapper { VC1541 *vc1541; };
struct DatasetteWrapper { Datasette *datasette; };
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
- (AddressingMode) addressingMode:(uint8_t)opcode {
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
- (uint8_t) peekFrom:(uint16_t)addr memtype:(MemoryType)type {
    return wrapper->mem->peekFrom(addr, type); }
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

- (instancetype) initWithKeyboard:(Keyboard *)keyboard
{
    if (self = [super init]) {
        wrapper = new KeyboardWrapper();
        wrapper->keyboard = keyboard;
    }
    return self;
}

- (void) dump { wrapper->keyboard->dumpState(); }
- (void) pressKey:(C64KeyFingerprint)c { wrapper->keyboard->pressKey(c); }
- (void) releaseKey:(C64KeyFingerprint)c { wrapper->keyboard->releaseKey(c); }
- (void) pressRunstopKey { wrapper->keyboard->pressRunstopKey(); }
- (void) releaseRunstopKey { wrapper->keyboard->releaseRunstopKey(); }
- (void) pressShiftRunstopKey { wrapper->keyboard->pressShiftRunstopKey(); }
- (void) releaseShiftRunstopKey { wrapper->keyboard->releaseShiftRunstopKey(); }
- (void) pressRestoreKey { wrapper->keyboard->pressRestoreKey(); }
- (void) releaseRestoreKey { wrapper->keyboard->releaseRestoreKey(); }
- (void) pressCommodoreKey { wrapper->keyboard->pressCommodoreKey(); }
- (void) releaseCommodoreKey { wrapper->keyboard->releaseCommodoreKey(); }
- (void) pressHomeKey { wrapper->keyboard->pressHomeKey(); }
- (void) releaseHomeKey { wrapper->keyboard->releaseHomeKey(); }
- (void) pressClearKey { wrapper->keyboard->pressClearKey(); }
- (void) releaseClearKey { wrapper->keyboard->releaseClearKey(); }
- (void) pressDeleteKey { wrapper->keyboard->pressDeleteKey(); }
- (void) releaseDeleteKey { wrapper->keyboard->releaseDeleteKey(); }
- (void) pressInsertKey { wrapper->keyboard->pressInsertKey(); }
- (void) releaseInsertKey { wrapper->keyboard->releaseInsertKey(); }

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

@implementation JoystickProxy

- (instancetype) initWithJoystick:(Joystick *)joystick
{
    if (self = [super init]) {
        wrapper = new JoystickWrapper();
        wrapper->joystick = joystick;
    }
    return self;
}

- (void) setButton:(NSInteger)pressed { wrapper->joystick->setButton((bool)pressed); }
- (void) pressButton { wrapper->joystick->pressButton(); }
- (void) releaseButton { wrapper->joystick->releaseButton(); }
- (void) pullUp { wrapper->joystick->pullUp(); }
- (void) pullDown { wrapper->joystick->pullDown(); }
- (void) pullLeft { wrapper->joystick->pullLeft(); }
- (void) pullRight { wrapper->joystick->pullRight(); }
- (void) releaseAxes { wrapper->joystick->releaseAxes(); }
- (void) setXAxis:(NSInteger)value { wrapper->joystick->setXAxis((int)value); }
- (void) setYAxis:(NSInteger)value { wrapper->joystick->setYAxis((int)value); }
- (void) releaseXAxis { wrapper->joystick->releaseXAxis(); }
- (void) releaseYAxis { wrapper->joystick->releaseYAxis(); }

/*
- (void) pullJoystick:(JoystickDirection)dir {
    switch(dir) {
        case LEFT:  wrapper->joystick->pullLeft(); return;
        case RIGHT: wrapper->joystick->pullRight(); return;
        case UP:    wrapper->joystick->pullUp(); return;
        case DOWN:  wrapper->joystick->pullDown(); return;
        case FIRE:  wrapper->joystick->pressButton(); return;
        default:    return;
    }
}

- (void) releaseJoystick:(JoystickDirection)dir {
    switch(dir) {
        case LEFT:  wrapper->joystick->releaseXAxis(); return;
        case RIGHT: wrapper->joystick->releaseXAxis(); return;
        case UP:    wrapper->joystick->releaseYAxis(); return;
        case DOWN:  wrapper->joystick->releaseYAxis(); return;
        case FIRE:  wrapper->joystick->releaseButton(); return;
        default:    return;
    }
}
*/

- (void) dump { wrapper->joystick->dumpState(); }

@end

// --------------------------------------------------------------------------
//                                    SID
// --------------------------------------------------------------------------

@implementation SIDProxy

- (instancetype) initWithSID:(SIDWrapper *)sid
{
    if (self = [super init]) {
        wrapper = new SidWrapperWrapper();
        wrapper->sid = sid;
    }
    return self;
}

- (void) dump { wrapper->sid->dumpState(); }
- (uint32_t) sampleRate { return wrapper->sid->getSampleRate(); }
- (void) setSampleRate:(uint32_t)rate { wrapper->sid->setSampleRate(rate); }
- (float) getSample {
    float sample;
    [self readMonoSamples:&sample size:1];
    return sample;
}
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
- (void) setTraceMode:(bool)b { wrapper->iec->setTraceMode(b); }
- (void) connectDrive { wrapper->iec->connectDrive(); }
- (void) disconnectDrive { wrapper->iec->disconnectDrive(); }
- (bool) isDriveConnected { return wrapper->iec->driveIsConnected(); }

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
- (void) setTraceMode:(bool)b { wrapper->via->setTraceMode(b); }

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
- (void) setTraceMode:(bool)b { wrapper->vc1541->setTraceMode(b); }
- (bool) hasRedLED { return wrapper->vc1541->getRedLED(); }
- (bool) hasDisk { return wrapper->vc1541->hasDisk(); }
- (void) ejectDisk { wrapper->vc1541->ejectDisk(); }
- (bool) writeProtection { return wrapper->vc1541->disk.isWriteProtected(); }
- (void) setWriteProtection:(bool)b { wrapper->vc1541->disk.setWriteProtection(b); }
- (bool) DiskModified { return wrapper->vc1541->disk.isModified(); }
- (void) setDiskModified:(bool)b { wrapper->vc1541->disk.setModified(b); }
- (bool) bitAccuracy { return wrapper->vc1541->getBitAccuracy(); }
- (void) setBitAccuracy:(bool)b { wrapper->vc1541->setBitAccuracy(b); }
- (bool) soundMessagesEnabled { return wrapper->vc1541->soundMessagesEnabled(); }
- (void) setSendSoundMessages:(bool)b { wrapper->vc1541->setSendSoundMessages(b); }
- (bool) exportToD64:(NSString *)path { return wrapper->vc1541->exportToD64([path UTF8String]); }

#if 0
- (D64ArchiveProxy *) convertToD64
{
    D64Archive *archive = wrapper->vc1541->convertToD64();
    return archive ? [[D64ArchiveProxy alloc] initWithArchive:archive] : nil;
}
#endif 

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
- (void) pressRewind { wrapper->datasette->rewind(); }
- (void) ejectTape { wrapper->datasette->ejectTape(); }
- (NSInteger) getType { return wrapper->datasette->getType(); }
- (long) durationInCycles { return wrapper->datasette->getDurationInCycles(); }
- (int) durationInSeconds { return wrapper->datasette->getDurationInSeconds(); }
- (int) head { return wrapper->datasette->getHead(); }
- (long) headInCycles { return wrapper->datasette->getHeadInCycles(); }
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
@synthesize joystickA, joystickB;
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
    joystickA = [[JoystickProxy alloc] initWithJoystick:&c64->joystickA];
    joystickB = [[JoystickProxy alloc] initWithJoystick:&c64->joystickB];
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

- (void) awakeFromNib
{
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
- (void) setSamplingMethod:(long)value { wrapper->c64->setSamplingMethod((sampling_method)value); }
- (int) chipModel { return (chip_model)(wrapper->c64->getChipModel()); }
- (void) setChipModel:(long)value {wrapper->c64->setChipModel((chip_model)value); }
- (void) rampUp { wrapper->c64->sid.rampUp(); }
- (void) rampUpFromZero { wrapper->c64->sid.rampUpFromZero(); }
- (void) rampDown { wrapper->c64->sid.rampDown(); }

- (void) _loadFromSnapshotWrapper:(SnapshotWrapper *)w
{
    wrapper->c64->loadFromSnapshotSafe(w->snapshot);
}

- (void) loadFromSnapshot:(SnapshotProxy *)snapshot
{
    [self _loadFromSnapshotWrapper:[snapshot wrapper]];
}

- (void) _saveToSnapshotWrapper:(SnapshotWrapper *)w
{
    wrapper->c64->saveToSnapshotSafe(w->snapshot);
}

- (void) saveToSnapshot:(SnapshotProxy *)snapshot
{
    [self _saveToSnapshotWrapper:[snapshot wrapper]];
}

- (CIAProxy *) cia:(int)num { assert(num == 1 || num == 2); return (num == 1) ? [self cia1] : [self cia2]; }

- (void) dump { wrapper->c64->dumpState(); }
- (Message *)message { return wrapper->c64->getMessage(); }
- (void) putMessage:(int)msg { wrapper->c64->putMessage(msg); }

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

- (uint8_t) missingRoms { return wrapper->c64->getMissingRoms(); }
- (bool) isBasicRom:(NSString *)filename {
    return wrapper->c64->mem.isBasicRom([filename UTF8String]); }
- (bool) loadBasicRom:(NSString *)filename {
    return [self isBasicRom:filename] && wrapper->c64->loadRom([filename UTF8String]); }
- (bool) isCharRom:(NSString *)filename {
    return wrapper->c64->mem.isCharRom([filename UTF8String]); }
- (bool) loadCharRom:(NSString *)filename {
    return [self isCharRom:filename] && wrapper->c64->loadRom([filename UTF8String]); }
- (bool) isKernelRom:(NSString *)filename {
    return wrapper->c64->mem.isKernelRom([filename UTF8String]); }
- (bool) loadKernelRom:(NSString *)filename {
    return [self isKernelRom:filename] && wrapper->c64->loadRom([filename UTF8String]); }
- (bool) isVC1541Rom:(NSString *)filename {
    return wrapper->c64->floppy.mem.is1541Rom([filename UTF8String]); }
- (bool) loadVC1541Rom:(NSString *)filename {
    return [self isVC1541Rom:filename] && wrapper->c64->loadRom([filename UTF8String]); }
- (bool) isRom:(NSString *)filename {
    return [self isBasicRom:filename] || [self isCharRom:filename] || [self isKernelRom:filename] || [self isVC1541Rom:filename]; }
- (bool) loadRom:(NSString *)filename {
    return [self loadBasicRom:filename] || [self loadCharRom:filename] || [self loadKernelRom:filename] || [self loadVC1541Rom:filename]; }

- (bool) attachCartridgeAndReset:(CRTContainerProxy *)c {
    return wrapper->c64->attachCartridgeAndReset([c wrapper]->crtcontainer); }
- (void) detachCartridgeAndReset { wrapper->c64->detachCartridgeAndReset(); }
- (bool) isCartridgeAttached { return wrapper->c64->isCartridgeAttached(); }

- (bool) mountArchive:(ArchiveProxy *)a {
    return wrapper->c64->mountArchive([a wrapper]->archive);
}
- (bool) flushArchive:(ArchiveProxy *)a item:(NSInteger)nr {
    return wrapper->c64->flushArchive([a wrapper]->archive, (int)nr);
}
- (bool) insertTape:(TAPContainerProxy *)c {
    return wrapper->c64->insertTape([c wrapper]->tapcontainer);
}
- (bool) warp { return wrapper->c64->getWarp(); }
- (void) setWarp:(bool)b { wrapper->c64->setWarp(b); }
- (bool) alwaysWarp { return wrapper->c64->getAlwaysWarp(); }
- (void) setAlwaysWarp:(bool)b { wrapper->c64->setAlwaysWarp(b); }
- (bool) warpLoad { return wrapper->c64->getWarpLoad(); }
- (void) setWarpLoad:(bool)b { wrapper->c64->setWarpLoad(b); }

- (long) cycles { return (long)wrapper->c64->getCycles(); }
- (long) frames { return (long)wrapper->c64->getFrame(); }

// Cheatbox
- (NSInteger) historicSnapshots { return wrapper->c64->numHistoricSnapshots(); }

- (NSInteger) historicSnapshotHeaderSize:(NSInteger)nr
    { Snapshot *s = wrapper->c64->getHistoricSnapshot((int)nr); return s ? s->getHeaderSize() : 0; }

- (uint8_t *) historicSnapshotHeader:(NSInteger)nr
    { Snapshot *s = wrapper->c64->getHistoricSnapshot((int)nr); return s ? s->getHeader() : NULL; }


- (NSInteger) historicSnapshotDataSize:(NSInteger)nr
    { Snapshot *s = wrapper->c64->getHistoricSnapshot((int)nr); return s ? s->getDataSize() : 0; }

- (time_t)historicSnapshotTimestamp:(NSInteger)nr { Snapshot *s = wrapper->c64->getHistoricSnapshot((int)nr); return s ? s->getTimestamp() : 0; }

- (uint8_t *) historicSnapshotData:(NSInteger)nr
    { Snapshot *s = wrapper->c64->getHistoricSnapshot((int)nr); return s ? s->getData() : NULL; }

- (unsigned char *)historicSnapshotImageData:(NSInteger)nr
    { Snapshot *s = wrapper->c64->getHistoricSnapshot((int)nr); return s ? s->getImageData() : NULL; }
- (NSInteger)historicSnapshotImageWidth:(NSInteger)nr
    { Snapshot *s = wrapper->c64->getHistoricSnapshot((int)nr); return s ? s->getImageWidth() : 0; }
- (NSInteger)historicSnapshotImageHeight:(NSInteger)nr
{ Snapshot *s = wrapper->c64->getHistoricSnapshot((int)nr); return s ? s->getImageHeight() : 0; }

- (bool)restoreHistoricSnapshot:(NSInteger)nr {
    return wrapper->c64->restoreHistoricSnapshotSafe((unsigned)nr); }

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
//                                Snapshot
// --------------------------------------------------------------------------

@implementation SnapshotProxy

- (instancetype) init
{
    if (self = [super init]) {
        wrapper = new SnapshotWrapper();
        wrapper->snapshot = new Snapshot;
    }
    return self;
}

- (instancetype) initWithSnapshot:(Snapshot *)snapshot
{
    if (snapshot == nil) return nil;
    
    if (self = [super init]) {
        wrapper = new SnapshotWrapper();
        wrapper->snapshot = snapshot;
    }
    return self;
}

- (void) dealloc
{	
	NSLog(@"V64Snapshot::dealloc");

	if (wrapper->snapshot) delete wrapper->snapshot;
    if (wrapper) delete wrapper;
}

+ (BOOL) isSnapshotFile:(NSString *)filename {
    return Snapshot::isSnapshot([filename UTF8String]);
}
+ (BOOL) isSnapshotFile:(NSString *)filename major:(int)x minor:(int)y subminor:(int)z {
    return Snapshot::isSnapshot([filename UTF8String], x, y, z);
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

- (SnapshotWrapper *)wrapper { return wrapper; }
- (bool) readDataFromFile:(NSString *)path {
    return wrapper->snapshot->readFromFile([path UTF8String]); }
- (bool) writeDataToFile:(NSString *)path {
    return wrapper->snapshot->writeToFile([path UTF8String]); }

@end


// --------------------------------------------------------------------------
//                                  Archive
// --------------------------------------------------------------------------

@implementation ArchiveProxy

- (instancetype) initWithArchive:(Archive *)archive
{
    NSLog(@"ArchiveProxy::initWithArchive %p", archive);

    if (archive == NULL) return nil;
    
    if (self = [super init]) {
        wrapper = new ArchiveWrapper();
        wrapper->archive = archive;
    }
    return self;
}

- (void)dealloc
{
    NSLog(@"ArchiveProxy %p deleted", wrapper->archive);
    
    if (wrapper->archive) delete wrapper->archive;
    if (wrapper) delete wrapper;
}

- (ArchiveWrapper *)wrapper { return wrapper; }
- (NSString *)getPath { return [NSString stringWithUTF8String:wrapper->archive->getPath()]; }
- (NSString *)getName { return [NSString stringWithUTF8String:wrapper->archive->getName()]; }
- (NSInteger)getType { return (NSInteger)wrapper->archive->getType(); }

- (NSInteger)getNumberOfItems {
    return (NSInteger)wrapper->archive->getNumberOfItems();
}
- (NSString *)getNameOfItem:(NSInteger)item {
    return [NSString stringWithUTF8String:wrapper->archive->getNameOfItem((int)item)];
}
- (NSString *)getUnicodeNameOfItem:(NSInteger)item maxChars:(NSInteger)max {

    const unsigned short *name = wrapper->archive->getUnicodeNameOfItem((int)item, max);
    
    if (name == NULL)
        return NULL;
    
    unsigned numChars;
    for (numChars = 0; name[numChars] != 0; numChars++);
    
    return [NSString stringWithCharacters:name length:numChars];
}

- (const char *)getNameOfItemUTF8:(NSInteger)item {
    return wrapper->archive->getNameOfItem((int)item);
}

// - (const char *)getNameOfItemAsPETStringUTF8:(NSInteger)item {
//     return wrapper->archive->getNameOfItemAsPETString((int)item);
// }

- (NSInteger) getSizeOfItem:(NSInteger)item {
    return wrapper->archive->getSizeOfItem((int)item);
}
- (NSInteger) getSizeOfItemInBlocks:(NSInteger)item {
    return wrapper->archive->getSizeOfItemInBlocks((int)item);
}
- (NSString *) getTypeOfItem:(NSInteger)item {
    return [NSString stringWithUTF8String:wrapper->archive->getTypeOfItem((int)item)];
}
- (BOOL)writeToFile:(NSString *)filename {
    return wrapper->archive->writeToFile([filename UTF8String]);
}

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
    Archive *other = [otherArchive wrapper]->archive;
    T64Archive *archive = T64Archive::archiveFromArchive(other);
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
    D64Archive *other = (D64Archive *)[otherArchive wrapper]->archive;
    D64Archive *archive = D64Archive::archiveFromD64Archive(other);
    return archive ? [[D64ArchiveProxy alloc] initWithArchive:archive] : nil;
}

+ (instancetype) archiveFromArchive:(ArchiveProxy *)otherArchive
{
    Archive *other = [otherArchive wrapper]->archive;
    D64Archive *archive = D64Archive::archiveFromArchive(other);
    return archive ? [[D64ArchiveProxy alloc] initWithArchive:archive] : nil;
}

+ (instancetype) archiveFromVC1541:(VC1541Proxy *)vc1541
{
    D64Archive *archive = [vc1541 wrapper]->vc1541->convertToD64();
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
    Archive *other = [otherArchive wrapper]->archive;
    PRGArchive *archive = PRGArchive::archiveFromArchive(other);
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
    Archive *other = [otherArchive wrapper]->archive;
    P00Archive *archive = P00Archive::archiveFromArchive(other);
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

- (instancetype) initWithTAPContainer:(TAPContainer *)container
{
    NSLog(@"TAPContainerProxy::initWithTAPContainer");
    
    if (container == NULL) return nil;
    
    if (self = [super init]) {
        wrapper = new TAPContainerWrapper();
        wrapper->tapcontainer = container;
    }
    return self;
}

- (void)dealloc
{
    NSLog(@"TAPContainerProxy::dealloc (deleted TAPContainer at %p)", wrapper->tapcontainer);
    
    if (wrapper) {
        if (wrapper->tapcontainer)
            delete wrapper->tapcontainer;
        delete wrapper;
    }
}

- (TAPContainerWrapper *)wrapper { return wrapper; }

+ (BOOL) isTAPFile:(NSString *)filename
{
    return TAPContainer::isTAPFile([filename UTF8String]);
}

+ (instancetype) containerFromTAPFile:(NSString *)filename
{
    TAPContainer *container = TAPContainer::containerFromTAPFile([filename UTF8String]);
    return container ? [[TAPContainerProxy alloc] initWithTAPContainer:container] : nil;
}

- (NSString *)getPath { return [NSString stringWithUTF8String:wrapper->tapcontainer->getPath()]; }
- (NSString *)getName { return [NSString stringWithUTF8String:wrapper->tapcontainer->getName()]; }
- (NSInteger)getType { return (NSInteger)wrapper->tapcontainer->getType(); }
- (NSInteger)TAPversion { return (NSInteger)wrapper->tapcontainer->TAPversion(); }

@end


// --------------------------------------------------------------------------
//                                 Cartridge
// --------------------------------------------------------------------------

@implementation CRTContainerProxy

- (CRTContainerWrapper *)wrapper { return wrapper; }

- (instancetype) initWithCRTContainer:(CRTContainer *)container
{
    if (self = [super init]) {
        wrapper = new CRTContainerWrapper();
        wrapper->crtcontainer = container;
    }
    return self;
}

- (void)dealloc
{
    NSLog(@"CRTContainerProxy::dealloc (deleted CRTContainer at %p)", wrapper->crtcontainer);
    
    if (wrapper) {
        if (wrapper->crtcontainer)
            delete wrapper->crtcontainer;
        delete wrapper;
    }
}

+ (BOOL) isCRTFile:(NSString *)filename
{
    return CRTContainer::isCRTFile([filename UTF8String]);
}

+ (instancetype)containerFromCRTFile:(NSString *)filename
{
    CRTContainer *container = CRTContainer::containerFromCRTFile([filename UTF8String]);
    return container ? [[CRTContainerProxy alloc] initWithCRTContainer:container] : nil;
}

- (CartridgeType)type { return wrapper->crtcontainer->getCartridgeType(); }
- (NSString *)typeName { return [NSString stringWithUTF8String:wrapper->crtcontainer->getCartridgeTypeName()]; }
- (BOOL) isSupportedType { return Cartridge::isSupportedType((CartridgeType)[self type]); }


@end

