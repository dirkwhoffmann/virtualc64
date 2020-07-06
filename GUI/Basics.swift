// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

//
// Logging / Debugging
// 

public func track(_ message: String = "",
                  path: String = #file, function: String = #function, line: Int = #line ) {
    
    if let file = URL.init(string: path)?.deletingPathExtension().lastPathComponent {
        if message == "" {
            print("\(file).\(line)::\(function)")
        } else {
            print("\(file).\(line)::\(function): \(message)")
        }
    }
}

//
// Handling URLs
//

extension URL {
    
    func addTimeStamp() -> URL {
        
        let path = self.deletingPathExtension().path
        let suffix = self.pathExtension
        
        let date = Date.init()
        let formatter = DateFormatter()
        formatter.dateFormat = "yyyy-MM-dd"
        let dateString = formatter.string(from: date)
        formatter.dateFormat = "hh.mm.ss"
        let timeString = formatter.string(from: date)
        let timeStamp = dateString + " at " + timeString

        return URL(fileURLWithPath: path + " " + timeStamp + "." + suffix)
    }
    
    func makeUnique() -> URL {
        
        let path = self.deletingPathExtension().path
        let suffix = self.pathExtension
        let fileManager = FileManager.default
        
        for i in 0...127 {
            
            let numberStr = (i == 0) ? "." : " \(i)."
            let url = URL(fileURLWithPath: path + numberStr + suffix)

            if !fileManager.fileExists(atPath: url.path) {
                return url
            }
        }
        return self
    }
}

//
// Handling images
//

extension Data {
    var bitmap: NSBitmapImageRep? {
        return NSBitmapImageRep(data: self)
    }
}

extension NSImage {
    
    func representation(using: NSBitmapImageRep.FileType) -> Data? {
        
        let bitmap = tiffRepresentation?.bitmap
        return bitmap?.representation(using: using, properties: [:])
    }
    
    func resizeImage(width: CGFloat, height: CGFloat,
                     cutout: NSRect,
                     interpolation: NSImageInterpolation = .high) -> NSImage {
        
        let img = NSImage(size: CGSize(width: width, height: height))
        
        img.lockFocus()
        let ctx = NSGraphicsContext.current
        ctx?.imageInterpolation = interpolation // NSImageInterpolation.none // .high
        self.draw(in: cutout,
                  from: NSRect.init(x: 0, y: 0, width: size.width, height: size.height),
                  operation: .copy,
                  fraction: 1)
        img.unlockFocus()
        
        return img
    }
    
    func resizeImage(width: CGFloat, height: CGFloat) -> NSImage {
        
        let cutout = NSRect.init(x: 0, y: 0, width: width, height: height)
        return resizeImage(width: width, height: height,
                           cutout: cutout)
    }

    func resizeImageSharp(width: CGFloat, height: CGFloat) -> NSImage {
        
        let cutout = NSRect.init(x: 0, y: 0, width: width, height: height)
        return resizeImage(width: width, height: height,
                           cutout: cutout,
                           interpolation: .none)
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
}

//
// C64 Proxy extensions
//

public extension C64Proxy {
    
    func image(data: UnsafeMutablePointer<UInt8>?, size: NSSize) -> NSImage {
        
        var bitmap = data
        let width = Int(size.width)
        let height = Int(size.height)
        let imageRep = NSBitmapImageRep(bitmapDataPlanes: &bitmap,
                                        pixelsWide: width,
                                        pixelsHigh: height,
                                        bitsPerSample: 8,
                                        samplesPerPixel: 4,
                                        hasAlpha: true,
                                        isPlanar: false,
                                        colorSpaceName: NSColorSpaceName.calibratedRGB,
                                        bytesPerRow: 4*width,
                                        bitsPerPixel: 32)
        let image = NSImage(size: (imageRep?.size)!)
        image.addRepresentation(imageRep!)
        image.makeGlossy()
        
        return image
    }
    
    func autoSnapshotImage(_ item: Int) -> NSImage {
        
        let data = autoSnapshotImageData(item)
        return image(data: data, size: autoSnapshotImageSize(item))
    }

    func userSnapshotImage(_ item: Int) -> NSImage {
        
        let data = userSnapshotImageData(item)
       return image(data: data, size: userSnapshotImageSize(item))
    }
}
