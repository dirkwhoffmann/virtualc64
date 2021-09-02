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
    var what: String
    
    init(_ exception: ExceptionWrapper) {

        self.errorCode = exception.errorCode
        self.what = exception.what
    }
    
    init(_ errorCode: ErrorCode, _ what: String = "") {
        
        self.errorCode = errorCode
        self.what = what
    }

    static func alert(_ msg1: String, _ msg2: String, style: NSAlert.Style,
                      async: Bool = false, icon: String?) {
        
        if async {
            
            DispatchQueue.main.async {
                alert(msg1, msg2, style: style, async: false, icon: icon)
            }
            
        } else {
            
            let alert = NSAlert()
            alert.alertStyle = style
            alert.icon = icon != nil ? NSImage(named: icon!) : nil
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
        VC64Error.warning(msg, what, async: async, icon: icon)
    }

    func critical(_ msg: String, async: Bool = false, icon: String? = nil) {
        VC64Error.warning(msg, what, async: async, icon: icon)
    }
    
    //
    // Customized alerts
    //
    
    func cantOpen(url: URL, async: Bool = false) {
        warning("\"\(url.lastPathComponent)\" can't be opened.", async: async)
    }
    
    /*
    static func unsupportedCrtAlert(type: Int, async: Bool = false) {
        
        let crtType = CartridgeType(rawValue: type)
        let name = crtType?.description ?? "Unknown"
        
        warning("This cartridge cannot be attached.",
                "The emulator does not support cartridges of type " +
                    "\(type) " + "(\(name)) yet.", async: async)
    }
    */
}
