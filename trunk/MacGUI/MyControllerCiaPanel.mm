/*
 * (C) 2011 Dirk W. Hoffmann. All rights reserved.
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

@implementation MyController(CiaPanel) 

- (IBAction)searchAction:(id)sender
{
	uint16_t addr = [sender intValue];
	[memTableView scrollRowToVisible:addr/4];
	[self refresh];
}

- (void)setMemSource:(Memory::MemoryType)source
{
	// memsource = source;
	[self refresh];
}

- (IBAction)setMemSourceToRAM:(id)sender
{
	[self setMemSource:Memory::MEM_RAM];
}

- (IBAction)setMemSourceToROM:(id)sender
{
	[self setMemSource:Memory::MEM_ROM];
}

- (IBAction)setMemSourceToIO:(id)sender
{
	[self setMemSource:Memory::MEM_IO];
}

- (void)refreshMemory
{
}

- (Memory::MemoryType)currentMemSource
{
	if ([ramSource intValue]) return Memory::MEM_RAM;
	if ([romSource intValue]) return Memory::MEM_ROM;
	if ([ioSource intValue]) return Memory::MEM_IO;
	
	assert(false);
	return Memory::MEM_RAM;
}

- (void)doubleClickInMemTable:(id)sender
{
	[self refresh];
}

- (void)changeMemValue:(uint16_t)addr value:(int16_t)v memtype:(Memory::MemoryType)t
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] changeMemValue:addr value:[[c64 mem] peekFrom:addr memtype:t] memtype:t];
	if (![undo isUndoing]) [undo setActionName:@"Memory contents"];
	
	[[c64 mem] pokeTo:addr value:v memtype:t];
	[self refresh];	
}

- (void)setMemObjectValue:(id)anObject forTableColumn:(NSTableColumn *)aTableColumn row:(int)row
{
	
	uint16_t addr = row * 4;
	int16_t value = [anObject intValue];
	NSString *id  = [aTableColumn identifier];
	
	// Compute exact address
	if ([id isEqual:@"hex1"]) addr += 1;
	if ([id isEqual:@"hex2"]) addr += 2;
	if ([id isEqual:@"hex3"]) addr += 3;
	
	uint8_t oldValue = [[c64 mem] peekFrom:addr memtype:[self currentMemSource]];
	if (oldValue == value)
		return; 
	
	[self changeMemValue:addr value:value memtype:[self currentMemSource]];
}

// --------------------------------------------------------------------------------
// Action methods (CIA)
// --------------------------------------------------------------------------------

- (void)_ciaDataPortAAction:(int)nr value:(uint8_t)v
{
	[[c64 cia:nr] setDataPortA:v];
	[self refresh];
}

- (IBAction)cia1DataPortAAction:(id)sender 
{	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaDataPortAAction:1 value:[[c64 cia1] dataPortA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA1 data port A"];
	
	[self _ciaDataPortAAction:1 value:[sender intValue]];
} 

- (IBAction)cia2DataPortAAction:(id)sender 
{	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaDataPortAAction:2 value:[[c64 cia2] dataPortA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA2 data port A"];
	
	[self _ciaDataPortAAction:2 value:[sender intValue]];
} 

- (void)_ciaDataPortDirectionAAction:(int)nr value:(uint8_t)v
{
	[[c64 cia:nr] setDataPortDirectionA:v];
	[self refresh];
}

- (IBAction)cia1DataPortDirectionAAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaDataPortDirectionAAction:1 value:[[c64 cia1] dataPortDirectionA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA1 data port direction A"];
	
	[self _ciaDataPortDirectionAAction:1 value:[sender intValue]];
}

- (IBAction)cia2DataPortDirectionAAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaDataPortDirectionAAction:2 value:[[c64 cia2] dataPortDirectionA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA2 data port direction A"];
	
	[self _ciaDataPortDirectionAAction:2 value:[sender intValue]];
}

- (void)_ciaTimerAAction:(int)nr value:(uint16_t)v
{
	[[c64 cia:nr] setTimerA:v];
	[self refresh];
}

- (IBAction)cia1TimerAAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaTimerAAction:1 value:[[c64 cia1] dataPortDirectionA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA1 timer A"];
	
	[self _ciaTimerAAction:1 value:[sender intValue]];
}

- (IBAction)cia2TimerAAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaTimerAAction:2 value:[[c64 cia2] dataPortDirectionA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA2 timer A"];
	
	[self _ciaTimerAAction:2 value:[sender intValue]];
}

- (void)_ciaLatchedTimerAAction:(int)nr value:(uint16_t)v
{
	[[c64 cia:nr] setTimerLatchA:v];
	[self refresh];
}

- (IBAction)cia1LatchedTimerAAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaLatchedTimerAAction:1 value:[[c64 cia1] timerLatchA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA1 timer latch A"];
	
	[self _ciaLatchedTimerAAction:1 value:[sender intValue]];
}

- (IBAction)cia2LatchedTimerAAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaLatchedTimerAAction:2 value:[[c64 cia2] timerLatchA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA2 timer latch A"];
	
	[self _ciaLatchedTimerAAction:2 value:[sender intValue]];
}

- (void)_ciaRunningAAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setStartFlagA:b];
	[self refresh];
}

- (IBAction)cia1RunningAAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaRunningAAction:1 value:[[c64 cia1] startFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"Start/Stop CIA1 timer A"];
	
	[self _ciaRunningAAction:1 value:[sender intValue]];
}

- (IBAction)cia2RunningAAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaRunningAAction:2 value:[[c64 cia2] startFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"Start/Stop CIA2 timer A"];
	
	[self _ciaRunningAAction:2 value:[sender intValue]];
}

- (void)_ciaOneShotAAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setOneShotFlagA:b];
	[self refresh];
}

- (IBAction)cia1OneShotAAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaOneShotAAction:1 value:[[c64 cia1] oneShotFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA1 one shot flag A"];
	
	[self _ciaOneShotAAction:1 value:[sender intValue]];
}

- (IBAction)cia2OneShotAAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaOneShotAAction:2 value:[[c64 cia2] oneShotFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA2 one shot flag A"];
	
	[self _ciaOneShotAAction:2 value:[sender intValue]];
}

- (void)_ciaCountUnterflowsAAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setUnderflowFlagA:b];
	[self refresh];
}

- (IBAction)cia1CountUnterflowsAAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaCountUnterflowsAAction:1 value:[[c64 cia1] underflowFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA1 underflow flag A"];
	
	[self _ciaCountUnterflowsAAction:1 value:[sender intValue]];
}

- (IBAction)cia2CountUnterflowsAAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaCountUnterflowsAAction:2 value:[[c64 cia2] underflowFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA2 underflow flag A"];
	
	[self _ciaCountUnterflowsAAction:2 value:[sender intValue]];
}

- (void)_ciaSignalPendingAAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setPendingSignalFlagA:b];
	[self refresh];
}

- (IBAction)cia1SignalPendingAAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaSignalPendingAAction:1 value:[[c64 cia1] pendingSignalFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA1 signal pending A"];
	
	[self _ciaSignalPendingAAction:1 value:[sender intValue]];
}

- (IBAction)cia2SignalPendingAAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaSignalPendingAAction:2 value:[[c64 cia2] pendingSignalFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA2 signal pending A"];
	
	[self _ciaSignalPendingAAction:2 value:[sender intValue]];
}

- (void)_ciaInterruptEnableAAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setInterruptEnableFlagA:b];
	[self refresh];
}

- (IBAction)cia1InterruptEnableAAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaInterruptEnableAAction:1 value:[[c64 cia1] interruptEnableFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA1 interrupt enable flag A"];
	
	[self _ciaInterruptEnableAAction:1 value:[sender intValue]];
}

- (IBAction)cia2InterruptEnableAAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaInterruptEnableAAction:2 value:[[c64 cia2] interruptEnableFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA2 interrupt enable flag A"];
	
	[self _ciaInterruptEnableAAction:2 value:[sender intValue]];
}

- (void)_ciaDataPortBAction:(int)nr value:(uint8_t)v
{
	[[c64 cia:nr] setDataPortB:v];
	[self refresh];
}

- (IBAction)cia1DataPortBAction:(id)sender 
{	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaDataPortBAction:1 value:[[c64 cia1] dataPortB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA1 data port B"];
	
	[self _ciaDataPortBAction:1 value:[sender intValue]];
} 

- (IBAction)cia2DataPortBAction:(id)sender 
{	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaDataPortBAction:2 value:[[c64 cia2] dataPortB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA2 data port B"];
	
	[self _ciaDataPortBAction:2 value:[sender intValue]];
} 

- (void)_ciaDataPortDirectionBAction:(int)nr value:(uint8_t)v
{
	[[c64 cia:nr] setDataPortDirectionB:v];
	[self refresh];
}

- (IBAction)cia1DataPortDirectionBAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaDataPortDirectionBAction:1 value:[[c64 cia1] dataPortDirectionB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA1 data port direction B"];
	
	[self _ciaDataPortDirectionBAction:1 value:[sender intValue]];
}

- (IBAction)cia2DataPortDirectionBAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaDataPortDirectionBAction:2 value:[[c64 cia2] dataPortDirectionB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA2 data port direction B"];
	
	[self _ciaDataPortDirectionBAction:2 value:[sender intValue]];
}

- (void)_ciaTimerBAction:(int)nr value:(uint16_t)v
{
	[[c64 cia:nr] setTimerB:v];
	[self refresh];
}

- (IBAction)cia1TimerBAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaTimerBAction:1 value:[[c64 cia1] timerB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA1 timer B"];
	
	[self _ciaTimerBAction:1 value:[sender intValue]];
}

- (IBAction)cia2TimerBAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaTimerBAction:2 value:[[c64 cia2] timerB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA2 timer B"];
	
	[self _ciaTimerBAction:2 value:[sender intValue]];
}

- (void)_ciaLatchedTimerBAction:(int)nr value:(uint16_t)v
{
	[[c64 cia:nr] setTimerLatchB:v];
	[self refresh];
}

- (IBAction)cia1LatchedTimerBAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaLatchedTimerBAction:1 value:[[c64 cia1] timerLatchB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA1 timer latch B"];
	
	[self _ciaLatchedTimerBAction:1 value:[sender intValue]];
}

- (IBAction)cia2LatchedTimerBAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaLatchedTimerBAction:2 value:[[c64 cia2] timerLatchB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA2 timer latch B"];
	
	[self _ciaLatchedTimerBAction:2 value:[sender intValue]];
}

- (void)_ciaRunningBAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setStartFlagB:b];
	[self refresh];
}

- (IBAction)cia1RunningBAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaRunningBAction:1 value:[[c64 cia1] startFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"Start/Stop CIA1 timer B"];
	
	[self _ciaRunningBAction:1 value:[sender intValue]];
}

- (IBAction)cia2RunningBAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaRunningBAction:2 value:[[c64 cia2] startFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"Start/Stop CIA2 timer B"];
	
	[self _ciaRunningBAction:2 value:[sender intValue]];
}

- (void)_ciaOneShotBAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setOneShotFlagB:b];
	[self refresh];
}

- (IBAction)cia1OneShotBAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaOneShotBAction:1 value:[[c64 cia1] oneShotFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA1 one shot flag B"];
	
	[self _ciaOneShotBAction:1 value:[sender intValue]];
}

- (IBAction)cia2OneShotBAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaOneShotBAction:2 value:[[c64 cia2] oneShotFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA2 one shot flag B"];
	
	[self _ciaOneShotBAction:2 value:[sender intValue]];
}

- (void)_ciaCountUnterflowsBAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setUnderflowFlagB:b];
	[self refresh];
}

- (IBAction)cia1CountUnterflowsBAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaCountUnterflowsBAction:1 value:[[c64 cia1] underflowFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA1 underflow flag B"];
	
	[self _ciaCountUnterflowsBAction:1 value:[sender intValue]];
}

- (IBAction)cia2CountUnterflowsBAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaCountUnterflowsBAction:2 value:[[c64 cia2] underflowFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA2 underflow flag B"];
	
	[self _ciaCountUnterflowsBAction:2 value:[sender intValue]];
}

- (void)_ciaSignalPendingBAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setPendingSignalFlagB:b];
	[self refresh];
}

- (IBAction)cia1SignalPendingBAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaSignalPendingBAction:1 value:[[c64 cia1] pendingSignalFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA1 signal pending B"];
	
	[self _ciaSignalPendingBAction:1 value:[sender intValue]];
}

- (IBAction)cia2SignalPendingBAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaSignalPendingBAction:2 value:[[c64 cia2] pendingSignalFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA2 signal pending B"];
	
	[self _ciaSignalPendingBAction:2 value:[sender intValue]];
}

- (void)_ciaInterruptEnableBAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setInterruptEnableFlagB:b];
	[self refresh];
}

- (IBAction)cia1InterruptEnableBAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaInterruptEnableBAction:1 value:[[c64 cia1] interruptEnableFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA1 interrupt enable flag B"];
	
	[self _ciaInterruptEnableBAction:1 value:[sender intValue]];
}

- (IBAction)cia2InterruptEnableBAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaInterruptEnableBAction:2 value:[[c64 cia2] interruptEnableFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA2 interrupt enable flag B"];
	
	[self _ciaInterruptEnableBAction:2 value:[sender intValue]];
}

- (IBAction)_todHoursAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setTodHours:value];
	[self refresh];
}

- (IBAction)tod1HoursAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todHoursAction:1 value:[[c64 cia1] todHours]];
	if (![undo isUndoing]) [undo setActionName:@"TOD1 hours"];
	
	[self _todHoursAction:1 value:[sender intValue]];
}	

- (IBAction)tod2HoursAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todHoursAction:2 value:[[c64 cia2] todHours]];
	if (![undo isUndoing]) [undo setActionName:@"TOD2 hours"];
	
	[self _todHoursAction:2 value:[sender intValue]];
}	

- (IBAction)_todMinutesAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setTodMinutes:value];
	[self refresh];
}

- (IBAction)tod1MinutesAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todMinutesAction:1 value:[[c64 cia1] todMinutes]];
	if (![undo isUndoing]) [undo setActionName:@"TOD1 minutes"];
	
	[self _todMinutesAction:1 value:[sender intValue]];
}

- (IBAction)tod2MinutesAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todMinutesAction:2 value:[[c64 cia2] todMinutes]];
	if (![undo isUndoing]) [undo setActionName:@"TOD2 minutes"];
	
	[self _todMinutesAction:2 value:[sender intValue]];
}

- (IBAction)_todSecondsAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setTodSeconds:value];
	[self refresh];
}

- (IBAction)tod1SecondsAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todSecondsAction:1 value:[[c64 cia1] todSeconds]];
	if (![undo isUndoing]) [undo setActionName:@"TOD1 seconds"];
	
	[self _todSecondsAction:1 value:[sender intValue]];
}

- (IBAction)tod2SecondsAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todSecondsAction:2 value:[[c64 cia2] todSeconds]];
	if (![undo isUndoing]) [undo setActionName:@"TOD2 seconds"];
	
	[self _todSecondsAction:2 value:[sender intValue]];
}

- (IBAction)_todTenthAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setTodTenth:value];
	[self refresh];
}

- (IBAction)tod1TenthAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todTenthAction:1 value:[[c64 cia1] todTenth]];
	if (![undo isUndoing]) [undo setActionName:@"TOD1 hours"];
	
	[self _todTenthAction:1 value:[sender intValue]];
}

- (IBAction)tod2TenthAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todTenthAction:2 value:[[c64 cia2] todTenth]];
	if (![undo isUndoing]) [undo setActionName:@"TOD2 hours"];
	
	[self _todTenthAction:2 value:[sender intValue]];
}

- (IBAction)_alarmHoursAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setAlarmHours:value];
	[self refresh];
}

- (IBAction)alarm1HoursAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _alarmHoursAction:1 value:[[c64 cia1] alarmHours]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm1 hours"];
	
	[self _alarmHoursAction:1 value:[sender intValue]];
}

- (IBAction)alarm2HoursAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _alarmHoursAction:2 value:[[c64 cia2] alarmHours]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm2 hours"];
	
	[self _alarmHoursAction:2 value:[sender intValue]];
}

- (IBAction)_alarmMinutesAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setAlarmMinutes:value];
	[self refresh];
}

- (IBAction)alarm1MinutesAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _alarmMinutesAction:1 value:[[c64 cia1] alarmMinutes]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm1 minutes"];
	
	[self _alarmMinutesAction:1 value:[sender intValue]];
}

- (IBAction)alarm2MinutesAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _alarmMinutesAction:2 value:[[c64 cia2] alarmMinutes]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm2 minutes"];
	
	[self _alarmMinutesAction:2 value:[sender intValue]];
}

- (IBAction)_alarmSecondsAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setAlarmSeconds:value];
	[self refresh];
}

- (IBAction)alarm1SecondsAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _alarmSecondsAction:1 value:[[c64 cia1] alarmSeconds]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm1 seconds"];
	
	[self _alarmSecondsAction:1 value:[sender intValue]];
}

- (IBAction)alarm2SecondsAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _alarmSecondsAction:2 value:[[c64 cia2] alarmSeconds]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm2 seconds"];
	
	[self _alarmSecondsAction:2 value:[sender intValue]];
}

- (IBAction)_alarmTenthAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setAlarmTenth:value];
	[self refresh];
}

- (IBAction)alarm1TenthAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _alarmTenthAction:1 value:[[c64 cia1] alarmTenth]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm1 hours"];
	
	[self _alarmTenthAction:1 value:[sender intValue]];
}

- (IBAction)alarm2TenthAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _alarmTenthAction:2 value:[[c64 cia2] alarmTenth]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm2 hours"];
	
	[self _alarmTenthAction:2 value:[sender intValue]];
}

- (IBAction)_todInterruptEnabledAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setTodInterruptEnabled:b];
	[self refresh];
}

- (IBAction)tod1InterruptEnabledAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todInterruptEnabledAction:1 value:[[c64 cia1] isTodInterruptEnabled]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm1 enable"];
	
	[self _todInterruptEnabledAction:1 value:[sender intValue]];
}

- (IBAction)tod2InterruptEnabledAction:(id)sender 
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todInterruptEnabledAction:2 value:[[c64 cia2] isTodInterruptEnabled]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm2 enable"];
	
	[self _todInterruptEnabledAction:2 value:[sender intValue]];
}

- (void)refreshCIA
{	
    // CIA 1
	[cia1DataPortA setIntValue:[[c64 cia1] dataPortA]];
	[cia1DataPortDirectionA setIntValue:[[c64 cia1] dataPortDirectionA]];
	[cia1TimerA setIntValue:[[c64 cia1] timerA]];
	[cia1LatchedTimerA setIntValue:[[c64 cia1] timerLatchA]];
	[cia1RunningA setIntValue:[[c64 cia1] startFlagA]];
	[cia1OneShotA setIntValue:[[c64 cia1] oneShotFlagA]];
	[cia1SignalPendingA setIntValue:[[c64 cia1] pendingSignalFlagA]];
	[cia1InterruptEnableA setIntValue:[[c64 cia1] interruptEnableFlagA]];
	
	[cia1DataPortB setIntValue:[[c64 cia1] dataPortB]];
	[cia1DataPortDirectionB setIntValue:[[c64 cia1] dataPortDirectionB]];
	[cia1TimerB setIntValue:[[c64 cia1] timerB]];
	[cia1LatchedTimerB setIntValue:[[c64 cia1]timerLatchB]];
	[cia1RunningB setIntValue:[[c64 cia1] startFlagB]];
	[cia1OneShotB setIntValue:[[c64 cia1] oneShotFlagB]];
	[cia1SignalPendingB setIntValue:[[c64 cia1] pendingSignalFlagB]];
	[cia1InterruptEnableB setIntValue:[[c64 cia1] interruptEnableFlagB]];
	
	[tod1Hours setIntValue:[[c64 cia1] todHours]];
	[tod1Minutes setIntValue:[[c64 cia1] todMinutes]];
	[tod1Seconds setIntValue:[[c64 cia1] todSeconds]];
	[tod1Tenth setIntValue:[[c64 cia1] todTenth]];
	
	[alarm1Hours setIntValue:[[c64 cia1] alarmHours]];
	[alarm1Minutes setIntValue:[[c64 cia1] alarmMinutes]];
	[alarm1Seconds setIntValue:[[c64 cia1] alarmSeconds]];
	[alarm1Tenth setIntValue:[[c64 cia1] alarmTenth]];	
	[tod1InterruptEnabled setIntValue:[[c64 cia1] isTodInterruptEnabled]];

    // CIA 2
	[cia2DataPortA setIntValue:[[c64 cia2] dataPortA]];
	[cia2DataPortDirectionA setIntValue:[[c64 cia2] dataPortDirectionA]];
	[cia2TimerA setIntValue:[[c64 cia2] timerA]];
	[cia2LatchedTimerA setIntValue:[[c64 cia2] timerLatchA]];
	[cia2RunningA setIntValue:[[c64 cia2] startFlagA]];
	[cia2OneShotA setIntValue:[[c64 cia2] oneShotFlagA]];
	[cia2SignalPendingA setIntValue:[[c64 cia2] pendingSignalFlagA]];
	[cia2InterruptEnableA setIntValue:[[c64 cia2] interruptEnableFlagA]];
	
	[cia2DataPortB setIntValue:[[c64 cia2] dataPortB]];
	[cia2DataPortDirectionB setIntValue:[[c64 cia2] dataPortDirectionB]];
	[cia2TimerB setIntValue:[[c64 cia2] timerB]];
	[cia2LatchedTimerB setIntValue:[[c64 cia2]timerLatchB]];
	[cia2RunningB setIntValue:[[c64 cia2] startFlagB]];
	[cia2OneShotB setIntValue:[[c64 cia2] oneShotFlagB]];
	[cia2SignalPendingB setIntValue:[[c64 cia2] pendingSignalFlagB]];
	[cia2InterruptEnableB setIntValue:[[c64 cia2] interruptEnableFlagB]];
	
	[tod2Hours setIntValue:[[c64 cia2] todHours]];
	[tod2Minutes setIntValue:[[c64 cia2] todMinutes]];
	[tod2Seconds setIntValue:[[c64 cia2] todSeconds]];
	[tod2Tenth setIntValue:[[c64 cia2] todTenth]];
	
	[alarm2Hours setIntValue:[[c64 cia2] alarmHours]];
	[alarm2Minutes setIntValue:[[c64 cia2] alarmMinutes]];
	[alarm2Seconds setIntValue:[[c64 cia2] alarmSeconds]];
	[alarm2Tenth setIntValue:[[c64 cia2] alarmTenth]];	
	[tod2InterruptEnabled setIntValue:[[c64 cia2] isTodInterruptEnabled]];

}

@end
