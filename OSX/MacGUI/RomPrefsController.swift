//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

let knownBasicRoms : [UInt64 : String] = [
    0x0000000000000000:
    "This 8 KB Rom contains Commodore's Basic interpreter. There is essentially only revision of this chip.",
    0x20765FEA67A8762D:
    "Commodore 64 Basic V2"
]

let knownCharacterRoms : [UInt64 : String] = [
    0x0000000000000000:
    "This 4 KB Rom contains the C64's character set. Depending on the region, Commodore has shipped slightly different versions of this chip.",
    0x3CA9D37AA3DE0969:
    "Character ROM from a Swedish C64 (C2D007)",
    0x6E3827A65FFF116F:
    "Character ROM from a Swedish C64",
    0x623CDF045B74B691:
    "Character ROM from a Spanish C64C",
    0x79A236A3B3645231:
    "Character rom from a Danish C64",
    0xACC576F7B332AC15:
    "The Commodore 64 character set"
]

let knownKernalRoms : [UInt64 : String] = [
    0x0000000000000000:
    "This 8 KB Rom contains the low-level operating system. There are three major revisions of this chip, the third being the most common.",
    0xFB166E49AF709AB8:
    "Commodore 64 Kernal (1st revision)",
    0x4232D81CCD24FAAE:
    "Commodore 64 Kernal (2nd revision)",
    0x4AF60EE54BEC9701:
    "Commodore 64 Kernal (3rd revision)",
    0x429EA22675CAB478:
    "Commodore 64 Kernal (3rd revision, Danish)",
    0x8C4548E2202CB366:
    "Commodore SX-64 Kernal",
    0x746EB1BC008B07E1:
    "Commodore SX-64 Kernal (Scandinavian)",
    0xDE7F07008B787040:
    "Commodore 64 Kernel (JiffyDOS patch)",
    0xA9D2AD1A4E5F782C:
    "Commodore SX-64 Kernal (JiffyDOS patch)"
]

let knownVc1541Roms : [UInt64: String] = [
    0x0000000000000000:
    "This 16 KB Rom contains the firmware of Commodore's VC1541 floppy drive.",
    0x44BBA0EAC5898597:
    "1541-II firmware released in 1987",
    0xA1D36980A17C8756:
    "1541-II firmware appearing in drives with a modern Newtronics Motor",
    0x361A1EC48F04F5A4:
    "Firmware of the old-style 1541 in white case",
    0xB938E2DA07F4FE40:
    "Upgraded firmware for the 1541C drive",
    0x47CBA55F16FB3E09:
    "1541-II firmware with relocation patch",
    0x8B2A523E29BED889:
    "1541-II firmware (JiffyDOS patch)",
    0xF7F4D931219DBB5D:
    "1541-II firmware (JiffyDOS patch 6.00)"
]

class RomPrefsController : UserDialogController {
    
    let romImage = NSImage.init(named: "rom")
    let romImageLight = NSImage.init(named: "rom_light")
    let romImageMedium = NSImage.init(named: "rom_medium")

    @IBOutlet weak var headerImage: NSImageView!
    @IBOutlet weak var headerText: NSTextField!
    @IBOutlet weak var headerSubText: NSTextField!

    @IBOutlet weak var basicRom: NSImageView!
    @IBOutlet weak var basicRomDragText: NSTextField!
    @IBOutlet weak var basicRomHash: NSTextField!
    @IBOutlet weak var basicRomPath: NSTextField!
    @IBOutlet weak var basicRomDescription: NSTextField!
    @IBOutlet weak var basicRomButton: NSButton!

    @IBOutlet weak var kernalRom: NSImageView!
    @IBOutlet weak var kernalRomDragText: NSTextField!
    @IBOutlet weak var kernalRomHash: NSTextField!
    @IBOutlet weak var kernalRomPath: NSTextField!
    @IBOutlet weak var kernalRomDescription: NSTextField!
    @IBOutlet weak var kernelRomButton: NSButton!
    
    @IBOutlet weak var characterRom: NSImageView!
    @IBOutlet weak var characterRomDragText: NSTextField!
    @IBOutlet weak var characterRomHash: NSTextField!
    @IBOutlet weak var characterRomPath: NSTextField!
    @IBOutlet weak var characterRomDescription: NSTextField!
    @IBOutlet weak var characterRomButton: NSButton!

    @IBOutlet weak var vc1541Rom: NSImageView!
    @IBOutlet weak var vc1541RomDragText: NSTextField!
    @IBOutlet weak var vc1541RomHash: NSTextField!
    @IBOutlet weak var vc1541RomPath: NSTextField!
    @IBOutlet weak var vc1541RomDescription: NSTextField!
    @IBOutlet weak var vc1541RomButton: NSButton!

    @IBOutlet weak var okButton: NSButton!
    
    override func awakeFromNib()
    {
        // The user might delete Roms, so we better pause emulation while the
        // dialog is open.
        c64.halt()
        
        refresh()
    }

    func refresh()
    {
        let hasBasicRom = c64.isBasicRomLoaded()
        let hasKernalRom = c64.isKernalRomLoaded()
        let hasCharacterRom = c64.isCharRomLoaded()
        let hasVc1541Rom = c64.isVC1541RomLoaded()

        let defaults = UserDefaults.standard
        let basicURL = defaults.url(forKey: VC64Keys.basicRom)
        let characterURL = defaults.url(forKey: VC64Keys.charRom)
        let kernalURL = defaults.url(forKey: VC64Keys.kernalRom)
        let vc1541URL = defaults.url(forKey: VC64Keys.vc1541Rom)
        
        let basicHash = parent.c64.basicRomFingerprint()
        let kernalHash = parent.c64.kernalRomFingerprint()
        let characterHash = parent.c64.charRomFingerprint()
        let vc1541Hash = parent.c64.vc1541RomFingerprint()

        if c64.isRunnable() {
            
            headerImage.image = NSImage.init(named: "AppIcon")
            headerText.stringValue = "All required ROMs are loaded."
            headerSubText.stringValue = "VirtualC64 is ready to run."
            okButton.title = "OK"
            
        } else {
            
            headerImage.image = NSImage.init(named: "alertIcon")
            headerText.stringValue = "VirtualC64 cannot run because some ROMs are missing."
            headerSubText.stringValue = "Use drag and drop to add ROM images."
            okButton.title = "Quit"
        }
        
        basicRom.image = hasBasicRom ? romImage : romImageLight
        basicRomDragText.isHidden = hasBasicRom
        basicRomHash.isHidden = !hasBasicRom
        basicRomHash.stringValue = String(format: "Hash: %llX", basicHash)
        basicRomPath.isHidden = !hasBasicRom
        basicRomPath.stringValue = basicURL?.path ?? ""
        basicRomButton.isHidden = !hasBasicRom
        if let description = knownBasicRoms[basicHash] {
            basicRomDescription.stringValue = description
            basicRomDescription.textColor = NSColor.textColor
        } else {
            basicRomDescription.stringValue = "An unknown, possibly patched Basic ROM."
            basicRomDescription.textColor = .red
        }
        
        kernalRom.image = hasKernalRom ? romImage : romImageLight
        kernalRomDragText.isHidden = hasKernalRom
        kernalRomHash.isHidden = !hasKernalRom
        kernalRomHash.stringValue = String(format: "Hash: %llX", kernalHash)
        kernalRomPath.isHidden = !hasKernalRom
        kernalRomPath.stringValue = kernalURL?.path ?? ""
        kernelRomButton.isHidden = !hasKernalRom
        if let description = knownKernalRoms[kernalHash] {
            kernalRomDescription.stringValue = description
            kernalRomDescription.textColor = NSColor.textColor
        } else {
            kernalRomDescription.stringValue = "An unknown, possibly patched Kernal ROM."
            kernalRomDescription.textColor = .red
        }

        characterRom.image = hasCharacterRom ? romImage : romImageLight
        characterRomDragText.isHidden = hasCharacterRom
        characterRomHash.isHidden = !hasCharacterRom
        characterRomHash.stringValue = String(format: "Hash: %llX", characterHash)
        characterRomDescription.textColor = NSColor.textColor
        characterRomPath.isHidden = !hasCharacterRom
        characterRomPath.stringValue = characterURL?.path ?? ""
        characterRomButton.isHidden = !hasCharacterRom
        if let description = knownCharacterRoms[characterHash] {
            characterRomDescription.stringValue = description
            characterRomDescription.textColor = NSColor.textColor
        } else {
            characterRomDescription.stringValue = "An unknown, possibly patched Character ROM."
            characterRomDescription.textColor = .red
        }

        vc1541Rom.image = hasVc1541Rom ? romImage : romImageLight
        vc1541RomDragText.isHidden = hasVc1541Rom
        vc1541RomHash.isHidden = !hasVc1541Rom
        vc1541RomHash.stringValue = String(format: "Hash: %llX", vc1541Hash)
        vc1541RomDescription.textColor = NSColor.textColor
        vc1541RomPath.isHidden = !hasVc1541Rom
        vc1541RomPath.stringValue = vc1541URL?.path ?? ""
        vc1541RomButton.isHidden = !hasVc1541Rom
        if let description = knownVc1541Roms[vc1541Hash] {
            vc1541RomDescription.stringValue = description
            vc1541RomDescription.textColor = .textColor
        } else {
            vc1541RomDescription.stringValue = "An unknown, possibly patched 15xx ROM."
            vc1541RomDescription.textColor = .red
        }
    }

    
    //
    // Action methods
    //
    
    @IBAction func deleteBasicRom(_ sender: Any!)
    {
        let defaults = UserDefaults.standard
        
        track()
        defaults.set(URL.init(string: ""), forKey: VC64Keys.basicRom)
        parent.c64.mem.deleteBasicRom()
        refresh()
    }
    
    @IBAction func deleteCharacterRom(_ sender: Any!)
    {
        let defaults = UserDefaults.standard

        track()
        defaults.set(URL.init(string: ""), forKey: VC64Keys.charRom)
        parent.c64.mem.deleteCharacterRom()
        refresh()
    }
    
    @IBAction func deleteKernalRom(_ sender: Any!)
    {
        let defaults = UserDefaults.standard
        
        track()
        defaults.set(URL.init(string: ""), forKey: VC64Keys.kernalRom)
        parent.c64.mem.deleteKernalRom()
        refresh()
    }
    
    @IBAction func deleteVC1541Rom(_ sender: Any!)
    {
        let defaults = UserDefaults.standard
        
        track()
        defaults.set(URL.init(string: ""), forKey: VC64Keys.vc1541Rom)
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
        
        if (c64.isRunnable()) {
            c64.run()
        } else {
           NSApp.terminate(self)
        }
    }
    
}

