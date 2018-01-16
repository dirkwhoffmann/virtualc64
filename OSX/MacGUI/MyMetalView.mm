/*
 * Author: Dirk W. Hoffmann, 2015
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
#import "MyMetalDefs.h"
#import "VirtualC64-Swift.h"

@implementation MyMetalView {
}

// -----------------------------------------------------------------------------------------------
//                                           Properties
// -----------------------------------------------------------------------------------------------

@synthesize controller;
@synthesize c64proxy;

@synthesize semaphore;

@synthesize library;
@synthesize queue;
@synthesize pipeline;
@synthesize depthState;
@synthesize commandBuffer;
@synthesize commandEncoder;
@synthesize drawable;

@synthesize bgTexture;
@synthesize emulatorTexture;
@synthesize upscaledTexture;
@synthesize filteredTexture;
@synthesize depthTexture;

@synthesize positionBuffer;
@synthesize uniformBuffer2D;
@synthesize uniformBuffer3D;
@synthesize uniformBufferBg;

@synthesize metalLayer;
@synthesize layerWidth;
@synthesize layerHeight;
@synthesize layerIsDirty;

@synthesize currentXAngle;
@synthesize targetXAngle;
@synthesize deltaXAngle;
@synthesize currentYAngle;
@synthesize targetYAngle;
@synthesize deltaYAngle;
@synthesize currentZAngle;
@synthesize targetZAngle;
@synthesize deltaZAngle;
@synthesize currentEyeX;
@synthesize targetEyeX;
@synthesize deltaEyeX;
@synthesize currentEyeY;
@synthesize targetEyeY;
@synthesize deltaEyeY;
@synthesize currentEyeZ;
@synthesize targetEyeZ;
@synthesize deltaEyeZ;
@synthesize currentAlpha;
@synthesize targetAlpha;
@synthesize deltaAlpha;

@synthesize textureXStart;
@synthesize textureYStart;
@synthesize textureXEnd;
@synthesize textureYEnd;
@synthesize videoUpscaler;
@synthesize videoFilter;
@synthesize enableMetal;
@synthesize fullscreen;
@synthesize fullscreenKeepAspectRatio;
@synthesize drawC64texture;

// -----------------------------------------------------------------------------------------------
//                                         Initialization
// -----------------------------------------------------------------------------------------------

- (id)initWithCoder:(NSCoder *)c
{
    NSLog(@"MyMetalView::initWithCoder");
    
    if ((self = [super initWithCoder:c]) == nil) {
        NSLog(@"Error: Can't initiaize MetalView");
    }
    return self;
}

-(void)awakeFromNib
{
    NSLog(@"MyMetalView::awakeFromNib");
    
    // Create semaphore
    semaphore = dispatch_semaphore_create(1);
    
    // Set initial scene position and drawing properties
    [self initAnimation];
    
    // Properties
    enableMetal = false;
    fullscreen = false;
    fullscreenKeepAspectRatio = true;
    drawC64texture = false;
    
    // Metal
    layerWidth = 0;
    layerHeight = 0;
    layerIsDirty = YES;
    
    positionBuffer = nil;
    uniformBuffer2D = nil;
    uniformBuffer3D = nil;
    uniformBufferBg = nil;
    
    bgTexture = nil;
    emulatorTexture = nil;
    upscaledTexture = nil;
    filteredTexture = nil;
    depthTexture = nil;
    
    // Check if machine is capable to run the Metal graphics interface
    if (!MTLCreateSystemDefaultDevice()) {
        NSAlert *alert = [[NSAlert alloc] init];
        [alert setIcon:[NSImage imageNamed:@"metal.png"]];
        [alert setMessageText:@"No suitable GPU hardware found"];
        [alert setInformativeText:@"VirtualC64 can only run on machines supporting the Metal graphics technology (2012 models and above)."];
        [alert addButtonWithTitle:@"Ok"];
        [alert runModal];
        [NSApp terminate:self];
    }
        
    // Register for drag and drop
    [self setupDragAndDrop];
}

- (void) dealloc
{
    [self cleanup];
}

@end
