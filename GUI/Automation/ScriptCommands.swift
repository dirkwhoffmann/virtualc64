// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

//
// Commands
//

func resetScriptCmd(arguments: [AnyHashable: Any]?) -> Bool {
    
    /*
    proxy?.powerUp()
    */
    return true
}

func configureScriptCmd(arguments: [AnyHashable: Any]?) -> Bool {
    
    // Hardware model
    if let argument = arguments?["VC64HwModel"] as? String {
        switch argument {
        case "C64_PAL": proxy?.setModel(Int(C64_PAL.rawValue))
        case "C64_II_PAL": proxy?.setModel(Int(C64_II_PAL.rawValue))
        case "C64_OLD_PAL": proxy?.setModel(Int(C64_OLD_PAL.rawValue))
        case "C64_NTSC": proxy?.setModel(Int(C64_NTSC.rawValue))
        case "C64_II_NTSC": proxy?.setModel(Int(C64_II_NTSC.rawValue))
        case "C64_OLD_NTSC": proxy?.setModel(Int(C64_OLD_NTSC.rawValue))
        default: return false
        }
    }
    
    // Auto warp
    if let argument = arguments?["VC64AutoWarp"] as? String {

        switch argument {
        case "on": track(); proxy?.setWarpLoad(true)
        case "off": track(); proxy?.setWarpLoad(false)
        default: return false
        }
    }
    
    // Always warp
    if let argument = arguments?["VC64AlwaysWarp"] as? String {

        switch argument {
        case "on": track(); proxy?.setAlwaysWarp(true)
        case "off": track(); proxy?.setAlwaysWarp(false)
        default: return false
        }
    }
        
    return true
}

func mountScriptCmd(arguments: [AnyHashable: Any]?) -> Bool {
    
    if let argument = arguments?["VC64Path"] as? String {
        let url = URL(fileURLWithPath: argument)
        do {
            try myDocument?.createAttachment(from: url)
            myDocument?.mountAttachment(action: .flashFirstFile, text: "RUN\n")
        } catch {
            track("Remote control: Unable to mount \(url).")
        }
    }
    return false
}

func typeTextCmd(arguments: [AnyHashable: Any]?) -> Bool {
    
    if let text = arguments?[""] as? String {
        myController?.keyboard.type(text)
        return true
    }
    return false
}

func takeScreenshotCmd(arguments: [AnyHashable: Any]?) -> Bool {
    
    guard let path = arguments?["VC64Path"] as? String else {
        return false
    }

    do {
        try myController?.saveScreenshot(url: URL(fileURLWithPath: path))
        return true
    } catch {
        track("Remote control: Failed to save screenshot")
        return false
    }
}

func quitScriptCmd(arguments: [AnyHashable: Any]?) {
    
    myDocument?.updateChangeCount(.changeCleared)
    NSApplication.shared.terminate(nil)
}
