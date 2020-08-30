// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class Screenshot {
    
    // The actual screenshot
    var screen: NSImage?
    
    // Proposed file format for this screenshot
    var format = NSBitmapImageRep.FileType.jpeg
    
    // Creation date
    var data = Date()
    
    // Image width and heigt
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
    
    init(screen: NSImage, format: NSBitmapImageRep.FileType) {
        
        self.screen = screen
        self.format = format
        data = Date()
    }
    
    convenience init?(fromUrl url: URL) {
        
        guard let format = url.imageFormat else { return nil }
        guard let image = NSImage.init(contentsOf: url) else { return nil }

        self.init(screen: image, format: format)
    }
    
    /*
    func quicksave(format: NSBitmapImageRep.FileType) {
        
        // Get URL to desktop directory
        let paths = NSSearchPathForDirectoriesInDomains(.desktopDirectory,
                                                        .userDomainMask,
                                                        true)
        let desktop = NSURL.init(fileURLWithPath: paths[0])
        
        // Assemble filename
        if var url = desktop.appendingPathComponent("Screenshot.") {
            
            url = url.byAddingExtension(for: format)
            url = url.byAddingTimeStamp()
            url = url.makeUnique()
            
            do {
                try save(url: url, format: format)
            } catch {
                track("Failed to quicksave screenshot to \(url.path)")
            }
        }
    }
    */
        
    func save(id: UInt64) throws {
                
        if let url = Screenshot.newUrl(diskID: id, using: format) {
            try? save(url: url)
        }
    }

    func save(url: URL) throws {
        
        // Convert to target format
        let data = screen?.representation(using: format)
        
        // Save to file
        try data?.write(to: url, options: .atomic)
    }
    
    static func folder(forDisk diskID: UInt64) -> URL? {
        
        let subdir = String(format: "%08X", diskID)
        do {
            return try URL.appSupportFolder("Screenshots/\(subdir)")
        } catch {
            return nil
        }
    }
        
    static func fileExists(name: URL, type: NSBitmapImageRep.FileType) -> URL? {
        
        let url = name.byAddingExtension(for: type)
        return FileManager.default.fileExists(atPath: url.path) ? url : nil
    }
    
    static func url(forItem item: Int, in folder: URL?) -> URL? {
        
        if folder == nil { return nil }

        let types: [NSBitmapImageRep.FileType] = [ .tiff, .bmp, .gif, .jpeg, .png ]
        let url = folder!.appendingPathComponent(String(format: "%03d", item))
        
        for type in types {
            if let url = fileExists(name: url, type: type) { return url }
        }
        
        return nil
    }
        
    static func newUrl(diskID: UInt64,
                       using format: NSBitmapImageRep.FileType = .jpeg) -> URL? {

        let folder = Screenshot.folder(forDisk: diskID)
        return Screenshot.newUrl(in: folder, using: format)
    }

    static func newUrl(in folder: URL?,
                       using format: NSBitmapImageRep.FileType = .jpeg) -> URL? {
                
        if folder == nil { return nil }
        
        track("Determining next free URL in \(folder!)")

        // Get a list of all filenames without extensions
        let files = collectFiles(in: folder)
        let names = files.map({ (url) -> String in
            return url.deletingPathExtension().lastPathComponent
        })
        
        // Determine new name
        for i in 0...999 {
            let name = String(format: "%03d", i)
            if !names.contains(name) {
                let url = folder!.appendingPathComponent(name)
                return url.byAddingExtension(for: format)
            } else {
                track("\(name) already exists")
            }
        }
        
        /*
            let url = folder!.appendingPathComponent(filename).byAddingExtension(for: format)
            
            if !FileManager.default.fileExists(atPath: url.path) {
                return url
            }
        }
        */
        
        return nil
    }
        
    static func collectFiles(in folder: URL?) -> [URL] {
        
        var result = [URL]()
        
        for i in 0...999 {
            
            if let url = Screenshot.url(forItem: i, in: folder) {
                result.append(url)
            } else {
                break
            }
        }
        return result
    }

    static func collectFiles(forDisk diskID: UInt64) -> [URL] {
        
        return collectFiles(in: folder(forDisk: diskID))
    }
    
    static func deleteFolder(withUrl url: URL?) {
        
        let files = Screenshot.collectFiles(in: url)
        for file in files {
            try? FileManager.default.removeItem(at: file)
        }
    }

    static func deleteFolder(forDisk diskID: UInt64) {
        
        deleteFolder(withUrl: folder(forDisk: diskID))
    }

}
