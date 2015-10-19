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
    
    CAMetalLayer *_metalLayer;
    CGFloat layerWidth;
    CGFloat layerHeight;
    
    id <MTLRenderPipelineState> _pipeline;
    id <MTLDepthStencilState> _depthState;
    id <MTLCommandBuffer> _commandBuffer;
    id <MTLRenderCommandEncoder> _commandEncoder;
    id <MTLBuffer> _positionBuffer;
    id <MTLBuffer> _colorBuffer;

    id <CAMetalDrawable> _drawable;

    // Uniforms
    id <MTLBuffer> _uniformBuffer;
    id <MTLBuffer> _uniformBufferBg;
    matrix_float4x4 _modelMatrix;
    matrix_float4x4 _viewMatrix;
    matrix_float4x4 _projectionMatrix;
    matrix_float4x4 _modelViewProjectionMatrix;
    
    // Display link
    CVDisplayLinkRef displayLink;
    
    //
    BOOL _pipelineIsDirty;
}

// -----------------------------------------------------------------------------------------------
//                                          Configuration
// -----------------------------------------------------------------------------------------------

@synthesize enableMetal;

- (bool)drawInEntireWindow { return drawInEntireWindow; }
- (void)setDrawInEntireWindow:(bool)b
{
    if (drawInEntireWindow == b)
        return;
    
    NSRect r = self.frame;
    float borderThickness;
    if (b) {
        NSLog(@"Expanding Metal view");
        r.origin.y -= 24;
        r.size.height += 24;
        borderThickness = 0.0;
    } else {
        NSLog(@"Shrinking Metal view");
        r.origin.y += 24;
        r.size.height -= 24;
        borderThickness = 24.0;
    }
    
    self.frame = r;
    [[self window] setContentBorderThickness:borderThickness forEdge: NSMinYEdge];

     drawInEntireWindow = b;
}
    
- (bool)drawIn3D { return drawIn3D; }
- (void)setDrawIn3D:(bool)b { drawIn3D = b; }

- (bool)drawC64texture { return drawC64texture; }
- (void)setDrawC64texture:(bool)b { drawC64texture = b; }

- (bool)drawEntireCube { return drawEntireCube; }
- (void)setDrawEntireCube:(bool)b { drawEntireCube = b; }

- (bool)textureFilter { return filter; }
- (void)setTextureFilter:(TextureFilterType)type { filter = type; }


// -----------------------------------------------------------------------------------------------
//                                  Initialization (General)
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
    
    // Create lock used by the draw method
    lock = [NSRecursiveLock new];

    // Set initial scene position and drawing properties
    currentEyeX = targetEyeX = deltaEyeX = 0.0;
    currentEyeY = targetEyeY = deltaEyeY = 0.0;
    currentEyeZ = targetEyeZ = deltaEyeZ = 0.0;
    currentXAngle = targetXAngle = deltaXAngle = 0.0;
    currentYAngle = targetYAngle = deltaYAngle = 0.0;
    currentZAngle = targetZAngle = deltaZAngle = 0.0;
    currentAlpha = targetAlpha = 0.0; deltaAlpha = 0.0;
    
    enableMetal = true; 
    drawInEntireWindow = false;
    drawIn3D = true;
    drawC64texture = false;
    drawBackground = true;
    drawEntireCube = false;
    filter = TEX_FILTER_BLUR;
    
    // Core video and graphics stuff
    displayLink = nil;

    // Metal related stuff
    _pipelineIsDirty = YES;
    _depthTexture = nil;
    
    [self buildMetal];
    [self buildTextures];
    [self buildKernels];
    [self buildBuffers];
    [self reshape];
    
    // Keyboard initialization
    for (int i = 0; i < 256; i++) {
        pressedKeys[i] = 0;
    }
    
    // Register for drag and drop
    [self registerForDraggedTypes:
     [NSArray arrayWithObjects:NSFilenamesPboardType,NSFileContentsPboardType,nil]];

    // Fire off timer...
    [self setupDisplayLink];
}

- (void)buildMetal
{
    NSLog(@"MyMetalView::buildMetal");
    
    // Get metal device
    _device = MTLCreateSystemDefaultDevice();
    if (!_device) {
        NSLog(@"Error: No metal device");
        return;
    }

    // Configure view
    // view.depthPixelFormat   = MTLPixelFormatInvalid;
    // view.stencilPixelFormat = MTLPixelFormatInvalid;
    self.sampleCount = 1; // 4;

    // Get layer
    _metalLayer = (CAMetalLayer *)self.layer;
    _metalLayer.device = _device;
    _metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    _metalLayer.framebufferOnly = NO; // YES;
    
    
    layerWidth = _metalLayer.drawableSize.width;
    layerHeight = _metalLayer.drawableSize.height;
    
    // Create command queue
    _commandQueue = [_device newCommandQueue];
    if (!_commandQueue) {
        NSLog(@"Could not create command queue");
        exit(0);
    }
    
    // Load shader library
    _library = [_device newDefaultLibrary];
    if (!_library) {
        NSLog(@"Could not create shader library");
        exit(0);
    }
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
    
    if (lock) {
        lock = nil;
    }
}


// -----------------------------------------------------------------------------------------------
//                                 Initialization (Textures)
// -----------------------------------------------------------------------------------------------

- (void)buildTextures
{
    NSLog(@"MyMetalView::buildTextures");
    
    [self buildBackgroundTexture];
    [self buildC64Texture];
}

- (void)buildBackgroundTexture
{
    NSLog(@"MyMetalView::buildBackgroundTexture");
    
    NSURL *url = [[NSWorkspace sharedWorkspace] desktopImageURLForScreen:[NSScreen mainScreen]];
    NSImage *bgImage = [[NSImage alloc] initWithContentsOfURL:url];
    NSImage *bgImageResized = [self expandImage:bgImage toSize:NSMakeSize(BG_TEXTURE_WIDTH,BG_TEXTURE_HEIGHT)];
    _bgTexture = [self makeTexture:bgImageResized withDevice:_device];
}

- (void)buildC64Texture
{
    NSLog(@"MyMetalView::buildC64Texture");

    MTLTextureDescriptor *textureDescriptor =
    [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                       width:512
                                                      height:512
                                                   mipmapped:NO];
    _texture = [_device newTextureWithDescriptor:textureDescriptor];
    
    textureDescriptor.usage |= MTLTextureUsageShaderWrite;
    _filteredTexture = [_device newTextureWithDescriptor:textureDescriptor];
}

- (void)buildDepthBuffer
{
    NSLog(@"MyMetalView::buildDepthBuffer");
    
    MTLTextureDescriptor *depthTexDesc =
    [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                                       width:(layerWidth == 0) ? 512 : layerWidth
                                                      height:(layerHeight == 0) ? 512 : layerHeight
                                                   mipmapped:NO];
    {
        depthTexDesc.resourceOptions = MTLResourceStorageModePrivate;
        depthTexDesc.usage = MTLTextureUsageRenderTarget;
    }
    _depthTexture = [_device newTextureWithDescriptor:depthTexDesc];
}

- (void)buildBuffers
{
    // Vertex buffer
    _positionBuffer = [self buildVertexBuffer:_device];

    // Uniform buffer
    _uniformBuffer = [_device newBufferWithLength:sizeof(Uniforms) options:0];
    _uniformBufferBg = [_device newBufferWithLength:sizeof(Uniforms) options:0];
}

- (void) buildKernels
{
    bypassFilter = [BypassFilter filterWithDevice:_device library:_library];
    smoothFilter = [SaturationFilter filterWithFactor:1.0 device:_device library:_library];
    blurFilter = [BlurFilter filterWithRadius:2.0 device:_device library:_library];
    saturationFilter = [SaturationFilter filterWithFactor:0.5 device:_device library:_library];
    sepiaFilter = [SepiaFilter filterWithDevice:_device library:_library];
    crtFilter = [CrtFilter filterWithDevice:_device library:_library];
    grayscaleFilter = [SaturationFilter filterWithFactor:0.0 device:_device library:_library];
}

- (void)buildDepthStencilState
{
    MTLDepthStencilDescriptor *depthDescriptor = [MTLDepthStencilDescriptor new];
    {
        // depthDescriptor.depthCompareFunction = MTLCompareFunctionAlways;
        depthDescriptor.depthCompareFunction = MTLCompareFunctionLess;
        depthDescriptor.depthWriteEnabled = YES;
    }
    _depthState = [_device newDepthStencilStateWithDescriptor:depthDescriptor];
    if (_depthState) {
        NSLog(@"Failed to create depth stencil state");
        exit(0);
    }
}

- (void)buildPipeline
{
    NSError *error = nil;
    
    NSLog(@"MyMetalView::buildPipeline");
    
    id<MTLFunction> vertexFunc = [_library newFunctionWithName:@"vertex_main"];
    id<MTLFunction> fragmentFunc = [_library newFunctionWithName:@"fragment_main"];
    assert(vertexFunc != nil);
    assert(fragmentFunc != nil);

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
    MTLRenderPipelineColorAttachmentDescriptor *renderbufAttachment = pipelineDescriptor.colorAttachments[0];
    {
        pipelineDescriptor.label = @"C64 metal pipeline";

        renderbufAttachment.pixelFormat = MTLPixelFormatBGRA8Unorm;
        renderbufAttachment.blendingEnabled = YES;
        renderbufAttachment.rgbBlendOperation = MTLBlendOperationAdd;
        renderbufAttachment.alphaBlendOperation = MTLBlendOperationAdd;
        renderbufAttachment.sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
        renderbufAttachment.sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
        renderbufAttachment.destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
        renderbufAttachment.destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
        
        pipelineDescriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
        // pipelineDescriptor.sampleCount = self.sampleCount;
        pipelineDescriptor.vertexFunction = vertexFunc;
        pipelineDescriptor.fragmentFunction = fragmentFunc;
        // pipelineDescriptor.vertexDescriptor = vertexDescriptor;
    }
    _pipeline = [_device newRenderPipelineStateWithDescriptor:pipelineDescriptor
                                                        error:&error];
    if (!_pipeline) {
        NSLog(@"Error occurred when creating render pipeline: %@", error);
        exit(0);
    }
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

- (void)reshape
{
/*
    NSLog(@"%f %f %f %f %f %f", drawableSize.width, drawableSize.height, width, height,
 _metalLayer.drawableSize.width, _metalLayer.drawableSize.height);
    NSLog(@"Framebuffer width:%d height:%d w:%f h:%f",
          [_framebufferTexture width], [_framebufferTexture height],
          self.bounds.size.width, self.bounds.size.height);
*/

    // Update projection matrix
    // float aspect = fabs(self.bounds.size.width / self.bounds.size.height);
    float aspect = fabs(layerWidth / layerHeight);
    _projectionMatrix = vc64_matrix_from_perspective_fov_aspectLH(65.0f * (M_PI / 180.0f), aspect, 0.1f, 100.0f);    
    _viewMatrix = matrix_identity_float4x4;
    
    // Update matrices for background drawing
    _modelMatrix = matrix_identity_float4x4;
    _modelViewProjectionMatrix = _projectionMatrix * _viewMatrix * _modelMatrix;
    Uniforms *frameDataBg = (Uniforms *)[_uniformBufferBg contents];
    frameDataBg->model = _modelMatrix;
    frameDataBg->view = _viewMatrix;
    frameDataBg->projectionView = _modelViewProjectionMatrix;
    frameDataBg->alpha = 1.0;

    // Rebuild depth buffer and tmp drawing buffer
    [self buildDepthBuffer];
}

- (void)buildMatrices2D
{
    _modelMatrix = vc64_matrix_from_translation(0, 0, 0);
    _viewMatrix = matrix_identity_float4x4;
    _projectionMatrix = matrix_identity_float4x4;
    _modelViewProjectionMatrix = _projectionMatrix * _viewMatrix * _modelMatrix;
    
    Uniforms *frameData = (Uniforms *)[_uniformBuffer contents];
    frameData->model = _modelMatrix;
    frameData->view = _viewMatrix;
    frameData->projectionView = _modelViewProjectionMatrix;
    frameData->alpha = 1.0;
}

- (void)buildMatrices
{
    // _modelMatrix = vc64_matrix_from_translation(-currentEyeX, -currentEyeY, currentEyeZ+1.35);
    _modelMatrix = vc64_matrix_from_translation(-currentEyeX, -currentEyeY, currentEyeZ+1.39);
    
    if ([self animates]) {
        _modelMatrix = _modelMatrix *
        vc64_matrix_from_rotation(-(currentXAngle / 180.0)*M_PI, 0.5f, 0.0f, 0.0f) *
        vc64_matrix_from_rotation((currentYAngle / 180.0)*M_PI, 0.0f, 0.5f, 0.0f) *
        vc64_matrix_from_rotation((currentZAngle / 180.0)*M_PI, 0.0f, 0.0f, 0.5f);
    }
    
    _modelViewProjectionMatrix = _projectionMatrix * _viewMatrix * _modelMatrix;

    Uniforms *frameData = (Uniforms *)[_uniformBuffer contents];
    frameData->model = _modelMatrix;
    frameData->view = _viewMatrix;
    frameData->projectionView = _modelViewProjectionMatrix;
    frameData->alpha = (c64->isHalted()) ? 0.5 : currentAlpha;
}

- (void)startFrame
{
    CGSize drawableSize = _metalLayer.drawableSize;
    
    // if (!_depthTexture || _depthTexture.width != drawableSize.width || _depthTexture.height != drawableSize.height) {
    
    // Update all size dependent entities
    if (layerWidth != drawableSize.width || layerHeight != drawableSize.height) {
        layerWidth = drawableSize.width;
        layerHeight = drawableSize.height;
        [self reshape];
    }
    
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
    
    // "A command buffer represents a collection of render commands to be executed as a unit."
    _commandBuffer = [_commandQueue commandBuffer];
    
    // Apply filter to C64 screen texture
    TextureFilter *flt = [self currentFilter];
    [flt apply:_commandBuffer in:_texture out:_filteredTexture];
    
    // Create render pass
    /* "A render pass descriptor tells Metal what actions to take while an image is being rendered" */
    MTLRenderPassDescriptor *renderPass = [MTLRenderPassDescriptor renderPassDescriptor];
    {
        renderPass.colorAttachments[0].texture = _framebufferTexture;
        renderPass.colorAttachments[0].clearColor = MTLClearColorMake(1.0, 0, 0, 1);
        renderPass.colorAttachments[0].loadAction = MTLLoadActionClear;
        renderPass.colorAttachments[0].storeAction = MTLStoreActionStore;
        
        renderPass.depthAttachment.texture = _depthTexture;
        renderPass.depthAttachment.clearDepth = 1;
        renderPass.depthAttachment.loadAction = MTLLoadActionClear;
        renderPass.depthAttachment.storeAction = MTLStoreActionDontCare;
    }
    
    // "A command encoder is an object that is used to tell Metal what drawing we actually want to do."
    _commandEncoder = [_commandBuffer renderCommandEncoderWithDescriptor:renderPass];
    {
        [_commandEncoder setRenderPipelineState:_pipeline];
        [_commandEncoder setDepthStencilState:_depthState];
        [_commandEncoder setFragmentTexture:_bgTexture atIndex:0];
        [_commandEncoder setFragmentSamplerState:[flt sampler] atIndex:0];
        [_commandEncoder setVertexBuffer:_positionBuffer offset:0 atIndex:0];
        [_commandEncoder setVertexBuffer:_uniformBuffer offset:0 atIndex:1];
    }
}

- (TextureFilter *)currentFilter
{
    switch (filter) {
        case TEX_FILTER_NONE:
            return bypassFilter;
        case TEX_FILTER_SMOOTH:
            return smoothFilter;
        case TEX_FILTER_BLUR:
            return blurFilter;
        case TEX_FILTER_SATURATION:
            return saturationFilter;
        case TEX_FILTER_SEPIA:
            return sepiaFilter;
        case TEX_FILTER_GRAYSCALE:
            return grayscaleFilter;
        case TEX_FILTER_CRT:
            return crtFilter;
        default:
            assert(0);
    }
    return nil;
}

- (void)applyFilter
{
    switch (filter) {
        case TEX_FILTER_NONE:
            [bypassFilter apply:_commandBuffer in:_texture out:_filteredTexture];
            break;
            
        case TEX_FILTER_BLUR:
            [blurFilter apply:_commandBuffer in:_texture out:_filteredTexture];
            break;

        case TEX_FILTER_SATURATION:
            [saturationFilter apply:_commandBuffer in:_texture out:_filteredTexture];
            break;
        
        case TEX_FILTER_SEPIA:
            [sepiaFilter apply:_commandBuffer in:_texture out:_filteredTexture];
            break;

        case TEX_FILTER_GRAYSCALE:
            [grayscaleFilter apply:_commandBuffer in:_texture out:_filteredTexture];
            break;

        case TEX_FILTER_CRT:
            [crtFilter apply:_commandBuffer in:_texture out:_filteredTexture];
            break;

        default:
            assert(0);
    }
}

- (void)drawScene2D
{
    [self buildMatrices2D];
    
    [self startFrame];
    
    // Render quad
    [_commandEncoder setFragmentTexture:_filteredTexture atIndex:0];
    [_commandEncoder setVertexBuffer:_uniformBuffer offset:0 atIndex:1];
    [_commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:42 vertexCount:6 instanceCount:1];
    
    [self endFrame];
}

- (void)drawScene3D
{
    bool animates = [self animates];
    
    drawEntireCube = animates;
    drawBackground = true; // animates || !drawC64texture || halted;
    
    if (animates) {
        [self updateAngles];
    }
    [self buildMatrices];
    
    [self startFrame];
    
    // Render background
    if (drawBackground) {
        [_commandEncoder setFragmentTexture:_bgTexture atIndex:0];
        [_commandEncoder setVertexBuffer:_uniformBufferBg offset:0 atIndex:1];
        [_commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:6 instanceCount:1];
    }
    
    // Render cube
    if (drawC64texture) {
        [_commandEncoder setFragmentTexture:_filteredTexture atIndex:0];
        [_commandEncoder setVertexBuffer:_uniformBuffer offset:0 atIndex:1];
        [_commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:6 vertexCount:(drawEntireCube ? 36 : 6) instanceCount:1];
    }
    
    [self endFrame];
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

- (CVReturn)getFrameForTime:(const CVTimeStamp*)timeStamp flagsOut:(CVOptionFlags*)flagsOut
{
    @autoreleasepool {
        
        if (!c64 || !enableMetal)
            return kCVReturnSuccess;
        
        [lock lock];
        
        // Get latest C64 texture
        [self updateTexture:_commandBuffer];

        // Draw scene
        if (drawIn3D) {
            [self drawScene3D];
        } else {
            [self drawScene2D];
        }
        
        [lock unlock];
        return kCVReturnSuccess;
    }
}



@end
