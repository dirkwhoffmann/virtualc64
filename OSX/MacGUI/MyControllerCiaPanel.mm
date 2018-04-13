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

// --------------------------------------------------------------------------------
// Action methods (CIA)
// --------------------------------------------------------------------------------

- (void)refreshCIA
{
    CIAInfo info;
    
    // CIA 1
    info = [[c64 cia1] getInfo];
    
    // CIA 1
    [cia1DataPortA setIntValue:info.portA.reg];
    [cia1DataPortDirectionA setIntValue:info.portA.dir];
    [cia1TimerA setIntValue:info.timerA.count];
    [cia1LatchedTimerA setIntValue:info.timerA.latch];
	[cia1RunningA setIntValue:info.timerA.running];
	[cia1OneShotA setIntValue:info.timerA.oneShot];
	[cia1SignalPendingA setIntValue:info.timerB.interruptData];
	[cia1InterruptEnableA setIntValue:info.timerA.interruptMask];
	
	[cia1DataPortB setIntValue:info.portB.reg];
	[cia1DataPortDirectionB setIntValue:info.portB.dir];
	[cia1TimerB setIntValue:info.timerB.count];
	[cia1LatchedTimerB setIntValue:info.timerB.latch];
	[cia1RunningB setIntValue:info.timerB.running];
	[cia1OneShotB setIntValue:info.timerB.oneShot];
	[cia1SignalPendingB setIntValue:info.timerB.interruptData];
	[cia1InterruptEnableB setIntValue:info.timerB.interruptMask];
	
    [tod1Hours setIntValue:info.tod.time.hours];
	[tod1Minutes setIntValue:info.tod.time.minutes];
	[tod1Seconds setIntValue:info.tod.time.seconds];
	[tod1Tenth setIntValue:info.tod.time.tenth];
	
	[alarm1Hours setIntValue:info.tod.alarm.hours];
	[alarm1Minutes setIntValue:info.tod.alarm.minutes];
	[alarm1Seconds setIntValue:info.tod.alarm.seconds];
	[alarm1Tenth setIntValue:info.tod.alarm.tenth];
	[tod1InterruptEnabled setIntValue:info.todInterruptMask];

    // CIA 2
    info = [[c64 cia2] getInfo];
	[cia2DataPortA setIntValue:info.portA.reg];
	[cia2DataPortDirectionA setIntValue:info.portA.dir];
	[cia2TimerA setIntValue:info.timerA.count];
	[cia2LatchedTimerA setIntValue:info.timerA.latch];
	[cia2RunningA setIntValue:info.timerA.running];
	[cia2OneShotA setIntValue:info.timerA.oneShot];
	[cia2SignalPendingA setIntValue:info.timerA.interruptData];
	[cia2InterruptEnableA setIntValue:info.timerA.interruptMask];
	
	[cia2DataPortB setIntValue:info.portB.reg];
	[cia2DataPortDirectionB setIntValue:info.portB.dir];
	[cia2TimerB setIntValue:info.timerB.count];
	[cia2LatchedTimerB setIntValue:info.timerB.latch];
	[cia2RunningB setIntValue:info.timerB.running];
	[cia2OneShotB setIntValue:info.timerB.oneShot];
	[cia2SignalPendingB setIntValue:info.timerB.interruptMask];
	[cia2InterruptEnableB setIntValue:info.timerB.interruptData];
	
	[tod2Hours setIntValue:info.tod.time.hours];
	[tod2Minutes setIntValue:info.tod.time.minutes];
	[tod2Seconds setIntValue:info.tod.time.seconds];
	[tod2Tenth setIntValue:info.tod.time.tenth];
	
	[alarm2Hours setIntValue:info.tod.alarm.hours];
	[alarm2Minutes setIntValue:info.tod.alarm.minutes];
	[alarm2Seconds setIntValue:info.tod.alarm.seconds];
	[alarm2Tenth setIntValue:info.tod.alarm.tenth];
	[tod2InterruptEnabled setIntValue:info.todInterruptMask];
}

@end
