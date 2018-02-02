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

    // Bring up the new mount view
 
    
    @objc func showNewMountDialog(archive: ArchiveProxy) {
        
        let nibName = NSNib.Name(rawValue: "MountController")
        let controller = MountController.init(windowNibName: nibName)
        controller.c64 = c64
        controller.archive = archive
        controller.parentWindow = window
        
        if let sheetWindow = controller.window {
            
            // showMountDialog()
            /*
             let application = NSApplication.shared
             application.runModal(for: sheetWindow)
             window?.close()
            */
            
            window!.beginSheet(sheetWindow, completionHandler: { responseCode in
                controller.terminate()
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

    
}
