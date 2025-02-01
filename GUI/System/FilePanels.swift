// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import UniformTypeIdentifiers

class MyOpenPanel : NSObject, NSOpenSavePanelDelegate {
    
    let panel = NSOpenPanel()
    var allowedExtensions: [String]?
    var url: URL? { return panel.url }
    
    override init() {
        
        super.init()
        panel.delegate = self
    }
    
    func configure(prompt: String? = nil, types: [UTType]? = nil) {
 
        // Apply some standard settings
        panel.allowsMultipleSelection = false
        panel.canChooseDirectories = false
        panel.canCreateDirectories = false
        panel.canChooseFiles = true
        
        // Assign the prompt
        panel.prompt = prompt ?? "Open"
        
        // Collect the file extensions of all allowed UTTypes
        if let types = types {
            allowedExtensions = types.compactMap { $0.preferredFilenameExtension }
        } else {
            allowedExtensions = nil
        }
    }
    
    func open(for window: NSWindow?,
              _ handler: @escaping (NSApplication.ModalResponse) -> Void) {

        if let window = window {
            panel.beginSheetModal(for: window, completionHandler: handler)
        }
    }

    func panel(_ sender: Any, shouldEnable url: URL) -> Bool {
        
        // Allow directories
        var isDirectory: ObjCBool = false
        FileManager.default.fileExists(atPath: url.path, isDirectory: &isDirectory)
        if isDirectory.boolValue { return true }
        
        // Check files extensions
        if let extensions = allowedExtensions {
            return extensions.contains(url.pathExtension.lowercased())
        } else {
            return true
        }
    }
}
