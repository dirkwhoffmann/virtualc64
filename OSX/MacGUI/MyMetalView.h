/*
 * Author: Dirk W. Hoffmann, 2016 - 2017
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

#import "BypassUpscaler.h"
#import "EPXUpscaler.h"

#import "BypassFilter.h"
#import "BlurFilter.h"
#import "SaturationFilter.h"
#import "SepiaFilter.h"
#import "CrtFilter.h"

// Forward declaration
@class MyController;

// Size of C64 texture
const int C64_TEXTURE_WIDTH = 512;
const int C64_TEXTURE_HEIGHT= 512;
const int C64_TEXTURE_DEPTH = 4;

// Size of background texture
const int BG_TEXTURE_WIDTH = 1024;
const int BG_TEXTURE_HEIGHT= 512;
const int BG_TEXTURE_DEPTH = 4;

// Post-processing filters
enum TextureUpscalerType {
    TEX_UPSCALER_NONE = 1,
    TEX_UPSCALER_EPX,
};

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

/*! @brief    Fingerprint that uniquely identifies a key combination on the physical Mac keyboard
 *  @seealso  C64KeyFingerprint
 */
typedef unsigned long MacKeyFingerprint;


@interface MyMetalView : MTKView
{
    IBOutlet MyController *controller;
    IBOutlet C64Proxy* c64proxy;

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

    //
    // Textures
    //
    
    //! Background image behind the cube
    id <MTLTexture> bgTexture;
    
    //! Raw texture data provided by the emulator
    /*! Texture is updated in updateTexture which is called periodically in drawRect */
    id <MTLTexture> emulatorTexture;
    
    //! Upscaled emulator texture
    /*! In the first post-processing stage, the emulator texture is doubled in size.
     *  The user can choose between simply doubling pixels are applying a smoothing
     *   algorithm such as EPX */
    id <MTLTexture> upscaledTexture;
    
    //! Filteres emulator texture
    /*! In the second post-processing stage, the upscaled texture gets filtered.
     *  E.g., a CRT filter can be applied to mimic old CRT displays.
     */
    id <MTLTexture> filteredTexture;
    
    //! Final drawing target (GPU buffer)
    id <MTLTexture> framebufferTexture;
    
    //! Texture to hold the pixel depth information
    id <MTLTexture> depthTexture;

    // All currently supported texture upscalers
    ComputeKernel *bypassUpscaler;
    ComputeKernel *epxUpscaler;
        
    // All currently supported texture filters
    ComputeKernel *bypassFilter;
    ComputeKernel *smoothFilter;
    ComputeKernel *blurFilter;
    ComputeKernel *saturationFilter;
    ComputeKernel *sepiaFilter;
    ComputeKernel *grayscaleFilter;
    ComputeKernel *crtFilter;

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
 
    // Currently selected texture upscaler
    long videoUpscaler;
    
    // Currently selected texture filter
    long videoFilter;
    
    //! If true, no GPU drawing is performed (for performance profiling olny)
    bool enableMetal;
    
    //! Is set to true when fullscreen mode is entered (usually enables the 2D renderer)
    bool fullscreen;
    
    //! If true, the 3D renderer is also used in fullscreen mode
    bool fullscreenKeepAspectRatio;
    
    //! If false, the C64 screen is not drawn (background texture or black screen will be visible)
    bool drawC64texture;
    
}

#pragma mark Configuring

@property long videoUpscaler;
@property long videoFilter;
@property bool enableMetal;
@property bool fullscreen;
@property bool fullscreenKeepAspectRatio;
@property bool drawC64texture;

- (void)cleanup;

#pragma mark Drawing

//! Shrinks view vertically by the height of the status bar
- (void)shrink;

//! Expand view vertically by the height of the status bar
- (void)expand;

- (void)reshapeWithFrame:(CGRect)frame;
- (void)updateScreenGeometry;
- (void)buildMatrices3D;

@end
