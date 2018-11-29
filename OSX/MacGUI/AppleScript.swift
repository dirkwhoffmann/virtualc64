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

        currentProxy.powerUp()
        return true
    }
}

class VC64ForceQuitCommand: NSScriptCommand {
    
    override func performDefaultImplementation() -> Any? {
        
        // Initiate forced shut down ...
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
            currentDocument.updateChangeCount(.changeCleared)
            NSApp.terminate(self)
        }
    
        return true
    }
}

class VC64ConfigureCommand: NSScriptCommand {
    
    override func performDefaultImplementation() -> Any? {
        
        track()
        guard let arguments = evaluatedArguments else {
            return false
        }

        track("\(arguments)")

        // Hardware model
        if let argument = arguments["VC64HwModel"] as? String {
            switch argument {
            case "C64_PAL": currentProxy.setModel(Int(C64_PAL.rawValue))
            case "C64_II_PAL": currentProxy.setModel(Int(C64_II_PAL.rawValue))
            case "C64_OLD_PAL": currentProxy.setModel(Int(C64_II_PAL.rawValue))
            case "C64_NTSC": currentProxy.setModel(Int(C64_II_PAL.rawValue))
            case "C64_II_NTSC": currentProxy.setModel(Int(C64_II_PAL.rawValue))
            case "C64_OLD_NTSC": currentProxy.setModel(Int(C64_II_PAL.rawValue))
            default: return false
            }
        }
 
        track()
        // Auto warp
        if let argument = arguments["VC64AutoWarp"] as? String {
            track();
            switch argument {
            case "on": track(); currentProxy.setWarpLoad(true)
            case "off": track(); currentProxy.setWarpLoad(false)
            default: return false
            }
        }
        
        track()
        // Always warp
        if let argument = arguments["VC64AlwaysWarp"] as? String {
            track();
            switch argument {
            case "on": track(); currentProxy.setAlwaysWarp(true)
            case "off": track(); currentProxy.setAlwaysWarp(false)
            default: return false
            }
        }
   
        track()
        // Auto mount
        if let argument = arguments["VC64AutoMount"] as? String {
            track();
            switch argument {
            case "on": track(); currentController.autoMount = true
            case "off": track(); currentController.autoMount = false
            default: return false
            }
        }
        
        return true
    }
}

class VC64DragInCommand: NSScriptCommand {
    
    override func performDefaultImplementation() -> Any? {
        
        track()
        
        if let arguments = evaluatedArguments {
            if let argument = arguments["VC64Path"] as? String {
                let url = URL(fileURLWithPath: argument)
                do {
                    track("\(url)")
                    try currentDocument.createAttachment(from: url)
                    track()
                    return currentDocument.processAttachmentAfterDragAndDrop()
                } catch {
                    track("Remote control: Emulated drag operation failed.")
                }
            }
        }
        return false
    }
}

class VC64TypeTextCommand: NSScriptCommand {
    
    override func performDefaultImplementation() -> Any? {
        
        if let arguments = evaluatedArguments {
            track("\(arguments)")
            if let text = arguments[""] as? String {
                currentController.keyboardcontroller.type(text)
                return true
            }
        }
        return false
    }
}

class VC64TakeScreenshotCommand: NSScriptCommand {
    
    override func performDefaultImplementation() -> Any? {
        
        if let arguments = evaluatedArguments {
            
            var url: URL?
            var image: NSImage?
            var data: Data?
            
            // Compute URL
            if let path = arguments["VC64Path"] as? String {
                url = URL(fileURLWithPath: path)
            }
            
            // Take screenshot
            image = currentController.metalScreen.screenshot(afterUpscaling: false)
            if let format = arguments["VC64ImageFormat"] as? String {
                switch format {
                case "tiff":
                    data = image?.tiffRepresentation
                case "jpg":
                    data = image?.jpgRepresentation
                case "png":
                    data = image?.pngRepresentation
                default:
                    break
                }
            } else {
                data = image?.pngRepresentation
            }
            
            // Write screenshot to URL
            if url != nil && data != nil {
                do {
                    try data!.write(to: url!, options: .atomic)
                    return true
                } catch {
                    track("Remote control: Failed to save screenshot")
                }
            }
        }
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
