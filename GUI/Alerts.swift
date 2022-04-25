// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
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
        alert.icon = NSImage(named: "metal")
        alert.messageText = "No suitable GPU hardware found"
        alert.informativeText = "VirtualC64 can only run on machines supporting the Metal graphics technology (2012 models and above)."
        alert.addButton(withTitle: "Exit")
        alert.runModal()
    }
    
    @discardableResult
    func showDiskIsUnexportedAlert(drive: DriveID) -> NSApplication.ModalResponse {
       
        let label = drive == .DRIVE8 ? "8" : "9"

        let alert = NSAlert()
        alert.alertStyle = .warning
        alert.icon = NSImage(named: "diskette")
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
        alert.icon = NSImage(named: "diskette")
        alert.messageText = "Drive 1 and 2 contain unexported disks."
        alert.informativeText = "Your changes will be lost if you proceed."
        alert.addButton(withTitle: "Proceed")
        alert.addButton(withTitle: "Cancel")
        return alert.runModal()
    }
    
    func proceedWithUnexportedDisk(drive: DriveID) -> Bool {
        
        precondition(drive == .DRIVE8 || drive == .DRIVE9)
        
        if let controller = windowForSheet?.windowController as? MyController {
            if controller.pref.ejectWithoutAsking {
                return true
            }
        }
        
        let modified = (drive == .DRIVE8) ?
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
            if controller.pref.ejectWithoutAsking {
                return true
            }
        }
        
        let modified1 = c64.drive8.hasModifiedDisk()
        let modified2 = c64.drive9.hasModifiedDisk()
        
        if modified1 && modified2 {
            return showDiskIsUnexportedAlert() == .alertFirstButtonReturn
        } else if modified1 {
            return showDiskIsUnexportedAlert(drive: .DRIVE8) == .alertFirstButtonReturn
        } else if modified2 {
            return showDiskIsUnexportedAlert(drive: .DRIVE9) == .alertFirstButtonReturn
        } else {
            return true
        }
    }
}

extension MyController {
            
    func proceedWithUnexportedDisk(drive: DriveID) -> Bool {
        return mydocument.proceedWithUnexportedDisk(drive: drive)
    }

    func proceedWithUnexportedDisk() -> Bool {
        return mydocument.proceedWithUnexportedDisk()
    }
    
    func showMissingFFmpegAlert() {

        let alert = NSAlert()
        alert.alertStyle = .warning
        alert.icon = NSImage(named: "FFmpegIcon")
        alert.messageText = "Screen recording requires FFmpeg to be installed in /usr/local/bin."
        alert.informativeText = "Visit FFmpeg.org for installation instructions."
        alert.addButton(withTitle: "OK")
        alert.runModal()
    }

    func showFailedToLaunchFFmpegAlert() {

        let alert = NSAlert()
        alert.alertStyle = .warning
        alert.icon = NSImage(named: "FFmpegIcon")
        alert.messageText = "Failed to launch the screen recorder."
        alert.informativeText = "No content will be recorded."
        alert.addButton(withTitle: "OK")
        alert.runModal()
    }

    func showScreenRecorderAlert(url: URL) {

        let alert = NSAlert()
        alert.alertStyle = .critical
        alert.icon = NSImage(named: "FFmpegIcon")
        alert.messageText = "\"\(url.lastPathComponent)\" cannot be opened."
        alert.informativeText = "The screen recorder failed to open this file for output."
        alert.addButton(withTitle: "OK")
        
        alert.beginSheetModal(for: self.window!) { (_: NSApplication.ModalResponse) -> Void in }
    }
}
