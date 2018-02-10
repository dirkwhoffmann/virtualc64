//
//  MyController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 29.01.18.
//

import Foundation

// --------------------------------------------------------------------------------
//                                NSWindowDelegate
// --------------------------------------------------------------------------------

extension MyController : NSWindowDelegate {
 
    @objc public func windowWillResize(_ sender: NSWindow, to frameSize: NSSize) -> NSSize {

        // Get some basic parameters
        let windowFrame = sender.frame
        let deltaX = frameSize.width - windowFrame.size.width
        let deltaY = frameSize.height - windowFrame.size.height
    
        // How big would the metal view become?
        let metalFrame = metalScreen.frame
        let metalX = metalFrame.size.width + deltaX
        let metalY = metalFrame.size.height + deltaY
    
        // We want to achieve an aspect ratio of 804:621
        let newMetalY  = metalX * (621.0 / 804.0)
        let correction = newMetalY - metalY
    
        return NSMakeSize(frameSize.width, frameSize.height + correction)
    }
    
    /// Adjusts the window size programatically
    /// The size is adjusted to get the metal view's aspect ration right
    
    @objc func adjustWindowSize() {
        
        track()
        if var frame = window?.frame {
    
            // Compute size correction
            let newsize = windowWillResize(window!, to: frame.size)
            let correction = newsize.height - frame.size.height
    
            // Adjust frame
            frame.origin.y -= correction;
            frame.size = newsize;
    
            window!.setFrame(frame, display: true)
        }
    }

    @objc public func windowWillClose(_ notification: Notification) {
        
        track()
        
        // Stop timer
        timer.invalidate()
        timer = nil
        
        // Stop metal view
        metalScreen.cleanup()
    }
}

extension MyController {

    // --------------------------------------------------------------------------------
    //                          Window life cycle methods
    // --------------------------------------------------------------------------------
    
    
    override open func awakeFromNib() {

        track()
    }
    
    // --------------------------------------------------------------------------------
    //                               Game pad events
    // --------------------------------------------------------------------------------

    
    /// GamePadManager delegation method
    /// - Returns: true, iff a joystick event has been triggered on port A or port B
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
        var controller: UserDialogController!
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
        
        controller.showSheet(withParent: self)
    }

    // --------------------------------------------------------------------------------
    // Action methods (Drive)
    // --------------------------------------------------------------------------------
    
 
    // --------------------------------------------------------------------------------
    // Action methods (Cartridge)
    // --------------------------------------------------------------------------------

    @IBAction func cartridgeEjectAction(_ sender: Any!) {
  
        NSLog("\(#function)")
        c64.detachCartridgeAndReset()
    }
    
    
}
