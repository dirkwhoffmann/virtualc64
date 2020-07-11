// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import simd

struct TextureSize {
    
    static let original = MTLSizeMake(512, 512, 0)
    static let upscaled = MTLSizeMake(2048, 2048, 0)
}

extension Renderer {

    func setupMetal() {

        track()
        
        buildMetal()
        buildTextures()
        buildSamplers()
        buildKernels()
        buildDotMasks()
        buildPipeline()
        buildVertexBuffer()
        
        reshape()
    }
    
    internal func buildMetal() {
    
        track()
                
        // Metal layer
        metalLayer = mtkView.layer as? CAMetalLayer
        assert(metalLayer != nil, "Metal layer must not be nil")
        
        metalLayer.device = device
        metalLayer.pixelFormat = MTLPixelFormat.bgra8Unorm
        metalLayer.framebufferOnly = true
        metalLayer.frame = metalLayer.frame
    
        // Command queue
        queue = device.makeCommandQueue()
        assert(queue != nil, "Metal command queue must not be nil")
    
        // Shader library
        library = device.makeDefaultLibrary()
        assert(library != nil, "Metal library must not be nil")
    }
    
    internal func buildTextures() {

        track()
        
        // Texture usages
        let r: MTLTextureUsage = [ .shaderRead ]
        let rwt: MTLTextureUsage = [ .shaderRead, .shaderWrite, .renderTarget ]
        let rwtp: MTLTextureUsage = [ .shaderRead, .shaderWrite, .renderTarget, .pixelFormatView ]
        
        // Background texture used in window mode
        bgTexture = device.makeTexture(w: 512, h: 512)
        assert(bgTexture != nil, "Failed to create bgTexture")
                
        // Emulator texture (long frames)
        emulatorTexture = device.makeTexture(size: TextureSize.original, usage: r)
        assert(emulatorTexture != nil, "Failed to create emulatorTexture")
        
        // Build bloom textures
        bloomTextureR = device.makeTexture(size: TextureSize.original, usage: rwt)
        bloomTextureG = device.makeTexture(size: TextureSize.original, usage: rwt)
        bloomTextureB = device.makeTexture(size: TextureSize.original, usage: rwt)
        assert(bloomTextureR != nil, "Failed to create bloomTextureR")
        assert(bloomTextureG != nil, "Failed to create bloomTextureG")
        assert(bloomTextureB != nil, "Failed to create bloomTextureB")

        // Upscaled texture
        upscaledTexture = device.makeTexture(size: TextureSize.upscaled, usage: rwtp)
        scanlineTexture = device.makeTexture(size: TextureSize.upscaled, usage: rwtp)
        assert(upscaledTexture != nil, "Failed to create upscaledTexture")
        assert(scanlineTexture != nil, "Failed to create scanlineTexture")
    }
    
    internal func buildSamplers() {

        let descriptor = MTLSamplerDescriptor()
        descriptor.sAddressMode = MTLSamplerAddressMode.clampToEdge
        descriptor.tAddressMode = MTLSamplerAddressMode.clampToEdge
        descriptor.mipFilter = MTLSamplerMipFilter.notMipmapped

        // Nearest neighbor sampler
        descriptor.minFilter = MTLSamplerMinMagFilter.linear
        descriptor.magFilter = MTLSamplerMinMagFilter.linear
        samplerLinear = device.makeSamplerState(descriptor: descriptor)
        
        // Linear sampler
        descriptor.minFilter = MTLSamplerMinMagFilter.nearest
        descriptor.magFilter = MTLSamplerMinMagFilter.nearest
        samplerNearest = device.makeSamplerState(descriptor: descriptor)
    }
    
    internal func buildKernels() {
        
        precondition(library != nil)
        
        // Build upscalers
        upscalerGallery[0] = BypassUpscaler.init(device: device, library: library)
        upscalerGallery[1] = EPXUpscaler.init(device: device, library: library)
        upscalerGallery[2] = XBRUpscaler.init(device: device, library: library)
        
        // Build bloom filters
        bloomFilterGallery[0] = BypassFilter.init(device: device, library: library)
        bloomFilterGallery[1] = SplitFilter.init(device: device, library: library)
        bloomFilterGallery[2] = SplitFilter.init(device: device, library: library)

        // Build scanline filters
        scanlineFilterGallery[0] = BypassFilter.init(device: device, library: library)
        scanlineFilterGallery[1] = SimpleScanlines(device: device, library: library)
        scanlineFilterGallery[2] = BypassFilter.init(device: device, library: library)
    }
    
    func buildDotMasks() {
        
        let selected = shaderOptions.dotMask
        let max  = UInt8(85 + shaderOptions.dotMaskBrightness * 170)
        let base = UInt8((1 - shaderOptions.dotMaskBrightness) * 85)
        let none = UInt8(30 + (1 - shaderOptions.dotMaskBrightness) * 55)
        
        let R = UInt32.init(r: max, g: base, b: base)
        let G = UInt32.init(r: base, g: max, b: base)
        let B = UInt32.init(r: base, g: base, b: max)
        let M = UInt32.init(r: max, g: base, b: max)
        let W = UInt32.init(r: max, g: max, b: max)
        let N = UInt32.init(r: none, g: none, b: none)

        let maskSize = [
            CGSize.init(width: 1, height: 1),
            CGSize.init(width: 3, height: 1),
            CGSize.init(width: 4, height: 1),
            CGSize.init(width: 3, height: 9),
            CGSize.init(width: 4, height: 8)
            ]
        
        let maskData = [
            
            [ W ],
            [ M, G, N ],
            [ R, G, B, N ],
            [ M, G, N,
              M, G, N,
              N, N, N,
              N, M, G,
              N, M, G,
              N, N, N,
              G, N, M,
              G, N, M,
              N, N, N],
            [ R, G, B, N,
              R, G, B, N,
              R, G, B, N,
              N, N, N, N,
              B, N, R, G,
              B, N, R, G,
              B, N, R, G,
              N, N, N, N]
        ]
        
        for n in 0 ... 4 {
            
            // Create image representation in memory
            let cap = Int(maskSize[n].width) * Int(maskSize[n].height)
            let mask = calloc(cap, MemoryLayout<UInt32>.size)!
            let ptr = mask.bindMemory(to: UInt32.self, capacity: cap)
            for i in 0 ... cap - 1 {
                ptr[i] = maskData[n][i]
            }
            
            // Create image
            let image = NSImage.make(data: mask, rect: maskSize[n])
            
            // Create texture if the dotmask is the currently selected mask
            if n == selected {
                dotMaskTexture = image?.toTexture(device: device)
            }
            
            // Store preview image
            dotmaskImages[n] = image?.resizeImageSharp(width: 12, height: 12)
        }
    }
    
    func buildPipeline() {

        track()
        precondition(library != nil)
        
        // Get vertex and fragment shader from library
        let vertexFunc = library.makeFunction(name: "vertex_main")
        let fragmentFunc = library.makeFunction(name: "fragment_main")
        assert(vertexFunc != nil)
        assert(fragmentFunc != nil)

        // Create depth stencil state
        let stencilDescriptor = MTLDepthStencilDescriptor.init()
        stencilDescriptor.depthCompareFunction = MTLCompareFunction.less
        stencilDescriptor.isDepthWriteEnabled = true
        depthState = device.makeDepthStencilState(descriptor: stencilDescriptor)
        
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
            try pipeline = device.makeRenderPipelineState(descriptor: pipelineDescriptor)
        } catch {
            fatalError("Cannot create Metal graphics pipeline")
        }
    }
    
    func buildVertexBuffer() {
        
        bgRect = Node.init(device: device,
                           x: -1.0, y: -1.0, z: 0.99, w: 2.0, h: 2.0,
                           t: NSRect.init(x: 0.0, y: 0.0, width: 1.0, height: 1.0))
        
        quad2D = Node.init(device: device,
                           x: -1.0, y: -1.0, z: 0.0, w: 2.0, h: 2.0,
                           t: textureRect)
        
        quad3D = Quad.init(device: device,
                           x1: -0.64, y1: -0.48, z1: -0.64,
                           x2: 0.64, y2: 0.48, z2: 0.64,
                           t: textureRect)
        
        // OLD CODE:
        
        let capacity = 16 * 3 * 8
        let pos = UnsafeMutablePointer<Float>.allocate(capacity: capacity)
        
        func setVertex(_ i: Int, _ position: SIMD3<Float>, _ texture: SIMD2<Float>) {
            
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
        let upperLeft = SIMD2<Float>(Float(textureRect.minX), Float(textureRect.maxY))
        let upperRight = SIMD2<Float>(Float(textureRect.maxX), Float(textureRect.maxY))
        let lowerLeft = SIMD2<Float>(Float(textureRect.minX), Float(textureRect.minY))
        let lowerRight = SIMD2<Float>(Float(textureRect.maxX), Float(textureRect.minY))
        
        // Background
        setVertex(0, SIMD3<Float>(-bgx, +bgy, -bgz), SIMD2<Float>(0.0, 0.0))
        setVertex(1, SIMD3<Float>(-bgx, -bgy, -bgz), SIMD2<Float>(0.0, 1.0))
        setVertex(2, SIMD3<Float>(+bgx, -bgy, -bgz), SIMD2<Float>(1.0, 1.0))
        
        setVertex(3, SIMD3<Float>(-bgx, +bgy, -bgz), SIMD2<Float>(0.0, 0.0))
        setVertex(4, SIMD3<Float>(+bgx, +bgy, -bgz), SIMD2<Float>(1.0, 0.0))
        setVertex(5, SIMD3<Float>(+bgx, -bgy, -bgz), SIMD2<Float>(1.0, 1.0))
        
        // -Z
        setVertex(6, SIMD3<Float>(-dx, +dy, -dz), lowerLeft)
        setVertex(7, SIMD3<Float>(-dx, -dy, -dz), upperLeft)
        setVertex(8, SIMD3<Float>(+dx, -dy, -dz), upperRight)
        
        setVertex(9, SIMD3<Float>(-dx, +dy, -dz), lowerLeft)
        setVertex(10, SIMD3<Float>(+dx, +dy, -dz), lowerRight)
        setVertex(11, SIMD3<Float>(+dx, -dy, -dz), upperRight)
        
        // +Z
        setVertex(12, SIMD3<Float>(-dx, +dy, +dz), lowerRight)
        setVertex(13, SIMD3<Float>(-dx, -dy, +dz), upperRight)
        setVertex(14, SIMD3<Float>(+dx, -dy, +dz), upperLeft)
        
        setVertex(15, SIMD3<Float>(-dx, +dy, +dz), lowerRight)
        setVertex(16, SIMD3<Float>(+dx, +dy, +dz), lowerLeft)
        setVertex(17, SIMD3<Float>(+dx, -dy, +dz), upperLeft)
        
        // -X
        setVertex(18, SIMD3<Float>(-dx, +dy, -dz), lowerRight)
        setVertex(19, SIMD3<Float>(-dx, -dy, -dz), upperRight)
        setVertex(20, SIMD3<Float>(-dx, -dy, +dz), upperLeft)
        
        setVertex(21, SIMD3<Float>(-dx, +dy, -dz), lowerRight)
        setVertex(22, SIMD3<Float>(-dx, +dy, +dz), lowerLeft)
        setVertex(23, SIMD3<Float>(-dx, -dy, +dz), upperLeft)
        
        // +X
        setVertex(24, SIMD3<Float>(+dx, +dy, -dz), lowerLeft)
        setVertex(25, SIMD3<Float>(+dx, -dy, -dz), upperLeft)
        setVertex(26, SIMD3<Float>(+dx, -dy, +dz), upperRight)
        
        setVertex(27, SIMD3<Float>(+dx, +dy, -dz), lowerLeft)
        setVertex(28, SIMD3<Float>(+dx, +dy, +dz), lowerRight)
        setVertex(29, SIMD3<Float>(+dx, -dy, +dz), upperRight)
        
        // -Y
        setVertex(30, SIMD3<Float>(+dx, -dy, -dz), lowerLeft)
        setVertex(31, SIMD3<Float>(-dx, -dy, -dz), upperLeft)
        setVertex(32, SIMD3<Float>(-dx, -dy, +dz), upperRight)
        
        setVertex(33, SIMD3<Float>(+dx, -dy, -dz), lowerLeft)
        setVertex(34, SIMD3<Float>(+dx, -dy, +dz), lowerRight)
        setVertex(35, SIMD3<Float>(-dx, -dy, +dz), upperRight)
        
        // +Y
        setVertex(36, SIMD3<Float>(+dx, +dy, -dz), lowerLeft)
        setVertex(37, SIMD3<Float>(-dx, +dy, -dz), upperLeft)
        setVertex(38, SIMD3<Float>(-dx, +dy, +dz), upperRight)
        
        setVertex(39, SIMD3<Float>(+dx, +dy, -dz), lowerLeft)
        setVertex(40, SIMD3<Float>(-dx, +dy, +dz), upperRight)
        setVertex(41, SIMD3<Float>(+dx, +dy, +dz), lowerRight)
        
        // 2D drawing quad
        setVertex(42, SIMD3<Float>(-1, 1, 0), lowerLeft)
        setVertex(43, SIMD3<Float>(-1, -1, 0), upperLeft)
        setVertex(44, SIMD3<Float>( 1, -1, 0), upperRight)
        
        setVertex(45, SIMD3<Float>(-1, 1, 0), lowerLeft)
        setVertex(46, SIMD3<Float>( 1, 1, 0), lowerRight)
        setVertex(47, SIMD3<Float>( 1, -1, 0), upperRight)
        
        let opt = MTLResourceOptions.cpuCacheModeWriteCombined
        let len = capacity * 4
        positionBuffer = device.makeBuffer(bytes: pos, length: len, options: opt)
        precondition(positionBuffer != nil, "positionBuffer must not be nil")
    }
    
    func buildMatricesBg() {
        
        let model  = matrix_identity_float4x4
        let view   = matrix_identity_float4x4
        let aspect = Float(size.width) / Float(size.height)
        let proj   = Renderer.perspectiveMatrix(fovY: (Float(65.0 * (.pi / 180.0))),
                                             aspect: aspect,
                                             nearZ: 0.1,
                                             farZ: 100.0)
        
        vertexUniformsBg.mvp = proj * view * model
    }
    
    func buildMatrices2D() {
    
        let model = matrix_identity_float4x4
        let view  = matrix_identity_float4x4
        let proj  = matrix_identity_float4x4
        
        vertexUniforms2D.mvp = proj * view * model
    }
    
    func buildMatrices3D() {
    
        var model  = Renderer.translationMatrix(x: -currentEyeX,
                                             y: -currentEyeY,
                                             z: currentEyeZ + 1.39)
        let view   = matrix_identity_float4x4
        let aspect = Float(size.width) / Float(size.height)
        let proj   = Renderer.perspectiveMatrix(fovY: (Float(65.0 * (.pi / 180.0))),
                                             aspect: aspect,
                                             nearZ: 0.1,
                                             farZ: 100.0)
    
        if animates() {
            let xAngle: Float = -(currentXAngle / 180.0) * .pi
            let yAngle: Float =  (currentYAngle / 180.0) * .pi
            let zAngle: Float =  (currentZAngle / 180.0) * .pi
    
            model = model *
                Renderer.rotationMatrix(radians: xAngle, x: 0.5, y: 0.0, z: 0.0) *
                Renderer.rotationMatrix(radians: yAngle, x: 0.0, y: 0.5, z: 0.0) *
                Renderer.rotationMatrix(radians: zAngle, x: 0.0, y: 0.0, z: 0.5)
        }
        
        vertexUniforms3D.mvp = proj * view * model
    }

    func buildDepthBuffer() {
        
        // track("buildDepthBuffer")
        
        let width = Int(size.width)
        let height = Int(size.height)
        
        let descriptor = MTLTextureDescriptor.texture2DDescriptor(
            pixelFormat: MTLPixelFormat.depth32Float,
            width: width,
            height: height,
            mipmapped: false)
        descriptor.resourceOptions = MTLResourceOptions.storageModePrivate
        descriptor.usage = MTLTextureUsage.renderTarget
        
        depthTexture = device.makeTexture(descriptor: descriptor)
        precondition(depthTexture != nil, "Failed to create depth texture")
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
}
