//
//  RomDialogController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 12.02.18.
//

import Foundation

class RomDialogController : UserDialogController {
    
    let romImage = NSImage.init(named: NSImage.Name.init(rawValue: "rom"))
    let romImageLight = NSImage.init(named: NSImage.Name.init(rawValue: "rom_light"))
    let romImageMedium = NSImage.init(named: NSImage.Name.init(rawValue: "rom_medium"))

    // @IBOutlet weak var learnMore: NSTextField!
    @IBOutlet weak var kernalRom: NSImageView!
    @IBOutlet weak var kernalRomText: NSTextField!
    @IBOutlet weak var basicRom: NSImageView!
    @IBOutlet weak var basicRomText: NSTextField!
    @IBOutlet weak var characterRom: NSImageView!
    @IBOutlet weak var characterRomText: NSTextField!
    @IBOutlet weak var vc1541Rom: NSImageView!
    @IBOutlet weak var vc1541RomText: NSTextField!
    // @IBOutlet weak var dragBox: NSBox!
    // @IBOutlet weak var quitButton: NSButton!

    override func awakeFromNib()
    {
        refresh()
    }

    func refresh()
    {
        kernalRom.image = c64.isKernalRomLoaded() ? romImage : romImageLight
        kernalRomText.isHidden = c64.isKernalRomLoaded()
        basicRom.image = c64.isBasicRomLoaded() ? romImage : romImageLight
        basicRomText.isHidden = c64.isBasicRomLoaded()
        characterRom.image = c64.isCharRomLoaded() ? romImage : romImageLight
        characterRomText.isHidden = c64.isCharRomLoaded()
        vc1541Rom.image = c64.isVC1541RomLoaded() ? romImage : romImageLight
        vc1541RomText.isHidden = c64.isVC1541RomLoaded()
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
        track()
        hideSheet()
        NSApp.terminate(self)
    }
    
}

