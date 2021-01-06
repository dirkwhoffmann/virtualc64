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
        case .OUT_OF_MEMORY:
            return "Out of memory."
        case .FILE_NOT_FOUND:
            return "File not found."
        case .FILE_TYPE_MISMATCH:
            return "The content of the file does not match its format."
        case .FILE_CANT_READ:
            return "The file could not be opened for reading."
        case .FILE_CANT_WRITE:
            return "The file could not be opened for writing."
        case .FILE_CANT_CREATE:
            return "Unable to create file."
        case .DIR_CANT_CREATE:
            return "Unable to create directory."
        case .DIR_NOT_EMPTY:
            return "Directory is not empty."
        case .SNP_UNSUPPORTED:
            return "This snaphshot was created with a different version of VirtualC64."
        case .CRT_UNSUPPORTED:
            return "The format of this cartridge is not supported by the emulator yet."
        case .FS_UNSUPPORTED:
            return "Unsupported file system type."
        case .FS_WRONG_CAPACITY:
            return "Wrong file system capacity."
        case .FS_CORRUPTED:
            return "Corrupted file system."
        case .FS_HAS_CYCLES:
            return "The file system has cyclic references."
        case .FS_CANT_IMPORT:
            return "Failed to import the file system."
        case .FS_EXPECTED_VAL,
             .FS_EXPECTED_MIN,
             .FS_EXPECTED_MAX:
            fatalError()
        
        default:
            return ""
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
