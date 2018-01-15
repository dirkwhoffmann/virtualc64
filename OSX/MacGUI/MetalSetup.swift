//
//  MetalSetup.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 15.01.18.
//

import Foundation

public extension MyMetalView {

    @objc public func setupMetal() {

        NSLog("MyMetalView::setupMetal")
        
        self.buildMetal()
        self.buildTextures()
        self.buildKernels()
        self.buildBuffers()
        self.buildPipeline()
        
        self.reshape(withFrame: self.frame)
        enableMetal = true
    }
    
    @objc public func buildMetal() {
    
    NSLog("MyMetalView::buildMetal")
    
    // Metal device
    device = MTLCreateSystemDefaultDevice()
    precondition(device != nil, "Metal device must not be nil")
    
    // Metal layer
    metalLayer = self.layer as! CAMetalLayer
    precondition(metalLayer != nil, "Metal layer must not be nil")
        
    metalLayer.device = device
    metalLayer.pixelFormat = MTLPixelFormat.bgra8Unorm
    metalLayer.framebufferOnly = true
    metalLayer.frame = metalLayer.frame
    layerWidth = metalLayer.drawableSize.width
    layerHeight = metalLayer.drawableSize.height
    
    // Command queue
    queue = device?.makeCommandQueue()
    precondition(queue != nil, "Metal command queue must not be nil")
    
    // Shader library
    library = device?.makeDefaultLibrary()
    precondition(library != nil, "Metal library must not be nil")
    
    // View parameters
    self.sampleCount = 1
    }
    
    func buildTextures() {

        NSLog("MyMetalView::buildTextures")
        precondition(device != nil)

        let readWriteUsage =
            MTLTextureUsage.shaderRead.rawValue | MTLTextureUsage.shaderWrite.rawValue
        
        // Build background texture (drawn behind the cube)
        bgTexture = self.createBackgroundTexture()
    
        // Build C64 texture (as provided by the emulator)
        var descriptor = MTLTextureDescriptor.texture2DDescriptor(
            pixelFormat: MTLPixelFormat.rgba8Unorm,
            width: 512,
            height: 512,
            mipmapped: false)
        descriptor.usage = MTLTextureUsage.shaderRead
        emulatorTexture = device?.makeTexture(descriptor: descriptor)
        precondition(emulatorTexture != nil, "Failed to create emulator texture")
        
        // Upscaled C64 texture
        descriptor = MTLTextureDescriptor.texture2DDescriptor(
            pixelFormat: MTLPixelFormat.rgba8Unorm,
            width: 2048,
            height: 2048,
            mipmapped: false)
        descriptor.usage = MTLTextureUsage(rawValue: readWriteUsage)
        upscaledTexture = device?.makeTexture(descriptor: descriptor)
        precondition(upscaledTexture != nil, "Failed to create upscaling texture")
    
        // Final texture (upscaled and filtered)
        filteredTexture = device?.makeTexture(descriptor: descriptor)
        precondition(filteredTexture != nil, "Failed to create filtering texture")
    }
    
    @objc func buildBuffers() {
    
        // Vertex buffer
        buildVertexBuffer()
    
        // Uniform buffers
        
        // struct Uniforms {
        //     float4x4 modelViewProjection;     4 bytes * 16
        //     float    alpha;                +  4 bytes
        //     uint8    _pad[15]              + 15 bytes
        // };                                 ----------
        //                                    = 80 bytes

        let len = 80
        let opt = MTLResourceOptions.cpuCacheModeWriteCombined
        
        uniformBuffer2D = device?.makeBuffer(length: len, options: opt)
        uniformBuffer3D = device?.makeBuffer(length: len, options: opt)
        uniformBufferBg = device?.makeBuffer(length: len, options: opt)
        
        precondition(uniformBuffer2D != nil, "uniformBuffer2D must not be nil")
        precondition(uniformBuffer3D != nil, "uniformBuffer3D must not be nil")
        precondition(uniformBufferBg != nil, "uniformBufferBg must not be nil")
        
    }
    
    @objc public func buildVertexBuffer() {
    
        if device == nil {
            return
        }
        
        let capacity = 16 * 3 * 8
        let pos = UnsafeMutablePointer<Float>.allocate(capacity: capacity)
        
        func setVertex(_ i: Int, _ position: float3, _ texture: float2) {
            
            let first = i * 8
            pos[first + 0] = position.x
            pos[first + 1] = position.y
            pos[first + 2] = position.z
            pos[first + 3] = 1.0 /* alpha */
            pos[first + 4] = texture.x
            pos[first + 5] = texture.y
            pos[first + 6] = 0.0 /* alignment byte (unused) */
            pos[first + 7] = 0.0 /* alignment byte (unused) */
        }
        
        let dx = Float(0.64)
        let dy = Float(0.48)
        let dz = Float(0.64)
        let bgx = Float(6.4)
        let bgy = Float(4.8)
        let bgz = Float(-6.8)
    
        // Background
        setVertex(0, float3(-bgx, +bgy, -bgz), float2(0.0, 0.0))
        setVertex(1, float3(-bgx, -bgy, -bgz), float2(0.0, 1.0))
        setVertex(2, float3(+bgx, -bgy, -bgz), float2(1.0, 1.0))
    
        setVertex(3, float3(-bgx, +bgy, -bgz), float2(0.0, 0.0))
        setVertex(4, float3(+bgx, +bgy, -bgz), float2(1.0, 0.0))
        setVertex(5, float3(+bgx, -bgy, -bgz), float2(1.0, 1.0))
    
        // -Z
        setVertex(6, float3(-dx, +dy, -dz), float2(textureXStart, textureYStart))
        setVertex(7, float3(-dx, -dy, -dz), float2(textureXStart, textureYEnd))
        setVertex(8, float3(+dx, -dy, -dz), float2(textureXEnd, textureYEnd))
    
        setVertex(9, float3(-dx, +dy, -dz), float2(textureXStart, textureYStart))
        setVertex(10, float3(+dx, +dy, -dz), float2(textureXEnd, textureYStart))
        setVertex(11, float3(+dx, -dy, -dz), float2(textureXEnd, textureYEnd))
    
        // +Z
        setVertex(12, float3(-dx, +dy, +dz), float2(textureXEnd, textureYStart))
        setVertex(13, float3(-dx, -dy, +dz), float2(textureXEnd, textureYEnd))
        setVertex(14, float3(+dx, -dy, +dz), float2(textureXStart, textureYEnd))
    
        setVertex(15, float3(-dx, +dy, +dz), float2(textureXEnd, textureYStart))
        setVertex(16, float3(+dx, +dy, +dz), float2(textureXStart, textureYStart))
        setVertex(17, float3(+dx, -dy, +dz), float2(textureXStart, textureYEnd))
    
        // -X
        setVertex(18, float3(-dx, +dy, -dz), float2(textureXEnd, textureYStart))
        setVertex(19, float3(-dx, -dy, -dz), float2(textureXEnd, textureYEnd))
        setVertex(20, float3(-dx, -dy, +dz), float2(textureXStart, textureYEnd))
    
        setVertex(21, float3(-dx, +dy, -dz), float2(textureXEnd, textureYStart))
        setVertex(22, float3(-dx, +dy, +dz), float2(textureXStart, textureYStart))
        setVertex(23, float3(-dx, -dy, +dz), float2(textureXStart, textureYEnd))
    
        // +X
        setVertex(24, float3(+dx, +dy, -dz), float2(textureXStart, textureYStart))
        setVertex(25, float3(+dx, -dy, -dz), float2(textureXStart, textureYEnd))
        setVertex(26, float3(+dx, -dy, +dz), float2(textureXEnd, textureYEnd))
    
        setVertex(27, float3(+dx, +dy, -dz), float2(textureXStart, textureYStart))
        setVertex(28, float3(+dx, +dy, +dz), float2(textureXEnd, textureYStart))
        setVertex(29, float3(+dx, -dy, +dz), float2(textureXEnd, textureYEnd))
    
        // -Y
        setVertex(30, float3(+dx, -dy, -dz), float2(textureXStart, textureYStart))
        setVertex(31, float3(-dx, -dy, -dz), float2(textureXStart, textureYEnd))
        setVertex(32, float3(-dx, -dy, +dz), float2(textureXEnd, textureYEnd))
    
        setVertex(33, float3(+dx, -dy, -dz), float2(textureXStart, textureYStart))
        setVertex(34, float3(+dx, -dy, +dz), float2(textureXEnd, textureYStart))
        setVertex(35, float3(-dx, -dy, +dz), float2(textureXEnd, textureYEnd))
    
        // +Y
        setVertex(36, float3(+dx, +dy, -dz), float2(textureXStart, textureYStart))
        setVertex(37, float3(-dx, +dy, -dz), float2(textureXStart, textureYEnd))
        setVertex(38, float3(-dx, +dy, +dz), float2(textureXEnd, textureYEnd))
    
        setVertex(39, float3(+dx, +dy, -dz), float2(textureXStart, textureYStart))
        setVertex(40, float3(-dx, +dy, +dz), float2(textureXEnd, textureYEnd))
        setVertex(41, float3(+dx, +dy, +dz), float2(textureXEnd, textureYStart))
    
        // 2D drawing quad
        setVertex(42, float3(-1,  1,  0), float2(textureXStart, textureYStart))
        setVertex(43, float3(-1, -1,  0), float2(textureXStart, textureYEnd))
        setVertex(44, float3( 1, -1,  0), float2(textureXEnd, textureYEnd))
    
        setVertex(45, float3(-1,  1,  0), float2(textureXStart, textureYStart))
        setVertex(46, float3( 1,  1,  0), float2(textureXEnd, textureYStart))
        setVertex(47, float3( 1, -1,  0), float2(textureXEnd, textureYEnd))
    
        let opt = MTLResourceOptions.cpuCacheModeWriteCombined
        let len = capacity * 4
        positionBuffer = device?.makeBuffer(bytes: pos, length: len, options: opt)
        precondition(positionBuffer != nil, "positionBuffer must not be nil")
    }
 
    @objc public func buildDepthBuffer() {
        
        if device == nil {
            return
        }
        
        let width = (layerWidth < 2048) ? 2048 : Int(layerWidth)
        let height = (layerHeight < 2048) ? 2048 : Int(layerHeight)
        
        // Check if we need a new texture
        if depthTexture != nil {
            if (width < depthTexture.width && height < depthTexture.height) {
                return // Old texture is sufficiently large
            }
        }
        
        let descriptor = MTLTextureDescriptor.texture2DDescriptor(
            pixelFormat: MTLPixelFormat.depth32Float,
            width: width,
            height: height,
            mipmapped: false)
        descriptor.resourceOptions = MTLResourceOptions.storageModePrivate
        descriptor.usage = MTLTextureUsage.renderTarget
        
        depthTexture = device?.makeTexture(descriptor: descriptor)
        precondition(depthTexture != nil, "Failed to create depth texture")
    }
    
    @objc public func buildPipeline() {
    
        NSLog("MyMetalView::buildPipeline");
    
        precondition(device != nil)
        precondition(library != nil)
        
        // NSError *error = nil;
        
        // Get vertex and fragment shader from library
        let vertexFunc = library.makeFunction(name: "vertex_main")
        let fragmentFunc = library.makeFunction(name: "fragment_main")
        precondition(vertexFunc != nil)
        precondition(fragmentFunc != nil)

        // Create depth stencil state
        let stencilDescriptor = MTLDepthStencilDescriptor.init()
        stencilDescriptor.depthCompareFunction = MTLCompareFunction.less
        stencilDescriptor.isDepthWriteEnabled = true
        depthState = device?.makeDepthStencilState(descriptor: stencilDescriptor)
        
        // Setup vertex descriptor
        let vertexDescriptor = MTLVertexDescriptor.init()
        
        // Positions
        vertexDescriptor.attributes[0].format = MTLVertexFormat.float4
        vertexDescriptor.attributes[0].offset = 0
        vertexDescriptor.attributes[0].bufferIndex = 0
    
        // Texture coordinates
        vertexDescriptor.attributes[1].format = MTLVertexFormat.half2
        vertexDescriptor.attributes[1].offset = 16
        vertexDescriptor.attributes[1].bufferIndex = 1
    
        // Single interleaved buffer
        vertexDescriptor.layouts[0].stride = 24
        vertexDescriptor.layouts[0].stepRate = 1
        vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunction.perVertex
    
        // Render pipeline
        let pipelineDescriptor = MTLRenderPipelineDescriptor.init()
        pipelineDescriptor.label = "VirtualC64 Metal pipeline"
        pipelineDescriptor.depthAttachmentPixelFormat = MTLPixelFormat.depth32Float
        pipelineDescriptor.vertexFunction = vertexFunc
        pipelineDescriptor.fragmentFunction = fragmentFunc
        
        // Color attachment
        let colorAttachment = pipelineDescriptor.colorAttachments[0]!
        colorAttachment.pixelFormat = MTLPixelFormat.bgra8Unorm
        colorAttachment.isBlendingEnabled = true
        colorAttachment.rgbBlendOperation = MTLBlendOperation.add
        colorAttachment.alphaBlendOperation = MTLBlendOperation.add
        colorAttachment.sourceRGBBlendFactor = MTLBlendFactor.sourceAlpha
        colorAttachment.sourceAlphaBlendFactor = MTLBlendFactor.sourceAlpha
        colorAttachment.destinationRGBBlendFactor = MTLBlendFactor.oneMinusSourceAlpha
        colorAttachment.destinationAlphaBlendFactor = MTLBlendFactor.oneMinusSourceAlpha
        do {
            try pipeline = device?.makeRenderPipelineState(descriptor: pipelineDescriptor)
        } catch {
            fatalError("Cannot create Metal graphics pipeline")
        }
    }
}
