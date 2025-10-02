// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

//
// Logging / Debugging
//

public func log(_ enable: Int, _ msg: String = "",
                path: String = #file, function: String = #function, line: Int = #line) {

    if enable > 0 {

        if let file = URL(string: path)?.deletingPathExtension().lastPathComponent {
            if msg == "" {
                print("\(file).\(line)::\(function)")
            } else {
                print("\(file).\(line)::\(function): \(msg)")
            }
        }
    }
}

public func debug(_ enable: Int, _ msg: String = "",
                  path: String = #file, function: String = #function, line: Int = #line) {

    if !releaseBuild { log(enable, msg, path: path, function: function, line: line) }
}

public func warn(_ msg: String = "",
                 path: String = #file, function: String = #function, line: Int = #line) {

    log(1, "Warning: " + msg, path: path, function: function, line: line)
}

//
// Errors
//

class AppError: Error {
    
    var errorCode: vc64.Fault
    var what: String
    
    init(_ exception: ExceptionWrapper) {

        self.errorCode = exception.errorCode
        self.what = exception.what
    }
    
    init(_ errorCode: vc64.Fault, _ what: String = "") {
        
        self.errorCode = errorCode
        self.what = what
    }
}

extension NSError {

    convenience init(error: AppError) {

        self.init(domain: "VirtualC64",
                  code: error.errorCode.rawValue,
                  userInfo: [NSLocalizedRecoverySuggestionErrorKey: error.what])
    }
}

//
// Failures
//

enum Failure {

    case cantAttach
    case cantDecode
    case cantExport(url: URL)
    case cantInsert
    case cantInsertTape
    case cantOpen(url: URL)
    case cantRecord
    case cantRestore
    case cantRun
    case cantSaveRoms
    case noFFmpegFound(exec: String)
    case noFFmpegInstalled
    case noMetalSupport
    case unsupportedOSVersion
    case recorderAborted
    case recorderSandboxed(exec: String)
    case unknown

    var alertStyle: NSAlert.Style {

        switch self {

        case .noMetalSupport: return .critical
        case .unsupportedOSVersion: return .critical
            
        default: return .warning
        }
    }

    var icon: NSImage? {

        switch self {

        case .cantRecord: return NSImage(named: "FFmpegIcon")!
        // case .cantRun: return NSImage(named: "pref_transparent")!
        case .noFFmpegFound: return NSImage(named: "FFmpegIcon")!
        case .noFFmpegInstalled: return NSImage(named: "FFmpegIcon")!
        case .noMetalSupport: return NSImage(named: "metal")!
        case .recorderAborted: return NSImage(named: "mp4")!
        case .recorderSandboxed: return NSImage(named: "FFmpegIcon")!

        default: return nil
        }
    }

    var buttonTitle: String {

        switch self {

        case .noMetalSupport: return "Exit"

        default: return "OK"
        }
    }

    var description: String {

        switch self {

        case .cantAttach:
            return "Failed to attach cartridge."

        case .cantDecode:
            return "Unable to decode the file system."

        case let .cantExport(url: url):
            return "Cannot export disk to file \"\(url.path)\"."

        case .cantInsert:
            return "Failed to insert disk."

        case .cantInsertTape:
            return "Failed to insert tape."

        case let .cantOpen(url):
            return "\"\(url.lastPathComponent)\" can't be opened."

        case .cantRecord:
            return "Failed to launch the screen recorder."

        case .cantRestore:
            return "Failed to restore snapshot."

        case .cantRun:
            return "Configuration error"

        case .cantSaveRoms:
            return "Failed to save Roms."

        case .noFFmpegFound:
            return "Unable to locate FFmpeg."

        case .noFFmpegInstalled:
            return "Screen recording requires FFmpeg to be installed."

        case .noMetalSupport:
            return "No suitable GPU hardware found."

        case .unsupportedOSVersion:
            return "Incompatible macOS version."

        case .recorderAborted:
            return "Screen recording has been aborted."

        case .recorderSandboxed:
            return "The selected screen recorder is unavailable."

        default:
            return ""
        }
    }

    var explanation: String {

        switch self {

        case let .noFFmpegFound(exec): return
            "\"\(exec)\" not found."

        case .noFFmpegInstalled: return
            "Visit FFmpeg.org for installation instructions."

        case .noMetalSupport: return
            "The emulator can only run on machines supporting the Metal graphics " +
            "technology (2012 models and above)."

        case .unsupportedOSVersion: return
            "The emulator is not yet compatible with macOS Tahoe."

        case .recorderAborted: return
            "Failed to write to the FFmpeg pipes."

        case let .recorderSandboxed(exec): return
            "The emulator is running as a sandboxed application and has no " +
            "permission to access file \"\(exec)\"" +
            "Please copy the file to the Applications folder."

        default:
            return ""
        }
    }
}

//
// Alerts
//

extension NSAlert {

    @discardableResult
    func runSheet(for window: NSWindow, async: Bool = false) -> NSApplication.ModalResponse {

        if async {

            beginSheetModal(for: window)
            return .OK

        } else {

            beginSheetModal(for: window, completionHandler: NSApp.stopModal(withCode:))
            return NSApp.runModal(for: window)
        }
    }
}

extension MyDocument {

    func showAlert(_ failure: Failure, what: String? = nil,
                   async: Bool = false, window: NSWindow? = nil) {

        let alert = NSAlert()
        alert.alertStyle = failure.alertStyle
        alert.icon = failure.icon
        alert.messageText = failure.description
        alert.informativeText = what ?? failure.explanation
        alert.addButton(withTitle: failure.buttonTitle)

        if let window = window {
            alert.runSheet(for: window, async: async)
        } else if let window = windowForSheet {
            alert.runSheet(for: window, async: async)
        } else {
            alert.runModal()
        }
    }

    func showAlert(_ failure: Failure, error: Error,
                   async: Bool = false, window: NSWindow? = nil) {

        if let error = error as? AppError {
            showAlert(failure, what: error.what, async:
                        async, window: window)
        } else {
            showAlert(failure, what: error.localizedDescription,
                      async: async, window: window)
        }
    }
}

extension MyController {

    func showAlert(_ failure: Failure, what: String? = nil,
                   async: Bool = false, window: NSWindow? = nil) {

        mydocument.showAlert(failure, what: what, async: async, window: window)
    }

    func showAlert(_ failure: Failure, error: Error,
                   async: Bool = false, window: NSWindow? = nil) {

        mydocument.showAlert(failure, error: error, async: async, window: window)
    }
}

//
// Alert dialogs
//

extension MyDocument {

    func showMultipleFilesAlert(msg1: String, msg2: String) {

        let alert = NSAlert()
        alert.alertStyle = .informational
        alert.messageText = msg1
        alert.informativeText = msg1
        alert.addButton(withTitle: "OK")

        alert.runSheet(for: windowForSheet!)
    }

    func showLaunchAlert(error: Error) {
             
        var reason: String
        if let error = error as? AppError {
            reason = error.what
        } else {
            reason = error.localizedDescription
        }
        
        let alert = NSAlert()
        alert.alertStyle = .warning
        alert.icon = NSImage(named: "biohazard")
        alert.messageText = "The emulator failed to launch."
        alert.informativeText = "An unexpected exception interrupted the startup procedure:\n\n\(reason)"
        alert.addButton(withTitle: "Exit")
        
        if alert.runSheet(for: windowForSheet!) == .alertFirstButtonReturn {
            NSApp.terminate(self)
        }
    }
    
    func showIsUnsavedAlert(msg: String, icon: String) -> NSApplication.ModalResponse {

        let alert = NSAlert()
        alert.alertStyle = .warning
        alert.icon = NSImage(named: icon)
        alert.messageText = msg
        alert.informativeText = "Your changes will be lost if you proceed."
        alert.addButton(withTitle: "Proceed")
        alert.addButton(withTitle: "Cancel")

        return alert.runSheet(for: windowForSheet!)
    }

    func proceedWithUnsavedFloppyDisks(drives: [DriveProxy]) -> Bool {

        func name(drive: DriveProxy) -> String {

            switch drive.info.id {

            case DRIVE8: return "Drive 8"
            case DRIVE9: return "Drive 9"

            default:
                fatalError()
            }
        }

        let modified = drives.filter { $0.info.hasModifiedDisk }

        if modified.isEmpty || parent.pref.ejectWithoutAsking {
            return true
        }

        let names = modified.map({ name(drive: $0) }).joined(separator: " and ")
        let text = modified.count == 1 ?
        "\(names) contains an unsaved disk." :
        "\(names) contain unsaved disks."

        return showIsUnsavedAlert(msg: text, icon: "adf") == .alertFirstButtonReturn
    }

    func proceedWithUnsavedFloppyDisk(drive: DriveProxy) -> Bool {

        return proceedWithUnsavedFloppyDisks(drives: [drive])
    }

    func proceedWithUnsavedFloppyDisks() -> Bool {

        if emu == nil { return true }

        let drives = [emu!.drive8!, emu!.drive9!]
        return proceedWithUnsavedFloppyDisks(drives: drives)
    }

    func askToPowerOff() -> Bool {

        if emu?.poweredOn == true {

            let alert = NSAlert()

            alert.alertStyle = .informational
            alert.icon = NSImage(named: "powerSwitch")
            alert.messageText = "The emulator must be powered off to perform this operation."
            alert.informativeText = "Your changes will be lost if you proceed."
            alert.addButton(withTitle: "Proceed")
            alert.addButton(withTitle: "Cancel")

            if alert.runSheet(for: windowForSheet!) == .alertFirstButtonReturn {
                emu!.powerOff()
            } else {
                return false
            }
        }
        return true
    }
}

extension MyController {

    func showMultipleFilesAlert(msg1: String, msg2: String) {
        mydocument.showMultipleFilesAlert(msg1: msg1, msg2: msg2)
    }

    func proceedWithUnsavedFloppyDisk(drive: DriveProxy) -> Bool {
        return mydocument.proceedWithUnsavedFloppyDisk(drive: drive)
    }

    func proceedWithUnsavedFloppyDisks() -> Bool {
        return mydocument.proceedWithUnsavedFloppyDisks()
    }

    func askToPowerOff() -> Bool {
        return mydocument.askToPowerOff()
    }
}

extension MediaManager {

    func showMultipleFilesAlert(msg1: String, msg2: String) {
        document.showMultipleFilesAlert(msg1: msg1, msg2: msg2)
    }

    func proceedWithUnsavedFloppyDisk(drive: DriveProxy) -> Bool {
        document.proceedWithUnsavedFloppyDisk(drive: drive)
    }

    func proceedWithUnsavedFloppyDisks() -> Bool {
        document.proceedWithUnsavedFloppyDisks()
    }
}
