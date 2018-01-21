//
//  MyControllerCartridges.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 20.01.18.
//

import Foundation

extension MyController {

    @discardableResult @objc func mountCartridge() -> Bool {
    
        // Get cartridge attached to this document (if any)
        let document = self.document as! MyDocument
        guard let crtContainer = document.attachedCartridge else {
            return false;
        }
        
        // Check for cartridge support
        if !crtContainer.isSupportedType() {
            showCartridgeAlert(crtContainer.type())
            return false;
        }
        
        // Attach cartride 
        c64.attachCartridge(crtContainer)
        c64.reset()
        return true
    }
    
    @objc func showCartridgeAlert(_ type: NSInteger) {
        
        let alert = NSAlert()
        alert.icon = NSImage.init(named: NSImage.Name(rawValue: "rom.png"))
        alert.messageText = "Unsupported cartridge (type \(type))"
        alert.informativeText = "The provided cartridge containts custom hardware which is not supported in this release."
        alert.addButton(withTitle: "OK")
        alert.alertStyle = NSAlert.Style.warning
        // alert.runModal()
        alert.beginSheetModal(for: window!, completionHandler: nil)
    }

}
