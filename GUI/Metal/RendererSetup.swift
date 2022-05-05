// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import simd

struct TextureSize {
    
    static let background = MTLSizeMake(512, 512, 0)
    static let original = MTLSizeMake(TEX_WIDTH, TEX_HEIGHT, 0)
    static let upscaled = MTLSizeMake(4 * TEX_WIDTH, 4 * TEX_HEIGHT, 0)
}

extension Renderer {
    
    func setup() {
        
        buildMetal()
        buildDescriptors()
        buildShaders()
        buildLayers()
        buildPipeline()
        buildVertexBuffers()
        
        reshape()
    }
    
    internal func buildMetal() {
                
        // Command queue
        queue = device.makeCommandQueue()
        assert(queue != nil, "Failed to create command queue")

        // Metal layer
        metalLayer = view.layer as? CAMetalLayer
        assert(metalLayer != nil, "Failed to create CAMetalLayer")
        
        metalLayer.device = device
        metalLayer.pixelFormat = MTLPixelFormat.bgra8Unorm
        metalLayer.framebufferOnly = true
        metalLayer.frame = metalLayer.frame
    }
    
    func buildDescriptors() {
        
        // Render pass descriptor
        descriptor = MTLRenderPassDescriptor()
        descriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1)
        descriptor.colorAttachments[0].loadAction = MTLLoadAction.clear
        descriptor.colorAttachments[0].storeAction = MTLStoreAction.store
        descriptor.depthAttachment.clearDepth = 1
        descriptor.depthAttachment.loadAction = MTLLoadAction.clear
        descriptor.depthAttachment.storeAction = MTLStoreAction.dontCare
    }
    
    func buildShaders() {
        
        shaderOptions = ShaderOptions(
            
            blur: Int32(config.blur),
            blurRadius: config.blurRadius,
            bloom: Int32(config.bloom),
            bloomRadiusR: config.bloomRadiusR,
            bloomRadiusG: config.bloomRadiusG,
            bloomRadiusB: config.bloomRadiusB,
            bloomBrightness: config.bloomBrightness,
            bloomWeight: config.bloomWeight,
            dotMask: Int32(config.dotMask),
            dotMaskBrightness: config.dotMaskBrightness,
            scanlines: Int32(config.scanlines),
            scanlineBrightness: config.scanlineBrightness,
            scanlineWeight: config.scanlineWeight,
            disalignment: Int32(config.disalignment),
            disalignmentH: config.disalignmentH,
            disalignmentV: config.disalignmentV
        )
        
        ressourceManager = RessourceManager(view: view, device: device, renderer: self)
    }
    
    func buildLayers() {
        
        splashScreen = SplashScreen(renderer: self)
        canvas = Canvas(renderer: self)
        console = Console(renderer: self)
        dropZone = DropZone(renderer: self)
    }

    func buildPipeline() {
                
        // Read vertex shader from library
        let vertexFunc = ressourceManager.makeFunction(name: "vertex_main")
        metalAssert(vertexFunc != nil, "Failed to create vertex shader")
        
        // Read fragment shader from library
        let fragmentFunc = ressourceManager.makeFunction(name: "fragment_main")
        metalAssert(fragmentFunc != nil, "Failed to create fragment shader")
        
        // Create depth stencil state
        let stencilDescriptor = MTLDepthStencilDescriptor()
        stencilDescriptor.depthCompareFunction = MTLCompareFunction.less
        stencilDescriptor.isDepthWriteEnabled = true
        depthState = device.makeDepthStencilState(descriptor: stencilDescriptor)
        
        // Setup vertex descriptor
        let vertexDescriptor = MTLVertexDescriptor()
        
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
        let pipelineDescriptor = MTLRenderPipelineDescriptor()
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

        try? pipeline = device.makeRenderPipelineState(descriptor: pipelineDescriptor)
        metalAssert(pipeline != nil, "Failed to create the GPU pipeline.")
    }
    
    func buildVertexBuffers() {

        splashScreen.buildVertexBuffers()
        canvas.buildVertexBuffers()
    }
        
    func buildMatrices2D() {
        
        let model = matrix_identity_float4x4
        let view  = matrix_identity_float4x4
        let proj  = matrix_identity_float4x4
        let mvp = proj * view * model
        
        canvas.vertexUniforms2D.mvp = mvp
    }
    
    func buildMatrices3D() {
        
        let xAngle = -angleX.current / 180.0 * .pi
        let yAngle = angleY.current / 180.0 * .pi
        let zAngle = angleZ.current / 180.0 * .pi
        
        let xShift = -shiftX.current
        let yShift = -shiftY.current
        let zShift = shiftZ.current
        
        let aspect = Float(size.width) / Float(size.height)
        
        let view = matrix_identity_float4x4
        let proj = Renderer.perspectiveMatrix(fovY: Float(65.0 * (.pi / 180.0)),
                                              aspect: aspect,
                                              nearZ: 0.1,
                                              farZ: 100.0)
        
        let transEye = Renderer.translationMatrix(x: xShift,
                                                  y: yShift,
                                                  z: zShift + 1.393 - 0.16)
        
        let transRotX = Renderer.translationMatrix(x: 0.0,
                                                   y: 0.0,
                                                   z: 0.16)
        
        let rotX = Renderer.rotationMatrix(radians: xAngle, x: 0.5, y: 0.0, z: 0.0)
        let rotY = Renderer.rotationMatrix(radians: yAngle, x: 0.0, y: 0.5, z: 0.0)
        let rotZ = Renderer.rotationMatrix(radians: zAngle, x: 0.0, y: 0.0, z: 0.5)
        
        let model = transEye * rotX * transRotX * rotY * rotZ
        let mvp = proj * view * model
        
        canvas.vertexUniforms3D.mvp = mvp
    }
        
    //
    // Matrix utilities
    //
    
    static func perspectiveMatrix(fovY: Float,
                                  aspect: Float,
                                  nearZ: Float,
                                  farZ: Float) -> matrix_float4x4 {
        
        // Variant 1: Keeps correct aspect ratio independent of window size
        let yscale = 1.0 / tanf(fovY * 0.5) // 1 / tan == cot
        let xscale = yscale / aspect
        let q = farZ / (farZ - nearZ)
        
        // Alternative: Adjust to window size
        // float yscale = 1.0f / tanf(fovY * 0.5f);
        // float xscale = 0.75 * yscale;
        // float q = farZ / (farZ - nearZ);
        
        var m = matrix_float4x4()
        m.columns.0 = SIMD4<Float>(xscale, 0.0, 0.0, 0.0)
        m.columns.1 = SIMD4<Float>(0.0, yscale, 0.0, 0.0)
        m.columns.2 = SIMD4<Float>(0.0, 0.0, q, 1.0)
        m.columns.3 = SIMD4<Float>(0.0, 0.0, q * -nearZ, 0.0)
        
        return m
    }
    
    static func scalingMatrix(xs: Float, ys: Float, zs: Float) -> matrix_float4x4 {
        
        var m = matrix_float4x4()
        m.columns.0 = SIMD4<Float>(xs, 0.0, 0.0, 0.0)
        m.columns.1 = SIMD4<Float>(0.0, ys, 0.0, 0.0)
        m.columns.2 = SIMD4<Float>(0.0, 0.0, zs, 0.0)
        m.columns.3 = SIMD4<Float>(0.0, 0.0, 0.0, 1.0)
        
        return m
    }
    
    static func translationMatrix(x: Float,
                                  y: Float,
                                  z: Float) -> matrix_float4x4 {
        
        var m = matrix_identity_float4x4
        m.columns.3 = SIMD4<Float>(x, y, z, 1.0)
        
        return m
    }
    
    static func rotationMatrix(radians: Float,
                               x: Float,
                               y: Float,
                               z: Float) -> matrix_float4x4 {
        
        var v = vector_float3(x, y, z)
        v = normalize(v)
        let cos = cosf(radians)
        let cosp = 1.0 - cos
        let sin = sinf(radians)
        
        var m = matrix_float4x4()
        m.columns.0 = SIMD4<Float>(cos + cosp * v.x * v.x,
                                   cosp * v.x * v.y + v.z * sin,
                                   cosp * v.x * v.z - v.y * sin,
                                   0.0)
        m.columns.1 = SIMD4<Float>(cosp * v.x * v.y - v.z * sin,
                                   cos + cosp * v.y * v.y,
                                   cosp * v.y * v.z + v.x * sin,
                                   0.0)
        m.columns.2 = SIMD4<Float>(cosp * v.x * v.z + v.y * sin,
                                   cosp * v.y * v.z - v.x * sin,
                                   cos + cosp * v.z * v.z,
                                   0.0)
        m.columns.3 = SIMD4<Float>(0.0,
                                   0.0,
                                   0.0,
                                   1.0)
        return m
    }
    
    //
    // Error handling
    //
    
    func metalAssert(_ cond: Bool, _ msg: String) {
        
        if !cond {
            
            let alert = NSAlert()
            
            alert.alertStyle = .critical
            alert.icon = NSImage(named: "metal")
            alert.messageText = "Failed to initialize Metal Hardware"
            alert.informativeText = msg
            alert.addButton(withTitle: "OK")
            alert.runModal()
            
            exit(1)
        }
    }
}
