// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension URL {
    
    enum FolderError: Error {
        case noAppSupportFolder
    }
    enum UnpackError: Error {
        case noSupportedFiles
    }
    
    //
    // Working with folders
    //
    
    // Returns the URL of the application support folder of this application
    static func appSupportFolder() throws -> URL {
        
        let fm = FileManager.default
        let path = FileManager.SearchPathDirectory.applicationSupportDirectory
        let mask = FileManager.SearchPathDomainMask.userDomainMask
        
        if let url = fm.urls(for: path, in: mask).first {
            return url.appendingPathComponent("VirtualC64")
        } else {
            throw FolderError.noAppSupportFolder
        }
    }
    
    // Returns the URL of a subdirectory inside the application support folder
    static func appSupportFolder(_ name: String) throws -> URL {
        
        let support = try URL.appSupportFolder()
        
        let fm = FileManager.default
        let folder = support.appendingPathComponent("\(name)")
        var isDirectory: ObjCBool = false
        let folderExists = fm.fileExists(atPath: folder.path,
                                         isDirectory: &isDirectory)
        
        if !folderExists || !isDirectory.boolValue {
            
            try fm.createDirectory(at: folder,
                                   withIntermediateDirectories: true,
                                   attributes: nil)
        }
        
        return folder
    }
    
    // Returns the URL of an empty temporary folder
    static func tmpFolder() throws -> URL {
        
        let tmp = try appSupportFolder("tmp")
        try tmp.delete()
        return tmp
    }
    
    // Returns all files inside a folder
    func contents(allowedTypes: [String]? = nil) throws -> [URL] {
        
        let urls = try FileManager.default.contentsOfDirectory(
            at: self, includingPropertiesForKeys: nil,
            options: [.skipsHiddenFiles, .skipsSubdirectoryDescendants]
        )
        
        let filtered = urls.filter { allowedTypes?.contains($0.pathExtension) ?? true }
        // track("filtered = \(filtered)")
        return filtered
    }
    
    // Deletes all files inside a folder
    func delete() throws {
        
        let urls = try self.contents()
        for url in urls { try FileManager.default.removeItem(at: url) }
    }
    
    // Copies a file into the specified folder
    func copy(to folder: URL, replaceExtensionBy suffix: String) throws -> URL {
        
        // Create the destination URL
        var dest = folder.appendingPathComponent(self.lastPathComponent)
        dest.deletePathExtension()
        dest.appendPathExtension(suffix)
        
        // Copy the file
        try FileManager.default.copyItem(at: self, to: dest)
        
        return dest
    }
    
    var unpacked: URL {
        
        if self.pathExtension == "zip" || self.pathExtension == "adz" {
            
            do { return try unpackZip() } catch { }
        }
        
        if self.pathExtension == "gz" || self.pathExtension == "adz" {
            
            do { return try unpackGz() } catch { }
        }
        
        return self
    }
    
    func unpackZip() throws -> URL {
        
        let urls = try unpack(suffix: "zip")
        if let first = urls.first { return first }
        
        throw UnpackError.noSupportedFiles
    }
    
    func unpackGz() throws -> URL {
        
        let urls = try unpack(suffix: "gz")
        if let first = urls.first { return first }
        
        throw UnpackError.noSupportedFiles
    }
    
    func unpack(suffix: String) throws -> [URL] {
        
        // Request the URL of a tempory folder
        let tmp = try URL.tmpFolder()
        
        // Copy the compressed file into it and fix the extension
        let url = try self.copy(to: tmp, replaceExtensionBy: suffix)
        
        // Try to decompress the file
        var exec: String
        var args: [String]
        
        switch suffix {
            
        case "zip":
            exec = "/usr/bin/unzip"
            args = [ "-o", url.path, "-d", tmp.path ]
            
        case "gz":
            exec = "/usr/bin/gunzip"
            args = [ url.path ]
            
        default:
            fatalError()
        }
        
        track("exec = \(exec)")
        track("args = \(args)")
        
        if let result = FileManager.exec(launchPath: exec, arguments: args) {
            print("\(result)")
        }
        
        // Collect all extracted URLs with a supported file type
        let types = ["adf", "dms", "rom", "bin", "vAmiga"]
        let urls = try tmp.contents(allowedTypes: types)
        
        // Arrange the URLs in alphabetical order
        let sorted = urls.sorted { $0.path < $1.path }
        return sorted
    }
    
    var modificationDate: Date? {
        
        guard let resVal = try? resourceValues(forKeys: [.contentModificationDateKey]) else {
            return nil
        }
        
        return resVal.contentModificationDate
    }
    
    func byAddingTimeStamp() -> URL {
        
        let path = self.deletingPathExtension()
        var lastComp = path.lastPathComponent
        let suffix = self.pathExtension
        
        let date = Date()
        let formatter = DateFormatter()
        formatter.dateFormat = "yyyy-MM-dd"
        let dateString = formatter.string(from: date)
        formatter.dateFormat = "hh.mm.ss"
        let timeString = formatter.string(from: date)
        lastComp.append(contentsOf: " \(dateString) at \(timeString)")
        
        return self.deletingLastPathComponent().appendingPathComponent(lastComp, isDirectory: false).appendingPathExtension(suffix)
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
    
    func byAddingExtension(for format: NSBitmapImageRep.FileType) -> URL {
        
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
