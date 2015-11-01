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

// Synchronization lock
extern NSRecursiveLock *lock;

// Size of C64 texture
const int C64_TEXTURE_WIDTH = 512;
const int C64_TEXTURE_HEIGHT= 512;
const int C64_TEXTURE_DEPTH = 4;

// Size of background texture
const int BG_TEXTURE_WIDTH = 1024;
const int BG_TEXTURE_HEIGHT= 512;
const int BG_TEXTURE_DEPTH = 4;

// Post-processing filters
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

    // Synchronization semaphore
    dispatch_semaphore_t _inflightSemaphore;
    
    // Metal objects
    id <MTLDevice> device;
    id <MTLLibrary> library;
    id <MTLCommandQueue> queue;
    id <MTLRenderPipelineState> pipeline;
    id <MTLDepthStencilState> depthState;

    // Metal layer
    CAMetalLayer *metalLayer;
    CGFloat layerWidth;
    CGFloat layerHeight;
    bool layerIsDirty; // Indicated if we need to rebuild size specific entities

    // Buffers
    id <MTLBuffer> positionBuffer;
    id <MTLBuffer> uniformBuffer2D;
    id <MTLBuffer> uniformBuffer3D;
    id <MTLBuffer> uniformBufferBg;

    // Textures
    id <MTLTexture> bgTexture; // background image
    id <MTLTexture> textureFromEmulator; // plain C64 screen (as provided by the emulator)
    id <MTLTexture> filteredTexture; // post-processes C64 screen
    id <MTLTexture> framebufferTexture; // drawing target (GPU buffer)
    id <MTLTexture> depthTexture; // depth buffering

    // Post-processing filters
    TextureFilter *bypassFilter;
    TextureFilter *smoothFilter;
    TextureFilter *blurFilter;
    TextureFilter *saturationFilter;
    TextureFilter *sepiaFilter;
    TextureFilter *grayscaleFilter;
    TextureFilter *crtFilter;

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
    
    // Currently selected filters
    unsigned videoFilter;
    
    //! If true, no GPU drawing is performed (for performance profiling olny)
    bool enableMetal;
    
    //! Is set to true when fullscreen mode is entered (usually enables the 2D renderer)
    bool fullscreen;
    
    //! If true, the 3D renderer is also used in fullscreen mode
    bool fullscreenKeepAspectRatio;
    
    //! If true, the C64 canvas covers the whole window area (used to hide the status bar)
    bool drawInEntireWindow;

    //! If false, the C64 screen is not drawn (background texture or black screen will be visible)
    bool drawC64texture;
    
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

@property unsigned videoFilter;
@property bool enableMetal;
@property bool fullscreen;
@property bool fullscreenKeepAspectRatio;
@property bool drawInEntireWindow;
@property bool drawC64texture;

- (void)cleanup;

#pragma mark Drawing

- (void)reshapeWithFrame:(CGRect)frame;
- (void)updateScreenGeometry;
- (void)buildMatrices3D;

@end