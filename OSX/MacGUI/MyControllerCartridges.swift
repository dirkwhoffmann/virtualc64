//
//  MyControllerCartridges.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 20.01.18.
//

import Foundation

extension MyController {

    @IBAction func cartridgeEjectAction(_ sender: Any!) {
        
        c64.detachCartridgeAndReset()
    }
}
