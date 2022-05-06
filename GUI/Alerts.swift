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

extension MyController {

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
