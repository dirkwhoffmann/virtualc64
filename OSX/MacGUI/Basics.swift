//
//  Basics.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 03.02.18.
//

import Foundation

// -------------------------------------------------------------------
//                          Logging / Debugging
// -------------------------------------------------------------------

public func track(_ message: String = "",
                  path: String = #file, function: String = #function, line: Int = #line ) {
    
    if let file = URL.init(string: path)?.deletingPathExtension().lastPathComponent {
        print("\(file)::\(function).\(line): \(message)\n")
    }
}

// -------------------------------------------------------------------
//                          NSImage extensions
// -------------------------------------------------------------------

extension NSImage {
    func resizeImage(width: CGFloat, height: CGFloat) -> NSImage {
        let img = NSImage(size: CGSize(width:width, height:height))
        
        img.lockFocus()
        let ctx = NSGraphicsContext.current
        ctx?.imageInterpolation = .high
        self.draw(in: NSMakeRect(0, 0, width, height), from: NSMakeRect(0, 0, size.width, size.height), operation: .copy, fraction: 1)
        img.unlockFocus()
        
        return img
    }
    
    func makeGlossy() {
        
        let width  = size.width
        let height = size.height
        let glossy = NSImage(named: NSImage.Name(rawValue: "glossy.png"))
        let rect   = NSRect(x: 0, y: 0, width: width, height: height)
        
        lockFocus()
        let sourceOver = NSCompositingOperation.sourceOver
        //         draw(in: rect, from: NSZeroRect, operation: NSCompositeSourceOver, fraction: 1.0)
        //         glossy!.draw(in: rect, from: NSZeroRect, operation: NSCompositeSourceOver, fraction: 1.0)
        draw(in: rect, from: NSZeroRect, operation: sourceOver, fraction: 1.0)
        glossy!.draw(in: rect, from: NSZeroRect, operation: sourceOver, fraction: 1.0)
        unlockFocus()
    }
}


// -------------------------------------------------------------------
//                          C64 Proxy extensions
// -------------------------------------------------------------------

public extension C64Proxy {
    
    @objc func timetravelSnapshotImage(_ item: Int) -> NSImage {
        
        var data = historicSnapshotImageData(item)
        let width = historicSnapshotImageWidth(item)
        let height = historicSnapshotImageHeight(item)
        let imageRep = NSBitmapImageRep(bitmapDataPlanes: &data,
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
}

