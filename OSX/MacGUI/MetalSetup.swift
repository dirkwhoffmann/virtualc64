//
//  MetalSetup.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 15.01.18.
//

import Foundation
import simd

public extension MetalView {

    func checkForMetal() {
        
        guard let _ = MTLCreateSystemDefaultDevice() else {
            
            showNoMetalSupportAlert()
            NSApp.terminate(self)
            return
        }
    }
  
    public func setupMetal() {

        track()
        
        buildMetal()
        buildTextures()
        buildKernels()
        buildBuffers()
        buildPipeline()
        
        self.reshape(withFrame: self.frame)
        enableMetal = true
    }
    
    func buildMetal() {
    
        track()
            
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

        track()
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
    
    internal func buildKernels() {
        
        precondition(device != nil)
        precondition(library != nil)
        
        // Build upscalers
        upscaler[0] = BypassUpscaler.init(device: device!, library: library)
        upscaler[1] = EPXUpscaler.init(device: device!, library: library)
        upscaler[2] = XBRUpscaler.init(device: device!, library: library)
        
        bypassUpscaler = upscaler[0]
        epxUpscaler = upscaler[1]
        xbrUpscaler = upscaler[2]
        
        // Build filters
        filter[0] = BypassFilter.init(device: device!, library: library)
        filter[1] = SaturationFilter.init(device: device!, library: library, factor: 1.0)
        filter[2] = BlurFilter.init(device: device!, library: library, radius: 3.0)
        filter[3] = SaturationFilter.init(device: device!, library: library, factor: 0.5)
        filter[4] = SepiaFilter.init(device: device!, library: library)
        filter[5] = SaturationFilter.init(device: device!, library: library, factor: 0.0)
        filter[6] = CrtFilter.init(device: device!, library: library)

        bypassFilter = filter[0]
        smoothFilter = filter[1]
        blurFilter = filter[2]
        saturationFilter = filter[3]
        sepiaFilter = filter[4]
        grayscaleFilter = filter[5]
        crtFilter = filter[6]
    }
    
    func buildBuffers() {
    
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
    
    func fillMatrix(_ buffer: MTLBuffer?, _ matrix: simd_float4x4) {
        
        var _matrix = matrix
        if buffer != nil {
            let contents = buffer!.contents()
            memcpy(contents, &_matrix, 16 * 4)
        }
    }
    
    func fillAlpha(_ buffer: MTLBuffer?, _ alpha: Float) {
        
        var _alpha  = alpha
        if buffer != nil {
            let contents = buffer!.contents()
            memcpy(contents + 16 * 4, &_alpha, 4)
        }
    }
    
    func fillBuffer(_ buffer: MTLBuffer?, matrix: simd_float4x4, alpha: Float) {
        
        fillMatrix(buffer, matrix)
        fillAlpha(buffer, alpha)
    }
    
    func buildMatricesBg() {
        
        let model  = matrix_identity_float4x4
        let view   = matrix_identity_float4x4
        let aspect = Float(layerWidth) / Float(layerHeight)
        let proj   = matrix_from_perspective(fovY: (Float(65.0 * (.pi / 180.0))),
                                             aspect: aspect,
                                             nearZ: 0.1,
                                             farZ: 100.0)
        
        fillBuffer(uniformBufferBg, matrix: proj * view * model, alpha: 1.0)
    }
    
    func buildMatrices2D() {
    
        let model = matrix_identity_float4x4
        let view  = matrix_identity_float4x4
        let proj  = matrix_identity_float4x4
        
        fillBuffer(uniformBuffer2D, matrix: proj * view * model, alpha: 1.0)
    }
    
    func buildMatrices3D() {
    
        var model  = matrix_from_translation(x: -currentEyeX,
                                             y: -currentEyeY,
                                             z: currentEyeZ + 1.39)
        let view   = matrix_identity_float4x4
        let aspect = Float(layerWidth) / Float(layerHeight)
        let proj   = matrix_from_perspective(fovY: (Float(65.0 * (.pi / 180.0))),
                                             aspect: aspect,
                                             nearZ: 0.1,
                                             farZ: 100.0)
    
        if animates() {
            let xAngle: Float = -(currentXAngle / 180.0) * .pi;
            let yAngle: Float =  (currentYAngle / 180.0) * .pi;
            let zAngle: Float =  (currentZAngle / 180.0) * .pi;
    
            model = model *
                matrix_from_rotation(radians: xAngle, x: 0.5, y: 0.0, z: 0.0) *
                matrix_from_rotation(radians: yAngle, x: 0.0, y: 0.5, z: 0.0) *
                matrix_from_rotation(radians: zAngle, x: 0.0, y: 0.0, z: 0.5)
        }
        
        fillBuffer(uniformBuffer3D, matrix: proj * view * model, alpha: currentAlpha)
    }

    func buildVertexBuffer() {
    
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
        let upperLeft = float2(Float(textureRect.minX), Float(textureRect.maxY))
        let upperRight = float2(Float(textureRect.maxX), Float(textureRect.maxY))
        let lowerLeft = float2(Float(textureRect.minX), Float(textureRect.minY))
        let lowerRight = float2(Float(textureRect.maxX), Float(textureRect.minY))

        // Background
        setVertex(0, float3(-bgx, +bgy, -bgz), float2(0.0, 0.0))
        setVertex(1, float3(-bgx, -bgy, -bgz), float2(0.0, 1.0))
        setVertex(2, float3(+bgx, -bgy, -bgz), float2(1.0, 1.0))
    
        setVertex(3, float3(-bgx, +bgy, -bgz), float2(0.0, 0.0))
        setVertex(4, float3(+bgx, +bgy, -bgz), float2(1.0, 0.0))
        setVertex(5, float3(+bgx, -bgy, -bgz), float2(1.0, 1.0))
    
        // -Z
        setVertex(6, float3(-dx, +dy, -dz), lowerLeft)
        setVertex(7, float3(-dx, -dy, -dz), upperLeft)
        setVertex(8, float3(+dx, -dy, -dz), upperRight)
    
        setVertex(9, float3(-dx, +dy, -dz), lowerLeft)
        setVertex(10, float3(+dx, +dy, -dz), lowerRight)
        setVertex(11, float3(+dx, -dy, -dz), upperRight)
    
        // +Z
        setVertex(12, float3(-dx, +dy, +dz), lowerRight)
        setVertex(13, float3(-dx, -dy, +dz), upperRight)
        setVertex(14, float3(+dx, -dy, +dz), upperLeft)
    
        setVertex(15, float3(-dx, +dy, +dz), lowerRight)
        setVertex(16, float3(+dx, +dy, +dz), lowerLeft)
        setVertex(17, float3(+dx, -dy, +dz), upperLeft)
    
        // -X
        setVertex(18, float3(-dx, +dy, -dz), lowerRight)
        setVertex(19, float3(-dx, -dy, -dz), upperRight)
        setVertex(20, float3(-dx, -dy, +dz), upperLeft)
    
        setVertex(21, float3(-dx, +dy, -dz), lowerRight)
        setVertex(22, float3(-dx, +dy, +dz), lowerLeft)
        setVertex(23, float3(-dx, -dy, +dz), upperLeft)
    
        // +X
        setVertex(24, float3(+dx, +dy, -dz), lowerLeft)
        setVertex(25, float3(+dx, -dy, -dz), upperLeft)
        setVertex(26, float3(+dx, -dy, +dz), upperRight)
    
        setVertex(27, float3(+dx, +dy, -dz), lowerLeft)
        setVertex(28, float3(+dx, +dy, +dz), lowerRight)
        setVertex(29, float3(+dx, -dy, +dz), upperRight)
    
        // -Y
        setVertex(30, float3(+dx, -dy, -dz), lowerLeft)
        setVertex(31, float3(-dx, -dy, -dz), upperLeft)
        setVertex(32, float3(-dx, -dy, +dz), upperRight)
    
        setVertex(33, float3(+dx, -dy, -dz), lowerLeft)
        setVertex(34, float3(+dx, -dy, +dz), lowerRight)
        setVertex(35, float3(-dx, -dy, +dz), upperRight)
    
        // +Y
        setVertex(36, float3(+dx, +dy, -dz), lowerLeft)
        setVertex(37, float3(-dx, +dy, -dz), upperLeft)
        setVertex(38, float3(-dx, +dy, +dz), upperRight)
    
        setVertex(39, float3(+dx, +dy, -dz), lowerLeft)
        setVertex(40, float3(-dx, +dy, +dz), upperRight)
        setVertex(41, float3(+dx, +dy, +dz), lowerRight)
    
        // 2D drawing quad
        setVertex(42, float3(-1,  1,  0), lowerLeft)
        setVertex(43, float3(-1, -1,  0), upperLeft)
        setVertex(44, float3( 1, -1,  0), upperRight)
    
        setVertex(45, float3(-1,  1,  0), lowerLeft)
        setVertex(46, float3( 1,  1,  0), lowerRight)
        setVertex(47, float3( 1, -1,  0), upperRight)
    
        let opt = MTLResourceOptions.cpuCacheModeWriteCombined
        let len = capacity * 4
        positionBuffer = device?.makeBuffer(bytes: pos, length: len, options: opt)
        precondition(positionBuffer != nil, "positionBuffer must not be nil")
    }
 
    func buildDepthBuffer() {
        
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
    
    func buildPipeline() {

        track()
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
