//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

class NotiphyWithParams: NSScriptCommand, NSUserNotificationCenterDelegate {
    
    var notiphy = NSUserNotification()
    var notiphyCtr = NSUserNotificationCenter.default
    
    override func performDefaultImplementation() -> Any? {
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

extension AppDelegate {
    
    func setModelCommand() {
        
        
        track()
    }
}
