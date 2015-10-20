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
// 3. Repair "MetalView::screenshot"
// 4. Blur is too heavy
// 5. What kind of semaphore should we use?


#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>
#import <QuickTime/QuickTime.h>
#import "BypassFilter.h"
#import "BlurFilter.h"
#import "SaturationFilter.h"
#import "SepiaFilter.h"
#import "CrtFilter.h"

#import <simd/simd.h>
using namespace simd;

// Forward declaration
@class MyController;

// Graphics constants
const int C64_TEXTURE_WIDTH = 512;
const int C64_TEXTURE_HEIGHT= 512;
const int C64_TEXTURE_DEPTH = 4;

const int BG_TEXTURE_WIDTH = 1024;
const int BG_TEXTURE_HEIGHT= 512;
const int BG_TEXTURE_DEPTH = 4;

enum TextureFilterType {
    TEX_FILTER_NONE = 1,
    TEX_FILTER_SMOOTH,
    TEX_FILTER_BLUR,
    TEX_FILTER_SATURATION,
    TEX_FILTER_GRAYSCALE,
    TEX_FILTER_SEPIA,
    TEX_FILTER_CRT,
};

@interface MyMetalView : MTKView
{
    IBOutlet MyController *controller;
    IBOutlet C64Proxy* c64proxy;
    
    C64 *c64; // DEPRECATED. GET RID OF THIS VARIABLE AND RENAME c64proxy to c64

    // Synchronization lock
    NSRecursiveLock *lock;
    
    // Renderer globals
    id <MTLDevice> _device;
    id <MTLLibrary> _library;
    id <MTLCommandQueue> _commandQueue;
    
    // Textures
    id <MTLTexture> _bgTexture; // Background image
    id <MTLTexture> _texture; // C64 screen
    id <MTLTexture> _filteredTexture; // post-processes C64 screen
    id <MTLTexture> _framebufferTexture;
    id <MTLTexture> _depthTexture; // z buffer
    id <MTLSamplerState> _sampler;
    id <MTLSamplerState> _sampler2;

    
    // Animation parameters
    float currentXAngle, targetXAngle, deltaXAngle;
    float currentYAngle, targetYAngle, deltaYAngle;
    float currentZAngle, targetZAngle, deltaZAngle;
    float currentEyeX, targetEyeX, deltaEyeX;
    float currentEyeY, targetEyeY, deltaEyeY;
    float currentEyeZ, targetEyeZ, deltaEyeZ;
    float currentAlpha, targetAlpha, deltaAlpha;

    // Texture cut-out (first and last visible texture coordinates)
    float textureXStart;
    float textureXEnd;
    float textureYStart;
    float textureYEnd;

    // Prebuild filters
    TextureFilter *bypassFilter;
    TextureFilter *smoothFilter;
    TextureFilter *blurFilter;
    TextureFilter *saturationFilter;
    TextureFilter *sepiaFilter;
    TextureFilter *grayscaleFilter;
    TextureFilter *crtFilter;
    
    // Currently selected filters
    unsigned currentFilter;
    
    //! If true, the C64 canvas covers the whole window area (used to hide the status bar)
    bool drawInEntireWindow;

    //! If false, 3D drawing is switched off (2D drawing is used in fullscreen mode, only)
    bool drawIn3D;
    
    //! If false, the C64 screen is not drawn (background texture or black screen will be visible)
    bool drawC64texture;
    
    //! If false, the background is not drawn
    bool drawBackground;
    
    //! If false, only the front facing part of the texture cube is drawn
    bool drawEntireCube;
    
#pragma mark Keyboard and joystick emulation
    
    //! Stores which keys are currently pressed
    /*! Array index is a Mac keycode and the stored value the pressed key on the c64 keyboard */
    unsigned int pressedKeys[256];

    /*! Stores a fingerprint of each joystick emulation key.
     *  The user can choose from 2 maps */
    int joyKeycode[2][5];
    
    /*!  Stores a printabel character for each joystick emulation key.
     *   These values are only used in the properties dialog for pretty printing the keycodes */
    char joyChar[2][5];
    
}

#pragma mark Configuring

@property bool enableMetal;

- (bool)drawInEntireWindow;
- (void)setDrawInEntireWindow:(bool)b;

- (bool)drawIn3D;
- (void)setDrawIn3D:(bool)b;

- (bool)drawC64texture;
- (void)setDrawC64texture:(bool)b;

- (bool)drawEntireCube;
- (void)setDrawEntireCube:(bool)b;

- (unsigned)videoFilter;
- (void)setVideoFilter:(unsigned)filter;

- (void)cleanup;

#pragma mark Drawing

- (void)setupDisplayLink;
- (void)updateScreenGeometry;
-(CVReturn)getFrameForTime:(const CVTimeStamp *)timeStamp flagsOut:(CVOptionFlags *)flagsOut;



@end