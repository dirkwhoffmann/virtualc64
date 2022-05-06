// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class Screenshot: CustomStringConvertible {
    
    // The actual screenshot
    var screen: NSImage?
    
    // Proposed file format for this screenshot
    var format = NSBitmapImageRep.FileType.jpeg
    
    // Creation date
    var date = Date()

    // Image width and height
    var width: Int { return Int(screen?.size.width ?? 0) }
    var height: Int { return Int(screen?.size.height ?? 0) }

    // Indicates if the upscaled texture has been recorded
    var upscaled: Bool { return height > 1000 }

    // Textual description of the image source
    var sourceString: String {
        return upscaled ? "upscaled texture" : "emulator texture"
    }
    
    // Textual description of the image format
    var formatString: String {
        switch format {
        case .tiff: return "TIFF image"
        case .bmp:  return "BMP image"
        case .gif:  return "GIF image"
        case .jpeg: return "JPEG image"
        case .png:  return "PNG image"
        default:    return "Image"
        }
    }
    
    // Combined textual description
    var description: String {
        return formatString + " from " + sourceString
    }
    
    // Textual representation of the image size
    var sizeString: String {
        return screen != nil ? "\(width) x \(height)" : ""
    }
    
    static var folder: URL? {
        do {
            return try URL.appSupportFolder("Screenshots")
        } catch {
            return nil
        }
    }
    
    static var allFiles: [URL] {
        
        var result = [URL]()
        
        for i in 0...999 {
            
            if let url = Screenshot.url(forItem: i) {
                result.append(url)
            } else {
                break
            }
        }
        return result
    }
    
    init(screen: NSImage, format: NSBitmapImageRep.FileType) {
        
        self.screen = screen
        self.format = format
        date = Date()
    }
    
    convenience init?(fromUrl url: URL) {
        
        guard let format = url.imageFormat else { return nil }
        guard let image = NSImage(contentsOf: url) else { return nil }

        self.init(screen: image, format: format)
    }
    
    func save() throws {
                
        if let url = Screenshot.newUrl(format: format) {
            try? save(url: url)
        }
    }

    func save(url: URL) throws {
        
        // Convert to target format
        let data = screen?.representation(using: format)
        
        // Save to file
        try data?.write(to: url, options: .atomic)
    }
            
    static func fileExists(name: URL, type: NSBitmapImageRep.FileType) -> URL? {
        
        let url = name.byAddingExtension(for: type)
        return FileManager.default.fileExists(atPath: url.path) ? url : nil
    }
    
    static func url(forItem item: Int) -> URL? {
        
        if folder == nil { return nil }

        let types: [NSBitmapImageRep.FileType] = [ .tiff, .bmp, .gif, .jpeg, .png ]
        let url = folder!.appendingPathComponent(String(format: "%03d", item))
        
        for type in types {
            if let url = fileExists(name: url, type: type) { return url }
        }
        
        return nil
    }
    
    static func newUrl(format: NSBitmapImageRep.FileType = .jpeg) -> URL? {
                
        if folder == nil { return nil }
        
        debug("Determining next free URL in \(folder!)", level: 2)

        // Get a list of all filenames without extensions
        let names = allFiles.map({ (url) -> String in
            return url.deletingPathExtension().lastPathComponent
        })
        
        // Determine new name
        for i in 0...999 {
            let name = String(format: "%03d", i)
            if !names.contains(name) {
                let url = folder!.appendingPathComponent(name)
                return url.byAddingExtension(for: format)
            } else {
                debug("\(name) already exists", level: 2)
            }
        }
        
        return nil
    }
    
    static func deleteFolder() {
        
        for file in allFiles {
            try? FileManager.default.removeItem(at: file)
        }
    }
}
