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

@interface MyController(Toolbar)

- (void) setupToolbarIcons;

- (void) printDocument:(id) sender;

- (IBAction)joystick1Action:(id)sender;
- (IBAction)joystick2Action:(id)sender;
- (IBAction)switchJoysticksAction:(id)sender;

- (IBAction)fullscreenAction:(id)sender;

- (IBAction)debugOpenAction:(id)sender;
- (IBAction)debugCloseAction:(id)sender;
- (IBAction)debugAction:(id)sender;

- (IBAction)cheatboxOpenAction:(id)sender;
- (IBAction)cheatboxCloseAction:(id)sender;
- (IBAction)cheatboxAction:(id)sender;

// Experimental
- (IBAction)iPhoneAction:(id)sender;

@end
