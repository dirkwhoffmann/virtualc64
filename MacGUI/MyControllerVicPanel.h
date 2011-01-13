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

@interface MyController(VicPanel)

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

@end
