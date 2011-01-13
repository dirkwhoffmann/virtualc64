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

- (IBAction)ciaSelectCiaAction:(id)sender
{
	[self refresh];
}

- (void)_ciaDataPortAAction:(int)nr value:(uint8_t)v
{
	[[c64 cia:nr] setDataPortA:v];
	[self refresh];
}

- (IBAction)ciaDataPortAAction:(id)sender 
{	
	int nr = [self currentCIA];
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaDataPortAAction:nr value:[[c64 cia:nr] getDataPortA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA data port A"];
	
	[self _ciaDataPortAAction:nr value:[sender intValue]];
} 

- (void)_ciaDataPortDirectionAAction:(int)nr value:(uint8_t)v
{
	[[c64 cia:nr] setDataPortDirectionA:v];
	[self refresh];
}

- (IBAction)ciaDataPortDirectionAAction:(id)sender 
{
	int nr = [self currentCIA];
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaDataPortDirectionAAction:nr value:[[c64 cia:nr] getDataPortDirectionA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA data port direction A"];
	
	[self _ciaDataPortDirectionAAction:nr value:[sender intValue]];
}

- (void)_ciaTimerAAction:(int)nr value:(uint16_t)v
{
	[[c64 cia:nr] setTimerA:v];
	[self refresh];
}

- (IBAction)ciaTimerAAction:(id)sender 
{
	int nr = [self currentCIA];
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaTimerAAction:nr value:[[c64 cia:nr] getDataPortDirectionA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA timer A"];
	
	[self _ciaTimerAAction:nr value:[sender intValue]];
}

- (void)_ciaLatchedTimerAAction:(int)nr value:(uint16_t)v
{
	[[c64 cia:nr] setTimerLatchA:v];
	[self refresh];
}

- (IBAction)ciaLatchedTimerAAction:(id)sender
{
	int nr = [self currentCIA];	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaLatchedTimerAAction:nr value:[[c64 cia:nr] getTimerLatchA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA timer latch A"];
	
	[self _ciaLatchedTimerAAction:nr value:[sender intValue]];
}

- (void)_ciaRunningAAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setStartFlagA:b];
	[self refresh];
}

- (IBAction)ciaRunningAAction:(id)sender
{
	int nr = [self currentCIA];	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaRunningAAction:nr value:[[c64 cia:nr] getStartFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"Start/Stop CIA timer A"];
	
	[self _ciaRunningAAction:nr value:[sender intValue]];
}


- (void)_ciaOneShotAAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setOneShotFlagA:b];
	[self refresh];
}

- (IBAction)ciaOneShotAAction:(id)sender
{
	int nr = [self currentCIA];	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaOneShotAAction:nr value:[[c64 cia:nr] getOneShotFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA one shot flag A"];
	
	[self _ciaOneShotAAction:nr value:[sender intValue]];
}

- (void)_ciaCountUnterflowsAAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setUnderflowFlagA:b];
	[self refresh];
}

- (IBAction)ciaCountUnterflowsAAction:(id)sender
{
	int nr = [self currentCIA];	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaCountUnterflowsAAction:nr value:[[c64 cia:nr] getUnderflowFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA underflow flag A"];
	
	[self _ciaCountUnterflowsAAction:nr value:[sender intValue]];
}

- (void)_ciaSignalPendingAAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setPendingSignalFlagA:b];
	[self refresh];
}

- (IBAction)ciaSignalPendingAAction:(id)sender
{
	int nr = [self currentCIA];	
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaSignalPendingAAction:nr value:[[c64 cia:nr] getPendingSignalFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA signal pending A"];
	
	[self _ciaSignalPendingAAction:nr value:[sender intValue]];
}

- (void)_ciaInterruptEnableAAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setInterruptEnableFlagA:b];
	[self refresh];
}

- (IBAction)ciaInterruptEnableAAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaInterruptEnableAAction:nr value:[[c64 cia:nr] getInterruptEnableFlagA]];
	if (![undo isUndoing]) [undo setActionName:@"CIA interrupt enable flag A"];
	
	[self _ciaInterruptEnableAAction:nr value:[sender intValue]];
}

- (void)_ciaDataPortBAction:(int)nr value:(uint8_t)v
{
	[[c64 cia:nr] setDataPortB:v];
	[self refresh];
}

- (IBAction)ciaDataPortBAction:(id)sender 
{	
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaDataPortBAction:nr value:[[c64 cia:nr] getDataPortB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA data port B"];
	
	[self _ciaDataPortBAction:nr value:[sender intValue]];
} 

- (void)_ciaDataPortDirectionBAction:(int)nr value:(uint8_t)v
{
	[[c64 cia:nr] setDataPortDirectionB:v];
	[self refresh];
}

- (IBAction)ciaDataPortDirectionBAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaDataPortDirectionBAction:nr value:[[c64 cia:nr] getDataPortDirectionB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA data port direction B"];
	
	[self _ciaDataPortDirectionBAction:nr value:[sender intValue]];
}

- (void)_ciaTimerBAction:(int)nr value:(uint16_t)v
{
	[[c64 cia:nr] setTimerB:v];
	[self refresh];
}

- (IBAction)ciaTimerBAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaTimerBAction:nr value:[[c64 cia:nr] getTimerB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA timer B"];
	
	[self _ciaTimerBAction:nr value:[sender intValue]];
}

- (void)_ciaLatchedTimerBAction:(int)nr value:(uint16_t)v
{
	[[c64 cia:nr] setTimerLatchB:v];
	[self refresh];
}

- (IBAction)ciaLatchedTimerBAction:(id)sender
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaLatchedTimerBAction:nr value:[[c64 cia:nr] getTimerLatchB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA timer latch B"];
	
	[self _ciaLatchedTimerBAction:nr value:[sender intValue]];
}

- (void)_ciaRunningBAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setStartFlagB:b];
	[self refresh];
}

- (IBAction)ciaRunningBAction:(id)sender
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaRunningBAction:nr value:[[c64 cia:nr] getStartFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"Start/Stop CIA timer B"];
	
	[self _ciaRunningBAction:nr value:[sender intValue]];
}

- (void)_ciaOneShotBAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setOneShotFlagB:b];
	[self refresh];
}

- (IBAction)ciaOneShotBAction:(id)sender
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaOneShotBAction:nr value:[[c64 cia:nr] getOneShotFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA one shot flag B"];
	
	[self _ciaOneShotBAction:nr value:[sender intValue]];
}

- (void)_ciaCountUnterflowsBAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setUnderflowFlagB:b];
	[self refresh];
}

- (IBAction)ciaCountUnterflowsBAction:(id)sender
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaCountUnterflowsBAction:nr value:[[c64 cia:nr] getUnderflowFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA underflow flag B"];
	
	[self _ciaCountUnterflowsBAction:nr value:[sender intValue]];
}

- (void)_ciaSignalPendingBAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setPendingSignalFlagB:b];
	[self refresh];
}

- (IBAction)ciaSignalPendingBAction:(id)sender
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaSignalPendingBAction:nr value:[[c64 cia:nr] getPendingSignalFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA signal pending B"];
	
	[self _ciaSignalPendingBAction:nr value:[sender intValue]];
}

- (void)_ciaInterruptEnableBAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] setInterruptEnableFlagB:b];
	[self refresh];
}

- (IBAction)ciaInterruptEnableBAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _ciaInterruptEnableBAction:nr value:[[c64 cia:nr] getInterruptEnableFlagB]];
	if (![undo isUndoing]) [undo setActionName:@"CIA interrupt enable flag B"];
	
	[self _ciaInterruptEnableBAction:nr value:[sender intValue]];
}

- (IBAction)_todHoursAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setTodHours:value];
	[self refresh];
}

- (IBAction)todHoursAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todHoursAction:nr value:[[c64 cia:nr] getTodHours]];
	if (![undo isUndoing]) [undo setActionName:@"TOD hours"];
	
	[self _todHoursAction:nr value:[sender intValue]];
}	

- (IBAction)_todMinutesAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setTodMinutes:value];
	[self refresh];
}

- (IBAction)todMinutesAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todMinutesAction:nr value:[[c64 cia:nr] getTodMinutes]];
	if (![undo isUndoing]) [undo setActionName:@"TOD minutes"];
	
	[self _todMinutesAction:nr value:[sender intValue]];
}

- (IBAction)_todSecondsAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setTodSeconds:value];
	[self refresh];
}

- (IBAction)todSecondsAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todSecondsAction:nr value:[[c64 cia:nr] getTodSeconds]];
	if (![undo isUndoing]) [undo setActionName:@"TOD seconds"];
	
	[self _todSecondsAction:nr value:[sender intValue]];
}

- (IBAction)_todTenthAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setTodTenth:value];
	[self refresh];
}

- (IBAction)todTenthAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todTenthAction:nr value:[[c64 cia:nr] getTodTenth]];
	if (![undo isUndoing]) [undo setActionName:@"TOD hours"];
	
	[self _todTenthAction:nr value:[sender intValue]];
}

- (IBAction)_alarmHoursAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setAlarmHours:value];
	[self refresh];
}

- (IBAction)alarmHoursAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _alarmHoursAction:nr value:[[c64 cia:nr] getAlarmHours]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm hours"];
	
	[self _alarmHoursAction:nr value:[sender intValue]];
}

- (IBAction)_alarmMinutesAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setAlarmMinutes:value];
	[self refresh];
}

- (IBAction)alarmMinutesAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _alarmMinutesAction:nr value:[[c64 cia:nr] getAlarmMinutes]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm minutes"];
	
	[self _alarmMinutesAction:nr value:[sender intValue]];
}

- (IBAction)_alarmSecondsAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setAlarmSeconds:value];
	[self refresh];
}

- (IBAction)alarmSecondsAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _alarmSecondsAction:nr value:[[c64 cia:nr] getAlarmSeconds]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm seconds"];
	
	[self _alarmSecondsAction:nr value:[sender intValue]];
}

- (IBAction)_alarmTenthAction:(int)nr value:(uint8_t)value
{
	[[c64 cia:nr] setAlarmTenth:value];
	[self refresh];
}

- (IBAction)alarmTenthAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _alarmTenthAction:nr value:[[c64 cia:nr] getAlarmTenth]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm hours"];
	
	[self _alarmTenthAction:nr value:[sender intValue]];
}

- (IBAction)_todInterruptEnabledAction:(int)nr value:(bool)b
{
	[[c64 cia:nr] todSetInterruptEnabled:b];
	[self refresh];
}

- (IBAction)todInterruptEnabledAction:(id)sender 
{
	int nr = [self currentCIA];		
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _todInterruptEnabledAction:nr value:[[c64 cia:nr] todIsInterruptEnabled]];
	if (![undo isUndoing]) [undo setActionName:@"Alarm enable"];
	
	[self _todInterruptEnabledAction:nr value:[sender intValue]];
}

- (int)currentCIA
{
	if ([ciaSelector indexOfSelectedItem] == 0)
		return 1;
	else
		return 2;
}

- (void)refreshCIA
{
	CIAProxy *cia = [c64 cia:[self currentCIA]];
	
	[ciaDataPortA setIntValue:[cia getDataPortA]];
	[ciaDataPortDirectionA setIntValue:[cia getDataPortDirectionA]];
	[ciaTimerA setIntValue:[cia getTimerA]];
	[ciaLatchedTimerA setIntValue:[cia getTimerLatchA]];
	[ciaRunningA setIntValue:[cia getStartFlagA]];
	[ciaOneShotA setIntValue:[cia getOneShotFlagA]];
	[ciaSignalPendingA setIntValue:[cia getPendingSignalFlagA]];
	[ciaInterruptEnableA setIntValue:[cia getInterruptEnableFlagA]];
	
	[ciaDataPortB setIntValue:[cia getDataPortB]];
	[ciaDataPortDirectionB setIntValue:[cia getDataPortDirectionB]];
	[ciaTimerB setIntValue:[cia getTimerB]];
	[ciaLatchedTimerB setIntValue:[cia getTimerLatchB]];
	[ciaRunningB setIntValue:[cia getStartFlagB]];
	[ciaOneShotB setIntValue:[cia getOneShotFlagB]];
	[ciaSignalPendingB setIntValue:[cia getPendingSignalFlagB]];
	[ciaInterruptEnableB setIntValue:[cia getInterruptEnableFlagB]];
	
	[todHours setIntValue:[cia getTodHours]];
	[todMinutes setIntValue:[cia getTodMinutes]];
	[todSeconds setIntValue:[cia getTodSeconds]];
	[todTenth setIntValue:[cia getTodTenth]];
	
	[alarmHours setIntValue:[cia getAlarmHours]];
	[alarmMinutes setIntValue:[cia getAlarmMinutes]];
	[alarmSeconds setIntValue:[cia getAlarmSeconds]];
	[alarmTenth setIntValue:[cia getAlarmTenth]];	
	[todInterruptEnabled setIntValue:[cia todIsInterruptEnabled]];
}

@end
