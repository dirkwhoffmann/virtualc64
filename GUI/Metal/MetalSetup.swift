//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation
import simd

public extension MetalView {

    func checkForMetal() {
        
        if MTLCreateSystemDefaultDevice() == nil {
            
            showNoMetalSupportAlert()
            NSApp.terminate(self)
            return
        }
    }
  
    func setupMetal() {

        track()
        
        buildMetal()
        buildTextures()
        buildSamplers()
        buildKernels()
        buildDotMasks()
        buildVertexBuffer()
        buildPipeline()
        
        self.reshape(withFrame: self.frame)
        enableMetal = true
    }
    
    internal func buildMetal() {
    
        track()
            
        // Metal device
        device = MTLCreateSystemDefaultDevice()
        precondition(device != nil, "Metal device must not be nil")
    
        // Metal layer
        metalLayer = self.layer as? CAMetalLayer
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
    
    internal func buildTextures() {

        track()
        precondition(device != nil)

        let descriptor = MTLTextureDescriptor.texture2DDescriptor(
            pixelFormat: MTLPixelFormat.rgba8Unorm,
            width: 512,
            height: 512,
            mipmapped: false)

        // Build background texture (drawn behind the cube)
        bgTexture = self.createBackgroundTexture()
    
        //
        // 512 x 512 textures
        //
        
        // Build C64 texture (as provided by the emulator)
        descriptor.usage = [ .shaderRead ]
        emulatorTexture = device?.makeTexture(descriptor: descriptor)
        precondition(emulatorTexture != nil, "Failed to create emulator texture.")
        
        // Build bloom textures
        descriptor.usage = [ .shaderRead, .shaderWrite, .renderTarget ]
        bloomTextureR = device?.makeTexture(descriptor: descriptor)
        bloomTextureG = device?.makeTexture(descriptor: descriptor)
        bloomTextureB = device?.makeTexture(descriptor: descriptor)
        precondition(bloomTextureR != nil, "Failed to create bloom texture (R).")
        precondition(bloomTextureG != nil, "Failed to create bloom texture (G).")
        precondition(bloomTextureB != nil, "Failed to create bloom texture (B).")

        //
        // 2048 x 2048 textures
        //
        
        descriptor.width = 2048
        descriptor.height = 2048
        
        // Build upscaled C64 texture
        descriptor.usage = [ .shaderRead, .shaderWrite, .pixelFormatView, .renderTarget ]
        upscaledTexture = device?.makeTexture(descriptor: descriptor)
        precondition(upscaledTexture != nil, "Failed to create upscaling texture.")
        
        // Build scanline texture
        scanlineTexture = device?.makeTexture(descriptor: descriptor)
        precondition(scanlineTexture != nil, "Failed to create scanline texture.")
    }
    
    internal func buildSamplers() {

        let descriptor = MTLSamplerDescriptor()
        descriptor.sAddressMode = MTLSamplerAddressMode.clampToEdge
        descriptor.tAddressMode = MTLSamplerAddressMode.clampToEdge
        descriptor.mipFilter = MTLSamplerMipFilter.notMipmapped

        // Nearest neighbor sampler
        descriptor.minFilter = MTLSamplerMinMagFilter.linear
        descriptor.magFilter = MTLSamplerMinMagFilter.linear
        samplerLinear = device!.makeSamplerState(descriptor: descriptor)
        
        // Linear sampler
        descriptor.minFilter = MTLSamplerMinMagFilter.nearest
        descriptor.magFilter = MTLSamplerMinMagFilter.nearest
        samplerNearest = device!.makeSamplerState(descriptor: descriptor)
    }
    
    internal func buildKernels() {
        
        precondition(device != nil)
        precondition(library != nil)
        
        // Build upscalers
        upscalerGallery[0] = BypassUpscaler.init(device: device!, library: library)
        upscalerGallery[1] = EPXUpscaler.init(device: device!, library: library)
        upscalerGallery[2] = XBRUpscaler.init(device: device!, library: library)
        
        // Build bloom filters
        bloomFilterGallery[0] = BypassFilter.init(device: device!, library: library)
        bloomFilterGallery[1] = SplitFilter.init(device: device!, library: library)
        bloomFilterGallery[2] = SplitFilter.init(device: device!, library: library)

        // Build scanline filters
        scanlineFilterGallery[0] = BypassFilter.init(device: device!, library: library)
        scanlineFilterGallery[1] = SimpleScanlines(device: device!, library: library)
        scanlineFilterGallery[2] = BypassFilter.init(device: device!, library: library)
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
                dotMaskTexture = image?.toTexture(device: device!)
            }
            
            // Store preview image
            dotmaskImages[n] = image?.resizeImageSharp(width: 12, height: 12)
        }
    }
    
    func buildMatricesBg() {
        
        let model  = matrix_identity_float4x4
        let view   = matrix_identity_float4x4
        let aspect = Float(layerWidth) / Float(layerHeight)
        let proj   = matrix_from_perspective(fovY: (Float(65.0 * (.pi / 180.0))),
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
            let xAngle: Float = -(currentXAngle / 180.0) * .pi
            let yAngle: Float =  (currentYAngle / 180.0) * .pi
            let zAngle: Float =  (currentZAngle / 180.0) * .pi
    
            model = model *
                matrix_from_rotation(radians: xAngle, x: 0.5, y: 0.0, z: 0.0) *
                matrix_from_rotation(radians: yAngle, x: 0.0, y: 0.5, z: 0.0) *
                matrix_from_rotation(radians: zAngle, x: 0.0, y: 0.0, z: 0.5)
        }
        
        vertexUniforms3D.mvp = proj * view * model
    }

    func buildVertexBuffer() {
    
        if device == nil {
            return
        }
        
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
        positionBuffer = device?.makeBuffer(bytes: pos, length: len, options: opt)
        precondition(positionBuffer != nil, "positionBuffer must not be nil")
    }
 
    func buildDepthBuffer() {
        
        // track("buildDepthBuffer")

        if device == nil {
            return
        }
        
        let width = Int(layerWidth)
        let height = Int(layerHeight)
        
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
