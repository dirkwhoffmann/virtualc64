/*
 * Author: Dirk W. Hoffmann, 2016
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

#import "MyMetalView.h"

struct Uniforms {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
    float4x4 projectionView;
    float alpha;
};

@interface MyMetalView(Helper)
{
}

// --------------------------------------------------------------------------------
//                                 Image handling
// --------------------------------------------------------------------------------

- (NSImage *)screenshot;
- (NSImage *)flipImage:(NSImage *)image;
- (NSImage *)expandImage:(NSImage *)image toSize:(NSSize)size;
- (id<MTLTexture>)textureFromImage:(NSImage *)image;
- (id<MTLTexture>)defaultBackgroundTexture;
- (NSImage *)desktopAsImage; 
+ (NSImage *)imageFromTexture:(id<MTLTexture>)texture x1:(float)_x1 y1:(float)_y1 x2:(float)_x2 y2:(float)_y2;


// --------------------------------------------------------------------------------
//                               Animation effects
// --------------------------------------------------------------------------------

#pragma mark Animation

//! Returns true if view is currently drawing animation effects
- (bool) animates;

- (float)eyeX;
- (void)setEyeX:(float)newX;
- (float)eyeY;
- (void)setEyeY:(float)newY;
- (float)eyeZ;
- (void)setEyeZ:(float)newZ;

//! Trigger animation effect. Zooms in from far away
- (void) zoom;

//! Trigger animation effect. Scroll in from below
- (void) scroll;

//! Trigger animation effect. Combination of zooming and rotating
- (void) fadeIn;

//! Trigger animation effect. Blend in from full transparency
- (void) blendIn;

//! Trigger animation effect. Rotate cube in one direction
- (void) rotate;

//! Trigger animation effect. Rotate cube in other direction
- (void) rotateBack;

//! Compute geometry parameters for next animation cycle
- (void) updateAngles;


@end
