// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension URL {
    
    static var appSupportFolder: URL? {
        
        let fm = FileManager.default
        let path = FileManager.SearchPathDirectory.applicationSupportDirectory
        let mask = FileManager.SearchPathDomainMask.userDomainMask
        let url = fm.urls(for: path, in: mask).first
        return url?.appendingPathComponent("VirtualC64")
    }
    
    static func appSupportFolder(_ name: String) -> URL? {
    
        guard let support = URL.appSupportFolder else { return nil }

        let fm = FileManager.default
        let folder = support.appendingPathComponent("\(name)")
        var isDirectory: ObjCBool = false
        let folderExists = fm.fileExists(atPath: folder.path,
                                         isDirectory: &isDirectory)
        
        if !folderExists || !isDirectory.boolValue {
            
            do {
                try fm.createDirectory(at: folder,
                                       withIntermediateDirectories: true,
                                       attributes: nil)
            } catch {
                return nil
            }
        }
        
        return folder
    }
    
    func modificationDate() -> Date? {
        
        let attr = try? FileManager.default.attributesOfItem(atPath: self.path)
        
        if attr != nil {
            return attr![.creationDate] as? Date
        } else {
            return nil
        }
    }
    
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
    
    func addExtension(for format: NSBitmapImageRep.FileType) -> URL {
        
        let extensions: [NSBitmapImageRep.FileType: String] =
            [ .tiff: "tiff", .bmp: "bmp", .gif: "gif", .jpeg: "jpeg", .png: "png" ]
        
        guard let ext = extensions[format] else {
            track("Unsupported image format: \(format)")
            return self
        }
        
        return self.appendingPathExtension(ext)
    }
    
    var imageFormat: NSBitmapImageRep.FileType? {
        
        switch pathExtension {
        case "tiff": return .tiff
        case "bmp": return .bmp
        case "gif": return .gif
        case "jpg", "jpeg": return .jpeg
        case "png": return .png
        default: return nil
        }
    }
}
