//
//  Alerts.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 29.01.18.
//

import Foundation

extension NSError {
    
    static func snapshotVersionError(filename: String) -> NSError {
        return NSError(domain: "", code: 0, userInfo:
            [NSLocalizedDescriptionKey: "Snapshot from other VirtualC64 release.",
             NSLocalizedRecoverySuggestionErrorKey: "\(filename) was created with a different version of VirtualC64 and cannot be opened."])
    }

    static func unsupportedFormatError(filename: String) -> NSError {
        return NSError(domain: "", code: 0, userInfo:
            [NSLocalizedDescriptionKey: "Unsupported file format.",
             NSLocalizedRecoverySuggestionErrorKey: "\(filename) cannot be opened because its format is not supported."])
    }

    static func corruptedFileError(filename: String) -> NSError {        
        return NSError(domain: "", code: 0, userInfo:
            [NSLocalizedDescriptionKey: "Corrupted file contents.",
             NSLocalizedRecoverySuggestionErrorKey: "\(filename) does not comply to its purported format and cannot be opened."])
    }
}

public extension MetalView {
    
    func showNoMetalSupportAlert() {
        
        let alert = NSAlert()
        alert.alertStyle = .critical
        alert.icon = NSImage.init(named: NSImage.Name(rawValue: "metal"))
        alert.messageText = "No suitable GPU hardware found"
        alert.informativeText = "VirtualC64 can only run on machines supporting the Metal graphics technology (2012 models and above)."
        alert.addButton(withTitle: "Exit")
        alert.runModal()
    }
}

extension MyController {
    
    @discardableResult
    func showDiskIsUnsafedAlert() -> NSApplication.ModalResponse {
       
        let alert = NSAlert()
        alert.alertStyle = .warning
        alert.icon = NSImage.init(named: NSImage.Name(rawValue: "diskette"))
        alert.messageText = "The current disk contains modified data."
        alert.informativeText = "Your changes will be lost if you proceed."
        alert.addButton(withTitle: "Proceed")
        alert.addButton(withTitle: "Cancel")
        return alert.runModal()
    }

    func showDiskIsEmptyAlert(format: String) {
        
        let alert = NSAlert()
        alert.alertStyle = .critical
        alert.icon = NSImage.init(named: NSImage.Name(rawValue: "diskette"))
        alert.messageText = "Cannot export an empty disk."
        alert.informativeText = "The \(format) format is designed to store a single file."
        alert.addButton(withTitle: "OK")
        alert.runModal()
    }
 
    func showDiskHasMultipleFilesAlert(format: String) {
        
        let alert = NSAlert()
        alert.alertStyle = .informational
        alert.icon = NSImage.init(named: NSImage.Name(rawValue: "diskette"))
        alert.messageText = "Only the first file will be exported."
        alert.informativeText = "The \(format) format is designed to store a single file."
        alert.addButton(withTitle: "OK")
        alert.runModal()
    }
    
    func showUnsupportedCartridgeAlert(_ container: CRTProxy) {
        
        let name = container.cartridgeTypeName() as String
        
        let alert = NSAlert()
        alert.alertStyle = .warning
        alert.icon = NSImage.init(named: NSImage.Name(rawValue: "cartridge"))
        alert.messageText = "Unsupported cartridge type: \(name)"
        alert.informativeText = "The provided cartridge contains special hardware which is not supported by the emulator yet."
        alert.addButton(withTitle: "OK")
        alert.beginSheetModal(for: window!, completionHandler: nil)
    }
    
    func userSnapshotStorageFull() {
        
        let alert = NSAlert()
        alert.alertStyle = .informational
        // alert.icon = NSImage.init(named: NSImage.Name(rawValue: "diskette"))
        alert.messageText = "Cannot save snapshot"
        alert.informativeText = "All slots are filled. Delete some snapshots manually."
        alert.addButton(withTitle: "OK")
        alert.runModal()
    }

}
