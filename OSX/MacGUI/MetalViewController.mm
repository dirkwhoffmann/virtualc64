//
//  MetalViewController.m
//  V64
//
//  Created by Dirk Hoffmann on 09.10.15.
//
//

#import <Metal/Metal.h>
// #import <simd/simd.h>
#import <MetalKit/MetalKit.h>
#import "MetalViewController.h"

static CVReturn MetalRendererCallback(CVDisplayLinkRef displayLink,
                                      const CVTimeStamp *inNow,
                                      const CVTimeStamp *inOutputTime,
                                      CVOptionFlags flagsIn,
                                      CVOptionFlags *flagsOut,
                                      void *displayLinkContext)
{
    @autoreleasepool {
        
        return [(__bridge MetalViewController *)displayLinkContext getFrameForTime:inOutputTime flagsOut:flagsOut];
        
    }
}

@implementation MetalViewController {

    // View
    MTKView *_view;

    // Renderer
    CAMetalLayer *_metalLayer;
    id<MTLDevice> _device;
    id<MTLLibrary> _library;
    id<MTLRenderPipelineState> _pipeline;
    id<MTLCommandQueue> _commandQueue;
    id<MTLBuffer> _positionBuffer;
    id<MTLBuffer> _colorBuffer;

    // Texture
    id <MTLTexture> _texture;
    id<MTLSamplerState> _sampler;

    // Display link
    CVDisplayLinkRef displayLink;
}

- (void)viewDidLoad {
    
    NSLog(@"MetalViewController::viewDidLoad");
    
    [super viewDidLoad];
    
    [self buildMetal];
    [self buildAssets];
    [self buildPipeline];

    [self setupDisplayLink];
    
    [self reshape];

    NSLog(@"%@", _view);
}

- (void)buildMetal
{
    NSLog(@"MetalViewController::buildDevice");

    // View
    _view = (MTKView *)self.view;
    _view.delegate = self;
    _view.device = _device;
    
    // Device
    _device = MTLCreateSystemDefaultDevice();

    // Layer
    _metalLayer = (CAMetalLayer *)[_view layer];
    _metalLayer.device = _device;
    _metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;

    // Queue
    _commandQueue = [_device newCommandQueue];

    // Shader library
    _library = [_device newDefaultLibrary];
}

- (void)buildPipeline
{
    NSLog(@"MetalViewController::buildPipeline");

    id<MTLFunction> vertexFunc = [_library newFunctionWithName:@"vertex_main"];
    id<MTLFunction> fragmentFunc = [_library newFunctionWithName:@"fragment_main"];
    
    MTLRenderPipelineDescriptor *pipelineDescriptor = [MTLRenderPipelineDescriptor new];
    pipelineDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    pipelineDescriptor.vertexFunction = vertexFunc;
    pipelineDescriptor.fragmentFunction = fragmentFunc;
    
    NSError *error = nil;
    _pipeline = [_device newRenderPipelineStateWithDescriptor:pipelineDescriptor
                                                        error:&error];
    
    if (!_pipeline)
    {
        NSLog(@"Error occurred when creating render pipeline state: %@", error);
    }
    
    // _commandQueue = [_device newCommandQueue];
}

- (void)buildAssets
{
    // Vertex buffers
    
    static const float positions[] =
    {
        0.0,  0.5, 0, 1,     0.0, 0.0,
        -0.5, -0.5, 0, 1,    1.0, 0.0,
        0.5, -0.5, 0, 1,     1.1, 1.1,
    };
    
    static const float colors[] =
    {
        1, 0, 0, 1,
        0, 1, 0, 1,
        0, 0, 1, 1,
    };
    
    _positionBuffer = [_device newBufferWithBytes:positions
                                           length:sizeof(positions)
                                          options:MTLResourceOptionCPUCacheModeDefault];
    _colorBuffer = [_device newBufferWithBytes:colors
                                        length:sizeof(colors)
                                       options:MTLResourceOptionCPUCacheModeDefault];

    // Textures
    //create Texturedescriptor (blueprint for texture)
    MTLTextureDescriptor *mtlTextDesc =
    [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                       width:1024
                                                      height:1024
                                                   mipmapped:NO];
    
    _texture = [_device newTextureWithDescriptor:mtlTextDesc];

    // Sampler
    MTLSamplerDescriptor *samplerDescriptor = [MTLSamplerDescriptor new];
    samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
    samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
    samplerDescriptor.sAddressMode = MTLSamplerAddressModeClampToEdge;
    samplerDescriptor.tAddressMode = MTLSamplerAddressModeClampToEdge;

    samplerDescriptor.mipFilter = MTLSamplerMipFilterNotMipmapped;
    _sampler = [_device newSamplerStateWithDescriptor:samplerDescriptor];
}

- (void)setupDisplayLink
{
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

    NSLog(@"Display link for metal drawing activated");
}

- (void)render {
    
    /* "A drawable is little more than a wrapper around a texture. Each time we draw, we will ask our Metal layer for a drawable object, from which we can extract a texture that acts as our framebuffer."
     */
    id<CAMetalDrawable> drawable = [_metalLayer nextDrawable];
    id<MTLTexture> framebufferTexture = drawable.texture;
    
    /* "A render pass descriptor tells Metal what actions to take while an image is being rendered. At the beginning of the render pass, the loadAction determines whether the previous contents of the texture are cleared or retained. The storeAction determines what effect the rendering has on the texture: the results may either be stored or discarded. Since we want our pixels to wind up on the screen, we select our store action to be MTLStoreActionStore.
        The pass descriptor is also where we choose which color the screen will be cleared to 
        before we draw any geometry. "
     */
    MTLRenderPassDescriptor *renderPass = [MTLRenderPassDescriptor renderPassDescriptor];
    renderPass.colorAttachments[0].texture = framebufferTexture;
    renderPass.colorAttachments[0].clearColor = MTLClearColorMake(0.5, 0.5, 0.5, 1);
    renderPass.colorAttachments[0].storeAction = MTLStoreActionStore;
    renderPass.colorAttachments[0].loadAction = MTLLoadActionClear;

    // "A command queue is an object that keeps a list of render command buffers to be executed."
    id<MTLCommandQueue> commandQueue = [_device newCommandQueue];
    
    // "A command buffer represents a collection of render commands to be executed as a unit."
    id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
    
    /* "A command encoder is an object that is used to tell Metal what drawing we actually want to do. It is responsible for translating these high-level commands (set these shader parameters, draw these triangles, etc.) into low-level instructions that are then written into its corresponding command buffer. Once we have issued all of our draw calls (which we aren’t doing in this post), we send the endEncoding message to the command encoder so it has the chance to finish its encoding.
    */
    id<MTLRenderCommandEncoder> commandEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPass];
    [commandEncoder setRenderPipelineState:_pipeline];
    
    [commandEncoder setFragmentTexture:_texture atIndex:0];
    [commandEncoder setFragmentSamplerState:_sampler atIndex:0];
    
    [commandEncoder setVertexBuffer:_positionBuffer offset:0 atIndex:0 ];
    // [commandEncoder setVertexBuffer:_colorBuffer offset:0 atIndex:1 ];
    [commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3 instanceCount:1];
    [commandEncoder endEncoding];

    /* "As its last action, the command buffer will signal that its drawable will be ready to be shown on-screen once all preceding commands are complete. Then, we call commit to indicate that this command buffer is complete and ready to be placed in command queue for execution on the GPU." */
    [commandBuffer presentDrawable:drawable];
    [commandBuffer commit];

    
    
#if 0
    dispatch_semaphore_wait(_inflightSemaphore, DISPATCH_TIME_FOREVER);
    
    // Perofm any app logic, including updating any Metal buffers.
    [self update];
    
    // Create a new command buffer for each renderpass to the current drawable.
    id <MTLCommandBuffer> commandBuffer = [_commandQueue commandBuffer];
    commandBuffer.label = @"Main Command Buffer";
    
    // Obtain a renderPassDescriptor generated from the view's drawable textures.
    MTLRenderPassDescriptor* renderPassDescriptor = _view.currentRenderPassDescriptor;
    
    // Create a render command encoder so we can render into something.
    id <MTLRenderCommandEncoder> renderEncoder =
    [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    
    renderEncoder.label = @"Final Pass Encoder";
    
    // Set context state.
    [renderEncoder setViewport:{0, 0, _view.drawableSize.width, _view.drawableSize.height, 0, 1}];
    [renderEncoder setDepthStencilState:_depthState];
    [renderEncoder setRenderPipelineState:_pipelineState];
    
    // Set the our per frame uniforms.
    [renderEncoder setVertexBuffer:_frameUniformBuffers[_constantDataBufferIndex]
                            offset:0
                           atIndex:AAPLFrameUniformBuffer];
    
    [renderEncoder pushDebugGroup:@"Render Meshes"];
    
    // Render each of our meshes.
    for(AAPLMetalKitEssentialsMesh *mesh in _meshes) {
        [mesh renderWithEncoder:renderEncoder];
    }
    
    [renderEncoder popDebugGroup];
    
    // We're done encoding commands.
    [renderEncoder endEncoding];
    
    /*
     Call the view's completion handler which is required by the view since
     it will signal its semaphore and set up the next buffer.
     */
    __block dispatch_semaphore_t block_sema = _inflightSemaphore;
    [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
        dispatch_semaphore_signal(block_sema);
    }];
    
    /*
     The renderview assumes it can now increment the buffer index and that
     the previous index won't be touched until we cycle back around to the same index.
     */
    _constantDataBufferIndex = (_constantDataBufferIndex + 1) % AAPLBuffersInflightBuffers;
    
    // Schedule a present once the framebuffer is complete using the current drawable.
    [commandBuffer presentDrawable:_view.currentDrawable];
    
    // Finalize rendering here & push the command buffer to the GPU.
    [commandBuffer commit];
#endif
    
}

- (void)reshape {
#if 0
    /*
     When reshape is called, update the view and projection matricies since
     this means the view orientation or size changed.
     */
    float aspect = fabs(self.view.bounds.size.width / self.view.bounds.size.height);
    _projectionMatrix = matrix_from_perspective_fov_aspectLH(65.0f * (M_PI / 180.0f), aspect, 0.1f, 100.0f);
    
    _viewMatrix = matrix_identity_float4x4;
#endif
}

- (void)update {
#if 0
    AAPLFrameUniforms *frameData = (AAPLFrameUniforms *)[_frameUniformBuffers[_constantDataBufferIndex] contents];
    
    frameData->model = matrix_from_translation(0.0f, 0.0f, 2.0f) * matrix_from_rotation(_rotation, 1.0f, 1.0f, 0.0f);    frameData->view = _viewMatrix;
    
    matrix_float4x4 modelViewMatrix = frameData->view * frameData->model;
    
    frameData->projectionView = _projectionMatrix * modelViewMatrix;
    
    frameData->normal = matrix_invert(matrix_transpose(modelViewMatrix));
    
    _rotation += 0.05f;
#endif
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
#if 0
    [self reshape];
#endif
}

- (void)drawInMTKView:(nonnull MTKView *)view {
#if 0
    @autoreleasepool {
        [self render];
    }
#endif
}


- (CVReturn)getFrameForTime:(const CVTimeStamp*)timeStamp flagsOut:(CVOptionFlags*)flagsOut
{
    @autoreleasepool {
        
        // Update angles for screen animation
        // [self updateAngles];
        
        // Draw scene
        // [self drawRect:NSZeroRect];
        [self render];
        
        return kCVReturnSuccess;
    }
}





@end
