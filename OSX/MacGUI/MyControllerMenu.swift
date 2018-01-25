//
//  MyControllerMenu.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 25.01.18.
//

import Foundation

extension MyController {
    
    @objc func validateMenuItemSwift(_ menuItem: NSMenuItem) -> Bool {
        
        // NSLog("Validating menu item \(menuItem.title)")

        // if menuItem.action == #selector(MyController.finalCartAction(_:))
        
        // Cartridge menu
        if menuItem.title == "Final Cartridge III" {
            return c64.expansionport.cartridgeType() == CRT_FINAL_CARTRIDGE_III
        }
        
        return true
    }
    
    @IBAction func cartridgeMenuAction(_ sender: Any!) {
    }
    
    @IBAction func cartridgeButtonAction(_ sender: Any!) {
        c64.expansionport.pressButton()
    }
}
