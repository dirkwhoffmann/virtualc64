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
            showCartridgeAlert(crtContainer)
            return false;
        }
        
        // Attach cartride 
        c64.attachCartridgeAndReset(crtContainer)
        return true
    }
    
    @objc func showCartridgeAlert(_ crtcontainer: CRTContainerProxy) {
        
        let name = crtcontainer.typeName() as String
        let alert = NSAlert()
        alert.icon = NSImage.init(named: NSImage.Name(rawValue: "rom.png"))
        alert.messageText = "Unsupported cartridge type: \(name)"
        alert.informativeText = "The provided cartridge contains special hardware which is not supported by the emulator yet."
        alert.addButton(withTitle: "OK")
        alert.alertStyle = NSAlert.Style.warning
        // alert.runModal()
        alert.beginSheetModal(for: window!, completionHandler: nil)
    }

}
