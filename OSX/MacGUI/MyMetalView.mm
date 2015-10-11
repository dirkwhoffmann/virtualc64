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

#import "C64GUI.h"
#import "ShaderTypes.h"

static CVReturn MetalRendererCallback(CVDisplayLinkRef displayLink,
                                      const CVTimeStamp *inNow,
                                      const CVTimeStamp *inOutputTime,
                                      CVOptionFlags flagsIn,
                                      CVOptionFlags *flagsOut,
                                      void *displayLinkContext)
{
    @autoreleasepool {

        return [(__bridge MyMetalView *)displayLinkContext getFrameForTime:inOutputTime flagsOut:flagsOut];
        
    }
}

@implementation MyMetalView {
    
    // Renderer
    CAMetalLayer *_metalLayer;
    id <MTLDevice> _device;
    id <MTLLibrary> _library;
    id <MTLRenderPipelineState> _pipeline;
    id <MTLDepthStencilState> _depthState;
    id <MTLCommandQueue> _commandQueue;
    id <MTLCommandBuffer> _commandBuffer;
    id <MTLRenderCommandEncoder> _commandEncoder;
    id <MTLBuffer> _positionBuffer;
    id <MTLBuffer> _colorBuffer;

    // Textures
    id <CAMetalDrawable> _drawable;
    id <MTLTexture> _framebufferTexture;

    id <MTLTexture> _texture;
    id <MTLTexture> _depthTexture;
    id <MTLSamplerState> _sampler;

    // Uniforms
    id <MTLBuffer> _uniformBuffer;
    matrix_float4x4 _projectionMatrix;
    matrix_float4x4 _viewMatrix;
    float _angle; // Rotation angle
    
    // Display link
    CVDisplayLinkRef displayLink;
    
    //
    BOOL _pipelineIsDirty;
}

// -----------------------------------------------------------------------------------------------
//                                       Initialization
// -----------------------------------------------------------------------------------------------

- (id)initWithCoder:(NSCoder *)c
{
    NSLog(@"MyMetalView::initWithCoder");
    
    if ((self = [super initWithCoder:c]) == nil) {
        NSLog(@"ERROR: Can't initiaize MetalView");
    }
    return self;
}

-(void)awakeFromNib
{
    NSLog(@"MyMetalView::awakeFromNib");
    
    c64 = [c64proxy c64]; // DEPRECATED
    
    // Set initial scene position and drawing properties
    targetXAngle = targetYAngle = targetZAngle = 0;
    deltaXAngle = deltaYAngle = deltaZAngle = 0;
    currentEyeX = currentEyeY = currentEyeZ = 0;
    deltaEyeX = deltaEyeY = deltaEyeZ = 0;
    drawInEntireWindow = false;
    drawIn3D = true;
    drawC64texture = false;
    drawBackground = true;
    drawEntireCube = false;
    
    // Core video and graphics stuff
    displayLink = nil;

    // Metal related stuff
    _angle = 0;  // DEPRECATED
    _pipelineIsDirty = YES;
    _depthTexture = nil;
    
    [self buildMetal];
    [self buildTextures];
    [self buildBuffers];
    [self setupDisplayLink];
    
    [self reshape];
}

- (void) dealloc
{
    [self cleanup];
}

-(void)cleanup
{
    NSLog(@"MyMetalView::cleanup");
    
    if (displayLink) {
        CVDisplayLinkStop(displayLink);
        CVDisplayLinkRelease(displayLink);
        displayLink = NULL;
    }
}

- (void)buildMetal
{
    NSLog(@"MyMetalView::buildMetal");
    
    // Metal device
    _device = MTLCreateSystemDefaultDevice();
    if (!_device) {
        NSLog(@"Error: No metal device");
        return;
    }

    // Layer
    _metalLayer = (CAMetalLayer *)self.layer;
    _metalLayer.device = _device;
    _metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;

    // Create command queue
    _commandQueue = [_device newCommandQueue];

    // Load all shader files shipped with this project
    _library = [_device newDefaultLibrary];
 
    // Configure view
    self.sampleCount = 4; // 1;
}

- (void)buildTextures
{
    NSLog(@"MyMetalView::buildTextures");
    
    // C64 screen texture
    MTLTextureDescriptor *textureDescriptor =
    [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                       width:512
                                                      height:512
                                                   mipmapped:NO];
    _texture = [_device newTextureWithDescriptor:textureDescriptor];
    
    // Texture sampler
    MTLSamplerDescriptor *samplerDescriptor = [MTLSamplerDescriptor new];
    {
        samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
        samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
        samplerDescriptor.sAddressMode = MTLSamplerAddressModeClampToEdge;
        samplerDescriptor.tAddressMode = MTLSamplerAddressModeClampToEdge;
        samplerDescriptor.mipFilter = MTLSamplerMipFilterNotMipmapped;
    }
    _sampler = [_device newSamplerStateWithDescriptor:samplerDescriptor];
}

- (void)buildDepthBuffer
{
    NSLog(@"MyMetalView::buildDepthBuffer");
    
    CGSize drawableSize = self.drawableSize;
    
    MTLTextureDescriptor *depthTexDesc =
    [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                                       width:drawableSize.width
                                                      height:drawableSize.height
                                                   mipmapped:NO];
    {
        depthTexDesc.resourceOptions = MTLResourceStorageModePrivate;
        depthTexDesc.usage = MTLTextureUsageRenderTarget;
    }
    _depthTexture = [_device newTextureWithDescriptor:depthTexDesc];
}

- (void)buildBuffers
{
    NSLog(@"MyMetalView::buildBuffers");
    
    // Vertex buffer
    _positionBuffer = [self buildVertexBuffer:_device];

    // Uniform buffer
    _uniformBuffer = [_device newBufferWithLength:sizeof(Uniforms) options:0];
}

- (void)buildPipeline
{
    NSLog(@"MyMetalView::buildPipeline");

    id<MTLFunction> vertexFunc = [_library newFunctionWithName:@"vertex_main"];
    id<MTLFunction> fragmentFunc = [_library newFunctionWithName:@"fragment_main"];
    
    // Depth stencil state
    MTLDepthStencilDescriptor *depthDescriptor = [MTLDepthStencilDescriptor new];
    {
        depthDescriptor.depthCompareFunction = MTLCompareFunctionLess;
        depthDescriptor.depthWriteEnabled = YES;
    }
    _depthState = [_device newDepthStencilStateWithDescriptor:depthDescriptor];

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
        pipelineDescriptor.label = @"C64 metal pipeline";
        pipelineDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        pipelineDescriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
        // pipelineDescriptor.sampleCount = self.sampleCount;
        pipelineDescriptor.vertexFunction = vertexFunc;
        pipelineDescriptor.fragmentFunction = fragmentFunc;
        // pipelineDescriptor.vertexDescriptor = vertexDescriptor;
    }
    
    NSError *error = nil;
    _pipeline = [_device newRenderPipelineStateWithDescriptor:pipelineDescriptor
                                                        error:&error];
    if (!_pipeline)
    {
        NSLog(@"Error occurred when creating render pipeline: %@", error);
    }
    
    _commandQueue = [_device newCommandQueue];
}

- (void)setupDisplayLink
{
    NSLog(@"MyMetalView::setupDisplayLink");
    
    CVReturn success;

    // Create display link for the main display
    CVDisplayLinkCreateWithCGDisplay(kCGDirectMainDisplay, &displayLink);
    // checkForOpenGLErrors();

    if (!displayLink) {
        NSLog(@"Error: Can't create display link");
        exit(0);
    }
    
    // Set the current display of a display link
    if ((success = CVDisplayLinkSetCurrentCGDisplay(displayLink, kCGDirectMainDisplay)) != 0) {
        NSLog(@"CVDisplayLinkSetCurrentCGDisplay failed with return code %d", success);
        CVDisplayLinkRelease(displayLink);
        exit(0);
    }
    
    // Set the renderer output callback function
    if ((success = CVDisplayLinkSetOutputCallback(displayLink, &MetalRendererCallback, (__bridge void *)self)) != 0) {
        NSLog(@"CVDisplayLinkSetOutputCallback failed with return code %d", success);
        CVDisplayLinkRelease(displayLink);
        exit(0);
    }
    
    // Activates display link
    if ((success = CVDisplayLinkStart(displayLink)) != 0) {
        NSLog(@"CVDisplayLinkStart failed with return code %d", success);
        CVDisplayLinkRelease(displayLink);
        exit(0);
    }
}


// -----------------------------------------------------------------------------------------------
//                                           Drawing
// -----------------------------------------------------------------------------------------------

- (void)startFrame
{
    CGSize drawableSize = _metalLayer.drawableSize;
    
    if (!_depthTexture || _depthTexture.width != drawableSize.width || _depthTexture.height != drawableSize.height)
        [self buildDepthBuffer];
    
    _drawable = [_metalLayer nextDrawable];
    _framebufferTexture = _drawable.texture;
    
    if (!_framebufferTexture)
    {
        NSLog(@"Unable to retrieve texture; drawable may be nil");
        return;
    }
    
    if (_pipelineIsDirty)
    {
        [self buildPipeline];
        _pipelineIsDirty = NO;
    }

    // Render pass
    /* "A render pass descriptor tells Metal what actions to take while an image is being rendered" */
    MTLRenderPassDescriptor *renderPass = [MTLRenderPassDescriptor renderPassDescriptor];
    {
        renderPass.colorAttachments[0].texture = _framebufferTexture;
        renderPass.colorAttachments[0].clearColor = MTLClearColorMake(0.5, 0.5, 0.5, 1);
        renderPass.colorAttachments[0].loadAction = MTLLoadActionClear;
        renderPass.colorAttachments[0].storeAction = MTLStoreActionStore;
    
        renderPass.depthAttachment.texture = _depthTexture;
        renderPass.depthAttachment.clearDepth = 1;
        renderPass.depthAttachment.loadAction = MTLLoadActionClear;
        renderPass.depthAttachment.storeAction = MTLStoreActionDontCare;
    }
    
    // "A command buffer represents a collection of render commands to be executed as a unit."
    // "A command encoder is an object that is used to tell Metal what drawing we actually want to do."
    _commandBuffer = [_commandQueue commandBuffer];
    _commandEncoder = [_commandBuffer renderCommandEncoderWithDescriptor:renderPass];
    {
        [_commandEncoder setRenderPipelineState:_pipeline];
        [_commandEncoder setDepthStencilState:_depthState];
        [_commandEncoder setFragmentTexture:_texture atIndex:0];
        [_commandEncoder setFragmentSamplerState:_sampler atIndex:0];
        [_commandEncoder setVertexBuffer:_positionBuffer offset:0 atIndex:0];
        [_commandEncoder setVertexBuffer:_uniformBuffer offset:0 atIndex:1];
    }
}
    
- (void)render
{
    [_commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:36 instanceCount:1];
}

- (void)endFrame
{
    [_commandEncoder endEncoding];
    
    if (_drawable)
    {
        [_commandBuffer presentDrawable:_drawable];
        [_commandBuffer commit];
    }
}


- (void)updateScreenGeometry
{
    if (c64->isPAL()) {
        
        // PAL border will be 36 pixels wide and 34 pixels heigh
        textureXStart = (float)(PAL_LEFT_BORDER_WIDTH - 36.0) / (float)TEXTURE_WIDTH;
        textureXEnd = (float)(PAL_LEFT_BORDER_WIDTH + PAL_CANVAS_WIDTH + 36.0) / (float)TEXTURE_WIDTH;
        textureYStart = (float)(PAL_UPPER_BORDER_HEIGHT - 34.0) / (float)TEXTURE_HEIGHT;
        textureYEnd = (float)(PAL_UPPER_BORDER_HEIGHT + PAL_CANVAS_HEIGHT + 34.0) / (float)TEXTURE_HEIGHT;
        
    } else {
        
        // NTSC border will be 42 pixels wide and 9 pixels heigh
        textureXStart = (float)(NTSC_LEFT_BORDER_WIDTH - 42.0) / (float)TEXTURE_WIDTH;
        textureXEnd = (float)(NTSC_LEFT_BORDER_WIDTH + NTSC_CANVAS_WIDTH + 42.0) / (float)TEXTURE_WIDTH;
        textureYStart = (float)(NTSC_UPPER_BORDER_HEIGHT - 9) / (float)TEXTURE_HEIGHT;
        textureYEnd = (float)(NTSC_UPPER_BORDER_HEIGHT + NTSC_CANVAS_HEIGHT + 9) / (float)TEXTURE_HEIGHT;
    }
    
    // Enable this for debugging (will display the whole texture)
    /*
     textureXStart = 0.0;
     textureXEnd = 1.0;
     textureYStart = 0.0;
     textureYEnd = 1.0;
     */
    
    _positionBuffer = [self buildVertexBuffer:_device];
}

- (void)updateTexture:(id<MTLCommandBuffer>) cmdBuffer
{
    if (!c64) {
        NSLog(@"Can't access C64");
        return;
    }
    
    void *buf = c64->vic->screenBuffer();
    assert(buf != NULL);

    NSUInteger pixelSize = 4;
    NSUInteger width = NTSC_PIXELS;
    NSUInteger height = PAL_RASTERLINES;
    NSUInteger rowBytes = width * pixelSize;
    NSUInteger imageBytes = rowBytes * height;
    
    [_texture replaceRegion:MTLRegionMake2D(0,0,width,height)
           mipmapLevel:0 slice:0 withBytes:buf
           bytesPerRow:rowBytes bytesPerImage:imageBytes];
}

- (void)reshape {
    /*
     When reshape is called, update the view and projection matricies since
     this means the view orientation or size changed.
     */
    float aspect = fabs(self.bounds.size.width / self.bounds.size.height);
    _projectionMatrix = vc64_matrix_from_perspective_fov_aspectLH(65.0f * (M_PI / 180.0f), aspect, 0.1f, 100.0f);
    
    _viewMatrix = matrix_identity_float4x4;
}

- (void)updateRotationAngle {
    
    Uniforms *frameData = (Uniforms *)[_uniformBuffer contents];
    
    frameData->model =
    vc64_matrix_from_translation(0.0f, 0.0f, 2.0f) *
    vc64_matrix_from_rotation(_angle, 1.0f, 1.0f, 0.0f);
    frameData->view = _viewMatrix;
    
    matrix_float4x4 modelViewMatrix = frameData->view * frameData->model;
    
    frameData->projectionView = _projectionMatrix * modelViewMatrix;
    
    _angle += 0.05f;
}

- (CVReturn)getFrameForTime:(const CVTimeStamp*)timeStamp flagsOut:(CVOptionFlags*)flagsOut
{
    @autoreleasepool {
        
        // Update angles for screen animation
        [self updateRotationAngle];
        
        // Update texture
        [self updateTexture:_commandBuffer];
        
        // Draw scene
        [self startFrame];
        [self render];
        [self endFrame];
        
        return kCVReturnSuccess;
    }
}



@end
