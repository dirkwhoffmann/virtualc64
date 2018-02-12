//
//  RomDialogController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 12.02.18.
//

import Foundation

class RomDialogController : UserDialogController {
    
    let romImage = NSImage.init(named: NSImage.Name.init(rawValue: "romchip"))
    let noRomImage = NSImage.init(named: NSImage.Name.init(rawValue: "romchipMissing"))
    
    // @IBOutlet weak var learnMore: NSTextField!
    @IBOutlet weak var kernelRom: NSImageView!
    @IBOutlet weak var basicRom: NSImageView!
    @IBOutlet weak var characterRom: NSImageView!
    @IBOutlet weak var vc1541Rom: NSImageView!
    @IBOutlet weak var dragBox: NSBox!
    @IBOutlet weak var cancelButton: NSButton!

    override func awakeFromNib()
    {
        refresh()
    }

    func refresh()
    {
        kernelRom.image = c64.isKernelRomLoaded() ? romImage : noRomImage
        basicRom.image = c64.isBasicRomLoaded() ? romImage : noRomImage
        characterRom.image = c64.isCharRomLoaded() ? romImage : noRomImage
        vc1541Rom.image = c64.isVC1541RomLoaded() ? romImage : noRomImage
    }

    //
    // Action methods
    //
    
    @IBAction func helpAction(_ sender: Any!)
    {
        if let url = URL.init(string: "http://www.dirkwhoffmann.de/virtualc64/ROMs.html") {
            NSWorkspace.shared.open(url)
        }
    }
    
    @IBAction func quitAction(_ sender: Any!)
    {
        hideSheet()
        NSApp.terminate(self)
    }
    
}

