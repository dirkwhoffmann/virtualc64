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

#import "MyController.h"

@interface MyController(CiaPanel)

- (IBAction)cia1DataPortAAction:(id)sender;
- (IBAction)cia1DataPortDirectionAAction:(id)sender;
- (IBAction)cia1TimerAAction:(id)sender;
- (IBAction)cia1LatchedTimerAAction:(id)sender;
- (IBAction)cia1RunningAAction:(id)sender;
- (IBAction)cia1OneShotAAction:(id)sender;
- (IBAction)cia1CountUnterflowsAAction:(id)sender;
- (IBAction)cia1SignalPendingAAction:(id)sender;
- (IBAction)cia1InterruptEnableAAction:(id)sender;

- (IBAction)cia1DataPortBAction:(id)sender;
- (IBAction)cia1DataPortDirectionBAction:(id)sender;
- (IBAction)cia1TimerBAction:(id)sender;
- (IBAction)cia1LatchedTimerBAction:(id)sender;
- (IBAction)cia1RunningBAction:(id)sender;
- (IBAction)cia1OneShotBAction:(id)sender;
- (IBAction)cia1CountUnterflowsBAction:(id)sender;
- (IBAction)cia1SignalPendingBAction:(id)sender;
- (IBAction)cia1InterruptEnableBAction:(id)sender;

- (IBAction)tod1HoursAction:(id)sender;
- (IBAction)tod1MinutesAction:(id)sender;
- (IBAction)tod1SecondsAction:(id)sender;
- (IBAction)tod1TenthAction:(id)sender;
- (IBAction)tod1InterruptEnabledAction:(id)sender;

- (IBAction)alarm1HoursAction:(id)sender;
- (IBAction)alarm1MinutesAction:(id)sender;
- (IBAction)alarm1SecondsAction:(id)sender;
- (IBAction)alarm1TenthAction:(id)sender;

- (IBAction)cia2DataPortAAction:(id)sender;
- (IBAction)cia2DataPortDirectionAAction:(id)sender;
- (IBAction)cia2TimerAAction:(id)sender;
- (IBAction)cia2LatchedTimerAAction:(id)sender;
- (IBAction)cia2RunningAAction:(id)sender;
- (IBAction)cia2OneShotAAction:(id)sender;
- (IBAction)cia2CountUnterflowsAAction:(id)sender;
- (IBAction)cia2SignalPendingAAction:(id)sender;
- (IBAction)cia2InterruptEnableAAction:(id)sender;

- (IBAction)cia2DataPortBAction:(id)sender;
- (IBAction)cia2DataPortDirectionBAction:(id)sender;
- (IBAction)cia2TimerBAction:(id)sender;
- (IBAction)cia2LatchedTimerBAction:(id)sender;
- (IBAction)cia2RunningBAction:(id)sender;
- (IBAction)cia2OneShotBAction:(id)sender;
- (IBAction)cia2CountUnterflowsBAction:(id)sender;
- (IBAction)cia2SignalPendingBAction:(id)sender;
- (IBAction)cia2InterruptEnableBAction:(id)sender;

- (IBAction)tod2HoursAction:(id)sender;
- (IBAction)tod2MinutesAction:(id)sender;
- (IBAction)tod2SecondsAction:(id)sender;
- (IBAction)tod2TenthAction:(id)sender;
- (IBAction)tod2InterruptEnabledAction:(id)sender;

- (IBAction)alarm2HoursAction:(id)sender;
- (IBAction)alarm2MinutesAction:(id)sender;
- (IBAction)alarm2SecondsAction:(id)sender;
- (IBAction)alarm2TenthAction:(id)sender;

- (void)refreshCIA;

@end
