//
//  MyDocument.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 31.01.18.
//

import Foundation

extension MyDocument {

    override open func makeWindowControllers() {
        
        NSLog("\(#function)")
        
        let nibName = NSNib.Name(rawValue: "MyDocument")
        let controller = MyController.init(windowNibName: nibName)
        self.addWindowController(controller)
    }
}

