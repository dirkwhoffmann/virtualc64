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

@implementation MyMetalView(Setup)

- (void)setupMetal
{
    [self buildMetal];
    [self buildTextures];
    [self buildKernels];
#if 0
    [self buildBuffers];
    [self buildPipeline];
    [self reshape];
#endif
}

- (void)buildMetal
{
    NSLog(@"MyMetalView::buildMetal");
    
    // Metal device
    device = MTLCreateSystemDefaultDevice();
    NSAssert(device != nil, @"Metal device must not be nil");

    // Metal layer
    metalLayer = (CAMetalLayer *)self.layer;
    NSAssert(device != nil, @"Metal layer must not be nil");
    metalLayer.device = device;
    metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    metalLayer.framebufferOnly = NO; // YES;
    metalLayer.frame = self.layer.frame;
    layerWidth = metalLayer.drawableSize.width;
    layerHeight = metalLayer.drawableSize.height;

    // Create command queue
    commandQueue = [device newCommandQueue];
    NSAssert(device != nil, @"Metal command queue must not be nil");
    
    // Load shader library
    library = [device newDefaultLibrary];
    NSAssert(device != nil, @"Metal library must not be nil");
    
    // View parameters
    self.sampleCount = 1; // 4;
}

- (void)buildTextures
{
    NSLog(@"MyMetalView::buildTextures");
    
    // Background
    NSURL *url = [[NSWorkspace sharedWorkspace] desktopImageURLForScreen:[NSScreen mainScreen]];
    NSImage *bgImage = [[NSImage alloc] initWithContentsOfURL:url];
    NSImage *bgImageResized = [self expandImage:bgImage toSize:NSMakeSize(BG_TEXTURE_WIDTH,BG_TEXTURE_HEIGHT)];
    bgTexture = [self makeTexture:bgImageResized withDevice:device];

    // C64 screen (raw emulator data)
    MTLTextureDescriptor *textureDescriptor =
    [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                       width:512
                                                      height:512
                                                   mipmapped:NO];
    textureFromEmulator = [device newTextureWithDescriptor:textureDescriptor];
    
    // C64 screen (post-processed)
    textureDescriptor.usage |= MTLTextureUsageShaderWrite;
    filteredTexture = [device newTextureWithDescriptor:textureDescriptor];
}

- (void)buildKernels
{
    bypassFilter = [BypassFilter filterWithDevice:device library:library];
    smoothFilter = [SaturationFilter filterWithFactor:1.0 device:device library:library];
    blurFilter = [BlurFilter filterWithRadius:1.0 device:device library:library];
    saturationFilter = [SaturationFilter filterWithFactor:0.5 device:device library:library];
    sepiaFilter = [SepiaFilter filterWithDevice:device library:library];
    crtFilter = [CrtFilter filterWithDevice:device library:library];
    grayscaleFilter = [SaturationFilter filterWithFactor:0.0 device:device library:library];
    
    [self setVideoFilter:TEX_FILTER_CRT];
}

@end