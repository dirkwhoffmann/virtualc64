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

#ifndef INC_MYCONTROLLER_CIA_PANEL
#define INC_MYCONTROLLER_CIA_PANEL

#import "MyController.h"

@interface MyController(CiaPanel)

- (IBAction)ciaSelectCiaAction:(id)sender;

- (IBAction)ciaDataPortAAction:(id)sender;
- (IBAction)ciaDataPortDirectionAAction:(id)sender;
- (IBAction)ciaTimerAAction:(id)sender;
- (IBAction)ciaLatchedTimerAAction:(id)sender;
- (IBAction)ciaRunningAAction:(id)sender;
- (IBAction)ciaOneShotAAction:(id)sender;
- (IBAction)ciaCountUnterflowsAAction:(id)sender;
- (IBAction)ciaSignalPendingAAction:(id)sender;
- (IBAction)ciaInterruptEnableAAction:(id)sender;

- (IBAction)ciaDataPortBAction:(id)sender;
- (IBAction)ciaDataPortDirectionBAction:(id)sender;
- (IBAction)ciaTimerBAction:(id)sender;
- (IBAction)ciaLatchedTimerBAction:(id)sender;
- (IBAction)ciaRunningBAction:(id)sender;
- (IBAction)ciaOneShotBAction:(id)sender;
- (IBAction)ciaCountUnterflowsBAction:(id)sender;
- (IBAction)ciaSignalPendingBAction:(id)sender;
- (IBAction)ciaInterruptEnableBAction:(id)sender;

- (IBAction)todHoursAction:(id)sender;
- (IBAction)todMinutesAction:(id)sender;
- (IBAction)todSecondsAction:(id)sender;
- (IBAction)todTenthAction:(id)sender;

- (IBAction)alarmHoursAction:(id)sender;
- (IBAction)alarmMinutesAction:(id)sender;
- (IBAction)alarmSecondsAction:(id)sender;
- (IBAction)alarmTenthAction:(id)sender;
- (IBAction)todInterruptEnabledAction:(id)sender;

- (int)currentCIA;

- (void)refreshCIA;

@end

#endif
