// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

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
    
    static func fileAccessError(filename: String) -> NSError {
        return NSError(domain: "VirtualC64", code: 0, userInfo:
            [NSLocalizedDescriptionKey: "The document \"\(filename)\" could not be opened.",
                NSLocalizedRecoverySuggestionErrorKey: "Unable to access file."])
    }

    static func fileAccessError() -> NSError {
        return NSError(domain: "VirtualC64", code: 0, userInfo:
            [NSLocalizedDescriptionKey: "The document could not be opened.",
                NSLocalizedRecoverySuggestionErrorKey: "Unable to access file."])
    }
    
    static func unsupportedCartridgeError(filename: String, type: String) -> NSError {
        return NSError(domain: "VirtualC64", code: 0, userInfo:
            [NSLocalizedDescriptionKey: "The document \"\(filename)\" could not be opened.",
                NSLocalizedRecoverySuggestionErrorKey: "Cartridges of type \"\(type)\" are not supported by the emulator, yet."])
    }
}

extension MyDocument {
    
    func showNoMetalSupportAlert() {
        
        let alert = NSAlert()
        alert.alertStyle = .critical
        alert.icon = NSImage.init(named: "metal")
        alert.messageText = "No suitable GPU hardware found"
        alert.informativeText = "VirtualC64 can only run on machines supporting the Metal graphics technology (2012 models and above)."
        alert.addButton(withTitle: "Exit")
        alert.runModal()
    }
    
    @discardableResult
    func showDiskIsUnexportedAlert(drive: DriveID) -> NSApplication.ModalResponse {
       
        let label = drive == DRIVE8 ? "8" : "9"

        let alert = NSAlert()
        alert.alertStyle = .warning
        alert.icon = NSImage.init(named: "diskette")
        alert.messageText = "Drive \(label) contains an unexported disk."
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
    
    func proceedWithUnexportedDisk(drive: DriveID) -> Bool {
        
        precondition(drive == DRIVE8 || drive == DRIVE9)
        
        if let controller = windowForSheet?.windowController as? MyController {
            if controller.pref.driveEjectUnasked {
                return true
            }
        }
        
        let modified = (drive == DRIVE8) ?
            c64.drive8.hasModifiedDisk() :
            c64.drive9.hasModifiedDisk()

        if modified {
            return showDiskIsUnexportedAlert(drive: drive) == .alertFirstButtonReturn
        } else {
            return true
        }
    }
    
    func proceedWithUnexportedDisk() -> Bool {
    
        if let controller = windowForSheet?.windowController as? MyController {
            if controller.pref.driveEjectUnasked {
                return true
            }
        }
        
        let modified1 = c64.drive8.hasModifiedDisk()
        let modified2 = c64.drive9.hasModifiedDisk()
        
        if modified1 && modified2 {
            return showDiskIsUnexportedAlert() == .alertFirstButtonReturn
        } else if modified1 {
            return showDiskIsUnexportedAlert(drive: DRIVE8) == .alertFirstButtonReturn
        } else if modified2 {
            return showDiskIsUnexportedAlert(drive: DRIVE9) == .alertFirstButtonReturn
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

    func showCannotDecodeDiskAlert() {

        let alert = NSAlert()
        alert.alertStyle = .critical
        alert.icon = NSImage.init(named: "diskette")
        alert.messageText = "Failed to decode all sectors."
        alert.informativeText = "The disk is either copy protected or corrupted."
        alert.addButton(withTitle: "OK")
        alert.runModal()
    }
    
    func showConfigurationAltert(_ error: ErrorCode) {
        
        var msg: String
        
        switch error {
        case ERR_ROM_MISSING:
            msg = "A Basic Rom, Character Rom, and Kernal Rom is required to power up."
        case ERR_ROM_MEGA65_MISMATCH:
            msg = "The emulator cannot launch because OpenROMs with mismatched version strings have been installed."
        default:
            msg = ""
        }
        
        let alert = NSAlert()
        alert.alertStyle = .informational
        alert.icon = NSImage.init(named: "rom_mega65")
        alert.messageText = "Configuration error"
        alert.informativeText = msg
        alert.addButton(withTitle: "OK")
        alert.runModal()
    }
}

extension MyController {
            
    func proceedWithUnexportedDisk(drive: DriveID) -> Bool {
        return mydocument?.proceedWithUnexportedDisk(drive: drive) ?? false
    }

    func proceedWithUnexportedDisk() -> Bool {
        return mydocument?.proceedWithUnexportedDisk() ?? false
    }

}
