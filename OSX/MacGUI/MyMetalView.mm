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

- (void) dealloc
{
    [self cleanup];
}

@end
