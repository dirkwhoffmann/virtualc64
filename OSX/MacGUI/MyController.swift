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
            let nibName = NSNib.Name(rawValue: "CartridgeMountDialog")
            controller = CartridgeMountController.init(windowNibName: nibName)
            break

        case TAP_CONTAINER:
            // TODO
            break
            
        default:
            // There is no mount dialog availabe for the attachments type
            return
        }
        
        // Show dialog as a sheet
        controller.setParentController(self)
        if let sheetWindow = controller.window {
            window!.beginSheet(sheetWindow, completionHandler: { responseCode in
                
                // We need to do somethig with controller. Otherwise, ARC deletes it too early.
                controller.cleanup()
                
                if responseCode == NSApplication.ModalResponse.OK {
                    self.rotateBack()
                }
            })
        }
    }

    // --------------------------------------------------------------------------------
    // Action methods (Drive)
    // --------------------------------------------------------------------------------
    
    @IBAction func driveAction(_ sender: Any!) {
        
        NSLog("\(#function)")
        if c64.iec.isDriveConnected() {
            c64.iec.disconnectDrive()
        } else {
            c64.iec.connectDrive()
        }
    }
    
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
        alert.addButton(withTitle: "Export...")
        alert.addButton(withTitle: "Eject")
        alert.addButton(withTitle: "Cancel")
        alert.messageText = "Do you want to export the inserted disk to a D64 archive?"
        alert.informativeText = "Otherwise, your changes will be lost."
        alert.alertStyle = .critical
        
        while true {

            let result = alert.runModal()

            // Export... button
            if result == .alertFirstButtonReturn {
                if exportDiskDialogWorker(type: D64_CONTAINER) {
                    NSLog("Disk saved. Ejecting...")
                    drive.ejectDisk()
                    break
                } else {
                    NSLog("Export dialog cancelled. Asking again...")
                }
            }
    
            // Eject button
            if result == .alertSecondButtonReturn {
                NSLog("Ejecting disk...")
                drive.ejectDisk()
                break
            }
    
            // Cancel button
            if result == .alertThirdButtonReturn {
                NSLog("Canceling disk data loss warning dialog...")
                break
            }
        }
    }

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
