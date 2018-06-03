//
//  MemoryPanel.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 24.05.18.
//

import Foundation

extension MyController {
    
    @IBAction func setMemSource(_ sender: Any!) {
        
        let sender = sender as! NSPopUpButton
        memTableView.setMemView(sender.selectedTag())
    }
    
    @IBAction func setHighlighting(_ sender: Any!) {
        
        let sender = sender as! NSPopUpButton
        memTableView.setHighlighting(sender.selectedTag())
    }
}
