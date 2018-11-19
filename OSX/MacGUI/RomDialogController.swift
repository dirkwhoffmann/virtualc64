//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

class RomDialogController : UserDialogController {
    
    let romImage = NSImage.init(named: NSImage.Name.init(rawValue: "rom"))
    let romImageLight = NSImage.init(named: NSImage.Name.init(rawValue: "rom_light"))
    let romImageMedium = NSImage.init(named: NSImage.Name.init(rawValue: "rom_medium"))

    @IBOutlet weak var headerImage: NSImageView!
    @IBOutlet weak var headerText: NSTextField!
    @IBOutlet weak var headerSubText: NSTextField!

    @IBOutlet weak var basicRom: NSImageView!
    @IBOutlet weak var basicRomText: NSTextField!
    @IBOutlet weak var basicRomButton: NSButton!

    @IBOutlet weak var kernalRom: NSImageView!
    @IBOutlet weak var kernalRomText: NSTextField!
    @IBOutlet weak var kernelRomButton: NSButton!
    
    @IBOutlet weak var characterRom: NSImageView!
    @IBOutlet weak var characterRomText: NSTextField!
    @IBOutlet weak var characterRomButton: NSButton!

    @IBOutlet weak var vc1541Rom: NSImageView!
    @IBOutlet weak var vc1541RomText: NSTextField!
    @IBOutlet weak var vc1541RomButton: NSButton!

    @IBOutlet weak var okButton: NSButton!
    
    override func awakeFromNib()
    {
        refresh()
    }

    func refresh()
    {
        let basic = c64.isBasicRomLoaded()
        let kernal = c64.isKernalRomLoaded()
        let character = c64.isCharRomLoaded()
        let vc1541 = c64.isVC1541RomLoaded()

        let readyToRun = c64.isRunnable()

        headerImage.isHidden = readyToRun
        headerText.isHidden = readyToRun
        headerSubText.isHidden = readyToRun
        
        basicRom.image = basic ? romImage : romImageLight
        basicRomText.isHidden = basic
        basicRomButton.isHidden = !basic
        
        kernalRom.image = kernal ? romImage : romImageLight
        kernalRomText.isHidden = kernal
        kernelRomButton.isHidden = !kernal

        characterRom.image = character ? romImage : romImageLight
        characterRomText.isHidden = character
        characterRomButton.isHidden = !character

        vc1541Rom.image = vc1541 ? romImage : romImageLight
        vc1541RomText.isHidden = vc1541
        vc1541RomButton.isHidden = !vc1541
        
        okButton.stringValue = readyToRun ? "OK" : "Quit"
    }

    
    //
    // Action methods
    //
    
    @IBAction func deleteBasicRom(_ sender: Any!)
    {
        track()
        parent.c64.mem.deleteBasicRom()
        refresh()
    }
    
    @IBAction func deleteCharacterRom(_ sender: Any!)
    {
        track()
        parent.c64.mem.deleteCharacterRom()
        refresh()
    }
    
    @IBAction func deleteKernalRom(_ sender: Any!)
    {
        track()
        parent.c64.mem.deleteKernalRom()
        refresh()
    }
    
    @IBAction func deleteVC1541Rom(_ sender: Any!)
    {
        track()
        parent.c64.drive1.deleteRom()
        parent.c64.drive2.deleteRom()
        refresh()
    }
    
    @IBAction func helpAction(_ sender: Any!)
    {
        if let url = URL.init(string: "http://www.dirkwhoffmann.de/virtualc64/ROMs.html") {
            NSWorkspace.shared.open(url)
        }
    }
    
    @IBAction func okAction(_ sender: Any!)
    {
        track()
        hideSheet()
        
        if (!c64.isRunnable()) {
           NSApp.terminate(self)
        }
    }
    
}

