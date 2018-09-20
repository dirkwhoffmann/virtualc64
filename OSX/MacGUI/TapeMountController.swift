//
//  TapeMountController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 08.02.18.
//

import Foundation

class TapeMountController : UserDialogController {
    
    var tape: TAPProxy!
    
    // Outlets
    @IBOutlet weak var autoLoad: NSButton!
    @IBOutlet weak var autoPress: NSButton!
    
    override func showSheet(withParent controller: MyController,
                   completionHandler:(() -> Void)? = nil) {
        
        track()
        tape = controller.mydocument.attachment as? TAPProxy
        super.showSheet(withParent: controller, completionHandler: completionHandler)
    }
    
    //
    // Action methods
    //

    @IBAction func autoLoadAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        autoPress.isEnabled = (sender.integerValue == 1)
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        // Insert tape
        c64.insertTape(tape)
        parent.metalScreen.rotateBack()
        
        // Process options
        if autoLoad.integerValue == 1 {
            let kb = parent.keyboardcontroller!
            if autoPress.integerValue == 1 {
                kb.typeOnKeyboard(string: "LOAD\n", completion: c64.datasette.pressPlay)
            } else {
                kb.typeOnKeyboard(string: "LOAD\n", completion: nil)
            }
        }
        
        hideSheet()
    }
}

