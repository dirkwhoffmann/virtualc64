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

// --------------------------------------------------------------------------
//                                    CPU
// --------------------------------------------------------------------------

@implementation CPUProxy

- (id) initWithCPU:(CPU *)c
{
    self = [super init];	
	cpu = c;	
	return self;
}

- (void) dump { cpu->dumpState(); }
- (bool) tracingEnabled { return cpu->tracingEnabled(); }
- (void) setTraceMode:(bool)b { cpu->setTraceMode(b); }

- (uint16_t) PC { return cpu->getPC_at_cycle_0(); }
- (void) setPC:(uint16_t)pc { cpu->setPC_at_cycle_0(pc); }
- (uint8_t) SP { return cpu->getSP(); }
- (void) setSP:(uint8_t)sp { cpu->setSP(sp); }
- (uint8_t) A { return cpu->getA(); }
- (void) setA:(uint8_t)a { cpu->setA(a); }
- (uint8_t) X { return cpu->getX(); }
- (void) setX:(uint8_t)x { cpu->setX(x); }
- (uint8_t) Y { return cpu->getY(); }
- (void) setY:(uint8_t)y { cpu->setY(y); }
- (bool) N { return cpu->getN(); }
- (void) setN:(bool)b { cpu->setN(b); }
- (bool) Z { return cpu->getZ(); }
- (void) setZ:(bool)b { cpu->setZ(b); }
- (bool) C { return cpu->getC(); }
- (void) setC:(bool)b { cpu->setC(b); }
- (bool) I { return cpu->getI(); }
- (void) setI:(bool)b { cpu->setI(b); }
- (bool) B { return cpu->getB(); }
- (void) setB:(bool)b { cpu->setB(b); }
- (bool) D { return cpu->getD(); }
- (void) setD:(bool)b { cpu->setD(b); }
- (bool) V { return cpu->getV(); }
- (void) setV:(bool)b { cpu->setV(b); }

- (uint16_t) peekPC { return cpu->mem->peek(cpu->getPC_at_cycle_0()); }
- (uint8_t) lengthOfInstruction:(uint8_t)opcode { return cpu->getLengthOfInstruction(opcode); }
- (uint8_t) lengthOfInstructionAtAddress:(uint16_t)addr { return cpu->getLengthOfInstructionAtAddress(addr); }
- (uint8_t) lengthOfCurrentInstruction { return cpu->getLengthOfCurrentInstruction(); }
- (uint16_t) addressOfNextInstruction { return cpu->getAddressOfNextInstruction(); }
- (const char *) mnemonic:(uint8_t)opcode { return cpu->getMnemonic(opcode); }
- (CPU::AddressingMode) addressingMode:(uint8_t)opcode { return cpu->getAddressingMode(opcode); }

- (int) topOfCallStack { return cpu->getTopOfCallStack(); }
- (int) breakpoint:(int)addr { return cpu->getBreakpoint(addr); }
- (void) setBreakpoint:(int)addr tag:(uint8_t)t { cpu->setBreakpoint(addr, t); }
- (void) setHardBreakpoint:(int)addr { cpu->setHardBreakpoint(addr); }
- (void) deleteHardBreakpoint:(int)addr { cpu->deleteHardBreakpoint(addr); }
- (void) toggleHardBreakpoint:(int)addr { cpu->toggleHardBreakpoint(addr); }
- (void) setSoftBreakpoint:(int)addr { cpu->setSoftBreakpoint(addr); }
- (void) deleteSoftBreakpoint:(int)addr { cpu->deleteSoftBreakpoint(addr); }
- (void) toggleSoftBreakpoint:(int)addr { cpu->toggleSoftBreakpoint(addr); }

@end

// --------------------------------------------------------------------------
//                                   Memory
// --------------------------------------------------------------------------

@implementation MemoryProxy

- (id) initWithMemory:(Memory *)m
{
    self = [super init];	
	mem = m;	
	return self;
}

- (void) dump { mem->dumpState(); }

- (uint8_t) peek:(uint16_t)addr { return mem->peek(addr); }
- (uint16_t) peekWord:(uint16_t)addr { return mem->peekWord(addr); }
- (uint8_t) peekFrom:(uint16_t)addr memtype:(Memory::MemoryType)type { return mem->peekFrom(addr, type); }
- (void) poke:(uint16_t)addr value:(uint8_t)val { mem->poke(addr, val); }
- (void) pokeTo:(uint16_t)addr value:(uint8_t)val memtype:(Memory::MemoryType)type { mem->pokeTo(addr, val, type); }
- (bool) isValidAddr:(uint16_t)addr memtype:(Memory::MemoryType)type { return mem->isValidAddr(addr, type); }

@end

// --------------------------------------------------------------------------
//                                    VIC
// --------------------------------------------------------------------------

@implementation VICProxy

- (id) initWithVIC:(VIC *)v
{
    self = [super init];	
	vic = v;	
	return self;
}

- (void) dump { vic->dumpState(); }

- (void *) screenBuffer { return vic->screenBuffer(); }

- (NSColor *) color:(int)nr
{
    assert (0 <= nr && nr < 16);
    
    uint32_t color = vic->getColor(nr);
/*
    uint8_t r = (color >> 24) & 0xFF;
    uint8_t g = (color >> 16) & 0xFF;
    uint8_t b = (color >> 8) & 0xFF;
*/
    uint8_t r = color & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = (color >> 16) & 0xFF;
    
	return [NSColor colorWithCalibratedRed:(float)r/255.0 green:(float)g/255.0 blue:(float)b/255.0 alpha:1.0];
}

- (uint16_t) memoryBankAddr { return vic->getMemoryBankAddr(); }
- (void) setMemoryBankAddr:(uint16_t)addr { vic->setMemoryBankAddr(addr); }
- (uint16_t) screenMemoryAddr { return vic->getScreenMemoryAddr(); }
- (void) setScreenMemoryAddr:(uint16_t)addr { vic->setScreenMemoryAddr(addr); }
- (uint16_t) characterMemoryAddr { return vic->getCharacterMemoryAddr(); }
- (void) setCharacterMemoryAddr:(uint16_t)addr { vic->setCharacterMemoryAddr(addr); }

- (unsigned)totalScreenWidth { return vic->getTotalScreenWidth(); }
- (unsigned)totalScreenHeight {	return vic->getTotalScreenHeight(); }
- (unsigned)firstVisibleLine { return vic->getFirstVisibleLine(); }
- (unsigned)lastVisibleLine { return vic->getLastVisibleLine(); }

- (int) displayMode { return vic->getDisplayMode(); }
- (void) setDisplayMode:(int)mode { vic->setDisplayMode((VIC::DisplayMode)mode); }
- (int) screenGeometry { return (int)vic->getScreenGeometry(); }
- (void) setScreenGeometry:(int)mode { vic->setScreenGeometry((VIC::ScreenGeometry)mode); }
- (int) horizontalRasterScroll { return vic->getHorizontalRasterScroll(); }
- (void) setHorizontalRasterScroll:(int)offset { vic->setHorizontalRasterScroll(offset & 0x07); }
- (int) verticalRasterScroll { return vic->getVerticalRasterScroll(); }
- (void) setVerticalRasterScroll:(int)offset { vic->setVerticalRasterScroll(offset & 0x07); }

- (bool) spriteVisibilityFlag:(int)nr { return vic->spriteIsEnabled(nr); }
- (void) setSpriteVisibilityFlag:(int)nr value:(bool)flag { vic->setSpriteEnabled(nr, flag); }
- (void) toggleSpriteVisibilityFlag:(int)nr { vic->toggleSpriteEnabled(nr); }

- (bool) spriteSpriteCollisionFlag:(int)nr { return vic->getSpriteSpriteCollision(nr); }
- (void) setSpriteSpriteCollisionFlag:(int)nr value:(bool)flag { vic->setSpriteSpriteCollision(nr, flag); }
- (void) toggleSpriteSpriteCollisionFlag:(int)nr { vic->toggleSpriteSpriteCollisionFlag(nr); }

- (bool) spriteBackgroundCollisionFlag:(int)nr { return vic->getSpriteBackgroundCollision(nr); }
- (void) setSpriteBackgroundCollisionFlag:(int)nr value:(bool)flag { vic->setSpriteBackgroundCollision(nr, flag); }
- (void) toggleSpriteBackgroundCollisionFlag:(int)nr { vic->toggleSpriteBackgroundCollisionFlag(nr); }

- (bool) spriteBackgroundPriorityFlag:(int)nr { return vic->spriteIsDrawnInBackground(nr); }
- (void) setSpriteBackgroundPriorityFlag:(int)nr value:(bool)flag { vic->setSpriteInBackground(nr, flag); }
- (void) toggleSpriteBackgroundPriorityFlag:(int)nr { vic->spriteToggleBackgroundPriorityFlag(nr); }

- (bool) spriteMulticolorFlag:(int)nr { return vic->spriteIsMulticolor(nr); }
- (void) setSpriteMulticolorFlag:(int)nr value:(bool)flag { vic->setSpriteMulticolor(nr, flag); }
- (void) toggleSpriteMulticolorFlag:(int)nr { vic->toggleMulticolorFlag(nr); }

- (bool) spriteStretchXFlag:(int)nr { return vic->spriteWidthIsDoubled(nr); }
- (void) setSpriteStretchXFlag:(int)nr value:(bool)flag { vic->setSpriteStretchX(nr, flag); }
- (void) toggleSpriteStretchXFlag:(int)nr { vic->spriteToggleStretchXFlag(nr); }

- (bool) spriteStretchYFlag:(int)nr { return vic->spriteHeightIsDoubled(nr); }
- (void) setSpriteStretchYFlag:(int)nr value:(bool)flag { return vic->setSpriteStretchY(nr, flag); }
- (void) toggleSpriteStretchYFlag:(int)nr { vic->spriteToggleStretchYFlag(nr); }

- (int) spriteX:(int)nr { return vic->getSpriteX(nr); }
- (void) setSpriteX:(int)nr value:(int)x { vic->setSpriteX(nr, x); }
- (int) spriteY:(int)nr { return vic->getSpriteY(nr); }
- (void) setSpriteY:(int)nr value:(int)y { vic->setSpriteY(nr, y); }
- (int) spriteColor:(int)nr { return vic->spriteColor(nr); }
- (void) setSpriteColor:(int)nr value:(int)c { vic->setSpriteColor(nr, c); }

- (uint16_t) rasterline { return vic->getScanline(); }
- (void) setRasterline:(uint16_t)line { vic->setScanline(line); }
- (uint16_t) rasterInterruptLine { return vic->rasterInterruptLine(); }
- (void) setRasterInterruptLine:(uint16_t)line { vic->setRasterInterruptLine(line); }
- (bool) rasterInterruptFlag { return vic->rasterInterruptEnabled(); }
- (void) setRasterInterruptFlag:(bool)b { vic->setRasterInterruptEnable(b); }
- (void) toggleRasterInterruptFlag { vic->toggleRasterInterruptFlag(); }

- (bool) hideSprites { return vic->hideSprites(); }
- (void) setHideSprites:(bool)b { vic->setHideSprites(b); }
- (bool) showIrqLines { return vic->showIrqLines(); }
- (void) setShowIrqLines:(bool)b { vic->setShowIrqLines(b); }
- (bool) showDmaLines { return vic->showDmaLines(); }
- (void) setShowDmaLines:(bool)b { vic->setShowDmaLines(b); }

@end

// --------------------------------------------------------------------------
//                                     CIA
// --------------------------------------------------------------------------

@implementation CIAProxy

- (id) initWithCIA:(CIA *)c
{
    self = [super init];	
	cia = c;	
	return self;
}

- (void) dump { cia->dumpState(); }
- (bool) tracingEnabled { return cia->tracingEnabled(); }
- (void) setTraceMode:(bool)b { cia->setTraceMode(b); }

- (uint8_t) dataPortA { return cia->getDataPortA(); }
- (void) setDataPortA:(uint8_t)v { cia->setDataPortA(v); }
- (uint8_t) dataPortDirectionA { return cia->getDataPortDirectionA(); }
- (void) setDataPortDirectionA:(uint8_t)v { cia->setDataPortDirectionA(v); }
- (uint16_t) timerA { return cia->getCounterA(); }
- (void) setTimerA:(uint16_t)v { cia->setCounterA(v); }
- (uint16_t) timerLatchA { return cia->getLatchA(); }
- (void) setTimerLatchA:(uint16_t)v { cia->setLatchA(v); }
- (bool) startFlagA { return cia->isStartedA(); }
- (void) setStartFlagA:(bool)b { cia->setStartedA(b); }
- (void) toggleStartFlagA {cia->toggleStartFlagA(); }
- (bool) oneShotFlagA { return cia->isOneShotA(); }
- (void) setOneShotFlagA:(bool)b { cia->setOneShotA(b); }
- (void) toggleOneShotFlagA { cia->toggleOneShotFlagA(); }
- (bool) underflowFlagA { return cia->willIndicateUnderflowA(); }
- (void) setUnderflowFlagA:(bool)b { return cia->setIndicateUnderflowA(b); }
- (void) toggleUnderflowFlagA { cia->toggleUnderflowFlagA(); }
- (bool) pendingSignalFlagA {return cia->isSignalPendingA(); }
- (void) setPendingSignalFlagA:(bool)b { cia->setSignalPendingA(b); }
- (void) togglePendingSignalFlagA { cia->togglePendingSignalFlagA(); }
- (bool) interruptEnableFlagA { return cia->isInterruptEnabledA(); }
- (void) setInterruptEnableFlagA:(bool)b { cia->setInterruptEnabledA(b); }
- (void) toggleInterruptEnableFlagA {cia->toggleInterruptEnableFlagA(); }

- (uint8_t) dataPortB { return cia->getDataPortB(); }
- (void) setDataPortB:(uint8_t)v { cia->setDataPortB(v); }
- (uint8_t) dataPortDirectionB { return cia->getDataPortDirectionB(); }
- (void) setDataPortDirectionB:(uint8_t)v { cia->setDataPortDirectionB(v); }
- (uint16_t) timerB { return cia->getCounterB(); }
- (void) setTimerB:(uint16_t)v { cia->setCounterB(v); }
- (uint16_t) timerLatchB { return cia->getLatchB(); }
- (void) setTimerLatchB:(uint16_t)v { cia->setLatchB(v); }
- (bool) startFlagB { return cia->isStartedB(); }
- (void) setStartFlagB:(bool)b { cia->setStartedB(b); }
- (void) toggleStartFlagB {cia->toggleStartFlagB(); }
- (bool) oneShotFlagB { return cia->isOneShotB(); }
- (void) setOneShotFlagB:(bool)b { cia->setOneShotB(b); }
- (void) toggleOneShotFlagB { cia->toggleOneShotFlagB(); }
- (bool) underflowFlagB { return cia->willIndicateUnderflowB(); }
- (void) setUnderflowFlagB:(bool)b { return cia->setIndicateUnderflowB(b); }
- (void) toggleUnderflowFlagB { cia->toggleUnderflowFlagB(); }
- (bool) pendingSignalFlagB {return cia->isSignalPendingB(); }
- (void) setPendingSignalFlagB:(bool)b { cia->setSignalPendingB(b); }
- (void) togglePendingSignalFlagB { cia->togglePendingSignalFlagB(); }
- (bool) interruptEnableFlagB { return cia->isInterruptEnabledB(); }
- (void) setInterruptEnableFlagB:(bool)b { cia->setInterruptEnabledB(b); }
- (void) toggleInterruptEnableFlagB {cia->toggleInterruptEnableFlagB(); }

- (uint8_t) todHours { return cia->tod.getTodHours(); }
- (void) setTodHours:(uint8_t)value { cia->tod.setTodHours(value); }
- (uint8_t) todMinutes { return cia->tod.getTodMinutes(); }
- (void) setTodMinutes:(uint8_t)value { cia->tod.setTodMinutes(value); }
- (uint8_t) todSeconds { return cia->tod.getTodSeconds(); }
- (void) setTodSeconds:(uint8_t)value { cia->tod.setTodSeconds(value); }
- (uint8_t) todTenth { return cia->tod.getTodTenth(); }
- (void) setTodTenth:(uint8_t)value { cia->tod.setTodTenth(value); }

- (uint8_t) alarmHours { return cia->tod.getAlarmHours(); }
- (void) setAlarmHours:(uint8_t)value { cia->tod.setAlarmHours(value); }
- (uint8_t) alarmMinutes { return cia->tod.getAlarmMinutes(); }
- (void) setAlarmMinutes:(uint8_t)value { cia->tod.setAlarmMinutes(value); }
- (uint8_t) alarmSeconds { return cia->tod.getAlarmSeconds(); }
- (void) setAlarmSeconds:(uint8_t)value { cia->tod.setAlarmSeconds(value); }
- (uint8_t) alarmTenth { return cia->tod.getAlarmTenth(); }
- (void) setAlarmTenth:(uint8_t)value { cia->tod.setAlarmTenth(value); }
- (bool) isTodInterruptEnabled { return cia->isInterruptEnabledTOD(); }
- (void) setTodInterruptEnabled:(bool)b { cia->setInterruptEnabledTOD(b); }

@end 

// --------------------------------------------------------------------------
//                                    Keyboard
// --------------------------------------------------------------------------

@implementation KeyboardProxy

- (id) initWithKeyboard:(Keyboard *)kb
{
    self = [super init];	
	keyboard = kb;	
	return self;
}

- (void) dump { keyboard->dumpState(); }
- (void) pressRunstopKey { keyboard->pressRunstopKey(); }
- (void) releaseRunstopKey { keyboard->releaseRunstopKey(); }
- (void) pressCommodoreKey { keyboard->pressCommodoreKey(); }
- (void) releaseCommodoreKey { keyboard->releaseCommodoreKey(); }
- (void) typeFormat { keyboard->typeFormat(); }
- (void) typeRun { keyboard->typeRun(); }

@end

// --------------------------------------------------------------------------
//                                    SID
// --------------------------------------------------------------------------

@implementation SIDProxy

- (id) initWithSID:(SIDWrapper *)s
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

- (id) initWithIEC:(IEC *)bus
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
//                                     VIA
// -------------------------------------------------------------------------

@implementation VIAProxy

- (id) initWithVIA:(VIA6522 *)v
{
    self = [super init];	
	via = v;
	return self;
}

- (void) dump { via->dumpState(); }
- (bool) tracingEnabled { return via->tracingEnabled(); }
- (void) setTraceMode:(bool)b { via->setTraceMode(b); }

@end

// --------------------------------------------------------------------------
//                                    VC1541
// -------------------------------------------------------------------------

@implementation VC1541Proxy

@synthesize cpu, mem, via1, via2;

- (id) initWithVC1541:(VC1541 *)vc
{
    self = [super init];	
	vc1541 = vc;
	cpu = [[CPUProxy alloc] initWithCPU:vc->cpu];
	mem = [[MemoryProxy alloc] initWithMemory:vc->mem];
	via1 = [[VIAProxy alloc] initWithVIA:vc->via1];
	via2 = [[VIAProxy alloc] initWithVIA:vc->via2];
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
- (void) ejectDisk { vc1541->ejectDisc(); }
- (bool) hasRedLED { return vc1541->hasRedLED(); }
- (bool) hasDisk { return vc1541->hasDisk(); }

@end

// --------------------------------------------------------------------------
//                                     C64
// -------------------------------------------------------------------------

@implementation C64Proxy

@synthesize c64, cpu, mem, vic, cia1, cia2, sid, keyboard, iec, vc1541;
@synthesize iecBusIsBusy;

- (id) init
{
	NSLog(@"C64Proxy::init");
	
    self = [super init];
	
	// Create virtual machine and initialize references
	c64 = new C64();
	
	// Create sub proxys
	cpu = [[CPUProxy alloc] initWithCPU:c64->cpu];
	mem = [[MemoryProxy alloc] initWithMemory:c64->mem];
	vic = [[VICProxy alloc] initWithVIC:c64->vic];
	cia1 = [[CIAProxy alloc] initWithCIA:c64->cia1];
	cia2 = [[CIAProxy alloc] initWithCIA:c64->cia2];
	sid = [[SIDProxy alloc] initWithSID:c64->sid];
	keyboard = [[KeyboardProxy alloc] initWithKeyboard:c64->keyboard];
	iec = [[IECProxy alloc] initWithIEC:c64->iec];
	vc1541 = [[VC1541Proxy alloc] initWithVC1541:c64->floppy];
	
	// Initialize CoreAudio sound interface
	if (!(audioDevice = [[AudioDevice alloc] initWithC64:c64])) {
		NSLog(@"WARNING: Couldn't initialize AudioDevice. Sound disabled.");
	}
		
    return self;
}

- (void) awakeFromNib
{
	NSLog(@"C64Proxy::awakeFromNib");
}

- (void) kill
{
	assert(c64 != NULL);
	NSLog(@"Deleting C64...");

	// Delete sound device
	NSLog(@"  Deleting sound device");
	[self disableAudio];
	audioDevice = nil;
	
	NSLog(@"  Deleting virtual machine");
	delete c64;
	c64 = NULL;
}

- (void) refresh
{
	// TODO: Implement iecBusIsBusy in core emulator
	// iecBusIsBusy = c64->iecBusIsBusy;
	
}

- (VIC::ColorScheme) colorScheme { return c64->getColorScheme(); }
- (void) setColorScheme:(VIC::ColorScheme)scheme { c64->setColorScheme(scheme); }
- (bool) audioFilter { return c64->getAudioFilter(); }
- (void) setAudioFilter:(bool)b { c64->setAudioFilter(b); }
- (bool) reSID { return c64->getReSID(); }
- (void) setReSID:(bool)b { c64->setReSID(b); }
- (int) samplingMethod { return (int)(c64->getSamplingMethod()); }
- (void) setSamplingMethod:(int)value { c64->setSamplingMethod((sampling_method)value); }
- (int) chipModel { return (chip_model)(c64->getChipModel()); }
- (void) setChipModel:(int)value {c64->setChipModel((chip_model)value); }

- (void) loadFromSnapshot:(V64Snapshot *)snapshot 
{ 
    c64->suspend(); 
    c64->loadFromSnapshot([snapshot snapshot]); 
    if ([self isPAL]) 
        [screen setPAL];
    else
        [screen setNTSC];
    c64->resume(); 
}

- (void) saveToSnapshot:(V64Snapshot *)snapshot 
{ 
    c64->suspend(); 
    c64->saveToSnapshot([snapshot snapshot]); 
    c64->resume(); 
}

- (CIAProxy *) cia:(int)num { if (num == 1) return [self cia1]; else if (num == 2) return [self cia2]; else assert(0); } 

- (void) dump { c64->dumpState(); }

- (Message *)message { return c64->getMessage(); }
- (void) putMessage:(int)msg { c64->putMessage(msg); }
- (void) reset { c64->reset(); }
- (void) fastReset { c64->fastReset(); }
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
- (int) frameDelayOffset { return c64->getFrameDelayOffset(); }
- (void) setFrameDelayOffset:(int)delay { c64->setFrameDelayOffset(delay); }
- (int) buildNr { return c64->build(); }

//- (int) numberOfMissingRoms { return c64->numberOfMissingRoms(); }
- (uint8_t) missingRoms { return c64->getMissingRoms(); }
- (bool) loadBasicRom:(NSString *)filename { return c64->mem->isBasicRom([filename UTF8String]) && c64->loadRom([filename UTF8String]); }
- (bool) loadCharRom:(NSString *)filename { return c64->mem->isCharRom([filename UTF8String]) && c64->loadRom([filename UTF8String]); }
- (bool) loadKernelRom:(NSString *)filename { return c64->mem->isKernelRom([filename UTF8String]) && c64->loadRom([filename UTF8String]); }
- (bool) loadVC1541Rom:(NSString *)filename { return c64->floppy->mem->is1541Rom([filename UTF8String]) && c64->loadRom([filename UTF8String]); }

- (bool) attachCartridge:(Cartridge *)c { return c64->attachCartridge(c); }
- (bool) detachCartridge { return c64->detachCartridge(); }
- (bool) isCartridgeAttached { return c64->isCartridgeAttached(); }

- (bool) mountArchive:(D64Archive *)a { return c64->mountArchive(a); }
- (bool) flushArchive:(Archive *)a item:(int)nr { return c64->flushArchive(a,nr); }

- (bool) warp { return c64->getWarp(); }
- (void) setWarp:(bool)b { c64->setWarp(b); }	
- (bool) alwaysWarp { return c64->getAlwaysWarp(); }
- (void) setAlwaysWarp:(bool)b { c64->setAlwaysWarp(b); }
- (bool) warpLoad { return c64->getWarpLoad(); }
- (void) setWarpLoad:(bool)b { c64->setWarpLoad(b); }

- (long) cycles { return (long)c64->getCycles(); }

// Cheatbox
- (int) historicSnapshots { return c64->numHistoricSnapshots(); }
- (uint8_t *) historicSnapshotFileContents:(int)nr { Snapshot *s = c64->getHistoricSnapshot(nr); return s ? s->getFileContents() : NULL; }
- (unsigned) historicSnapshotFileContentsSize:(int) nr { Snapshot *s = c64->getHistoricSnapshot(nr); return s ? s->getFileContentsSize() : 0; }
- (unsigned char *)historicSnapshotImageData:(int)nr { Snapshot *s = c64->getHistoricSnapshot(nr); return s ? s->getImageData() : NULL; }
- (time_t)historicSnapshotTimestamp:(int)nr { Snapshot *s = c64->getHistoricSnapshot(nr); return s ? s->getTimestamp() : 0; }
- (bool)historicSnapshotIsPAL:(int)nr { Snapshot *s = c64->getHistoricSnapshot(nr); return s ? s->isPAL() : false; }
- (bool)revertToHistoricSnapshot:(int)nr { Snapshot *s = c64->getHistoricSnapshot(nr); return s ? c64->loadFromSnapshot(s), true : false; }

// Joystick
- (Joystick *) joystick:(int)nr
{
    if (nr == 1) return c64->joystick1;
    if (nr == 2) return c64->joystick2;
    assert(0);
};

// Audio hardware
- (void) enableAudio { [audioDevice startPlayback]; }
- (void) disableAudio {	[audioDevice stopPlayback]; }

// User triggered interrupts
- (void) keyboardPressRunstopRestore { c64->runstopRestore(); }

@end

// --------------------------------------------------------------------------
//                                  Snapshot
// --------------------------------------------------------------------------

@implementation V64Snapshot

@synthesize snapshot;

- (id) init
{
	NSLog(@"V64Snapshot::init");
	
	if (!(self = [super init]))
		return nil;

	snapshot = new Snapshot;
	return self;
}

- (id) initWithSnapshot:(Snapshot *)s
{
	NSLog(@"V64Snapshot::initWithSnapshot");
	
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

+ (id) snapshotFromC64:(C64Proxy *)c64
{
	NSLog(@"V64Snapshot::snapshotFromC64");

	V64Snapshot *newSnapshot = [[self alloc] init];
	[c64 saveToSnapshot:newSnapshot];
	return newSnapshot;
}

+ (id) snapshotFromSnapshot:(Snapshot *)snapshot
{
	NSLog(@"V64Snapshot::snapshotFromSnapshot");
	
	if (snapshot == NULL)
		return nil;
	
	V64Snapshot *newSnapshot = [[self alloc] initWithSnapshot:snapshot];
	return newSnapshot;
}
	
+ (id) snapshotFromFile:(NSString *)path
{
	NSLog(@"V64Snapshot::snapshotFromFile");

	return [self snapshotFromSnapshot:Snapshot::snapshotFromFile([path UTF8String])];
}

+ (id) snapshotFromBuffer:(const void *)buffer length:(unsigned)length
{
	NSLog(@"V64Snapshot::snapshotFromBuffer");
	
	return [self snapshotFromSnapshot:Snapshot::snapshotFromBuffer(buffer, length)];
}

- (uint8_t *)fileContents { return snapshot->getFileContents(); }
- (unsigned)fileContentsSize { return snapshot->getFileContentsSize(); }
- (unsigned char *)imageData { return snapshot->getImageData(); }
- (time_t)timeStamp { return snapshot->getTimestamp(); }
- (bool) readDataFromFile:(NSString *)path { return snapshot->readFromFile([path UTF8String]); }
- (bool) writeDataToFile:(NSString *)path { return snapshot->writeToFile([path UTF8String]); }

@end

	
