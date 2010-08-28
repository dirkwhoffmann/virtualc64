/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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

#import "MyDocument.h"
#import "AudioDevice.h"

// --------------------------------------------------------------------------
//                                    CPU
// --------------------------------------------------------------------------

@implementation CPUProxy

- (id) initWithCPU:(CPU *)c;
{
    self = [super init];	
	cpu = c;	
	return self;
}

- (void) dump { cpu->dumpState(); }

- (bool) tracingEnabled { return cpu->tracingEnabled(); }
- (void) setTraceMode:(bool)b { cpu->setTraceMode(b); }

- (uint16_t) getPC { return cpu->getPC_at_cycle_0(); }
- (void) setPC:(uint16_t)pc { cpu->setPC_at_cycle_0(pc); }
- (uint8_t) getSP { return cpu->getSP(); }
- (void) setSP:(uint8_t)sp { cpu->setSP(sp); }
- (uint8_t) getA { return cpu->getA(); }
- (void) setA:(uint8_t)a { cpu->setA(a); }
- (uint8_t) getX { return cpu->getX(); }
- (void) setX:(uint8_t)x { cpu->setX(x); }
- (uint8_t) getY { return cpu->getY(); }
- (void) setY:(uint8_t)y { cpu->setY(y); }
- (bool) getN { return cpu->getN(); }
- (void) setN:(bool)b { cpu->setN(b); }
- (bool) getZ { return cpu->getZ(); }
- (void) setZ:(bool)b { cpu->setZ(b); }
- (bool) getC { return cpu->getC(); }
- (void) setC:(bool)b { cpu->setC(b); }
- (bool) getI { return cpu->getI(); }
- (void) setI:(bool)b { cpu->setI(b); }
- (bool) getB { return cpu->getB(); }
- (void) setB:(bool)b { cpu->setB(b); }
- (bool) getD { return cpu->getD(); }
- (void) setD:(bool)b { cpu->setD(b); }
- (bool) getV { return cpu->getV(); }
- (void) setV:(bool)b { cpu->setV(b); }

- (uint16_t) peekPC { return cpu->mem->peek(cpu->getPC_at_cycle_0()); }
- (uint8_t) getLengthOfInstruction:(uint8_t)opcode { return cpu->getLengthOfInstruction(opcode); }
- (uint8_t) getLengthOfCurrentInstruction { return cpu->getLengthOfCurrentInstruction(); }
- (uint16_t) getAddressOfNextIthInstruction:(int)i from:(uint16_t)addr { return cpu->getAddressOfNextIthInstruction(i, addr); }
- (uint16_t) getAddressOfNextInstruction { return cpu->getAddressOfNextInstruction(); }
- (const char *) getMnemonic:(uint8_t)opcode { return cpu->getMnemonic(opcode); }
- (CPU::AddressingMode) getAddressingMode:(uint8_t)opcode { return cpu->getAddressingMode(opcode); }

- (int) getTopOfCallStack { return cpu->getTopOfCallStack(); }

- (int) getBreakpoint:(int)addr { return cpu->getBreakpoint(addr); }
- (void) setBreakpoint:(int)addr tag:(uint8_t)t { cpu->setBreakpoint(addr, t); }
- (void) setHardBreakpoint:(int)addr { cpu->setHardBreakpoint(addr); };
- (void) deleteHardBreakpoint:(int)addr { cpu->deleteHardBreakpoint(addr); }
- (void) toggleHardBreakpoint:(int)addr { cpu->toggleHardBreakpoint(addr); }
- (void) setSoftBreakpoint:(int)addr { cpu->setSoftBreakpoint(addr); };
- (void) deleteSoftBreakpoint:(int)addr { cpu->deleteSoftBreakpoint(addr); }
- (void) toggleSoftBreakpoint:(int)addr { cpu->toggleSoftBreakpoint(addr); }

@end

// --------------------------------------------------------------------------
//                                    VIC
// --------------------------------------------------------------------------

@implementation VICProxy

- (id) initWithVIC:(VIC *)v;
{
    self = [super init];	
	vic = v;	
	return self;
}

- (void *) screenBuffer { return vic->screenBuffer(); }
- (void) setColorScheme:(VIC::ColorScheme)scheme { vic->setColorScheme(scheme); }
- (NSColor *) getColor:(VIC::ColorScheme)scheme nr:(int)nr
{
	uint8_t r, g, b;
	
	vic->getColor(scheme, nr, &r, &g, &b);
	return [NSColor colorWithCalibratedRed:(float)r/255.0 green:(float)g/255.0 blue:(float)b/255.0 alpha:1.0];
}

- (void) setColor:(int)color rgba:(NSColor *)rgba
{	
	float r, g, b, a;
	[rgba getRed:&r green:&g blue:&b alpha:&a];
	vic->setColor(color, (uint8_t)(r * 0xff), (uint8_t)(g * 0xff), (uint8_t)(b * 0xff), (uint8_t)(a * 0xff));
}

- (void) setColorInt:(int)color rgba:(int)rgba
{
	uint8_t r = (rgba >> 24) & 0xff;
	uint8_t g = (rgba >> 16) & 0xff;
	uint8_t b = (rgba >> 8)  & 0xff;
	uint8_t a = rgba         & 0xff;
	vic->setColor(color, r, g, b, a);
}

- (uint16_t) getMemoryBankAddr { return vic->getMemoryBankAddr(); }
- (void) setMemoryBankAddr:(uint16_t)addr { vic->setMemoryBankAddr(addr); }
- (uint16_t) getScreenMemoryAddr { return vic->getScreenMemoryAddr(); }
- (void) setScreenMemoryAddr:(uint16_t)addr { vic->setScreenMemoryAddr(addr); }
- (uint16_t) getCharacterMemoryAddr { return vic->getCharacterMemoryAddr(); }
- (void) setCharacterMemoryAddr:(uint16_t)addr { vic->setCharacterMemoryAddr(addr); }

- (int) getDisplayMode { return vic->getDisplayMode(); }
- (void) setDisplayMode:(int)mode { vic->setDisplayMode((VIC::DisplayMode)mode); }
- (int) getScreenGeometry { return (int)vic->getScreenGeometry(); }
- (void) setScreenGeometry:(int)mode { vic->setScreenGeometry((VIC::ScreenGeometry)mode); }
- (int) getHorizontalRasterScroll { return vic->getHorizontalRasterScroll(); }
- (void) setHorizontalRasterScroll:(int)offset { vic->setHorizontalRasterScroll(offset & 0x07); }
- (int) getVerticalRasterScroll { return vic->getVerticalRasterScroll(); }
- (void) setVerticalRasterScroll:(int)offset { vic->setVerticalRasterScroll(offset & 0x07); }

- (bool) spriteGetVisibilityFlag:(int)nr { return vic->spriteIsEnabled(nr); }
- (void) spriteSetVisibilityFlag:(int)nr value:(bool)flag { vic->setSpriteEnabled(nr, flag); }
- (void) spriteToggleVisibilityFlag:(int)nr { vic->toggleSpriteEnabled(nr); }

- (bool) spriteGetSpriteSpriteCollisionFlag:(int)nr { return vic->getSpriteSpriteCollision(nr); }
- (void) spriteSetSpriteSpriteCollisionFlag:(int)nr value:(bool)flag { vic->setSpriteSpriteCollision(nr, flag); }
- (void) spriteToggleSpriteSpriteCollisionFlag:(int)nr { vic->toggleSpriteSpriteCollisionFlag(nr); }

- (bool) spriteGetSpriteBackgroundCollisionFlag:(int)nr { return vic->getSpriteBackgroundCollision(nr); }
- (void) spriteSetSpriteBackgroundCollisionFlag:(int)nr value:(bool)flag { vic->setSpriteBackgroundCollision(nr, flag); }
- (void) spriteToggleSpriteBackgroundCollisionFlag:(int)nr { vic->toggleSpriteBackgroundCollisionFlag(nr); }

- (bool) spriteGetBackgroundPriorityFlag:(int)nr { return vic->spriteIsDrawnInBackground(nr); }
- (void) spriteSetBackgroundPriorityFlag:(int)nr value:(bool)flag { vic->setSpriteInBackground(nr, flag); }
- (void) spriteToggleBackgroundPriorityFlag:(int)nr { vic->spriteToggleBackgroundPriorityFlag(nr); }

- (bool) spriteGetMulticolorFlag:(int)nr { return vic->spriteIsMulticolor(nr); }
- (void) spriteSetMulticolorFlag:(int)nr value:(bool)flag { vic->setSpriteMulticolor(nr, flag); }
- (void) spriteToggleMulticolorFlag:(int)nr { vic->toggleMulticolorFlag(nr); }

- (bool) spriteGetStretchXFlag:(int)nr { return vic->spriteWidthIsDoubled(nr); }
- (void) spriteSetStretchXFlag:(int)nr value:(bool)flag { vic->setSpriteStretchX(nr, flag); }
- (void) spriteToggleStretchXFlag:(int)nr { vic->spriteToggleStretchXFlag(nr); }

- (bool) spriteGetStretchYFlag:(int)nr { return vic->spriteHeightIsDoubled(nr); }
- (void) spriteSetStretchYFlag:(int)nr value:(bool)flag { return vic->setSpriteStretchY(nr, flag); }
- (void) spriteToggleStretchYFlag:(int)nr { vic->spriteToggleStretchYFlag(nr); }

- (int) spriteGetX:(int)nr { return vic->getSpriteX(nr); }
- (void) spriteSetX:(int)nr value:(int)x { vic->setSpriteX(nr, x); }
- (int) spriteGetY:(int)nr { return vic->getSpriteY(nr); }
- (void) spriteSetY:(int)nr value:(int)y { vic->setSpriteY(nr, y); }
- (int) spriteGetColor:(int)nr { return vic->spriteColor(nr); }
- (void) spriteSetColor:(int)nr value:(int)c { vic->setSpriteColor(nr, c); }

- (uint16_t) getRasterLine { return vic->getScanline(); }
- (void) setRasterLine:(uint16_t)line { vic->setScanline(line); }
- (uint16_t) getRasterInterruptLine { return vic->rasterInterruptLine(); }
- (void) setRasterInterruptLine:(uint16_t)line { vic->setRasterInterruptLine(line); }
- (bool) getRasterInterruptFlag { return vic->rasterInterruptEnabled(); }
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

- (id) initWithCIA:(CIA *)c;
{
    self = [super init];	
	cia = c;	
	return self;
}

- (uint8_t) getDataPortA { return cia->getDataPortA(); }
- (void) setDataPortA:(uint8_t)v { cia->setDataPortA(v); }
- (uint8_t) getDataPortDirectionA { return cia->getDataPortDirectionA(); }
- (void) setDataPortDirectionA:(uint8_t)v { cia->setDataPortDirectionA(v); }
- (uint16_t) getTimerA { return cia->timerA.getTimer(); }
- (void) setTimerA:(uint16_t)v { cia->timerA.setTimer(v); }
- (uint16_t) getTimerLatchA { return cia->timerA.getTimerLatch(); }
- (void) setTimerLatchA:(uint16_t)v { cia->timerA.setTimerLatch(v); }
- (bool) getStartFlagA { return cia->timerA.isStarted(); }
- (void) setStartFlagA:(bool)b { cia->timerA.setStarted(b); }
- (void) toggleStartFlagA {cia->timerA.toggleStartFlag(); }
- (bool) getOneShotFlagA { return cia->timerA.isOneShot(); }
- (void) setOneShotFlagA:(bool)b { cia->timerA.setOneShot(b); }
- (void) toggleOneShotFlagA { cia->timerA.toggleOneShotFlag(); }
- (bool) getUnderflowFlagA { return cia->timerA.willIndicateUnderflow(); }
- (void) setUnderflowFlagA:(bool)b { return cia->timerA.setIndicateUnderflow(b); }
- (void) toggleUnderflowFlagA { cia->timerA.toggleUnderflowFlag(); }
- (bool) getPendingSignalFlagA {return cia->isSignalPendingA(); }
- (void) setPendingSignalFlagA:(bool)b { cia->setSignalPendingA(b); }
- (void) togglePendingSignalFlagA { cia->togglePendingSignalFlagA(); }
- (bool) getInterruptEnableFlagA { return cia->isInterruptEnabledA(); }
- (void) setInterruptEnableFlagA:(bool)b { cia->setInterruptEnabledA(b); }
- (void) toggleInterruptEnableFlagA {cia->toggleInterruptEnableFlagA(); }

- (uint8_t) getDataPortB { return cia->getDataPortB(); }
- (void) setDataPortB:(uint8_t)v { cia->setDataPortB(v); }
- (uint8_t) getDataPortDirectionB { return cia->getDataPortDirectionB(); }
- (void) setDataPortDirectionB:(uint8_t)v { cia->setDataPortDirectionB(v); }
- (uint16_t) getTimerB { return cia->timerB.getTimer(); }
- (void) setTimerB:(uint16_t)v { cia->timerB.setTimer(v); }
- (uint16_t) getTimerLatchB { return cia->timerB.getTimerLatch(); }
- (void) setTimerLatchB:(uint16_t)v { cia->timerB.setTimerLatch(v); }
- (bool) getStartFlagB { return cia->timerB.isStarted(); }
- (void) setStartFlagB:(bool)b { cia->timerB.setStarted(b); }
- (void) toggleStartFlagB {cia->timerB.toggleStartFlag(); }
- (bool) getOneShotFlagB { return cia->timerB.isOneShot(); }
- (void) setOneShotFlagB:(bool)b { cia->timerB.setOneShot(b); }
- (void) toggleOneShotFlagB { cia->timerB.toggleOneShotFlag(); }
- (bool) getUnderflowFlagB { return cia->timerB.willIndicateUnderflow(); }
- (void) setUnderflowFlagB:(bool)b { return cia->timerB.setIndicateUnderflow(b); }
- (void) toggleUnderflowFlagB { cia->timerB.toggleUnderflowFlag(); }
- (bool) getPendingSignalFlagB {return cia->isSignalPendingB(); }
- (void) setPendingSignalFlagB:(bool)b { cia->setSignalPendingB(b); }
- (void) togglePendingSignalFlagB { cia->togglePendingSignalFlagB(); }
- (bool) getInterruptEnableFlagB { return cia->isInterruptEnabledB(); }
- (void) setInterruptEnableFlagB:(bool)b { cia->setInterruptEnabledB(b); }
- (void) toggleInterruptEnableFlagB {cia->toggleInterruptEnableFlagB(); }

- (uint8_t) getTodHours { return cia->tod.getTodHours(); }
- (void) setTodHours:(uint8_t)value { cia->tod.setTodHours(value); }
- (uint8_t) getTodMinutes { return cia->tod.getTodMinutes(); }
- (void) setTodMinutes:(uint8_t)value { cia->tod.setTodMinutes(value); }
- (uint8_t) getTodSeconds { return cia->tod.getTodSeconds(); }
- (void) setTodSeconds:(uint8_t)value { cia->tod.setTodSeconds(value); }
- (uint8_t) getTodTenth { return cia->tod.getTodTenth(); }
- (void) setTodTenth:(uint8_t)value { cia->tod.setTodTenth(value); }

- (uint8_t) getAlarmHours { return cia->tod.getAlarmHours(); }
- (void) setAlarmHours:(uint8_t)value { cia->tod.setAlarmHours(value); }
- (uint8_t) getAlarmMinutes { return cia->tod.getAlarmMinutes(); }
- (void) setAlarmMinutes:(uint8_t)value { cia->tod.setAlarmMinutes(value); }
- (uint8_t) getAlarmSeconds { return cia->tod.getAlarmSeconds(); }
- (void) setAlarmSeconds:(uint8_t)value { cia->tod.setAlarmSeconds(value); }
- (uint8_t) getAlarmTenth { return cia->tod.getAlarmTenth(); }
- (void) setAlarmTenth:(uint8_t)value { cia->tod.setAlarmTenth(value); }
- (bool) todIsInterruptEnabled { return cia->isInterruptEnabledTOD(); }
- (void) todSetInterruptEnabled:(bool)b { cia->setInterruptEnabledTOD(b); }

@end 

// --------------------------------------------------------------------------
//                                    Keyboard
// --------------------------------------------------------------------------

@implementation KeyboardProxy

- (id) initWithKeyboard:(Keyboard *)kb;
{
    self = [super init];	
	keyboard = kb;	
	return self;
}

- (void) pressRunstopKey { keyboard->pressRunstopKey(); }
- (void) releaseRunstopKey { keyboard->releaseRunstopKey(); }
- (void) pressCommodoreKey { keyboard->pressCommodoreKey(); }
- (void) releaseCommodoreKey { keyboard->releaseCommodoreKey(); }
- (void) typeFormat { keyboard->typeFormat(); }

@end

// --------------------------------------------------------------------------
//                                    SID
// --------------------------------------------------------------------------

@implementation SIDProxy

- (id) initWithSID:(SID *)s;
{
    self = [super init];	
	sid = s;	
	return self;
}

- (float) getVolumeControl 
{
	return sid->getVolumeControl(); 
}

- (void) setVolumeControl:(float)value 
{ 
	sid->setVolumeControl(value); 
} 

@end


@implementation C64Proxy

// --------------------------------------------------------------------------
// Initialization
// --------------------------------------------------------------------------

- (id) initWithDocument:(MyDocument *)d;
{
	return [self initWithDocument:d withScreen:nil];
}

- (id) initWithDocument:(MyDocument *)d withScreen:(VICScreen *)s;
{
    self = [super init];
	
	// Create virtual machine and initialize references
	c64 = new C64();
	[s setC64:c64];
	cia[0] = NULL; // unused
	cia[1] = c64->cia1;
	cia[2] = c64->cia2;
	iec = c64->iec;
	mem = c64->mem;
	
	// Create sub proxys
	cpuproxy = [[CPUProxy alloc] initWithCPU:c64->cpu];
	vicproxy = [[VICProxy alloc] initWithVIC:c64->vic];
	ciaproxy1 = [[CIAProxy alloc] initWithCIA:c64->cia1];
	ciaproxy2 = [[CIAProxy alloc] initWithCIA:c64->cia2];
	sidproxy = [[SIDProxy alloc] initWithSID:c64->sid];
	keyboardproxy = [[KeyboardProxy alloc] initWithKeyboard:c64->keyboard];
		
	// Initialize CoreAudio sound interface
	audioDevice = [[AudioDevice alloc] initWithSID:c64->sid];
	if (!audioDevice)
	{
		NSLog(@"WARNING: Couldn't initialize AudioDevice. Sound disabled.");
	}
		
    return self;
}

- (void) release
{
	assert(c64 != NULL);
	NSLog(@"Deleting C64...");

	// Delete sound device
	NSLog(@"  Deleting sound device");
	[audioDevice release];
	audioDevice = nil;
	
	NSLog(@"  Deleting virtual machine");
	delete c64;
	c64 = NULL;
}

- (CPUProxy *) cpu { return cpuproxy; }
- (VICProxy *) vic { return vicproxy; }
- (CIAProxy *) cia:(int)num { if (num == 1) return ciaproxy1; else if (num == 2) return ciaproxy2; else assert(0); } 
- (SIDProxy *) sid { return sidproxy; }
- (KeyboardProxy *) keyboard { return keyboardproxy; }



// TO BE DEPRECATED...
- (C64 *) getC64
{
	return c64;
}





// --------------------------------------------------------------------------
// Bridge functions (cross the Objective-C / C++)
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// C64
// --------------------------------------------------------------------------

- (Message *)getMessage { return c64->getMessage(); }
- (void) reset { c64->reset(); }
- (void) halt { c64->halt(); }
- (void) step { c64->step(); }
- (void) run { c64->run(); }
- (bool) isHalted { return c64->isHalted(); }
- (bool) isRunnable { return c64->isRunnable(); }
- (bool) isRunning { return c64->isRunning(); }
- (void) setPAL { c64->setPAL(); }
- (void) setNTSC { c64->setNTSC(); }
- (int) getFrameDelay { return c64->getFrameDelay(); }
- (void) setFrameDelay:(int)delay { c64->setFrameDelay(delay); }
- (int) buildNr { return c64->build(); }

- (int) numberOfMissingRoms { return c64->numberOfMissingRoms(); }
- (int) missingRoms { return c64->getMissingRoms(); }
- (bool) loadBasicRom:(NSString *)filename { return c64->mem->isBasicRom([filename UTF8String]) && c64->loadRom([filename UTF8String]); }
- (bool) loadCharRom:(NSString *)filename { return c64->mem->isCharRom([filename UTF8String]) && c64->loadRom([filename UTF8String]); }
- (bool) loadKernelRom:(NSString *)filename { return c64->mem->isKernelRom([filename UTF8String]) && c64->loadRom([filename UTF8String]); }
- (bool) loadVC1541Rom:(NSString *)filename { return c64->floppy->mem->is1541Rom([filename UTF8String]) && c64->loadRom([filename UTF8String]); }
- (bool) isCartridgeAttached { return c64->isCartridgeAttached(); }
	
- (bool) c64GetWarpMode { return c64->getWarpMode(); }
- (void) c64SetWarpMode:(bool)b {  c64->setWarpMode(b); }

// - (bool) cpuTracingEnabled { return cpu->tracingEnabled(); }
// - (void) cpuSetTraceMode:(bool)b { cpu->setTraceMode(b); }
- (bool) iecTracingEnabled { return iec->tracingEnabled(); }
- (void) iecSetTraceMode:(bool)b { iec->setTraceMode(b); }
- (bool) vc1541CpuTracingEnabled { return c64->floppy->cpu->tracingEnabled(); }
- (void) vc1541CpuSetTraceMode:(bool)b { c64->floppy->cpu->setTraceMode(b); }
- (bool) viaTracingEnabled { return c64->floppy->via2->tracingEnabled(); }
- (void) viaSetTraceMode:(bool)b { c64->floppy->via2->setTraceMode(b); }

- (void) dumpC64 { c64->dumpState(); }
- (void) dumpC64CIA1 { c64->cia1->dumpState(); }
- (void) dumpC64CIA2 { c64->cia2->dumpState(); }
- (void) dumpC64VIC { c64->vic->dumpState(); }
- (void) dumpC64SID { c64->sid->dumpState(); }
- (void) dumpC64Memory { c64->mem->dumpState(); }
- (void) dumpVC1541 { c64->floppy->dumpState(); }
- (void) dumpVC1541CPU { c64->floppy->cpu->dumpState(); }
- (void) dumpVC1541VIA1 { c64->floppy->via1->dumpState(); }
- (void) dumpVC1541VIA2 { c64->floppy->via2->dumpState(); }
- (void) dumpVC1541Memory { c64->floppy->mem->dumpState(); }
- (void) dumpKeyboard { c64->keyboard->dumpState(); }
- (void) dumpIEC { c64->iec->dumpState(); }
	
- (long) c64GetCycles { return (long)c64->getCycles(); }

// --------------------------------------------------------------------------
// JOYSTICK
// --------------------------------------------------------------------------
- (void) switchInputDevice:(int)devNo { c64->switchInputDevice( devNo ); }
- (void) switchInputDevices { c64->switchInputDevices(); }
- (uint8_t) getPortAssignment:(int)devNo { return c64->getDeviceOfPort(devNo); }
- (Joystick *) addJoystick { return c64->addJoystick(); }
- (void) removeJoystick:(Joystick *)joystick { return c64->removeJoystick( joystick ); }

// --------------------------------------------------------------------------
// MEM
// --------------------------------------------------------------------------

- (uint8_t) memPeek:(uint16_t)addr { return mem->peek(addr); }
- (uint16_t) memPeekWord:(uint16_t)addr { return mem->peekWord(addr); }
- (uint8_t) memPeekFrom:(uint16_t)addr memtype:(Memory::MemoryType)type { return mem->peekFrom(addr, type); }
- (void) memPoke:(uint16_t)addr value:(uint8_t)val { mem->poke(addr, val); }
- (void) memPokeTo:(uint16_t)addr value:(uint8_t)val memtype:(Memory::MemoryType)type { mem->pokeTo(addr, val, type); }
- (bool) memIsValidAddr:(uint16_t)addr memtype:(Memory::MemoryType)type { return mem->isValidAddr(addr, type); }

- (Memory::WatchpointType) memGetWatchpointType:(uint16_t)addr { return mem->getWatchpointType(addr); }
- (void) memSetWatchpoint:(uint16_t)addr { mem->setWatchpoint(addr); }
- (void) memSetWatchpoint:(uint16_t)addr watchvalue:(uint8_t)value {mem->setWatchpoint(addr, value); }
- (void) memSetWatchpoint:(uint16_t)addr tag:(Memory::WatchpointType)type watchvalue:(uint8_t)value { mem->setWatchpoint(addr, (uint8_t)type, value); }
- (void) memDeleteWatchpoint:(uint16_t)addr { mem->deleteWatchpoint(addr); }
- (uint8_t) memGetWatchValue:(uint16_t)addr { return mem->getWatchValue(addr); }



- (void) fastReset { c64->fastReset(); }


// --------------------------------------------------------------------------
// audio hardware
// --------------------------------------------------------------------------

- (void) enableAudio
{
	/* if the audio hardware couldn't be initialized, nothing wil happen here (message to nil) */
	[audioDevice startPlayback];
}
- (void) disableAudio
{
	/* if the audio hardware couldn't be initialized, nothing wil happen here (message to nil) */
	[audioDevice stopPlayback];
}


- (void) keyboardPressRunstopRestore { c64->runstopRestore(); }


// --------------------------------------------------------------------------
// Drive
// --------------------------------------------------------------------------

- (void) ejectDisk { c64->floppy->ejectDisc(); }
- (void) connectDrive { c64->iec->connectDrive(); }
- (void) disconnectDrive { c64->iec->disconnectDrive(); }
- (bool) isDriveConnected { return c64->iec->driveIsConnected(); }
@end


	
