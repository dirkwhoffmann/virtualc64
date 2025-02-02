// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import UniformTypeIdentifiers

@MainActor
class MyFilePanel : NSObject, NSOpenSavePanelDelegate {

    var allowedExtensions: [String]?

    func configure(types: [UTType]? = nil) {
        
        // Collect the file extensions of all allowed UTTypes
        if let types = types {
            allowedExtensions = types.compactMap { $0.preferredFilenameExtension?.lowercased() }
        } else {
            allowedExtensions = nil
        }
    }
    
    func panel(_ sender: Any, shouldEnable url: URL) -> Bool {
        
        func isDirectory(_ url: URL) -> Bool {
            
            let resourceValues = try? url.resourceValues(forKeys: [.isDirectoryKey, .isPackageKey])
            return resourceValues?.isDirectory == true && resourceValues?.isPackage != true
        }
        
        // Allow directories
        if isDirectory(url) { return true }
 
        // Check files extensions
        return allowedExtensions?.contains(url.pathExtension.lowercased()) ?? true
    }
}

class MyOpenPanel : MyFilePanel {
    
    let panel = NSOpenPanel()
    
    let defaultPrompt: String
    let defaultTitle: String
    let defaultNameFieldStringValue: String
    
    var url: URL? { return panel.url }

    override init() {
        
        defaultPrompt = panel.prompt
        defaultTitle = panel.title
        defaultNameFieldStringValue = panel.nameFieldStringValue
        
        super.init()
        
        panel.delegate = self
    }
    
    func configure(types: [UTType]? = nil,
                   prompt: String? = nil,
                   title: String? = nil,
                   nameFieldLabel: String? = nil,
                   nameFieldStringValue: String? = nil) {
 
        super.configure(types: types)
        
        panel.canCreateDirectories = false
        panel.prompt = prompt ?? defaultPrompt
        panel.title = title ?? defaultTitle
        panel.nameFieldStringValue = nameFieldStringValue ?? defaultNameFieldStringValue
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
    
    let defaultPrompt: String
    let defaultTitle: String
    let defaultNameFieldLabel: String
    let defaultNameFieldStringValue: String

    var url: URL? { return panel.url }

    override init() {
        
        defaultPrompt = panel.prompt
        defaultTitle = panel.title
        defaultNameFieldLabel = panel.nameFieldLabel
        defaultNameFieldStringValue = panel.nameFieldStringValue

        super.init()

        panel.delegate = self
    }
    
    func configure(types: [UTType]? = nil,
                   prompt: String? = nil,
                   title: String? = nil,
                   nameFieldLabel: String? = nil,
                   nameFieldStringValue: String? = nil) {
 
        super.configure(types: types)
        
        panel.canCreateDirectories = false
        panel.prompt = prompt ?? defaultPrompt
        panel.title = title ?? defaultTitle
        panel.nameFieldLabel = nameFieldLabel ?? defaultNameFieldLabel
        panel.nameFieldStringValue = nameFieldStringValue ?? defaultNameFieldStringValue
    }
    
    func open(for window: NSWindow?,
              _ handler: @escaping (NSApplication.ModalResponse) -> Void) {

        if let window = window {
            panel.beginSheetModal(for: window, completionHandler: handler)
        }
    }
}
