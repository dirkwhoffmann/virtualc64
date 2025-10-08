// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension FileManager {
    
    static func exec(launchPath: String, arguments: [String]) -> String {
        
        let task = Process()
        task.launchPath = launchPath
        task.arguments = arguments
        
        let pipe = Pipe()
        task.standardOutput = pipe
        task.standardError = pipe
        task.launch()
        
        let data = pipe.fileHandleForReading.readDataToEndOfFile()
        // let result = String(data: data, encoding: .utf8)
        let result = String(decoding: data, as: UTF8.self)
        task.waitUntilExit()
        
        return result
    }
    
    @discardableResult
    static func copy(from source: URL, to dest: URL) -> Bool {
        
        do {
            if FileManager.default.fileExists(atPath: dest.path) {
                try FileManager.default.removeItem(at: dest)
            }
            try FileManager.default.copyItem(at: source, to: dest)
        } catch {
            return false
        }
        return true
    }
}
