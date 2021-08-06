// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class VC64Error: Error {
    
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
            return "To prevent accidental exports, the disk exporter " +
                "refuses to work on non-empty folders."
        case .ROM_BASIC_MISSING:
            return "The emulator cannot launch because no Basic Rom installed."
        case .ROM_CHAR_MISSING:
            return "The emulator cannot launch because no Kernal Rom installed."
        case .ROM_KERNAL_MISSING:
            return "The emulator cannot launch because no Kernal Rom installed."
        case .ROM_MEGA65_MISMATCH:
            return "The emulator cannot launch because Open Roms with " +
                "mismatched version strings are installed."
        case .SNP_TOO_OLD:
            return "The snapshot was created with an older version of " +
                "VirtualC64 and is incompatible with this release."
        case .SNP_TOO_NEW:
            return "The snapshot was created with a newer version of " +
                "VirtualC64 and is incompatible with this release."
        case .CRT_UNSUPPORTED:
            return "The format of this cartridge is not supported by the emulator yet."
        case .FS_UNSUPPORTED:
            return "Unsupported file system type."
        case .FS_WRONG_CAPACITY:
            return "Wrong file system capacity."
        case .FS_CORRUPTED:
            return "Corrupted file system."
        case .FS_HAS_NO_FILES:
            return "The file system contains no files."
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
            if icon != nil { alert.icon = NSImage(named: icon!) }
            alert.messageText = msg1
            alert.informativeText = msg2
            alert.addButton(withTitle: "OK")
            alert.runModal()
        }
    }

    static func informational(_ msg1: String, _ msg2: String,
                              async: Bool = false, icon: String? = nil) {
        alert(msg1, msg2, style: .informational, async: async, icon: icon)
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
        VC64Error.warning(msg, description, async: async, icon: icon)
    }

    func critical(_ msg: String, async: Bool = false, icon: String? = nil) {
        VC64Error.warning(msg, description, async: async, icon: icon)
    }
    
    //
    // Customized alerts
    //
    
    func cantOpen(url: URL, async: Bool = false) {
        warning("\"\(url.lastPathComponent)\" can't be opened.", async: async)
    }
    
    static func unsupportedCrtAlert(type: Int, async: Bool = false) {
        
        let crtType = CartridgeType(rawValue: type)
        let name = crtType?.description ?? "Unknown"
        
        warning("This cartridge cannot be attached.",
                "The emulator does not support cartridges of type " +
                    "\(type) " + "(\(name)) yet.", async: async)
    }
}
