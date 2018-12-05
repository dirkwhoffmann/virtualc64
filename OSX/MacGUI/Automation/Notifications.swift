//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

extension AppDelegate {
    
    @objc func vc64ResetCommand(_ notification: Notification) {
        
        if !resetScriptCmd(arguments: notification.userInfo) {
            track("Failed to execute 'reset' command.")
        }
    }
 
    @objc func vc64ConfigureCommand(_ notification: Notification) {
        
        if !configureScriptCmd(arguments: notification.userInfo) {
            track("Failed to execute 'configure' command.")
        }
    }
    
    @objc func vc64DragInCommand(_ notification: Notification) {
        
        if !dragInScriptCmd(arguments: notification.userInfo) {
            track("Failed to execute 'dragIn' command.")
        }
    }
    
    @objc func vc64TypeTextCommand(_ notification: Notification) {
        
        if !typeTextCmd(arguments: notification.userInfo) {
            track("Failed to execute 'typeText' command.")
        }
    }
    
    @objc func vc64TakeScreenshotCommand(_ notification: Notification) {
        
        if !takeScreenshotCmd(arguments: notification.userInfo) {
            track("Failed to execute 'takeScreenshot' command.")
        }
    }
    
    @objc func vc64QuitCommand(_ notification: Notification) {
        
        quitScriptCmd(arguments: notification.userInfo)
    }
}
