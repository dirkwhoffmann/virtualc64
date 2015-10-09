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

@implementation MetalViewController {

    // View
    MTKView *_view;

}

- (void)viewDidLoad {
    
    NSLog(@"***** METAL VIEW ***** viewDidLoad");
    NSLog(@"%@", _view);
    
    [super viewDidLoad];
    
    // [self setupMetal];
    // [self setupView];
    // [self loadAssets];
    // [self reshape];
}

- (void)render {
    
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
    @autoreleasepool {
        [self render];
    }
}




@end
