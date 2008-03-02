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

// --------------------------------------------------------------------------
// Listener Proxy
// --------------------------------------------------------------------------

ListenerProxy::ListenerProxy()
{
	doc = nil;
	screen = nil;
}

void ListenerProxy::drawAction(int *screenBuffer) 
{ 
	[screen updateTexture:screenBuffer];
}	

void ListenerProxy::runAction() 
{ 
	[doc runAction]; 
}

void ListenerProxy::haltAction() 
{
	[doc haltAction]; 
}

void ListenerProxy::okAction() 
{
	[doc okAction];
}

void ListenerProxy::breakpointAction() 
{
	[doc breakpointAction];
}

void ListenerProxy::watchpointAction() 
{
	[doc watchpointAction];
}

void ListenerProxy::illegalInstructionAction() 
{
	[doc illegalInstructionAction];
}

void ListenerProxy::missingRomAction() 
{
	[doc missingRomAction];
}

void ListenerProxy::connectDriveAction()
{
	[doc connectDriveAction];
}

void ListenerProxy::insertDiskAction()
{
	[doc insertDiskAction];
}

void ListenerProxy::ejectDiskAction()
{
	[doc ejectDiskAction];
}

void ListenerProxy::disconnectDriveAction()
{
	[doc disconnectDriveAction];
}

void ListenerProxy::startDiskAction()
{
	[doc startDiskAction];
}

void ListenerProxy::stopDiskAction()
{
	[doc stopDiskAction];
}

void ListenerProxy::startWarpAction()
{
	[doc startWarpAction];
}

void ListenerProxy::stopWarpAction()
{
	[doc stopWarpAction];
}	

void ListenerProxy::logAction(char *message)
{
	[doc logAction:message];
}	

@implementation C64Proxy

// --------------------------------------------------------------------------
// Initialization
// --------------------------------------------------------------------------

- (id) init
{
    self = [super init];
	
	// Create virtual machine and initialize references
	c64 = new C64();
	cia[0] = NULL; // unused
	cia[1] = c64->cia1;
	cia[2] = c64->cia2;
	iec = c64->iec;
	cpu = c64->cpu;
	mem = c64->mem;
	// cpu = c64->floppy->cpu;
	// mem = c64->floppy->mem;
	
	// Initialize CoreAudio sound interface
	sidDevice = new SIDDevice();	// create Core Audio sound device		
	int result = sidDevice->SetupDevice(c64->sid); // setup and start playback
	if (0 != result ) // failure
	{
		if (result == -2)
			debug("Connected audio hardware doesn't support mono or stereo playback\n");
		debug("Failure: Couldn't enable sound.\n");
	}
	debug("Sound enabled!\n");
	
	// Create listener object
	listener = new ListenerProxy();

    return self;
}

- (void) release
{
	assert(c64 != NULL);
	debug("Deleting C64...\n");

	// Delete sound device
	debug("  Deleting sound device\n");
	delete sidDevice;

	debug("  Deleting virtual machine\n");
	delete c64;
	c64 = NULL;
}


// TO BE DEPRECATED...
- (C64 *) getC64
{
	return c64;
}

- (SIDDevice *) getSIDDevice
{
	return sidDevice;
}

- (void) setDocument:(MyDocument *)d
{
	assert (d != nil);
	listener->setDocument(d);

	/* Register listener, if registration is complete */	
	if (listener->getDocument() != nil && listener->getScreen() != nil)
		c64->setListener(listener);	
}

- (void) setScreen:(VICScreen *)s
{
	assert (s != nil);
	listener->setScreen(s);
	[s setC64:c64];

	/* Register listener, if registration is complete */	
	if (listener->getDocument() != nil && listener->getScreen() != nil)
			c64->setListener(listener);	
}

// --------------------------------------------------------------------------
// Bridge functions (cross the Objective-C / C++)
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// C64
// --------------------------------------------------------------------------

- (void) reset { c64->reset(); }
- (void) halt { c64->halt(); }
- (void) step { cpu->step(); }
- (void) run { c64->run(); }
- (bool) isHalted { return c64->isHalted(); }
- (bool) isRunning { return c64->isRunning(); }
- (int) buildNr { debug("buildNrtrtr\n"); return c64->build(); }

- (bool) loadSnapshot:(NSString *)filename { return c64->loadSnapshot([filename UTF8String]); }
- (bool) saveSnapshot:(NSString *)filename { return c64->saveSnapshot([filename UTF8String]); }

- (void) setWarpLoad:(bool)b { c64->setWarpLoad(b); }
- (void) setFastReset:(bool)b { c64->setFastReset(b); }

- (void) dumpCPU { cpu->dumpState(); }
- (void) dumpCIA { c64->cia1->dumpState(); c64->cia2->dumpState(); }
- (void) dumpVIC { c64->vic->dumpState(); }
- (void) dumpIEC { c64->iec->dumpState(); }
- (void) dumpMemory { mem->dumpState(); }
- (void) dumpDrive { c64->floppy->dumpState(); }


// --------------------------------------------------------------------------
// CPU
// --------------------------------------------------------------------------

- (void) cpuEnableIllegalInstructions:(bool)illegalInstructions { cpu->enableIllegalInstructions(illegalInstructions); }
- (NSNumber *) cpuGetMHz { return [NSNumber numberWithFloat:cpu->getMHz()]; }
- (void) cpuSetMHz:(id)mhz { cpu->setMHz([mhz floatValue]); }
- (NSNumber *) cpuGetKHz { return [NSNumber numberWithFloat:(cpu->getMHz() * 1000.0)]; }
- (void) cpuSetKHz:(id)mhz { cpu->setMHz([mhz floatValue] / 1000.0); }
- (bool) cpuRunsAtNativeSpeed { return !cpu->getWarpMode(); }
- (void) cpuSetNativeSpeedFlag:(bool)b { cpu->setWarpMode(!b); }
- (void) cpuToggleNativeSpeedFlag { cpu->setWarpMode(!cpu->getWarpMode()); }
- (long) cpuGetCycles { return cpu->getCycles(); }
- (bool) cpuTracingEnabled { return cpu->tracingEnabled(); }
- (void) cpuSetTraceMode:(bool)b { cpu->setTraceMode(b); }
- (bool) iecTracingEnabled { return iec->tracingEnabled(); }
- (void) iecSetTraceMode:(bool)b { iec->setTraceMode(b); }

- (uint16_t) cpuGetPC { return cpu->getPC(); }
- (void) cpuSetPC:(uint16_t)pc { cpu->setPC(pc); }
- (uint8_t) cpuGetSP { return cpu->getSP(); }
- (void) cpuSetSP:(uint8_t)sp { cpu->setSP(sp); }
- (uint8_t) cpuGetA { return cpu->getA(); }
- (void) cpuSetA:(uint8_t)a { cpu->setA(a); }
- (uint8_t) cpuGetX { return cpu->getX(); }
- (void) cpuSetX:(uint8_t)x { cpu->setX(x); }
- (uint8_t) cpuGetY { return cpu->getY(); }
- (void) cpuSetY:(uint8_t)y { cpu->setY(y); }
- (bool) cpuGetN { return cpu->getN(); }
- (void) cpuSetN:(bool)b { cpu->setN(b); }
- (bool) cpuGetZ { return cpu->getZ(); }
- (void) cpuSetZ:(bool)b { cpu->setZ(b); }
- (bool) cpuGetC { return cpu->getC(); }
- (void) cpuSetC:(bool)b { cpu->setC(b); }
- (bool) cpuGetI { return cpu->getI(); }
- (void) cpuSetI:(bool)b { cpu->setI(b); }
- (bool) cpuGetB { return cpu->getB(); }
- (void) cpuSetB:(bool)b { cpu->setB(b); }
- (bool) cpuGetD { return cpu->getD(); }
- (void) cpuSetD:(bool)b { cpu->setD(b); }
- (bool) cpuGetV { return cpu->getV(); }
- (void) cpuSetV:(bool)b { cpu->setV(b); }

- (uint16_t) cpuPeekPC { return cpu->peekPC(); }
- (uint8_t) cpuGetLengthOfInstruction:(uint8_t)opcode { return cpu->getLengthOfInstruction(opcode); }
- (uint8_t) cpuGetLengthOfCurrentInstruction { return cpu->getLengthOfCurrentInstruction(); }
- (uint16_t) cpuGetAddressOfNextIthInstruction:(int)i from:(uint16_t)addr { return cpu->getAddressOfNextIthInstruction(i, addr); }
- (uint16_t) cpuGetAddressOfNextInstruction { return cpu->getAddressOfNextInstruction(); }
- (char *) cpuGetMnemonic:(uint8_t)opcode { return cpu->getMnemonic(opcode); }
- (CPU::AddressingMode) cpuGetAddressingMode:(uint8_t)opcode { return cpu->getAddressingMode(opcode); }

// - (CPU::ErrorState) cpuGetErrorState { return cpu->getErrorState(); }
- (int) cpuGetTopOfCallStack { return cpu->getTopOfCallStack(); }

- (int)   cpuGetBreakpoint:(int)addr { return cpu->getBreakpoint(addr); }
- (void)  cpuSetBreakpoint:(int)addr tag:(uint8_t)t { cpu->setBreakpoint(addr, t); }
- (void)  cpuSetHardBreakpoint:(int)addr { cpu->setHardBreakpoint(addr); };
- (void)  cpuDeleteHardBreakpoint:(int)addr { cpu->deleteHardBreakpoint(addr); }
- (void)  cpuToggleHardBreakpoint:(int)addr { cpu->toggleHardBreakpoint(addr); }
- (void)  cpuSetSoftBreakpoint:(int)addr { cpu->setSoftBreakpoint(addr); };
- (void)  cpuDeleteSoftBreakpoint:(int)addr { cpu->deleteSoftBreakpoint(addr); }
- (void)  cpuToggleSoftBreakpoint:(int)addr { cpu->toggleSoftBreakpoint(addr); }


// --------------------------------------------------------------------------
// JOYSTICK
// --------------------------------------------------------------------------
- (Joystick *) getJoystickOnPortA { return c64->cia1->getJoystickOnPortA(); }
- (Joystick *) getJoystickOnPortB { return c64->cia1->getJoystickOnPortB(); }
- (void) switchInputDevice:(int)devNo { c64->switchInputDevice( devNo ); }
- (void) switchInputDevices { c64->switchInputDevices(); }
- (uint8_t) getPortAssignment:(int)devNo { return c64->getDeviceOfPort(devNo); }


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


// --------------------------------------------------------------------------
// CIA
// --------------------------------------------------------------------------

- (uint8_t) ciaGetDataPortA:(int)nr { return cia[nr]->getDataPortA(); }
- (void) ciaSetDataPortA:(int)nr value:(uint8_t)v { cia[nr]->setDataPortA(v); }
- (uint8_t) ciaGetDataPortDirectionA:(int)nr { return cia[nr]->getDataPortDirectionA(); }
- (void) ciaSetDataPortDirectionA:(int)nr value:(uint8_t)v { cia[nr]->setDataPortDirectionA(v); }
- (uint16_t) ciaGetTimerA:(int)nr { return cia[nr]->getTimerA(); }
- (void) ciaSetTimerA:(int)nr value:(uint16_t)v { cia[nr]->setTimerA(v); }
- (uint16_t) ciaGetTimerLatchA:(int)nr { return cia[nr]->getTimerLatchA(); }
- (void) ciaSetTimerLatchA:(int)nr value:(uint16_t)v { cia[nr]->setTimerLatchA(v); }
- (bool) ciaGetStartFlagA:(int)nr { return cia[nr]->isStartedA(); }
- (void) ciaSetStartFlagA:(int)nr value:(bool)b { cia[nr]->setStartedA(b); }
- (void) ciaToggleStartFlagA:(int)nr {cia[nr]->toggleStartFlagA(); }
- (bool) ciaGetOneShotFlagA:(int)nr { return cia[nr]->isOneShotA(); }
- (void) ciaSetOneShotFlagA:(int)nr value:(bool)b { cia[nr]->setOneShotA(b); }
- (void) ciaToggleOneShotFlagA:(int)nr { cia[nr]->toggleOneShotFlagA(); }
- (bool) ciaGetUnderflowFlagA:(int)nr { return cia[nr]->willIndicateUnderflowA(); }
- (void) ciaSetUnderflowFlagA:(int)nr value:(bool)b { return cia[nr]->setIndicateUnderflowA(b); }
- (void) ciaToggleUnderflowFlagA:(int)nr { cia[nr]->toggleUnderflowFlagA(); }
- (bool) ciaGetPendingSignalFlagA:(int)nr {return cia[nr]->isSignalPendingA(); }
- (void) ciaSetPendingSignalFlagA:(int)nr value:(bool)b { cia[nr]->setSignalPendingA(b); }
- (void) ciaTogglePendingSignalFlagA:(int)nr { cia[nr]->togglePendingSignalFlagA(); }
- (bool) ciaGetInterruptEnableFlagA:(int)nr { return cia[nr]->isInterruptEnabledA(); }
- (void) ciaSetInterruptEnableFlagA:(int)nr value:(bool)b { cia[nr]->setInterruptEnabledA(b); }
- (void) ciaToggleInterruptEnableFlagA:(int)nr {cia[nr]->toggleInterruptEnableFlagA(); }

- (uint8_t) ciaGetDataPortB:(int)nr { return cia[nr]->getDataPortB(); }
- (void) ciaSetDataPortB:(int)nr value:(uint8_t)v { cia[nr]->setDataPortB(v); }
- (uint8_t) ciaGetDataPortDirectionB:(int)nr { return cia[nr]->getDataPortDirectionB(); }
- (void) ciaSetDataPortDirectionB:(int)nr value:(uint8_t)v { cia[nr]->setDataPortDirectionB(v); }
- (uint16_t) ciaGetTimerB:(int)nr { return cia[nr]->getTimerB(); }
- (void) ciaSetTimerB:(int)nr value:(uint16_t)v { cia[nr]->setTimerB(v); }
- (uint16_t) ciaGetTimerLatchB:(int)nr { return cia[nr]->getTimerLatchB(); }
- (void) ciaSetTimerLatchB:(int)nr value:(uint16_t)v { cia[nr]->setTimerLatchB(v); }
- (bool) ciaGetStartFlagB:(int)nr { return cia[nr]->isStartedB(); }
- (void) ciaSetStartFlagB:(int)nr value:(bool)b { cia[nr]->setStartedB(b); }
- (void) ciaToggleStartFlagB:(int)nr {cia[nr]->toggleStartFlagB(); }
- (bool) ciaGetOneShotFlagB:(int)nr { return cia[nr]->isOneShotB(); }
- (void) ciaSetOneShotFlagB:(int)nr value:(bool)b { cia[nr]->setOneShotB(b); }
- (void) ciaToggleOneShotFlagB:(int)nr { cia[nr]->toggleOneShotFlagB(); }
- (bool) ciaGetUnderflowFlagB:(int)nr { return cia[nr]->willIndicateUnderflowB(); }
- (void) ciaSetUnderflowFlagB:(int)nr value:(bool)b { return cia[nr]->setIndicateUnderflowB(b); }
- (void) ciaToggleUnderflowFlagB:(int)nr { cia[nr]->toggleUnderflowFlagB(); }
- (bool) ciaGetPendingSignalFlagB:(int)nr {return cia[nr]->isSignalPendingB(); }
- (void) ciaSetPendingSignalFlagB:(int)nr value:(bool)b { cia[nr]->setSignalPendingB(b); }
- (void) ciaTogglePendingSignalFlagB:(int)nr { cia[nr]->togglePendingSignalFlagB(); }
- (bool) ciaGetInterruptEnableFlagB:(int)nr { return cia[nr]->isInterruptEnabledB(); }
- (void) ciaSetInterruptEnableFlagB:(int)nr value:(bool)b { cia[nr]->setInterruptEnabledB(b); }
- (void) ciaToggleInterruptEnableFlagB:(int)nr {cia[nr]->toggleInterruptEnableFlagB(); }

- (uint8_t) ciaGetTodHours:(int)nr { return cia[nr]->tod.getTodHours(); }
- (uint8_t) ciaGetTodMinutes:(int)nr { return cia[nr]->tod.getTodMinutes(); }
- (uint8_t) ciaGetTodSeconds:(int)nr { return cia[nr]->tod.getTodSeconds(); }
- (uint8_t) ciaGetTodTenth:(int)nr { return cia[nr]->tod.getTodTenth(); }

- (uint8_t) ciaGetAlarmHours:(int)nr { return cia[nr]->tod.getAlarmHours(); }
- (uint8_t) ciaGetAlarmMinutes:(int)nr { return cia[nr]->tod.getAlarmMinutes(); }
- (uint8_t) ciaGetAlarmSeconds:(int)nr { return cia[nr]->tod.getAlarmSeconds(); }
- (uint8_t) ciaGetAlarmTenth:(int)nr { return cia[nr]->tod.getAlarmTenth(); }

// --------------------------------------------------------------------------
// VIC
// --------------------------------------------------------------------------

- (void) vicSetColorScheme:(VIC::ColorScheme)scheme { c64->vic->setColorScheme(scheme); }
- (NSColor *) vicGetColor:(VIC::ColorScheme)scheme nr:(int)nr
{
	uint8_t r, g, b;
	
	c64->vic->getColor(scheme, nr, &r, &g, &b);
	return [NSColor colorWithCalibratedRed:(float)r/255.0 green:(float)g/255.0 blue:(float)b/255.0 alpha:1.0];
}

- (void) vicSetColor:(int)color rgba:(NSColor *)rgba
{	
	float r, g, b, a;
	[rgba getRed:&r green:&g blue:&b alpha:&a];
	c64->vic->setColor(color, (uint8_t)(r * 0xff), (uint8_t)(g * 0xff), (uint8_t)(b * 0xff), (uint8_t)(a * 0xff));
}

- (void) vicSetColorInt:(int)color rgba:(int)rgba
{
	uint8_t r = (rgba >> 24) & 0xff;
	uint8_t g = (rgba >> 16) & 0xff;
	uint8_t b = (rgba >> 8)  & 0xff;
	uint8_t a = rgba         & 0xff;
	c64->vic->setColor(color, r, g, b, a);
}

- (uint16_t) vicGetMemoryBankAddr { return c64->vic->getMemoryBankAddr(); }
- (void) vicSetMemoryBankAddr:(uint16_t)addr { c64->vic->setMemoryBankAddr(addr); }
- (uint16_t) vicGetScreenMemoryAddr { return c64->vic->getScreenMemoryAddr(); }
- (void) vicSetScreenMemoryAddr:(uint16_t)addr { c64->vic->setScreenMemoryAddr(addr); }
- (uint16_t) vicGetCharacterMemoryAddr { return c64->vic->getCharacterMemoryAddr(); }
- (void) vicSetCharacterMemoryAddr:(uint16_t)addr { c64->vic->setCharacterMemoryAddr(addr); }

- (int) vicGetDisplayMode { return c64->vic->getDisplayMode(); }
- (void) vicSetDisplayMode:(int)mode { c64->vic->setDisplayMode((VIC::DisplayMode)mode); }
- (int) vicGetScreenGeometry { return (int)c64->vic->getScreenGeometry(); }
- (void) vicSetScreenGeometry:(int)mode { c64->vic->setScreenGeometry((VIC::ScreenGeometry)mode); }
- (int) vicGetHorizontalRasterScroll { return c64->vic->getHorizontalRasterScroll(); }
- (void) vicSetHorizontalRasterScroll:(int)offset { c64->vic->setHorizontalRasterScroll(clip(offset,0,7)); }
- (int) vicGetVerticalRasterScroll { return c64->vic->getVerticalRasterScroll(); }
- (void) vicSetVerticalRasterScroll:(int)offset { c64->vic->setVerticalRasterScroll(clip(offset,0,7)); }

- (bool) spriteGetVisibilityFlag:(int)nr { return c64->vic->spriteIsEnabled(nr); }
- (void) spriteSetVisibilityFlag:(int)nr value:(bool)flag { c64->vic->setSpriteEnabled(nr, flag); }
- (void) spriteToggleVisibilityFlag:(int)nr { c64->vic->toggleSpriteEnabled(nr); }

- (bool) spriteGetSpriteSpriteCollisionFlag:(int)nr { return c64->vic->getSpriteSpriteCollision(nr); }
- (void) spriteSetSpriteSpriteCollisionFlag:(int)nr value:(bool)flag { c64->vic->setSpriteSpriteCollision(nr, flag); }
- (void) spriteToggleSpriteSpriteCollisionFlag:(int)nr { c64->vic->toggleSpriteSpriteCollisionFlag(nr); }

- (bool) spriteGetSpriteBackgroundCollisionFlag:(int)nr { return c64->vic->getSpriteBackgroundCollision(nr); }
- (void) spriteSetSpriteBackgroundCollisionFlag:(int)nr value:(bool)flag { c64->vic->setSpriteBackgroundCollision(nr, flag); }
- (void) spriteToggleSpriteBackgroundCollisionFlag:(int)nr { c64->vic->toggleSpriteBackgroundCollisionFlag(nr); }

- (bool) spriteGetBackgroundPriorityFlag:(int)nr { return c64->vic->spriteIsDrawnInBackground(nr); }
- (void) spriteSetBackgroundPriorityFlag:(int)nr value:(bool)flag { c64->vic->setSpriteInBackground(nr, flag); }
- (void) spriteToggleBackgroundPriorityFlag:(int)nr {c64->vic->spriteToggleBackgroundPriorityFlag(nr); }

- (bool) spriteGetMulticolorFlag:(int)nr { return c64->vic->spriteIsMulticolor(nr); }
- (void) spriteSetMulticolorFlag:(int)nr value:(bool)flag { c64->vic->setSpriteMulticolor(nr, flag); }
- (void) spriteToggleMulticolorFlag:(int)nr { c64->vic->toggleMulticolorFlag(nr); }

- (bool) spriteGetStretchXFlag:(int)nr { return c64->vic->spriteWidthIsDoubled(nr); }
- (void) spriteSetStretchXFlag:(int)nr value:(bool)flag { c64->vic->setSpriteStretchX(nr, flag); }
- (void) spriteToggleStretchXFlag:(int)nr { c64->vic->spriteToggleStretchXFlag(nr); }

- (bool) spriteGetStretchYFlag:(int)nr { return c64->vic->spriteHeightIsDoubled(nr); }
- (void) spriteSetStretchYFlag:(int)nr value:(bool)flag { return c64->vic->setSpriteStretchY(nr, flag); }
- (void) spriteToggleStretchYFlag:(int)nr { c64->vic->spriteToggleStretchYFlag(nr); }

- (int) spriteGetX:(int)nr { return c64->vic->getSpriteX(nr); }
- (void) spriteSetX:(int)nr value:(int)x { c64->vic->setSpriteX(nr, x); }
- (int) spriteGetY:(int)nr { return c64->vic->getSpriteY(nr); }
- (void) spriteSetY:(int)nr value:(int)y { c64->vic->setSpriteY(nr, y); }
- (int) spriteGetColor:(int)nr { return c64->vic->spriteColor(nr); }
- (void) spriteSetColor:(int)nr value:(int)c { c64->vic->setSpriteColor(nr, c); }

- (uint16_t) vicGetRasterLine { return c64->vic->getScanline(); }
- (void) vicSetRasterLine:(uint16_t)line {c64->vic->setScanline(line); }
- (uint16_t) vicGetRasterInterruptLine { return c64->vic->rasterInterruptLine(); }
- (void) vicSetRasterInterruptLine:(uint16_t)line { c64->vic->setRasterInterruptLine(line); }
- (bool) vicGetRasterInterruptFlag { return c64->vic->rasterInterruptEnabled(); }
- (void) vicSetRasterInterruptFlag:(bool)b { c64->vic->setRasterInterruptEnable(b); }
- (void) vicToggleRasterInterruptFlag { c64->vic->toggleRasterInterruptFlag(); }

- (void) vicToggleDrawSprites { c64->vic->toggleDrawSprites(); }
- (void) vicToggleMarkIRQLines { c64->vic->toggleMarkIRQLines(); }


// --------------------------------------------------------------------------
// SID
// --------------------------------------------------------------------------

- (float) sidGetVolumeControl { return c64->sid->getVolumeControl(); }
- (void) sidSetVolumeControl:(float)value { c64->sid->setVolumeControl(value); } 


// --------------------------------------------------------------------------
// Keyboard
// --------------------------------------------------------------------------

- (void) keyboardPressRunstopRestore { c64->runstopRestore(); }
- (void) keyboardPressRunstopKey { c64->keyboard->pressRunstopKey(); }
- (void) keyboardReleaseRunstopKey { c64->keyboard->releaseRunstopKey(); }
- (void) keyboardPressCommodoreKey { c64->keyboard->pressCommodoreKey(); }
- (void) keyboardReleaseCommodoreKey { c64->keyboard->releaseCommodoreKey(); }


// --------------------------------------------------------------------------
// Drive
// --------------------------------------------------------------------------

- (void) ejectDisk { c64->floppy->ejectDisc(); }
- (void) connectDrive { c64->iec->connectDrive(); }
- (void) disconnectDrive { c64->iec->disconnectDrive(); }
- (bool) isDriveConnected { return c64->iec->driveIsConnected(); }

@end

	
