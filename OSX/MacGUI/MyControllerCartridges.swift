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
        
        // Is it a supported cartridge?
        let crt = attachment as! CRTProxy
         if !crt.isSupported() {
            showUnsupportedCartridgeAlert(crt)
            return false;
        }
        
        // Attach cartride 
        c64.attachCartridgeAndReset(crt)
        return true
    }
    
    /*
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
     */
}
