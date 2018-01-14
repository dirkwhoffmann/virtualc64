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

@implementation MyMetalView(Setup)

- (void)setupMetal
{
    NSLog(@"MyMetalView::setupMetal");

    [self buildMetal];
    [self buildTextures];
    [self buildKernels];
    [self buildBuffers];
    [self buildPipeline];
    
    [self reshapeWithFrame:[self frame]];
    // [self setupDisplayLink];
    enableMetal = true;
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
    metalLayer.framebufferOnly = YES; // NO;
    metalLayer.frame = self.layer.frame;
    layerWidth = metalLayer.drawableSize.width;
    layerHeight = metalLayer.drawableSize.height;

    // Command queue
    queue = [device newCommandQueue];
    NSAssert(device != nil, @"Metal command queue must not be nil");
    
    // Shader library
    library = [device newDefaultLibrary];
    NSAssert(library != nil, @"Metal library must not be nil");
    
    // View parameters
    self.sampleCount = 1; // 4;
}

- (void)buildTextures
{
    NSLog(@"MyMetalView::buildTextures");
    
    // Background texture (drawn behind the cube)
    bgTexture = [[[BackgroundImage alloc] initWithDevice:device] texture];
    
    // C64 texture (as provided by the emulator)
    MTLTextureDescriptor *textureDescriptor =
    [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                       width:512
                                                      height:512
                                                   mipmapped:NO];
    textureDescriptor.usage = MTLTextureUsageShaderRead;
    emulatorTexture = [device newTextureWithDescriptor:textureDescriptor];
    NSAssert(emulatorTexture != nil, @"Failed to create emulator texture");
    if (emulatorTexture == nil) { exit(0); }
 
    // Upscaled C64 texture
    MTLTextureDescriptor *textureDescriptorUpscaled =
    [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                       width:2048
                                                      height:2048
                                                   mipmapped:NO];
    textureDescriptorUpscaled.usage = MTLTextureUsageShaderRead | MTLTextureUsageShaderWrite;
    upscaledTexture = [device newTextureWithDescriptor:textureDescriptorUpscaled];
    NSAssert(upscaledTexture != nil, @"Failed to create upscaling texture");
    if (upscaledTexture == nil) { exit(0); }
    
    // Final texture (upscaled and filtered)
    MTLTextureDescriptor *textureDescriptorPP =
    [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                       width:2048
                                                      height:2048
                                                   mipmapped:NO];
    textureDescriptorPP.usage = MTLTextureUsageShaderRead | MTLTextureUsageShaderWrite;
    filteredTexture = [device newTextureWithDescriptor:textureDescriptorPP];
    NSAssert(filteredTexture != nil, @"Failed to create filtering texture");
    if (filteredTexture == nil) { exit(0); }
}

- (void)buildBuffers
{
    // Vertex buffer
    [self buildVertexBuffer];
    
    // Uniform buffers
    uniformBuffer2D = [device newBufferWithLength:sizeof(Uniforms) options:0];
    uniformBuffer3D = [device newBufferWithLength:sizeof(Uniforms) options:0];
    uniformBufferBg = [device newBufferWithLength:sizeof(Uniforms) options:0];
}

- (void)buildVertexBuffer
{
    // NSLog(@"MyMetalView::buildVertexBuffer");
    
    if (!device)
        return;

    const float dx = 0.64;
    const float dy = 0.48;
    const float dz = 0.64;
    const float bgx = 6.4;
    const float bgy = 4.8;
    const float bgz = -6.8;
    
    float positions[] =
    {
        // Background
        -bgx, +bgy, -bgz, 1,   0.0, 0.0, 0,0,
        -bgx, -bgy, -bgz, 1,   0.0, 1.0, 0,0,
        +bgx, -bgy, -bgz, 1,   1.0, 1.0, 0,0,
        
        -bgx, +bgy, -bgz, 1,   0.0, 0.0, 0,0,
        +bgx, +bgy, -bgz, 1,   1.0, 0.0, 0,0,
        +bgx, -bgy, -bgz, 1,   1.0, 1.0, 0,0,
        
        // -Z
        -dx, +dy, -dz, 1,   textureXStart, textureYStart, 0,0,
        -dx, -dy, -dz, 1,   textureXStart, textureYEnd, 0,0,
        +dx, -dy, -dz, 1,   textureXEnd, textureYEnd, 0,0,
        
        -dx, +dy, -dz, 1,   textureXStart, textureYStart, 0,0,
        +dx, +dy, -dz, 1,   textureXEnd, textureYStart, 0,0,
        +dx, -dy, -dz, 1,   textureXEnd, textureYEnd, 0,0,
        
        // +Z
        -dx, +dy, +dz, 1,   textureXEnd, textureYStart, 0,0,
        -dx, -dy, +dz, 1,   textureXEnd, textureYEnd, 0,0,
        +dx, -dy, +dz, 1,   textureXStart, textureYEnd, 0,0,
        
        -dx, +dy, +dz, 1,   textureXEnd, textureYStart, 0,0,
        +dx, +dy, +dz, 1,   textureXStart, textureYStart, 0,0,
        +dx, -dy, +dz, 1,   textureXStart, textureYEnd, 0,0,
        
        // -X
        -dx, +dy, -dz, 1,   textureXEnd, textureYStart, 0,0,
        -dx, -dy, -dz, 1,   textureXEnd, textureYEnd, 0,0,
        -dx, -dy, +dz, 1,   textureXStart, textureYEnd, 0,0,
        
        -dx, +dy, -dz, 1,   textureXEnd, textureYStart, 0,0,
        -dx, +dy, +dz, 1,   textureXStart, textureYStart, 0,0,
        -dx, -dy, +dz, 1,   textureXStart, textureYEnd, 0,0,
        
        // +X
        +dx, +dy, -dz, 1,   textureXStart, textureYStart, 0,0,
        +dx, -dy, -dz, 1,   textureXStart, textureYEnd, 0,0,
        +dx, -dy, +dz, 1,   textureXEnd, textureYEnd, 0,0,
        
        +dx, +dy, -dz, 1,   textureXStart, textureYStart, 0,0,
        +dx, +dy, +dz, 1,   textureXEnd, textureYStart, 0,0,
        +dx, -dy, +dz, 1,   textureXEnd, textureYEnd, 0,0,
        
        // -Y
        +dx, -dy, -dz, 1,   textureXStart, textureYStart, 0,0,
        -dx, -dy, -dz, 1,   textureXStart, textureYEnd, 0,0,
        -dx, -dy, +dz, 1,   textureXEnd, textureYEnd, 0,0,
        
        +dx, -dy, -dz, 1,   textureXStart, textureYStart, 0,0,
        +dx, -dy, +dz, 1,   textureXEnd, textureYStart, 0,0,
        -dx, -dy, +dz, 1,   textureXEnd, textureYEnd, 0,0,
        
        // +Y
        +dx, +dy, -dz, 1,   textureXStart, textureYStart, 0,0,
        -dx, +dy, -dz, 1,   textureXStart, textureYEnd, 0,0,
        -dx, +dy, +dz, 1,   textureXEnd, textureYEnd, 0,0,
        
        +dx, +dy, -dz, 1,   textureXStart, textureYStart, 0,0,
        -dx, +dy, +dz, 1,   textureXEnd, textureYEnd, 0,0,
        +dx, +dy, +dz, 1,   textureXEnd, textureYStart, 0,0,
        
        // 2D drawing quad
        -1, +1, +0, +1,   textureXStart, textureYStart, 0,0,
        -1, -1, +0, +1,   textureXStart, textureYEnd, 0,0,
        +1, -1, +0, +1,   textureXEnd, textureYEnd, 0,0,
        
        -1, +1, +0, +1,   textureXStart, textureYStart, 0,0,
        +1, +1, +0, +1,   textureXEnd, textureYStart, 0,0,
        +1, -1, +0, +1,   textureXEnd, textureYEnd, 0,0,
    };
    
    positionBuffer = [device newBufferWithBytes:positions
                                         length:sizeof(positions)
                                        options:MTLResourceOptionCPUCacheModeDefault];
    
    NSAssert(positionBuffer != nil, @"positionBuffer must not be nil");
}

- (void)buildDepthBuffer
{
    // NSLog(@"MyMetalView::buildDepthBuffer");
    
    if (!device)
        return;
    
    // NSUInteger w = (layerWidth < 1) ? 512 : ((layerWidth > 2048) ? 2048 : layerWidth);
    // NSUInteger h = (layerHeight < 1) ? 512 : ((layerHeight > 2048) ? 2048 : layerHeight);
    NSUInteger w = (layerWidth < 1) ? 512 : ((layerWidth > 2048) ? layerWidth : layerWidth);
    NSUInteger h = (layerHeight < 1) ? 512 : ((layerHeight > 2048) ? layerHeight : layerHeight);

    MTLTextureDescriptor *depthTexDesc =
    [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                                       width:w
                                                      height:h
                                                   mipmapped:NO];
    {
        depthTexDesc.resourceOptions = MTLResourceStorageModePrivate;
        depthTexDesc.usage = MTLTextureUsageRenderTarget;
    }
    depthTexture = [device newTextureWithDescriptor:depthTexDesc];
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


// Matrix utilities
matrix_float4x4
vc64_matrix_identity()
{
    vector_float4 X = { 1, 0, 0, 0 };
    vector_float4 Y = { 0, 1, 0, 0 };
    vector_float4 Z = { 0, 0, 1, 0 };
    vector_float4 W = { 0, 0, 0, 1 };
    
    matrix_float4x4 identity = { {X, Y, Z, W} };
    
    return identity;
}

matrix_float4x4
vc64_matrix_from_perspective_fov_aspectLH(float fovY, float aspect, float nearZ, float farZ)
{
    
    /* OLD CODE: Keeps correct aspect ratio independent of window size */
    float yscale = 1.0f / tanf(fovY * 0.5f); // 1 / tan == cot
    float xscale = yscale / aspect;
    float q = farZ / (farZ - nearZ);
    
    /* Alternative: Adjust to window size */
    /*
    float yscale = 1.0f / tanf(fovY * 0.5f);
    float xscale = 0.75 * yscale;
    float q = farZ / (farZ - nearZ);
    */
    
    matrix_float4x4 m = {
        .columns[0] = { xscale, 0.0f, 0.0f, 0.0f },
        .columns[1] = { 0.0f, yscale, 0.0f, 0.0f },
        .columns[2] = { 0.0f, 0.0f, q, 1.0f },
        .columns[3] = { 0.0f, 0.0f, q * -nearZ, 0.0f }
    };
    
    return m;
}

matrix_float4x4
vc64_matrix_from_translation(float x, float y, float z)
{
    matrix_float4x4 m = matrix_identity_float4x4;
    m.columns[3] = (vector_float4) { x, y, z, 1.0 };
    return m;
}

matrix_float4x4
vc64_matrix_from_rotation(float radians, float x, float y, float z)
{
    vector_float3 v = vector_normalize(((vector_float3){x, y, z}));
    float cos = cosf(radians);
    float cosp = 1.0f - cos;
    float sin = sinf(radians);
    
    return (matrix_float4x4) {
        .columns[0] = {
            cos + cosp * v.x * v.x,
            cosp * v.x * v.y + v.z * sin,
            cosp * v.x * v.z - v.y * sin,
            0.0f,
        },
        
        .columns[1] = {
            cosp * v.x * v.y - v.z * sin,
            cos + cosp * v.y * v.y,
            cosp * v.y * v.z + v.x * sin,
            0.0f,
        },
        
        .columns[2] = {
            cosp * v.x * v.z + v.y * sin,
            cosp * v.y * v.z - v.x * sin,
            cos + cosp * v.z * v.z,
            0.0f,
        },
        
        .columns[3] = { 0.0f, 0.0f, 0.0f, 1.0f
        }
    };
}
