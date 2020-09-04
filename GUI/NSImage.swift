// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension NSImage {
    
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
    
    var cgImage: CGImage? {
        var rect = CGRect.init(origin: .zero, size: self.size)
        return self.cgImage(forProposedRect: &rect, context: nil, hints: nil)
    }

    func representation(using: NSBitmapImageRep.FileType) -> Data? {
        
        let bitmap = tiffRepresentation?.bitmap
        return bitmap?.representation(using: using, properties: [:])
    }
    
    func expand(toSize size: NSSize) -> NSImage? {
        
        let newImage = NSImage.init(size: size)
        
        NSGraphicsContext.saveGraphicsState()
        newImage.lockFocus()
        
        let t = NSAffineTransform()
        t.translateX(by: 0.0, yBy: size.height)
        t.scaleX(by: 1.0, yBy: -1.0)
        t.concat()
        
        let inRect = NSRect.init(x: 0, y: 0, width: size.width, height: size.height)
        let fromRect = NSRect.init(x: 0, y: 0, width: self.size.width, height: self.size.height)
        let operation = NSCompositingOperation.copy
        self.draw(in: inRect, from: fromRect, operation: operation, fraction: 1.0)
        
        newImage.unlockFocus()
        NSGraphicsContext.restoreGraphicsState()
        
        return newImage
    }
    
    func resizeImage(width: CGFloat, height: CGFloat,
                     cutout: NSRect,
                     interpolation: NSImageInterpolation = .high) -> NSImage {
        
        let img = NSImage(size: CGSize(width: width, height: height))
        
        img.lockFocus()
        let ctx = NSGraphicsContext.current
        ctx?.imageInterpolation = interpolation
        self.draw(in: cutout,
                  from: NSRect.init(x: 0, y: 0, width: size.width, height: size.height),
                  operation: .sourceOver,
                  fraction: 1)
        img.unlockFocus()
        
        return img
    }
    
    func resize(width: CGFloat, height: CGFloat) -> NSImage {
        
        let cutout = NSRect.init(x: 0, y: 0, width: width, height: height)
        return resizeImage(width: width, height: height,
                           cutout: cutout)
    }
    
    func resize(size: CGSize) -> NSImage {
        
        return resize(width: size.width, height: size.height)
    }
    
    func resizeSharp(width: CGFloat, height: CGFloat) -> NSImage {
        
        let cutout = NSRect.init(x: 0, y: 0, width: width, height: height)
        return resizeImage(width: width, height: height,
                           cutout: cutout,
                           interpolation: .none)
    }
    
    func resizeSharp(size: CGSize) -> NSImage {
        
        return resizeSharp(width: size.width, height: size.height)
    }
    
    func roundCorners(withRadius radius: CGFloat) -> NSImage {
        
        let rect = NSRect.init(origin: NSPoint.zero, size: size)
        if
            let cgImage = self.cgImage,
            let context = CGContext(data: nil,
                                    width: Int(size.width),
                                    height: Int(size.height),
                                    bitsPerComponent: 8,
                                    bytesPerRow: 4 * Int(size.width),
                                    space: CGColorSpaceCreateDeviceRGB(),
                                    bitmapInfo: CGImageAlphaInfo.premultipliedFirst.rawValue) {
            context.beginPath()
            context.addPath(CGPath(roundedRect: rect, cornerWidth: radius, cornerHeight: radius, transform: nil))
            context.closePath()
            context.clip()
            context.draw(cgImage, in: rect)
            
            if let composedImage = context.makeImage() {
                return NSImage(cgImage: composedImage, size: size)
            }
        }
        
        return self
    }
    
    func roundCorners() -> NSImage {
        
        return self.roundCorners(withRadius: size.height / 30)
    }
        
    func makeGlossy() {
        
        let width  = size.width
        let height = size.height
        let glossy = NSImage(named: "glossy")
        let rect   = NSRect(x: 0, y: 0, width: width, height: height)
        
        lockFocus()
        let sourceOver = NSCompositingOperation.sourceOver
        draw(in: rect, from: NSRect.zero, operation: sourceOver, fraction: 1.0)
        glossy!.draw(in: rect, from: NSRect.zero, operation: sourceOver, fraction: 1.0)
        unlockFocus()
    }
    
    func tint(_ color: NSColor) {
        
        let imageRect = NSRect.init(origin: .zero, size: size)
        
        lockFocus()
        color.set()
        imageRect.fill(using: .sourceAtop)
        unlockFocus()
    }
    
    func darken() {
        
        tint(NSColor.init(red: 0.0, green: 0.0, blue: 0.0, alpha: 0.33))
    }

    func pressed() {
        
        tint(NSColor.init(red: 1.0, green: 0.0, blue: 0.0, alpha: 0.5))
        // tint(NSColor.init(red: 0.0, green: 0.0, blue: 0.0, alpha: 0.15))
    }

    func red() {
        
        tint(NSColor.init(red: 1.0, green: 0.0, blue: 0.0, alpha: 0.5))
    }
        
    func toTexture(device: MTLDevice) -> MTLTexture? {
        
        // let imageRect = NSMakeRect(0, 0, self.size.width, self.size.height);
        let imageRef = self.cgImage(forProposedRect: nil, context: nil, hints: nil)
        
        // Create a suitable bitmap context for extracting the bits of the image
        let width = imageRef!.width
        let height = imageRef!.height
        
        if width == 0 || height == 0 { return nil }
        
        // Allocate memory
        guard let data = malloc(height * width * 4) else { return nil }
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
        
        free(data)
        return texture
    }
    
    func imprint(text: String, x: CGFloat, y: CGFloat, fontSize: CGFloat) {
        
        let font = NSFont.systemFont(ofSize: fontSize)
        
        let w = size.width
        let h = size.height
        
        let textRect = CGRect(x: x, y: -y, width: w - x, height: h - y)
        let attributes: [NSAttributedString.Key: Any] = [
            .font: font,
            .foregroundColor: NSColor.secondaryLabelColor
        ]
        lockFocus()
        text.draw(in: textRect, withAttributes: attributes)
        unlockFocus()
    }
    
    func imprint(character c: Character, x: CGFloat, y: CGFloat, fontSize: CGFloat) {

        return imprint(text: String(c), x: x, y: y, fontSize: fontSize)
    }
}
