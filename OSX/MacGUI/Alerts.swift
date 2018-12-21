//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
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
        alert.icon = NSImage.init(named: "metal")
        alert.messageText = "No suitable GPU hardware found"
        alert.informativeText = "VirtualC64 can only run on machines supporting the Metal graphics technology (2012 models and above)."
        alert.addButton(withTitle: "Exit")
        alert.runModal()
    }
}

extension MyDocument {
    
    @discardableResult
    func showDiskIsUnexportedAlert(drive nr: Int) -> NSApplication.ModalResponse {
       
        let alert = NSAlert()
        alert.alertStyle = .warning
        alert.icon = NSImage.init(named: "diskette")
        alert.messageText = "Drive \(nr) contains an unexported disk."
        alert.informativeText = "Your changes will be lost if you proceed."
        alert.addButton(withTitle: "Proceed")
        alert.addButton(withTitle: "Cancel")
        return alert.runModal()
    }
    
    @discardableResult
    func showDiskIsUnexportedAlert() -> NSApplication.ModalResponse {
        
        let alert = NSAlert()
        alert.alertStyle = .warning
        alert.icon = NSImage.init(named: "diskette")
        alert.messageText = "Drive 1 and 2 contain unexported disks."
        alert.informativeText = "Your changes will be lost if you proceed."
        alert.addButton(withTitle: "Proceed")
        alert.addButton(withTitle: "Cancel")
        return alert.runModal()
    }
    
    func proceedWithUnexportedDisk(drive nr: Int) -> Bool {
        
        precondition(nr == 1 || nr == 2)
        
        let controller = windowForSheet!.windowController! as! MyController
        if controller.ejectWithoutAsking {
            return true
        }
        
        let modified = (nr == 1) ?
            c64.drive1.hasModifiedDisk() :
            c64.drive2.hasModifiedDisk()

        if modified {
            return showDiskIsUnexportedAlert(drive: nr) == .alertFirstButtonReturn
        } else {
            return true
        }
    }
    
    func proceedWithUnexportedDisk() -> Bool {
    
        let controller = windowForSheet!.windowController! as! MyController
        if controller.ejectWithoutAsking {
            return true
        }
        
        let modified1 = c64.drive1.hasModifiedDisk()
        let modified2 = c64.drive2.hasModifiedDisk()
        
        if modified1 && modified2 {
            return showDiskIsUnexportedAlert() == .alertFirstButtonReturn
        } else if modified1 {
            return showDiskIsUnexportedAlert(drive: 1) == .alertFirstButtonReturn
        } else if modified2 {
            return showDiskIsUnexportedAlert(drive: 2) == .alertFirstButtonReturn
        } else {
            return true
        }
    }
    
    func showDiskIsEmptyAlert(format: String) {
        
        let alert = NSAlert()
        alert.alertStyle = .critical
        alert.icon = NSImage.init(named: "diskette")
        alert.messageText = "Cannot export an empty disk."
        alert.informativeText = "The \(format) format is designed to store a single file."
        alert.addButton(withTitle: "OK")
        alert.runModal()
    }
    
    func showDiskHasMultipleFilesAlert(format: String) {
        
        let alert = NSAlert()
        alert.alertStyle = .informational
        alert.icon = NSImage.init(named: "diskette")
        alert.messageText = "Only the first file will be exported."
        alert.informativeText = "The \(format) format is designed to store a single file."
        alert.addButton(withTitle: "OK")
        alert.runModal()
    }
    
    func showExportErrorAlert(url: URL) {
        
        let path = url.path
        let alert = NSAlert()
        alert.alertStyle = .critical
        alert.icon = NSImage.init(named: "diskette")
        alert.messageText = "Failed to export disk to file"
        alert.informativeText = "\(path)."
        alert.addButton(withTitle: "OK")
        alert.runModal()
    }
}

extension MyController {
            
    func proceedWithUnexportedDisk(drive nr: Int) -> Bool {
        return mydocument.proceedWithUnexportedDisk(drive: nr)
    }

    func proceedWithUnexportedDisk() -> Bool {
        return mydocument.proceedWithUnexportedDisk()
    }

}
