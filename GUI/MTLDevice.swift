// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension MTLDevice {
    
    func makeTexture(size: MTLSize,
                     buffer: UnsafeMutablePointer<UInt32>? = nil,
                     usage: MTLTextureUsage = [.shaderRead]) -> MTLTexture? {
        
        let descriptor = MTLTextureDescriptor.texture2DDescriptor(
            pixelFormat: MTLPixelFormat.rgba8Unorm,
            width: size.width,
            height: size.height,
            mipmapped: false)
        descriptor.usage = usage
        
        let texture = makeTexture(descriptor: descriptor)
        texture?.replace(size: size, buffer: buffer)
        return texture
    }
    
    func makeTexture(w: Int, h: Int,
                     buffer: UnsafeMutablePointer<UInt32>? = nil,
                     usage: MTLTextureUsage = [.shaderRead]) -> MTLTexture? {
        
        let size = MTLSizeMake(w, h, 0)
        return makeTexture(size: size, buffer: buffer, usage: usage)
    }
    
    func makeBackgroundTexture() -> MTLTexture? {

         // Grab the current wallpaper as an NSImage
         let opt = CGWindowListOption.optionOnScreenOnly
         let id = CGWindowID(0)
         guard
             let windows = CGWindowListCopyWindowInfo(opt, id)! as? [NSDictionary],
             let screenBounds = NSScreen.main?.frame else { return nil }
         
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
             guard
                 let bounds = window["kCGWindowBounds"] as? NSDictionary,
                 let width  = bounds["Width"] as? CGFloat,
                 let height = bounds["Height"] as? CGFloat  else { continue }

             if width != screenBounds.width || height != screenBounds.height {
                 continue
             }
             
             // Skip all windows without having a name
             guard let name = window["kCGWindowName"] as? String else {
                 continue
             }
                 
             // Skip all windows with a name other than "Desktop picture - ..."
             if !name.hasPrefix("Desktop Picture") {
                 continue
             }
                 
             // Found it!
             guard let nr = window["kCGWindowNumber"] as? Int else {
                 continue
             }

             cgImage = CGWindowListCreateImage(
                 CGRect.null,
                 CGWindowListOption(arrayLiteral: CGWindowListOption.optionIncludingWindow),
                 CGWindowID(nr),
                 [])!
             break
         }
         
         // Create image
         var wallpaper: NSImage?
         if cgImage != nil {
             wallpaper = NSImage.init(cgImage: cgImage!, size: NSSize.zero)
             wallpaper = wallpaper?.expand(toSize: NSSize(width: 1024, height: 512))
         } else {
             // Fall back to an opaque gray background
             let size = NSSize(width: 128, height: 128)
             wallpaper = NSImage(color: .lightGray, size: size)
         }
         
         // Return image as texture
         return wallpaper?.toTexture(device: self)
     }
}
