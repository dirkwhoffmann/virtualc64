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

@implementation MyController(VicPanel) 

- (void)_vicVideoModeAction:(int)mode
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _vicVideoModeAction:[[c64 vic] displayMode]];
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
	[[undo prepareWithInvocationTarget:self] _vicScreenGeometryAction:[[c64 vic] screenGeometry]];
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
	[[undo prepareWithInvocationTarget:self] vicSetMemoryBank:[[c64 vic] memoryBankAddr]];
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
	[[undo prepareWithInvocationTarget:self] vicSetScreenMemory:[[c64 vic] screenMemoryAddr]];
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
	[[undo prepareWithInvocationTarget:self] vicSetCharacterMemory:[[c64 vic] characterMemoryAddr]];
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
	[[undo prepareWithInvocationTarget:self] vicDXAction:[NSNumber numberWithInt:[[c64 vic] horizontalRasterScroll]]];
	if (![undo isUndoing]) [undo setActionName:@"Horizontal raster scroll"];
	
	[[c64 vic] setHorizontalRasterScroll:[sender intValue]];
	[self refresh];
}

- (IBAction)vicDYAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicDYAction:[NSNumber numberWithInt:[[c64 vic] verticalRasterScroll]]];
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
	
	[[c64 vic] toggleSpriteVisibilityFlag:nr];
	[self refresh];
}

- (IBAction)vicSpriteSelectAction:(id)sender
{
	// selectedSprite = [sender selectedTag];
	
	NSLog(@"selectedSprite = %d", [self currentSprite]);
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
	
	[[c64 vic] toggleSpriteMulticolorFlag:nr];
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
	
	[[c64 vic] toggleSpriteStretchXFlag:nr];
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
	
	[[c64 vic] toggleSpriteStretchYFlag:nr];
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
	
	[[c64 vic] toggleSpriteBackgroundPriorityFlag:nr];
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
	
	[[c64 vic] toggleSpriteSpriteCollisionFlag:nr];
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
	
	[[c64 vic] toggleSpriteBackgroundCollisionFlag:nr];
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
	if (![VicVideoMode selectItemWithTag:[[c64 vic] displayMode]])
		[VicVideoMode selectItemWithTag:1];
	if (![VicScreenGeometry selectItemWithTag:[[c64 vic] screenGeometry]])
		NSLog(@"Can't refresh screen geometry field");
	if (![VicMemoryBank selectItemWithTag:[[c64 vic] memoryBankAddr]])
		NSLog(@"Can't refresh memory bank field");
	if (![VicScreenMemory selectItemWithTag:[[c64 vic] screenMemoryAddr]])
		NSLog(@"Can't refresh screen memory field");
	if (![VicCharacterMemory selectItemWithTag:[[c64 vic] characterMemoryAddr]])
		NSLog(@"Can't refresh screen memory field");
	[VicDX setIntValue:[[c64 vic] horizontalRasterScroll]];
	[VicDXStepper setIntValue:[[c64 vic] horizontalRasterScroll]];
	[VicDY setIntValue:[[c64 vic] verticalRasterScroll]];
	[VicDYStepper setIntValue:[[c64 vic] verticalRasterScroll]];
	[VicSpriteActive setIntValue:[[c64 vic] spriteVisibilityFlag:[self currentSprite]]];
	[VicSpriteMulticolor setIntValue:[[c64 vic] spriteMulticolorFlag:[self currentSprite]]];
	[VicSpriteStretchX setIntValue:[[c64 vic] spriteStretchXFlag:[self currentSprite]]];
	[VicSpriteStretchY setIntValue:[[c64 vic] spriteStretchYFlag:[self currentSprite]]];
	[VicSpriteInFront setIntValue:[[c64 vic] spriteBackgroundPriorityFlag:[self currentSprite]]];
	[VicSpriteSpriteCollision setIntValue:[[c64 vic] spriteSpriteCollisionFlag:[self currentSprite]]];
	[VicSpriteBackgroundCollision setIntValue:[[c64 vic] spriteBackgroundCollisionFlag:[self currentSprite]]];
	[VicSpriteX setIntValue:[[c64 vic] spriteGetX:[self currentSprite]]];
	[VicSpriteY setIntValue:[[c64 vic] spriteGetY:[self currentSprite]]];
	[VicSpriteColor setIntValue:[[c64 vic] spriteGetColor:[self currentSprite]]];
	[VicRasterline setIntValue:[[c64 vic] getRasterLine]];
	[VicEnableRasterInterrupt setIntValue:[[c64 vic] getRasterInterruptFlag]];
	[VicRasterInterrupt setIntValue:[[c64 vic] getRasterInterruptLine]];
}

- (int)currentSprite
{
	// return selectedSprite; 
	
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
}

@end
