//
//  Alerts.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 29.01.18.
//

import Foundation

extension NSError {

    static func snapshotVersionError(filename: String) -> NSError {
        return NSError(domain: "VirtualC64", code: 0, userInfo:
            [NSLocalizedDescriptionKey: "The document \"\(filename)\" could not be opened.",
                NSLocalizedRecoverySuggestionErrorKey: "The snapshot was created with a different version of VirtualC64."])
    }

    static func unsupportedFormatError(filename: String) -> NSError {
        return NSError(domain: "VirtualC64", code: 0, userInfo:
            [NSLocalizedDescriptionKey: "The document \"\(filename)\" could not be opened.",
                NSLocalizedRecoverySuggestionErrorKey: "The format of this file is not supported."])
    }

    static func corruptedFileError(filename: String) -> NSError {        
        return NSError(domain: "VirtualC64", code: 0, userInfo:
            [NSLocalizedDescriptionKey: "The document \"\(filename)\" could not be opened.",
                NSLocalizedRecoverySuggestionErrorKey: "The file appears to be corrupt. It's contents does not match the purported format."])
    }
    
    static func unsupportedCartridgeError(filename: String, type: String) -> NSError {
        return NSError(domain: "VirtualC64", code: 0, userInfo:
            [NSLocalizedDescriptionKey: "The document \"\(filename)\" could not be opened.",
                NSLocalizedRecoverySuggestionErrorKey: "Cartridges of type \"\(type)\" are no supported by the emulator, yet."])
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

extension MyDocument {
    
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
    
    func proceedWithUnsavedDisk() -> Bool {
        
        if c64.vc1541.hasModifiedDisk() {
            return showDiskIsUnsafedAlert() == .alertFirstButtonReturn
        } else {
            return true
        }
    }
}

extension MyController {
    
    /*
    func showDiskHasBeenExportedAlert(url: URL) {
        
        let path = url.path
        let alert = NSAlert()
        alert.alertStyle = .informational
        alert.icon = NSImage.init(named: NSImage.Name(rawValue: "diskette"))
        alert.messageText = "Disk has been exported successfully."
        alert.informativeText = "Destination: \(path)"
        alert.addButton(withTitle: "OK")
        alert.runModal()
    }
    */
    
    func showExportErrorAlert(url: URL) {
        
        let path = url.path
        let alert = NSAlert()
        alert.alertStyle = .critical
        alert.icon = NSImage.init(named: NSImage.Name(rawValue: "diskette"))
        alert.messageText = "Failed to export disk to file"
        alert.informativeText = "\(path)."
        alert.addButton(withTitle: "OK")
        alert.runModal()
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
    
    func userSnapshotStorageFull() {
        
        let alert = NSAlert()
        alert.alertStyle = .warning
        // alert.icon = NSImage.init(named: NSImage.Name(rawValue: "diskette"))
        alert.messageText = "Cannot save snapshot"
        alert.informativeText = "All slots are filled. Delete some snapshots manually."
        alert.addButton(withTitle: "OK")
        alert.runModal()
    }
    
    func showResidSamplingMethodAlert() {
        
        let alert = NSAlert()
        alert.alertStyle = .warning // .critical
        alert.icon = NSImage.init(named: NSImage.Name(rawValue: "pref_sid"))
        alert.messageText = "Incompatible Sampling Method"
        alert.informativeText = "The current sampling method is not compatible with the selected chip model and will be changed to \"Interpolate\"."
        alert.addButton(withTitle: "OK")
        alert.runModal()
    }
    
    func proceedWithUnsavedDisk() -> Bool {
        
        let document = self.document as! MyDocument
        return document.proceedWithUnsavedDisk()
    }
}
