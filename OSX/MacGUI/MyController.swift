//
//  MyController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 29.01.18.
//

import Foundation

extension MyController {
    
    //! @brief  GamePadManager delegation method
    /*! @returns true, iff a joystick event has been triggered on port A or port B
     */
    @discardableResult
    func joystickEvent(slot: Int, event: JoystickEvent) -> Bool {
        
        if (slot == gamepadSlotA) {
            c64.joystickA.trigger(event)
            return true
        }

        if (slot == gamepadSlotB) {
            c64.joystickB.trigger(event)
            return true
        }
        
        return false
    }    

    // --------------------------------------------------------------------------------
    //                                 Modal dialogs
    // --------------------------------------------------------------------------------

    @objc func showMountDialog() {
        
        track()

        // Does this document have an attachment?
        let document = self.document as! MyDocument
        guard let attachment = document.attachment else {
            return
        }
        
        // Which mount dialog should we use?
        var controller: MountController!
        switch attachment.type() {
        case T64_CONTAINER,
             PRG_CONTAINER,
             P00_CONTAINER,
             D64_CONTAINER:
            let nibName = NSNib.Name(rawValue: "ArchiveMountDialog")
            controller = ArchiveMountController.init(windowNibName: nibName)
            break
            
        case G64_CONTAINER,
             NIB_CONTAINER:
            let nibName = NSNib.Name(rawValue: "DiskMountDialog")
            controller = DiskMountController.init(windowNibName: nibName)
            break

        case CRT_CONTAINER:
            track()
            
            // Check if we support this cartridge
            let crt = attachment as! CRTProxy
            if !crt.isSupported() {
                showUnsupportedCartridgeAlert(crt)
                return
            }
            
            let nibName = NSNib.Name(rawValue: "CartridgeMountDialog")
            controller = CartridgeMountController.init(windowNibName: nibName)
            break

        case TAP_CONTAINER:
            let nibName = NSNib.Name(rawValue: "TapeMountDialog")
            controller = TapeMountController.init(windowNibName: nibName)
            break
            
        default:
            // There is no mount dialog availabe for the attachments type
            return
        }
        
        controller.showSheet(self)
    }

    // --------------------------------------------------------------------------------
    // Action methods (Drive)
    // --------------------------------------------------------------------------------
    
 
    /*
    @IBAction func driveEjectAction(_ sender: Any!) {
    
        NSLog("\(#function)")
        
        let drive = c64.vc1541!
        
        // Eject disk only if disk is not modified
        if !drive.diskModified() {
            drive.ejectDisk()
            return
        }
        
        // If disk is modified, ask the user how to proceed
        let alert = NSAlert()
        alert.icon = NSImage.init(named: NSImage.Name(rawValue: "diskette"))
        alert.addButton(withTitle: "Eject")
        alert.addButton(withTitle: "Cancel")
        alert.messageText = "Do you want to export the inserted disk to a D64 archive?"
        alert.informativeText = "Otherwise, your changes will be lost."
        alert.alertStyle = .critical
        
        let result = alert.runModal()
            
        // Eject button
        if result == .alertFirstButtonReturn {
            NSLog("Ejecting disk...")
            drive.ejectDisk()
        }
    
        // Cancel button
        if result == .alertSecondButtonReturn {
            NSLog("Canceling disk data loss warning dialog...")
        }
    }
*/
    // --------------------------------------------------------------------------------
    // Action methods (Cartridge)
    // --------------------------------------------------------------------------------

    @IBAction func cartridgeEjectAction(_ sender: Any!) {
  
        NSLog("\(#function)")
        // let document = self.document as! MyDocument
        // document.attachment = nil
        c64.detachCartridgeAndReset()
    }
    
    
}
