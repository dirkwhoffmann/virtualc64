// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class TapeMountController: UserDialogController {
    
    var tape: TAPFileProxy!
    
    // Outlets
    @IBOutlet weak var autoLoad: NSButton!
    @IBOutlet weak var autoPress: NSButton!
    
    override func showSheet(completionHandler handler:(() -> Void)? = nil) {
        
        if let attachment = myDocument?.attachment as? TAPFileProxy {
            
            tape = attachment
            super.showSheet(completionHandler: handler)
        }
    }
    
    //
    // Action methods
    //

    @IBAction func autoLoadAction(_ sender: NSButton!) {
        
        autoPress.isEnabled = (sender.integerValue == 1)
    }
    
    @IBAction override func okAction(_ sender: Any!) {
        
        // Insert tape
        proxy?.datasette.insertTape(tape)
        myController?.metalScreen.rotateBack()
        
        // Process options
        if autoLoad.integerValue == 1 {
            
            let kbc = myController?.keyboard
            
            if autoPress.integerValue == 1 {
                kbc?.type(string: "LOAD\n", completion: proxy?.datasette.pressPlay)
            } else {
                kbc?.type(string: "LOAD\n", completion: nil)
            }
        }
        
        hideSheet()
    }
}
