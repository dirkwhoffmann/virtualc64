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

// Forward declaration
@class MyController;

// Size of C64 texture
const int C64_TEXTURE_WIDTH = 512;
const int C64_TEXTURE_HEIGHT= 512;
const int C64_TEXTURE_DEPTH = 4;

// Post-processing filters
typedef enum {
    TEX_UPSCALER_NONE = 1,
    TEX_UPSCALER_EPX,
    TEX_UPSCALER_XBR,
} TextureUpscalerType;

// Post-processing filters
typedef enum {
    TEX_FILTER_NONE = 1,
    TEX_FILTER_SMOOTH,
    TEX_FILTER_BLUR,
    TEX_FILTER_SATURATION,
    TEX_FILTER_GRAYSCALE,
    TEX_FILTER_SEPIA,
    TEX_FILTER_CRT,
} TextureFilterType;

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
    // id <MTLDevice> device;
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
    
    //! Filtered emulator texture
    /*! In the second post-processing stage, the upscaled texture gets filtered.
     *  E.g., a CRT filter can be applied to mimic old CRT displays.
     */
    id <MTLTexture> filteredTexture;
    
    //! Final drawing target (GPU buffer)
    id <MTLTexture> framebufferTexture;
    
    //! Texture to hold the pixel depth information
    id <MTLTexture> depthTexture;

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

@property id <MTLLibrary> library;
@property id <MTLCommandQueue> queue;
@property id <MTLRenderPipelineState> pipeline;
@property id <MTLDepthStencilState> depthState;


@property id <MTLTexture> bgTexture;
@property id <MTLTexture> emulatorTexture;
@property id <MTLTexture> upscaledTexture;
@property id <MTLTexture> filteredTexture;
@property id <MTLTexture> depthTexture;

@property id <MTLBuffer> positionBuffer;
@property id <MTLBuffer> uniformBuffer2D;
@property id <MTLBuffer> uniformBuffer3D;
@property id <MTLBuffer> uniformBufferBg;

@property CAMetalLayer *metalLayer;
@property CGFloat layerWidth;
@property CGFloat layerHeight;
@property bool layerIsDirty;

@property float currentXAngle;
@property float targetXAngle;
@property float deltaXAngle;
@property float currentYAngle;
@property float targetYAngle;
@property float deltaYAngle;
@property float currentZAngle;
@property float targetZAngle;
@property float deltaZAngle;
@property float currentEyeX;
@property float targetEyeX;
@property float deltaEyeX;
@property float currentEyeY;
@property float targetEyeY;
@property float deltaEyeY;
@property float currentEyeZ;
@property float targetEyeZ;
@property float deltaEyeZ;
@property float currentAlpha;
@property float targetAlpha;
@property float deltaAlpha;

@property float textureXStart;
@property float textureXEnd;
@property float textureYStart;
@property float textureYEnd;
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

- (void)buildKernels;
- (void)reshapeWithFrame:(CGRect)frame;
- (void)updateScreenGeometry;

@end
