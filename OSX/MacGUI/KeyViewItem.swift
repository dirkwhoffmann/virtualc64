//
//  KeyViewItem.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 19.02.18.
//

import Cocoa

class KeyViewItem: NSCollectionViewItem {

    override func viewDidLoad() {
        
        track()
        
        super.viewDidLoad()
        
        view.wantsLayer = true
        let img = NSImage(named: NSImage.Name(rawValue: "key.png"))
        imageView?.image = img
    }
    
}
