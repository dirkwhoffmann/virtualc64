// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import UniformTypeIdentifiers

class MyFilePanel : NSObject, NSOpenSavePanelDelegate {

    var allowedExtensions: [String]?

    func configure(types: [UTType]? = nil) {
        
        // Collect the file extensions of all allowed UTTypes
        if let types = types {
            allowedExtensions = types.compactMap { $0.preferredFilenameExtension }
        } else {
            allowedExtensions = nil
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

class MyOpenPanel : MyFilePanel {
    
    let panel = NSOpenPanel()
    var url: URL? { return panel.url }
    
    override init() {
        
        super.init()
        panel.delegate = self
    }
    
    func configure(prompt: String? = nil, types: [UTType]? = nil) {
 
        super.configure(types: types)
        
        panel.allowsMultipleSelection = false
        panel.canChooseDirectories = false
        panel.canCreateDirectories = false
        panel.canChooseFiles = true
        panel.prompt = prompt ?? "Open"
    }
    
    func open(for window: NSWindow?,
              _ handler: @escaping (NSApplication.ModalResponse) -> Void) {

        if let window = window {
            panel.beginSheetModal(for: window, completionHandler: handler)
        }
    }
}

class MySavePanel : MyFilePanel {
    
    let panel = NSSavePanel()
    var url: URL? { return panel.url }
    
    override init() {
        
        super.init()
        panel.delegate = self
    }
    
    func configure(prompt: String? = nil, types: [UTType]? = nil) {
 
        super.configure(types: types)
        
        panel.canCreateDirectories = false
        panel.prompt = prompt ?? "Save"
    }
    
    func open(for window: NSWindow?,
              _ handler: @escaping (NSApplication.ModalResponse) -> Void) {

        if let window = window {
            panel.beginSheetModal(for: window, completionHandler: handler)
        }
    }
}
