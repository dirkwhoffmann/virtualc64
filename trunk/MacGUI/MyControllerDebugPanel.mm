//
//  ControllerDebugPanel.m
//  V64
//
//  Created by Dirk Hoffmann on 11.01.11.
//  Copyright 2011 Hochschule Karlsruhe. All rights reserved.
//

#import "MyController.h"
#import "MyControllerDebugPanel.h"

@implementation MyController(DebugPanel) 

// --------------------------------------------------------------------------------
// Action methods (CPU Debug panel)
// --------------------------------------------------------------------------------

- (IBAction)aAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] aAction:[NSNumber numberWithInt:[[c64 cpu] getA]]];
	if (![undo isUndoing]) [undo setActionName:@"Set accumulator"];
	
	[[c64 cpu] setA:[sender intValue]];
	[self refresh];
}

- (IBAction)xAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] xAction:[NSNumber numberWithInt:[[c64 cpu] getX]]];
	if (![undo isUndoing]) [undo setActionName:@"Set X register"];
	
	[[c64 cpu] setX:[sender intValue]];
	[self refresh];
}

- (IBAction)yAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] yAction:[NSNumber numberWithInt:[[c64 cpu] getY]]];
	if (![undo isUndoing]) [undo setActionName:@"Set Y register"];
	
	[[c64 cpu] setY:[sender intValue]];
	[self refresh];
}

- (IBAction)pcAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] pcAction:[NSNumber numberWithInt:[[c64 cpu] getPC]]];
	if (![undo isUndoing]) [undo setActionName:@"Set program counter"];
	
	[[c64 cpu] setPC:[sender intValue]];
	[self refresh];
}

- (IBAction)spAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spAction:[NSNumber numberWithInt:[[c64 cpu] getSP]]];
	if (![undo isUndoing]) [undo setActionName:@"Set stack pointer"];
	
	[[c64 cpu] setSP:[sender intValue]];
	[self refresh];
}

- (IBAction)NAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] NAction:[NSNumber numberWithInt:[[c64 cpu] getN]]];
	if (![undo isUndoing]) [undo setActionName:@"Negative Flag"];
	
	[[c64 cpu] setN:[sender intValue]];
	[self refresh];
}

- (IBAction)ZAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] ZAction:[NSNumber numberWithInt:[[c64 cpu] getZ]]];
	if (![undo isUndoing]) [undo setActionName:@"Zero Flag"];
	
	[[c64 cpu] setZ:[sender intValue]];
	[self refresh];
}

- (IBAction)CAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] CAction:[NSNumber numberWithInt:[[c64 cpu] getC]]];
	if (![undo isUndoing]) [undo setActionName:@"Carry Flag"];
	
	[[c64 cpu] setC:[sender intValue]];
	[self refresh];
}

- (IBAction)IAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] IAction:[NSNumber numberWithInt:[[c64 cpu] getI]]];
	if (![undo isUndoing]) [undo setActionName:@"Interrupt Flag"];
	
	[[c64 cpu] setI:[sender intValue]];
	[self refresh];
}

- (IBAction)BAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] BAction:[NSNumber numberWithInt:[[c64 cpu] getB]]];
	if (![undo isUndoing]) [undo setActionName:@"Break Flag"];
	
	[[c64 cpu] setB:[sender intValue]];
	[self refresh];
}

- (IBAction)DAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] DAction:[NSNumber numberWithInt:[[c64 cpu] getD]]];
	if (![undo isUndoing]) [undo setActionName:@"Decimal Flag"];
	
	[[c64 cpu] setD:[sender intValue]];
	[self refresh];
}

- (IBAction)VAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] VAction:[NSNumber numberWithInt:[[c64 cpu] getV]]];
	if (![undo isUndoing]) [undo setActionName:@"Overflow Flag"];
	
	[[c64 cpu] setV:[sender intValue]];
	[self refresh];
}

- (IBAction)mhzAction:(id)sender
{
#if 0
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] mhzAction:[c64 cpuGetMHz]];
	if (![undo isUndoing]) [undo setActionName:@"Clock frequency"];
	
	// float value = clip([sender floatValue], 0.1f, 10.0f);
	[c64 cpuSetMHz:sender];
	[self refresh];
#endif
}

- (IBAction)setHardBreakpointAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] setHardBreakpointAction:[NSNumber numberWithInt:[sender intValue]]];
	if (![undo isUndoing]) [undo setActionName:@"Breakpoint"];
	
	[[c64 cpu] toggleHardBreakpoint:[sender intValue]];
	[self refresh];
}

- (void)doubleClickInCpuTable:(id)sender
{
	uint16_t addr;
	
	addr = [[c64 cpu] getAddressOfNextIthInstruction:[sender selectedRow] from:disassembleStartAddr];
	[self setHardBreakpointAction:[NSNumber numberWithInt:addr]];
}

- (void)refreshCPU
{
	[a setIntValue:[[c64 cpu] getA]];
	[x setIntValue:[[c64 cpu] getX]];
	[y setIntValue:[[c64 cpu] getY]];
	[pc setIntValue:[[c64 cpu] getPC]];
	[sp setIntValue:[[c64 cpu] getSP]];
	
	[N setIntValue:[[c64 cpu] getN]];
	[V setIntValue:[[c64 cpu] getV]];
	[B setIntValue:[[c64 cpu] getB]];
	[D setIntValue:[[c64 cpu] getD]];
	[I setIntValue:[[c64 cpu] getI]];
	[Z setIntValue:[[c64 cpu] getZ]];
	[C setIntValue:[[c64 cpu] getC]];	
}


// --------------------------------------------------------------------------------
// Action methods (Memory debug panel)
// --------------------------------------------------------------------------------

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


// --------------------------------------------------------------------------------
// Action methods (VIC debug panel)
// --------------------------------------------------------------------------------

- (void)_vicVideoModeAction:(int)mode
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _vicVideoModeAction:[[c64 vic] getDisplayMode]];
	if (![undo isUndoing]) [undo setActionName:@"Display mode"];
	
	[[c64 vic] setDisplayMode:mode];
	[self refresh];
}

- (IBAction)vicVideoModeAction:(id)sender
{
	[self _vicVideoModeAction:[[sender selectedItem] tag]];
}

- (void)_vicScreenGeometryAction:(int)mode
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _vicScreenGeometryAction:[[c64 vic] getScreenGeometry]];
	if (![undo isUndoing]) [undo setActionName:@"Screen geometry"];
	
	[[c64 vic] setScreenGeometry:mode];
	[self refresh];
}

- (IBAction)vicScreenGeometryAction:(id)sender
{
	[self _vicScreenGeometryAction:[[sender selectedItem] tag]];
}

- (void)vicSetMemoryBank:(int)addr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicSetMemoryBank:[[c64 vic] getMemoryBankAddr]];
	if (![undo isUndoing]) [undo setActionName:@"Memory bank"];
	
	[[c64 vic] setMemoryBankAddr:addr];
	[self refresh];
}

- (IBAction)vicMemoryBankAction:(id)sender
{
	[self vicSetMemoryBank:[[sender selectedItem] tag]];
}

- (void)vicSetScreenMemory:(int)addr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicSetScreenMemory:[[c64 vic] getScreenMemoryAddr]];
	if (![undo isUndoing]) [undo setActionName:@"Screen memory"];
	
	[[c64 vic] setScreenMemoryAddr:addr];
	[self refresh];
}

- (IBAction)vicScreenMemoryAction:(id)sender
{
	[self vicSetScreenMemory:[[sender selectedItem] tag]];
}

- (void)vicSetCharacterMemory:(int)addr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicSetCharacterMemory:[[c64 vic] getCharacterMemoryAddr]];
	if (![undo isUndoing]) [undo setActionName:@"Character memory"];
	
	[[c64 vic] setCharacterMemoryAddr:addr];
	[self refresh];
}

- (IBAction)vicCharacterMemoryAction:(id)sender
{
	[self vicSetCharacterMemory:[[sender selectedItem] tag]];
}

- (IBAction)vicDXAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicDXAction:[NSNumber numberWithInt:[[c64 vic] getHorizontalRasterScroll]]];
	if (![undo isUndoing]) [undo setActionName:@"Horizontal raster scroll"];
	
	[[c64 vic] setHorizontalRasterScroll:[sender intValue]];
	[self refresh];
}

- (IBAction)vicDYAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicDYAction:[NSNumber numberWithInt:[[c64 vic] getVerticalRasterScroll]]];
	if (![undo isUndoing]) [undo setActionName:@"Vertical raster scroll"];
	
	[[c64 vic] setVerticalRasterScroll:[sender intValue]];
	[self refresh];
}

- (IBAction)vicDXStepperAction:(id)sender
{
	[self vicDXAction:sender];
}

- (IBAction)vicDYStepperAction:(id)sender
{
	[self vicDYAction:sender];
}

- (void)spriteToggleVisibilityFlag:(int)nr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteToggleVisibilityFlag:nr];
	if (![undo isUndoing]) [undo setActionName:@"Sprite visability"];
	
	[[c64 vic] spriteToggleVisibilityFlag:nr];
	[self refresh];
}

- (IBAction)vicSpriteSelectAction:(id)sender
{
	selectedSprite = [sender selectedTag];
	
	NSLog(@"selectedSprite = %d", selectedSprite);
	[self refresh];
}

- (IBAction)vicSpriteActiveAction:(id)sender
{	
	// debug("Selected sprinte = %d\n", [
	[self spriteToggleVisibilityFlag:[self currentSprite]];
}

- (void)spriteToggleMulticolorFlag:(int)nr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteToggleMulticolorFlag:nr];
	if (![undo isUndoing]) [undo setActionName:@"Sprite multicolor"];
	
	[[c64 vic] spriteToggleMulticolorFlag:nr];
	[self refresh];
}

- (IBAction)vicSpriteMulticolorAction:(id)sender
{
	[self spriteToggleMulticolorFlag:[self currentSprite]];
}

- (void)spriteToggleStretchXFlag:(int)nr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteToggleStretchXFlag:nr];
	if (![undo isUndoing]) [undo setActionName:@"Sprite stretch X"];
	
	[[c64 vic] spriteToggleStretchXFlag:nr];
	[self refresh];
}

- (IBAction)vicSpriteStretchXAction:(id)sender
{
	[self spriteToggleStretchXFlag:[self currentSprite]];
}

- (void)spriteToggleStretchYFlag:(int)nr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteToggleStretchYFlag:nr];
	if (![undo isUndoing]) [undo setActionName:@"Sprite stretch Y"];
	
	[[c64 vic] spriteToggleStretchYFlag:nr];
	[self refresh];
}

- (IBAction)vicSpriteStretchYAction:(id)sender
{
	[self spriteToggleStretchYFlag:[self currentSprite]];
}

- (void)spriteToggleBackgroundPriorityFlag:(int)nr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteToggleBackgroundPriorityFlag:nr];
	if (![undo isUndoing]) [undo setActionName:@"Sprite background priority"];
	
	[[c64 vic] spriteToggleBackgroundPriorityFlag:nr];
	[self refresh];
}

- (IBAction)vicSpriteInFrontAction:(id)sender
{	
	[self spriteToggleBackgroundPriorityFlag:[self currentSprite]];
}

- (void)spriteToggleSpriteSpriteCollisionFlag:(int)nr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteToggleSpriteSpriteCollisionFlag:nr];
	if (![undo isUndoing]) [undo setActionName:@"Detect sprite/sprite collisions"];
	
	[[c64 vic] spriteToggleSpriteSpriteCollisionFlag:nr];
	[self refresh];
}

- (IBAction)vicSpriteSpriteCollisionAction:(id)sender
{
	[self spriteToggleSpriteSpriteCollisionFlag:[self currentSprite]];
}

- (void)spriteToggleSpriteBackgroundCollisionFlag:(int)nr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteToggleSpriteBackgroundCollisionFlag:nr];
	if (![undo isUndoing]) [undo setActionName:@"Detect sprite/background collisions"];
	
	[[c64 vic] spriteToggleSpriteBackgroundCollisionFlag:nr];
	[self refresh];
}

- (IBAction)vicSpriteBackgroundCollisionAction:(id)sender
{
	NSLog(@"%d", [sprite1 intValue]);
	NSLog(@"%d", [sprite2 intValue]);
	[self spriteToggleSpriteBackgroundCollisionFlag:[self currentSprite]];
}

- (void)spriteSetX:(int)nr value:(int)v
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteSetX:nr value:[[c64 vic] spriteGetX:nr]];
	if (![undo isUndoing]) [undo setActionName:@"Sprite X"];
	
	[[c64 vic] spriteSetX:nr value:v];
	[self refresh];
}

- (IBAction)vicSpriteXAction:(id)sender
{
	[self spriteSetX:[self currentSprite] value:[sender intValue]];
}

- (void)spriteSetY:(int)nr value:(int)v
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteSetY:nr value:[[c64 vic] spriteGetY:nr]];
	if (![undo isUndoing]) [undo setActionName:@"Sprite Y"];
	
	[[c64 vic] spriteSetY:nr value:v];
	[self refresh];
}

- (IBAction)vicSpriteYAction:(id)sender
{
	[self spriteSetY:[self currentSprite] value:[sender intValue]];
}

- (void)spriteSetColor:(int)nr value:(int)v
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteSetColor:nr value:[[c64 vic] spriteGetColor:nr]];
	if (![undo isUndoing]) [undo setActionName:@"Sprite color"];
	
	[[c64 vic] spriteSetColor:nr value:v];
	[self refresh];
}

- (IBAction)vicSpriteColorAction:(id)sender
{
	[self spriteSetColor:[self currentSprite] value:[sender intValue]];
	[self refresh];
}

- (IBAction)vicRasterlineAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicRasterlineAction:[NSNumber numberWithInt:[[c64 vic] getRasterLine]]];
	if (![undo isUndoing]) [undo setActionName:@"Raster line"];
	
	[[c64 vic] setRasterLine:[sender intValue]];
	[self refresh];
}

- (IBAction)vicEnableRasterInterruptAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicEnableRasterInterruptAction:sender];
	if (![undo isUndoing]) [undo setActionName:@"Raster interrupt"];
	
	[[c64 vic] toggleRasterInterruptFlag];
	[self refresh];
}

- (IBAction)vicRasterInterruptAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicRasterInterruptAction:[NSNumber numberWithInt:[[c64 vic] getRasterInterruptLine]]];
	if (![undo isUndoing]) [undo setActionName:@"Raster interrupt line"];
	
	[[c64 vic] setRasterInterruptLine:[sender intValue]];
	[self refresh];
}

- (IBAction)vicEnableOpenGL:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicEnableOpenGL:self];
	if (![undo isUndoing]) [undo setActionName:@"OpenGL"];
	
	[screen setEnableOpenGL:![screen enableOpenGL]];
	[self refresh];
}

- (void)refreshVIC
{
	if (![VicVideoMode selectItemWithTag:[[c64 vic] getDisplayMode]])
		[VicVideoMode selectItemWithTag:1];
	if (![VicScreenGeometry selectItemWithTag:[[c64 vic] getScreenGeometry]])
		NSLog(@"Can't refresh screen geometry field");
	if (![VicMemoryBank selectItemWithTag:[[c64 vic] getMemoryBankAddr]])
		NSLog(@"Can't refresh memory bank field");
	if (![VicScreenMemory selectItemWithTag:[[c64 vic] getScreenMemoryAddr]])
		NSLog(@"Can't refresh screen memory field");
	if (![VicCharacterMemory selectItemWithTag:[[c64 vic] getCharacterMemoryAddr]])
		NSLog(@"Can't refresh screen memory field");
	[VicDX setIntValue:[[c64 vic] getHorizontalRasterScroll]];
	[VicDXStepper setIntValue:[[c64 vic] getHorizontalRasterScroll]];
	[VicDY setIntValue:[[c64 vic] getVerticalRasterScroll]];
	[VicDYStepper setIntValue:[[c64 vic] getVerticalRasterScroll]];
	[VicSpriteActive setIntValue:[[c64 vic] spriteGetVisibilityFlag:[self currentSprite]]];
	[VicSpriteMulticolor setIntValue:[[c64 vic] spriteGetMulticolorFlag:[self currentSprite]]];
	[VicSpriteStretchX setIntValue:[[c64 vic] spriteGetStretchXFlag:[self currentSprite]]];
	[VicSpriteStretchY setIntValue:[[c64 vic] spriteGetStretchYFlag:[self currentSprite]]];
	[VicSpriteInFront setIntValue:[[c64 vic] spriteGetBackgroundPriorityFlag:[self currentSprite]]];
	[VicSpriteSpriteCollision setIntValue:[[c64 vic] spriteGetSpriteSpriteCollisionFlag:[self currentSprite]]];
	[VicSpriteBackgroundCollision setIntValue:[[c64 vic] spriteGetSpriteBackgroundCollisionFlag:[self currentSprite]]];
	[VicSpriteX setIntValue:[[c64 vic] spriteGetX:[self currentSprite]]];
	[VicSpriteY setIntValue:[[c64 vic] spriteGetY:[self currentSprite]]];
	[VicSpriteColor setIntValue:[[c64 vic] spriteGetColor:[self currentSprite]]];
	[VicRasterline setIntValue:[[c64 vic] getRasterLine]];
	[VicEnableRasterInterrupt setIntValue:[[c64 vic] getRasterInterruptFlag]];
	[VicRasterInterrupt setIntValue:[[c64 vic] getRasterInterruptLine]];
}

- (int)currentSprite
{
	return selectedSprite; 
	
#if 0	
	if ([sprite0 intValue]) return 0;
	if ([sprite1 intValue]) return 1;
	if ([sprite2 intValue]) return 2;
	if ([sprite3 intValue]) return 3;
	if ([sprite4 intValue]) return 4;
	if ([sprite5 intValue]) return 5;
	if ([sprite6 intValue]) return 6;
	if ([sprite7 intValue]) return 7;	
	
	assert(false);
	return 0;
#endif
}


// --------------------------------------------------------------------------------
// Action methods (SID debug panel)
// --------------------------------------------------------------------------------

- (void)refreshSID
{
	// SID yet to come...
}


@end
