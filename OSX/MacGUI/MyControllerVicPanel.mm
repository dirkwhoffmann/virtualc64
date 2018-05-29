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

/*
#import "C64GUI.h"
#import "VirtualC64-Bridging-Header.h"

@implementation MyController(VicPanel) 

- (void)_vicVideoModeAction:(long)mode
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _vicVideoModeAction:[[c64 vic] displayMode]];
	if (![undo isUndoing]) [undo setActionName:@"Display mode"];
	
	[[c64 vic] setDisplayMode:mode];
	[self refreshVIC];
}

- (IBAction)vicVideoModeAction:(id)sender
{
	[self _vicVideoModeAction:[[sender selectedItem] tag]];
}

- (void)_vicScreenGeometryAction:(long)mode
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] _vicScreenGeometryAction:[[c64 vic] screenGeometry]];
	if (![undo isUndoing]) [undo setActionName:@"Screen geometry"];
	
	[[c64 vic] setScreenGeometry:mode];
	[self refreshVIC];
}

- (IBAction)vicScreenGeometryAction:(id)sender
{
	[self _vicScreenGeometryAction:[[sender selectedItem] tag]];
}

- (void)vicSetMemoryBank:(uint16_t)addr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicSetMemoryBank:[[c64 vic] memoryBankAddr]];
	if (![undo isUndoing]) [undo setActionName:@"Memory bank"];
	
	[[c64 vic] setMemoryBankAddr:addr];
	[self refreshVIC];
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
	[self refreshVIC];
}

- (IBAction)vicScreenMemoryAction:(id)sender
{
    [self vicSetScreenMemory:(uint16_t)[[sender selectedItem] tag]];
}

- (void)vicSetCharacterMemory:(uint16_t)addr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicSetCharacterMemory:[[c64 vic] characterMemoryAddr]];
	if (![undo isUndoing]) [undo setActionName:@"Character memory"];
	
	[[c64 vic] setCharacterMemoryAddr:addr];
	[self refreshVIC];
}

- (IBAction)vicCharacterMemoryAction:(id)sender
{
    [self vicSetCharacterMemory:(uint16_t)[[sender selectedItem] tag]];
    [self refreshVIC];
}

- (IBAction)vicDXAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
    [[undo prepareWithInvocationTarget:self] vicDXAction:@((int)[[c64 vic] horizontalRasterScroll])];
	if (![undo isUndoing]) [undo setActionName:@"Horizontal raster scroll"];
	
	[[c64 vic] setHorizontalRasterScroll:[sender intValue]];
	[self refreshVIC];
}

- (IBAction)vicDYAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicDYAction:@((int)[[c64 vic] verticalRasterScroll])];
	if (![undo isUndoing]) [undo setActionName:@"Vertical raster scroll"];
	
	[[c64 vic] setVerticalRasterScroll:[sender intValue]];
	[self refreshVIC];
}

- (IBAction)vicDXStepperAction:(id)sender
{
	[self vicDXAction:sender];
    [self refreshVIC];
}

- (IBAction)vicDYStepperAction:(id)sender
{
	[self vicDYAction:sender];
    [self refreshVIC];
}

// SPRITES
- (void)spriteToggleActiveFlag:(NSInteger)nr
{
    NSUndoManager *undo = [self undoManager];
    [[undo prepareWithInvocationTarget:self] spriteToggleActiveFlag:nr];
    if (![undo isUndoing]) [undo setActionName:@"Sprite active"];
    
    [[c64 vic] toggleSpriteVisibilityFlag:nr];
    [self refreshVIC];
}

- (IBAction)vicSpriteActiveAction:(id)sender
{
    NSLog(@"vicSpriteActiveAction:%ld",(long)[sender tag]);
    
    [self spriteToggleActiveFlag:[sender tag]];
    [self refreshVIC];
}

- (void)spriteSetX:(NSInteger)nr value:(int)v
{
    NSUndoManager *undo = [self undoManager];
    [[undo prepareWithInvocationTarget:self] spriteSetX:nr value:[[c64 vic] spriteX:nr]];
    if (![undo isUndoing]) [undo setActionName:@"X coordinate"];
    
    [[c64 vic] setSpriteX:nr value:v];
    [self refreshVIC];
}

- (IBAction)vicSpriteXAction:(id)sender
{
    NSLog(@"vicSpriteXAction:%ld (%ld)",(long)[sender tag], (long)[sender intValue]);

    [self spriteSetX:[sender tag] value:[sender intValue]];
    [self refreshVIC];
}

- (void)spriteSetY:(NSInteger)nr value:(int)v
{
    NSUndoManager *undo = [self undoManager];
    [[undo prepareWithInvocationTarget:self] spriteSetY:nr value:[[c64 vic] spriteY:nr]];
    if (![undo isUndoing]) [undo setActionName:@"Y coordinate"];
    
    [[c64 vic] setSpriteY:nr value:v];
    [self refreshVIC];
}

- (IBAction)vicSpriteYAction:(id)sender
{
    NSLog(@"vicSpriteYAction:%ld (%ld)",(long)[sender tag], (long)[sender intValue]);

    [self spriteSetY:[sender tag] value:[sender intValue]];
    [self refreshVIC];
}

- (void)spriteSetColor:(NSInteger)nr value:(int)v
{
    NSUndoManager *undo = [self undoManager];
    [[undo prepareWithInvocationTarget:self] spriteSetColor:nr value:[[c64 vic] spriteColor:nr]];
    if (![undo isUndoing]) [undo setActionName:@"Sprite color"];
    
    [[c64 vic] setSpriteColor:nr value:v];
    [self refreshVIC];
}

- (IBAction)vicSpriteColorAction:(id)sender
{
    int color = [[c64 vic] spriteColor:[sender tag]];
    
    NSLog(@"vicSpriteColorAction:%ld",(long)[sender tag]);

    [self spriteSetColor:[sender tag] value:((color + 1) % 16)];
    [self refreshVIC];
}

- (void)spriteToggleMulticolorFlag:(NSInteger)nr
{
    NSUndoManager *undo = [self undoManager];
    [[undo prepareWithInvocationTarget:self] spriteToggleMulticolorFlag:nr];
    if (![undo isUndoing]) [undo setActionName:@"Multicolor flag"];
    
    [[c64 vic] toggleSpriteMulticolorFlag:nr];
    [self refreshVIC];
}

- (IBAction)vicSpriteMulticolorAction:(id)sender
{
    [self spriteToggleMulticolorFlag:[sender tag]];
    [self refreshVIC];
}

- (void)spriteToggleStretchXFlag:(NSInteger)nr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteToggleStretchXFlag:nr];
	if (![undo isUndoing]) [undo setActionName:@"X expansion"];
	
	[[c64 vic] toggleSpriteStretchXFlag:nr];
	[self refreshVIC];
}

- (IBAction)vicSpriteStretchXAction:(id)sender
{
	[self spriteToggleStretchXFlag:[sender tag]];
    [self refreshVIC];
}

- (void)spriteToggleStretchYFlag:(NSInteger)nr
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] spriteToggleStretchYFlag:nr];
	if (![undo isUndoing]) [undo setActionName:@"Y expansion"];
	
	[[c64 vic] toggleSpriteStretchYFlag:nr];
	[self refreshVIC];
}

- (IBAction)vicSpriteStretchYAction:(id)sender
{
	[self spriteToggleStretchYFlag:[sender tag]];
    [self refreshVIC];
}

- (IBAction)vicSpriteSpriteCollisionAction:(id)sender
{
    NSUndoManager *undo = [self undoManager];
    [[undo prepareWithInvocationTarget:self] vicSpriteSpriteCollisionAction:sender];
    if (![undo isUndoing]) [undo setActionName:@"S/S collision flag"];
    
    [[c64 vic] toggleSpriteSpriteCollisionFlag];
    [self refreshVIC];

}

- (IBAction)vicSpriteBackgroundCollisionAction:(id)sender
{
    NSUndoManager *undo = [self undoManager];
    [[undo prepareWithInvocationTarget:self] vicSpriteBackgroundCollisionAction:sender];
    if (![undo isUndoing]) [undo setActionName:@"S/B collision flag"];

    [[c64 vic] toggleSpriteBackgroundCollisionFlag];
    [self refreshVIC];
}

- (IBAction)vicRasterlineAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicRasterlineAction:@((int)[[c64 vic] rasterline])];
	if (![undo isUndoing]) [undo setActionName:@"Raster line"];
	
	[[c64 vic] setRasterline:[sender intValue]];
	[self refreshVIC];
}

- (IBAction)vicEnableRasterInterruptAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicEnableRasterInterruptAction:sender];
	if (![undo isUndoing]) [undo setActionName:@"Raster interrupt"];
	
	[[c64 vic] toggleRasterInterruptFlag];
	[self refreshVIC];
}

- (IBAction)vicRasterInterruptAction:(id)sender
{
	NSUndoManager *undo = [self undoManager];
	[[undo prepareWithInvocationTarget:self] vicRasterInterruptAction:@((int)[[c64 vic] rasterInterruptLine])];
	if (![undo isUndoing]) [undo setActionName:@"Raster interrupt line"];
	
	[[c64 vic] setRasterInterruptLine:[sender intValue]];
	[self refreshVIC];
}

- (void)refreshVIC
{
    VICProxy *vic = [c64 vic];

    [VicVideoMode selectItemWithTag:[vic displayMode]];
    [VicScreenGeometry selectItemWithTag:[vic screenGeometry]];
    [VicMemoryBank selectItemWithTag:[vic memoryBankAddr]];
    [VicScreenMemory selectItemWithTag:[vic screenMemoryAddr]];
    [VicCharacterMemory selectItemWithTag:[vic characterMemoryAddr]];
    
	[VicDX setIntValue:[vic horizontalRasterScroll]];
	[VicDXStepper setIntValue:[vic horizontalRasterScroll]];
	[VicDY setIntValue:[vic verticalRasterScroll]];
	[VicDYStepper setIntValue:[vic verticalRasterScroll]];

    [VicSpriteActive1 setIntValue:[vic spriteVisibilityFlag:0]];
    [VicSpriteX1 setIntValue:[vic spriteX:0]];
    [VicSpriteY1 setIntValue:[vic spriteY:0]];
    [[VicSpriteCol1 cell] setBackgroundColor:[vic color:[vic spriteColor:0]]];
    [VicSpriteMulticolor1 setIntValue:[vic spriteMulticolorFlag:0]];
	[VicSpriteStretchX1 setIntValue:[vic spriteStretchXFlag:0]];
	[VicSpriteStretchY1 setIntValue:[vic spriteStretchYFlag:0]];

    [VicSpriteActive2 setIntValue:[vic spriteVisibilityFlag:1]];
    [VicSpriteX2 setIntValue:[vic spriteX:1]];
    [VicSpriteY2 setIntValue:[vic spriteY:1]];
    [[VicSpriteCol2 cell] setBackgroundColor:[vic color:[vic spriteColor:1]]];
    [VicSpriteMulticolor2 setIntValue:[vic spriteMulticolorFlag:1]];
    [VicSpriteStretchX2 setIntValue:[vic spriteStretchXFlag:1]];
    [VicSpriteStretchY2 setIntValue:[vic spriteStretchYFlag:1]];

    [VicSpriteActive3 setIntValue:[vic spriteVisibilityFlag:2]];
    [VicSpriteX3 setIntValue:[vic spriteX:2]];
    [VicSpriteY3 setIntValue:[vic spriteY:2]];
    [[VicSpriteCol3 cell] setBackgroundColor:[vic color:[vic spriteColor:2]]];
    [VicSpriteMulticolor3 setIntValue:[vic spriteMulticolorFlag:2]];
    [VicSpriteStretchX3 setIntValue:[vic spriteStretchXFlag:2]];
    [VicSpriteStretchY3 setIntValue:[vic spriteStretchYFlag:2]];

    [VicSpriteActive4 setIntValue:[vic spriteVisibilityFlag:3]];
    [VicSpriteX4 setIntValue:[vic spriteX:3]];
    [VicSpriteY4 setIntValue:[vic spriteY:3]];
    [[VicSpriteCol4 cell] setBackgroundColor:[vic color:[vic spriteColor:3]]];
    [VicSpriteMulticolor4 setIntValue:[vic spriteMulticolorFlag:3]];
    [VicSpriteStretchX4 setIntValue:[vic spriteStretchXFlag:3]];
    [VicSpriteStretchY4 setIntValue:[vic spriteStretchYFlag:3]];

    [VicSpriteActive5 setIntValue:[vic spriteVisibilityFlag:4]];
    [VicSpriteX5 setIntValue:[vic spriteX:4]];
    [VicSpriteY5 setIntValue:[vic spriteY:4]];
    [[VicSpriteCol5 cell] setBackgroundColor:[vic color:[vic spriteColor:4]]];
    [VicSpriteMulticolor5 setIntValue:[vic spriteMulticolorFlag:4]];
    [VicSpriteStretchX5 setIntValue:[vic spriteStretchXFlag:4]];
    [VicSpriteStretchY5 setIntValue:[vic spriteStretchYFlag:4]];

    [VicSpriteActive6 setIntValue:[vic spriteVisibilityFlag:5]];
    [VicSpriteX6 setIntValue:[vic spriteX:5]];
    [VicSpriteY6 setIntValue:[vic spriteY:5]];
    [[VicSpriteCol6 cell] setBackgroundColor:[vic color:[vic spriteColor:5]]];
    [VicSpriteMulticolor6 setIntValue:[vic spriteMulticolorFlag:5]];
    [VicSpriteStretchX6 setIntValue:[vic spriteStretchXFlag:5]];
    [VicSpriteStretchY6 setIntValue:[vic spriteStretchYFlag:5]];

    [VicSpriteActive7 setIntValue:[vic spriteVisibilityFlag:6]];
    [VicSpriteX7 setIntValue:[vic spriteX:6]];
    [VicSpriteY7 setIntValue:[vic spriteY:6]];
    [[VicSpriteCol7 cell] setBackgroundColor:[vic color:[vic spriteColor:6]]];
    [VicSpriteMulticolor7 setIntValue:[vic spriteMulticolorFlag:6]];
    [VicSpriteStretchX7 setIntValue:[vic spriteStretchXFlag:6]];
    [VicSpriteStretchY7 setIntValue:[vic spriteStretchYFlag:6]];

    [VicSpriteActive8 setIntValue:[vic spriteVisibilityFlag:7]];
    [VicSpriteX8 setIntValue:[vic spriteX:7]];
    [VicSpriteY8 setIntValue:[vic spriteY:7]];
    [[VicSpriteCol8 cell] setBackgroundColor:[vic color:[vic spriteColor:7]]];
    [VicSpriteMulticolor8 setIntValue:[vic spriteMulticolorFlag:7]];
    [VicSpriteStretchX8 setIntValue:[vic spriteStretchXFlag:7]];
    [VicSpriteStretchY8 setIntValue:[vic spriteStretchYFlag:7]];

    [VicSpriteSpriteCollision setIntValue:[vic spriteSpriteCollisionFlag]];
	[VicSpriteBackgroundCollision setIntValue:[vic spriteBackgroundCollisionFlag]];

    [VicRasterline setIntValue:[[c64 vic] rasterline]];
	[VicEnableRasterInterrupt setIntValue:[[c64 vic] rasterInterruptFlag]];
	[VicRasterInterrupt setIntValue:[[c64 vic] rasterInterruptLine]];
}

@end

*/
