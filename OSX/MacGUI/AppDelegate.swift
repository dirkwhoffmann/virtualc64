//
//  AppDelegate.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 31.01.18.
//

import Cocoa

@NSApplicationMain
public class AppDelegate: NSObject, NSApplicationDelegate {
    
    
    
    public func applicationDidFinishLaunching(_ aNotification: Notification) {
        
        print("\(#function)")
        
        if #available(OSX 10.12.2, *) {
            NSApplication.shared.isAutomaticCustomizeTouchBarMenuItemEnabled = true
        }
    }
    
    public func applicationWillTerminate(_ aNotification: Notification) {

        print("\(#function)")
    }
    
    
}

