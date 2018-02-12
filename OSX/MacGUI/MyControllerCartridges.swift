//
//  MyControllerCartridges.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 20.01.18.
//

import Foundation

extension MyController {

    @discardableResult @objc func mountCartridge() -> Bool {
    
        // Does this document have an attachment?
        let document = self.document as! MyDocument
        guard let attachment = document.attachment else {
            return false;
        }
        
        // Is the attachment a cartridge?
        if attachment.type() != CRT_CONTAINER {
            return false;
        }
        
        // Attach cartride 
        c64.attachCartridgeAndReset(attachment as! CRTProxy)
        return true
    }
    
}
