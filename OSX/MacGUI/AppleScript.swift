//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

// The document of the currently active emulator instance
private var currentDocument: MyDocument {
    get {
        return NSApplication.shared.orderedDocuments.first as! MyDocument
    }
}
        
// The controller of the currently active emulator instance
private var currentController: MyController {
    get {
        return currentDocument.windowControllers.first as! MyController
    }
}

// The emulator proxy of the currently active emulator instance
private var currentProxy: C64Proxy {
    get {
        return currentDocument.c64
    }
}


//
// Remote control commands
//

class VC64ResetCommand: NSScriptCommand {
    
    override func performDefaultImplementation() -> Any? {
        
        track()

        currentProxy.powerUp()
        return true
    }
}

class VC64SetModelCommand: NSScriptCommand {
    
    override func performDefaultImplementation() -> Any? {
        
        track()

        if let arguments = evaluatedArguments {
            if let model = arguments["VC64Model"] as? String {
                switch model {
                    
                case "C64_PAL":
                    currentProxy.setModel(Int(C64_PAL.rawValue))
                    
                case "C64_II_PAL":
                    currentProxy.setModel(Int(C64_II_PAL.rawValue))
                    
                case "C64_OLD_PAL":
                    currentProxy.setModel(Int(C64_II_PAL.rawValue))
                    
                case "C64_NTSC":
                    currentProxy.setModel(Int(C64_II_PAL.rawValue))
                    
                case "C64_II_NTSC":
                    currentProxy.setModel(Int(C64_II_PAL.rawValue))
                    
                case "C64_OLD_NTSC":
                    currentProxy.setModel(Int(C64_II_PAL.rawValue))
                    
                default:
                    return false
                }
                
                track("Remote control: Setting model to \(model)")
                return true
            }
        }
        return false
    }
}

class VC64TypeTextCommand: NSScriptCommand {
    
    override func performDefaultImplementation() -> Any? {
        
        track()

        if let arguments = evaluatedArguments {
            if let text = arguments["VC64Text"] as? String {
                currentController.keyboardcontroller.type(text)
                return true
            }
        }
        return false
    }
}

class VC64TakeScreenshotCommand: NSScriptCommand {
    
    override func performDefaultImplementation() -> Any? {
        
        track()
        
        if let arguments = evaluatedArguments {
            track()
            if let path = arguments["VCPath"] as? String {
                track()
                let url = URL(fileURLWithPath: path)
                let image = currentController.metalScreen.screenshot()
                let data = image?.tiffRepresentation
                do {
                    try data?.write(to: url, options: .atomic)
                    return true
                } catch {
                    track("Remote control: Cannot quicksave screenshot")
                }
            }
        }
        track()
        return false
    }
}



// DELETE:

class NotiphyWithParams: NSScriptCommand, NSUserNotificationCenterDelegate {
    
    var notiphy = NSUserNotification()
    var notiphyCtr = NSUserNotificationCenter.default
    
    override func performDefaultImplementation() -> Any? {
        
        track()
        
        let parms = self.evaluatedArguments
        var aTitle = ""
        var aSubTitle = ""
        if let args = parms {
            if let title = args["NotiphyTitle"] as? String {
                aTitle = title
            }
            if let subtitle = args["NotiphySubtitle"] as? String {
                aSubTitle = subtitle
            }
        }
        
        notiphy.title = aTitle
        notiphy.subtitle = aSubTitle
        notiphyCtr.deliver(notiphy)
        
        if aTitle == "" && aSubTitle == "" {
            return false
        }
        
        let document = NSApplication.shared.orderedDocuments.first as! MyDocument
        let controller = document.windowControllers.first as! MyController
        controller.keyboardcontroller.typeOnKeyboard(string: aTitle, completion: nil)
        // document.c64.powerUp()
        
        
        return true
    }
}
