//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//


import Foundation

extension UInt32 {
    
    init(r: UInt8, g: UInt8, b: UInt8) {
        let red = UInt32(r) << 24
        let green = UInt32(g) << 16
        let blue = UInt32(b) << 8
        self.init(bigEndian: red | green | blue)
    }
}

//
// Extensions to CGImage
//

public extension CGImage {
    
    static func bitmapInfo() -> CGBitmapInfo {
        
        let noAlpha = CGImageAlphaInfo.noneSkipLast.rawValue
        let bigEn32 = CGBitmapInfo.byteOrder32Big.rawValue
    
        return CGBitmapInfo(rawValue: noAlpha | bigEn32)
    }
    
    static func dataProvider(data: UnsafeMutableRawPointer, size: CGSize) -> CGDataProvider? {
        
        let dealloc : CGDataProviderReleaseDataCallback = {
            
            (info: UnsafeMutableRawPointer?, data: UnsafeRawPointer, size: Int) -> () in
            
            // Core Foundation objects are memory managed, aren't they?
            return
        }
        
        return CGDataProvider(dataInfo: nil,
                              data: data,
                              size: 4 * Int(size.width) * Int(size.height),
                              releaseData: dealloc)
    }
    
    /// Creates a CGImage from a raw data stream in 32 bit big endian format
    static func make(data: UnsafeMutableRawPointer, size: CGSize) -> CGImage? {
        
        let w = Int(size.width)
        let h = Int(size.height)
        
        return CGImage(width: w, height: h,
                       bitsPerComponent: 8,
                       bitsPerPixel: 32,
                       bytesPerRow: 4 * w,
                       space: CGColorSpaceCreateDeviceRGB(),
                       bitmapInfo: bitmapInfo(),
                       provider: dataProvider(data: data, size: size)!,
                       decode: nil,
                       shouldInterpolate: false,
                       intent: CGColorRenderingIntent.defaultIntent)
    }
    
    /// Creates a CGImage from a MTLTexture
    static func make(texture: MTLTexture, rect: CGRect) -> CGImage? {
        
        // Compute texture cutout
        //   (x,y) : upper left corner
        //   (w,h) : width and height
        let x = Int(CGFloat(texture.width) * rect.minX)
        let y = Int(CGFloat(texture.height) * rect.minY)
        let w = Int(CGFloat(texture.width) * rect.width)
        let h = Int(CGFloat(texture.height) * rect.height)
        
        // Get texture data as a byte stream
        guard let data = malloc(4 * w * h) else { return nil; }
        texture.getBytes(data,
                         bytesPerRow: 4 * w,
                         from: MTLRegionMake2D(x, y, w, h),
                         mipmapLevel: 0)
        
        // Copy data over to a new buffer of double horizontal width
        /*
         let w2 = 2 * w
         let h2 = h
         let bytesPerRow2 = 2 * bytesPerRow
         let size2 = bytesPerRow2 * h2
         guard let data2 = malloc(size2) else { return nil; }
         let ptr = data.assumingMemoryBound(to: UInt32.self)
         let ptr2 = data2.assumingMemoryBound(to: UInt32.self)
         for i in 0 ... (w * h) {
         ptr2[2 * i] = ptr[i]
         ptr2[2 * i + 1] = ptr[i]
         }
         */
        
        return make(data: data, size: CGSize.init(width: w, height: h))
    }
}



//
// Extensions to MTLTexture
//

extension MTLTexture {
    
}


//
// Extensions to NSImage
//

public extension NSImage {
    
    convenience init(color: NSColor, size: NSSize) {
        
        self.init(size: size)
        lockFocus()
        color.drawSwatch(in: NSRect(origin: .zero, size: size))
        unlockFocus()
    }

    static func make(texture: MTLTexture, rect: CGRect) -> NSImage? {
        
        guard let cgImage = CGImage.make(texture: texture, rect: rect) else {
            track("Failed to create CGImage.")
            return nil
        }
        
        let size = NSSize(width: cgImage.width, height: cgImage.height)
        return NSImage(cgImage: cgImage, size: size)
    }

    static func make(data: UnsafeMutableRawPointer, rect: CGSize) -> NSImage? {
        
        guard let cgImage = CGImage.make(data: data, size: rect) else {
            track("Failed to create CGImage.")
            return nil
        }
        
        let size = NSSize(width: cgImage.width, height: cgImage.height)
        return NSImage(cgImage: cgImage, size: size)
    }
    
    
    func expand(toSize size: NSSize) -> NSImage? {
 
        let newImage = NSImage.init(size: size)
    
        NSGraphicsContext.saveGraphicsState()
        newImage.lockFocus()

        let t = NSAffineTransform()
        t.translateX(by: 0.0, yBy: size.height)
        t.scaleX(by: 1.0, yBy: -1.0)
        t.concat()
        
        let inRect = NSMakeRect(0,0,size.width,size.height)
        let fromRect = NSMakeRect(0,0,self.size.width, self.size.height)
        let operation = NSCompositingOperation.copy
        self.draw(in: inRect, from: fromRect, operation: operation, fraction: 1.0)
        
        newImage.unlockFocus()
        NSGraphicsContext.restoreGraphicsState()
        
        return newImage;
    }
    
    func toTexture(device: MTLDevice) -> MTLTexture? {
 
        // let imageRect = NSMakeRect(0, 0, self.size.width, self.size.height);
        let imageRef = self.cgImage(forProposedRect: nil, context: nil, hints: nil)
        
        // Create a suitable bitmap context for extracting the bits of the image
        let width = imageRef!.width
        let height = imageRef!.height
    
        if (width == 0 || height == 0) { return nil; }
        
        // Allocate memory
        guard let data = malloc(height * width * 4) else { return nil; }
        let rawBitmapInfo =
            CGImageAlphaInfo.noneSkipLast.rawValue |
                CGBitmapInfo.byteOrder32Big.rawValue
        let bitmapContext = CGContext(data: data,
                                      width: width,
                                      height: height,
                                      bitsPerComponent: 8,
                                      bytesPerRow: 4 * width,
                                      space: CGColorSpaceCreateDeviceRGB(),
                                      bitmapInfo: rawBitmapInfo)

        bitmapContext?.translateBy(x: 0.0, y: CGFloat(height))
        bitmapContext?.scaleBy(x: 1.0, y: -1.0)
        bitmapContext?.draw(imageRef!, in: CGRect.init(x: 0, y: 0, width: width, height: height))
        // CGContextDrawImage(bitmapContext!, CGRectMake(0, 0, CGFloat(width), CGFloat(height)), imageRef)
        
        let textureDescriptor = MTLTextureDescriptor.texture2DDescriptor(
            pixelFormat: MTLPixelFormat.rgba8Unorm,
            width: width,
            height: height,
            mipmapped: false)
        let texture = device.makeTexture(descriptor: textureDescriptor)
        let region = MTLRegionMake2D(0, 0, width, height)
        texture?.replace(region: region, mipmapLevel: 0, withBytes: data, bytesPerRow: 4 * width)

        free(data);
        return texture;
    }
}


//
// Extensions to MetalView
//


public extension MetalView
{

    //
    // Image handling
    //

    func screenshot() -> NSImage?
    {
        // Use the blitter to copy the texture data back from the GPU
        let queue = scanlineTexture.device.makeCommandQueue()!
        let commandBuffer = queue.makeCommandBuffer()!
        let blitEncoder = commandBuffer.makeBlitCommandEncoder()!
        blitEncoder.synchronize(texture: scanlineTexture, slice: 0, level: 0)
        blitEncoder.endEncoding()
        commandBuffer.commit()
        commandBuffer.waitUntilCompleted()
        
        return NSImage.make(texture: scanlineTexture, rect: textureRect)
    }
    
    func createBackgroundTexture() -> MTLTexture? {

        // Grab the current wallpaper as an NSImage
        let windows =
            CGWindowListCopyWindowInfo(CGWindowListOption.optionOnScreenOnly,
                                       CGWindowID(0))! as! [NSDictionary]
        let screenBounds = NSScreen.main?.frame
        
        // Iterate through all windows
        var cgImage: CGImage?
        for i in 0 ..< windows.count {
            
            let window = windows[i]
            
            // Skip all windows that are not owned by the dock
            let owner = window["kCGWindowOwnerName"] as? String
            if owner != "Dock" {
                continue
            }
            
            // Skip all windows that do not have the same bounds as the main screen
            let bounds = window["kCGWindowBounds"] as! NSDictionary
            let width  = bounds["Width"] as! CGFloat
            let height = bounds["Height"] as! CGFloat
            if (width != screenBounds?.width || height != screenBounds?.height) {
                continue
            }
            
            // Skip all windows without having a name
            guard let name = window["kCGWindowName"] as? String else {
                continue
            }
                
            // Skip all windows with a name other than "Desktop picture - ..."
            if name.hasPrefix("Desktop Picture") {
                
                // Found it!
                cgImage = CGWindowListCreateImage(
                    CGRect.null,
                    CGWindowListOption(arrayLiteral: CGWindowListOption.optionIncludingWindow),
                    CGWindowID(window["kCGWindowNumber"] as! Int),
                    [])!
                break
            }
        }
        
        // Create image
        var wallpaper: NSImage?
        if cgImage != nil {
            wallpaper = NSImage.init(cgImage: cgImage!, size: NSZeroSize)
            wallpaper = wallpaper?.expand(toSize: NSSize(width: 1024, height: 512))
        } else {
            // Fall back to an opaque gray background
            let size = NSSize(width: 128, height: 128)
            wallpaper = NSImage(color: .lightGray, size: size)
        }
        
        // Return image as texture
        return wallpaper?.toTexture(device: device!)
    }
}
