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
    [self buildBuffers];
    [self buildPipeline];
#if 0
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
    bgTexture = [self makeTexture:bgImageResized];

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

- (void)buildBuffers
{
    // Vertex buffer
    positionBuffer = [self buildVertexBuffer];
    
    // Uniform buffer
    uniformBuffer = [device newBufferWithLength:sizeof(Uniforms) options:0];
    uniformBufferBg = [device newBufferWithLength:sizeof(Uniforms) options:0];
}

- (id<MTLBuffer>)buildVertexBuffer
{
    NSLog(@"MyMetalView::buildVertexBuffer");
    
    const float dx = 0.64;
    const float dy = 0.48;
    const float dz = 0.64;
    const float bgx = 6.4;
    const float bgy = 4.8;
    const float bgz = -6.8;
    
    float positions[] =
    {
        // Background
        -bgx,  bgy, -bgz, 1,   0.0, 0.0,
        -bgx, -bgy, -bgz, 1,   0.0, 1.0,
        bgx, -bgy, -bgz, 1,   1.0, 1.0,
        
        -bgx,  bgy, -bgz, 1,   0.0, 0.0,
        bgx,  bgy, -bgz, 1,   1.0, 0.0,
        bgx, -bgy, -bgz, 1,   1.0, 1.0,
        
        // -Z
        -dx,  dy, -dz, 1,   textureXStart, textureYStart,
        -dx, -dy, -dz, 1,   textureXStart, textureYEnd,
        dx, -dy, -dz, 1,   textureXEnd, textureYEnd,
        
        -dx,  dy, -dz, 1,   textureXStart, textureYStart,
        dx,  dy, -dz, 1,   textureXEnd, textureYStart,
        dx, -dy, -dz, 1,   textureXEnd, textureYEnd,
        
        // +Z
        -dx,  dy,  dz, 1,   textureXEnd, textureYStart,
        -dx, -dy,  dz, 1,   textureXEnd, textureYEnd,
        dx, -dy,  dz, 1,   textureXStart, textureYEnd,
        
        -dx,  dy,  dz, 1,   textureXEnd, textureYStart,
        dx,  dy,  dz, 1,   textureXStart, textureYStart,
        dx, -dy,  dz, 1,   textureXStart, textureYEnd,
        
        // -X
        -dx,  dy, -dz, 1,   textureXEnd, textureYStart,
        -dx, -dy, -dz, 1,   textureXEnd, textureYEnd,
        -dx, -dy,  dz, 1,   textureXStart, textureYEnd,
        
        -dx,  dy, -dz, 1,   textureXEnd, textureYStart,
        -dx,  dy,  dz, 1,   textureXStart, textureYStart,
        -dx, -dy,  dz, 1,   textureXStart, textureYEnd,
        
        // +X
        dx,  dy, -dz, 1,   textureXStart, textureYStart,
        dx, -dy, -dz, 1,   textureXStart, textureYEnd,
        dx, -dy,  dz, 1,   textureXEnd, textureYEnd,
        
        dx,  dy, -dz, 1,   textureXStart, textureYStart,
        dx,  dy,  dz, 1,   textureXEnd, textureYStart,
        dx, -dy,  dz, 1,   textureXEnd, textureYEnd,
        
        // -Y
        dx, -dy, -dz, 1,   textureXStart, textureYStart,
        -dx, -dy, -dz, 1,   textureXStart, textureYEnd,
        -dx, -dy,  dz, 1,   textureXEnd, textureYEnd,
        
        dx, -dy, -dz, 1,   textureXStart, textureYStart,
        dx, -dy,  dz, 1,   textureXEnd, textureYStart,
        -dx, -dy,  dz, 1,   textureXEnd, textureYEnd,
        
        // +Y
        +dx, +dy, -dz, 1,   textureXStart, textureYStart,
        -dx, +dy, -dz, 1,   textureXStart, textureYEnd,
        -dx, +dy, +dz, 1,   textureXEnd, textureYEnd,
        
        +dx, +dy, -dz, 1,   textureXStart, textureYStart,
        -dx, +dy, +dz, 1,   textureXEnd, textureYEnd,
        +dx, +dy, +dz, 1,   textureXEnd, textureYStart,
        
        // 2D drawing quad
        -1,  1, 0, 1,  textureXStart, textureYStart,
        -1, -1, 0, 1,  textureXStart, textureYEnd,
        1, -1, 0, 1,  textureXEnd, textureYEnd,
        
        -1,  1, 0, 1,  textureXStart, textureYStart,
        1,  1, 0, 1,  textureXEnd, textureYStart,
        1, -1, 0, 1,  textureXEnd, textureYEnd,
    };
    
    return [device newBufferWithBytes:positions
                               length:sizeof(positions)
                              options:MTLResourceOptionCPUCacheModeDefault];
}

- (void)buildPipeline
{
    NSLog(@"MyMetalView::buildPipeline");

    NSError *error = nil;
    
    id<MTLFunction> vertexFunc = [library newFunctionWithName:@"vertex_main"];
    NSAssert(vertexFunc != nil, @"Vertex shader not found in metal shader library");
    
    id<MTLFunction> fragmentFunc = [library newFunctionWithName:@"fragment_main"];
    NSAssert(fragmentFunc != nil, @"Fragment shader not found in metal shader library");
    
    // Depth stencil state
    MTLDepthStencilDescriptor *depthDescriptor = [MTLDepthStencilDescriptor new];
    {
        depthDescriptor.depthCompareFunction = MTLCompareFunctionLess;
        depthDescriptor.depthWriteEnabled = YES;
    }
    depthState = [device newDepthStencilStateWithDescriptor:depthDescriptor];
    
    // Vertex descriptor
    MTLVertexDescriptor *vertexDescriptor = [MTLVertexDescriptor new];
    {
        // Positions
        vertexDescriptor.attributes[0].format = MTLVertexFormatFloat4;
        vertexDescriptor.attributes[0].offset = 0;
        vertexDescriptor.attributes[0].bufferIndex = 0;
        
        // Texture coordinates
        vertexDescriptor.attributes[1].format = MTLVertexFormatHalf2;
        vertexDescriptor.attributes[1].offset = 16;
        vertexDescriptor.attributes[1].bufferIndex = 1;
        
        // Single interleaved buffer
        vertexDescriptor.layouts[0].stride = 24;
        vertexDescriptor.layouts[0].stepRate = 1;
        vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
    }
    
    // Render pipeline
    MTLRenderPipelineDescriptor *pipelineDescriptor = [MTLRenderPipelineDescriptor new];
    {
        pipelineDescriptor.label = @"VirtualC64 metal pipeline";
        pipelineDescriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
        pipelineDescriptor.vertexFunction = vertexFunc;
        pipelineDescriptor.fragmentFunction = fragmentFunc;
    }
    MTLRenderPipelineColorAttachmentDescriptor *renderbufAttachment = pipelineDescriptor.colorAttachments[0];
    {
        renderbufAttachment.pixelFormat = MTLPixelFormatBGRA8Unorm;
        renderbufAttachment.blendingEnabled = YES;
        renderbufAttachment.rgbBlendOperation = MTLBlendOperationAdd;
        renderbufAttachment.alphaBlendOperation = MTLBlendOperationAdd;
        renderbufAttachment.sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
        renderbufAttachment.sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
        renderbufAttachment.destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
        renderbufAttachment.destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    }
    pipeline = [device newRenderPipelineStateWithDescriptor:pipelineDescriptor error:&error];
    if (!pipeline) {
        NSLog(@"Render pipeline creation failed with error: %@", error);
        exit(0);
    }
}

@end