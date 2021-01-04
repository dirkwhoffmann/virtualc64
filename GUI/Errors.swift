// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class MyError: Error {
    
    var errorCode: ErrorCode
    
    init(_ errorCode: ErrorCode) { self.errorCode = errorCode }

    var description: String {
        
        switch errorCode {
        
        case .OK:
            fatalError()
        case .UNKNOWN:
            return "An unknown error has occurred."
        case .OUT_OF_MEMORY:
            return "Out of memory."
        case .FILE_NOT_FOUND:
            return "File not found."
        case .INVALID_TYPE:
            return "The content of the file does not match its format."
        case .CANT_READ:
            return "The file could not be opened for reading."
        case .CANT_WRITE:
            return "The file could not be opened for writing."
        case .UNSUPPORTED_SNAPSHOT:
            return "This snaphshot was created with a different version of VirtualC64."
        case .UNSUPPORTED_CRT:
            return "The format of this cartridge is not supported by the emulator yet."
        default:
            return ""
            /*
                ERROR_UNSUPPORTED,
                ERROR_WRONG_CAPACITY,
                ERROR_HAS_CYCLES,
                ERROR_CORRUPTED,
                ERROR_IMPORT_ERROR,

                // Block errros
                ERROR_EXPECTED,
                ERROR_EXPECTED_MIN,
                ERROR_EXPECTED_MAX,

                // Export errors
                ERROR_DIRECTORY_NOT_EMPTY,
                ERROR_CANNOT_CREATE_DIR,
                ERROR_CANNOT_CREATE_FILE
            */
        }
    }
    
    static func alert(_ msg1: String, _ msg2: String, style: NSAlert.Style,
                      async: Bool = false, icon: String?) {
        
        if async == true {
            DispatchQueue.main.async {
                alert(msg1, msg2, style: style, async: false, icon: icon)
            }
        } else {
            let alert = NSAlert()
            alert.alertStyle = style
            if icon != nil { alert.icon = NSImage.init(named: icon!) }
            alert.messageText = msg1
            alert.informativeText = msg2
            alert.addButton(withTitle: "OK")
            alert.runModal()
        }
    }
    
    static func warning(_ msg1: String, _ msg2: String,
                        async: Bool = false, icon: String? = nil) {
        alert(msg1, msg2, style: .warning, async: async, icon: icon)
    }
    
    static func critical(_ msg1: String, _ msg2: String,
                         async: Bool = false, icon: String? = nil) {
        alert(msg1, msg2, style: .critical, async: async, icon: icon)
    }

    func warning(_ msg: String, async: Bool = false, icon: String? = nil) {
        MyError.warning(msg, description, async: async, icon: icon)
    }

    func critical(_ msg: String, async: Bool = false, icon: String? = nil) {
        MyError.warning(msg, description, async: async, icon: icon)
    }
    
    func cantOpen(url: URL, async: Bool = false) {
        warning("\"\(url.lastPathComponent)\" can't be opened.", async: async)
    }
}
