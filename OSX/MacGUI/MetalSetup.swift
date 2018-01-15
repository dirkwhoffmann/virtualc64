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
}
