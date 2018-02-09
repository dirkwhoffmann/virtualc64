//
//  TapeMountController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 08.02.18.
//

import Foundation

class TapeMountController : MountController {
    
    var tape: TAPProxy!
    
    // Outlets
    @IBOutlet weak var autoLoad: NSButton!
    @IBOutlet weak var autoPress: NSButton!
    
    override func setParentController(_ controller: MyController) {
        
        track()
        super.setParentController(controller)
        tape = attachment as! TAPProxy
    }
    
    //
    // Action methods
    //

    @IBAction func autoLoadAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        autoPress.isEnabled = (sender.integerValue == 1)
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        c64.insertTape(tape)

        if autoLoad.integerValue == 1 {
            if autoPress.integerValue == 1 {
                controller.simulateUserTypingTextAndPressPlay("LOAD\n")
            } else {
                controller.simulateUserTypingText("LOAD\n")
            }
        }

        window?.orderOut(self)
        parentWindow.endSheet(window!, returnCode: .OK)
    }
    
    @IBAction func cancelAction(_ sender: Any!) {
        
        window?.orderOut(self)
        parentWindow.endSheet(window!, returnCode: .cancel)
    }
}

