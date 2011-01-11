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

#ifndef INC_MYCONTROLLER_DEBUG_PANEL
#define INC_MYCONTROLLER_DEBUG_PANEL

#import "MyController.h"

@interface MyController(DebugPanel)

// Debug panel (CPU)
- (IBAction)aAction:(id)sender;
- (IBAction)xAction:(id)sender;
- (IBAction)yAction:(id)sender;
- (IBAction)mhzAction:(id)sender;
- (IBAction)pcAction:(id)sender;
- (IBAction)spAction:(id)sender;
- (IBAction)NAction:(id)sender;
- (IBAction)ZAction:(id)sender;
- (IBAction)CAction:(id)sender;
- (IBAction)IAction:(id)sender;
- (IBAction)BAction:(id)sender;
- (IBAction)DAction:(id)sender;
- (IBAction)VAction:(id)sender;
- (IBAction)setHardBreakpointAction:(id)sender;
- (void)doubleClickInCpuTable:(id)sender;
- (void)doubleClickInMemTable:(id)sender;

- (void)refreshCPU;

// Debug panel (Memory)
- (IBAction)searchAction:(id)sender;
- (IBAction)setMemSourceToRAM:(id)sender;
- (IBAction)setMemSourceToROM:(id)sender;
- (IBAction)setMemSourceToIO:(id)sender;

- (void)changeMemValue:(uint16_t)addr value:(int16_t)v memtype:(Memory::MemoryType)t;
- (Memory::MemoryType)currentMemSource;
- (void)setMemObjectValue:(id)anObject forTableColumn:(NSTableColumn *)aTableColumn row:(int)rowIndex;

- (void)refreshMemory;

// Debug pabel (CIA)
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

// Debug panel (VIC)
- (void)_vicVideoModeAction:(int)mode;
- (IBAction)vicVideoModeAction:(id)sender;
- (IBAction)_vicScreenGeometryAction:(int)mode;
- (IBAction)vicScreenGeometryAction:(id)sender;
- (IBAction)vicMemoryBankAction:(id)sender;
- (IBAction)vicScreenMemoryAction:(id)sender;
- (IBAction)vicCharacterMemoryAction:(id)sender;
- (IBAction)vicDXAction:(id)sender;
- (IBAction)vicDYAction:(id)sender;
- (IBAction)vicDXStepperAction:(id)sender;
- (IBAction)vicDYStepperAction:(id)sender;
- (IBAction)vicSpriteSelectAction:(id)sender;
- (IBAction)vicSpriteActiveAction:(id)sender;
- (IBAction)vicSpriteMulticolorAction:(id)sender;
- (IBAction)vicSpriteStretchXAction:(id)sender;
- (IBAction)vicSpriteStretchYAction:(id)sender;
- (IBAction)vicSpriteInFrontAction:(id)sender;
- (IBAction)vicSpriteSpriteCollisionAction:(id)sender;
- (IBAction)vicSpriteBackgroundCollisionAction:(id)sender;
- (IBAction)vicSpriteXAction:(id)sender;
- (IBAction)vicSpriteYAction:(id)sender;
- (IBAction)vicSpriteColorAction:(id)sender;
- (IBAction)vicRasterlineAction:(id)sender;
- (IBAction)vicEnableRasterInterruptAction:(id)sender;
- (IBAction)vicRasterInterruptAction:(id)sender;
- (IBAction)vicEnableOpenGL:(id)sender;

- (int)currentSprite;

- (void)refreshVIC;

// Debug panel (SID) 
// NOT IMPLEMENTED YED
- (void)refreshSID;

@end

#endif
