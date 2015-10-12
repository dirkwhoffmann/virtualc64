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

// Next steps:
// 1. Implement rotation (via transformation matrices)
// 2. Make it a cube


#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

#import <simd/simd.h>
using namespace simd;

// Forward declaration
@class MyController;

// Graphics constants
const int C64_TEXTURE_WIDTH = 512;
const int C64_TEXTURE_HEIGHT= 512;
const int C64_TEXTURE_DEPTH = 4;


@interface MyMetalView : MTKView
{
    IBOutlet MyController *controller;
    IBOutlet C64Proxy* c64proxy;
    
    C64 *c64; // DEPRECATED. GET RID OF THIS VARIABLE AND RENAME c64proxy to c64

    // Synchronization lock
    NSRecursiveLock *lock;
    
    // Animation parameters
    float currentXAngle, targetXAngle, deltaXAngle;
    float currentYAngle, targetYAngle, deltaYAngle;
    float currentZAngle, targetZAngle, deltaZAngle;
    float currentEyeX, targetEyeX, deltaEyeX;
    float currentEyeY, targetEyeY, deltaEyeY;
    float currentEyeZ, targetEyeZ, deltaEyeZ;

    // Texture cut-out (first and last visible texture coordinates)
    float textureXStart;
    float textureXEnd;
    float textureYStart;
    float textureYEnd;
    
    //! If true, the OpenGL view covers the whole window area (used to hide the status bar)
    bool drawInEntireWindow;

    //! If false, 3D drawing is switched off (2D drawing is used in fullscreen mode, only)
    bool drawIn3D;
    
    //! If false, the C64 screen is not drawn (background texture or black screen will be visible)
    bool drawC64texture;
    
    //! If false, the background is not drawn
    bool drawBackground;
    
    //! If false, only the front facing part of the texture cube is drawn
    bool drawEntireCube;
}

#pragma mark Configuring

- (bool)drawInEntireWindow;
- (void)setDrawInEntireWindow:(bool)b;

- (bool)drawIn3D;
- (void)setDrawIn3D:(bool)b;

- (bool)drawC64texture;
- (void)setDrawC64texture:(bool)b;

- (bool)drawEntireCube;
- (void)setDrawEntireCube:(bool)b;


#pragma mark Drawing

- (void)updateScreenGeometry;

-(CVReturn)getFrameForTime:(const CVTimeStamp *)timeStamp flagsOut:(CVOptionFlags *)flagsOut;

@end