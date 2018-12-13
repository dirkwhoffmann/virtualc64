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
    "This 8 KB Rom contains Commodore's Basic interpreter.",
    0x20765FEA67A8762D:
    "Commodore 64 Basic V2"
]

let knownCharacterRoms : [UInt64 : String] = [
    0x0000000000000000:
    "This 4 KB Rom contains the C64's character set.",
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
    "This 8 KB Rom contains the low-level operating system.",
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
    "This 16 KB Rom contains the firmware of Commodore's floppy drive.",
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

extension PreferencesController {

    func refreshRomTab() {
        
        let romImage = NSImage.init(named: "rom")
        let romImageLight = NSImage.init(named: "rom_light")
        
        // Gather information about Roms
        let hasBasicRom = c64.isBasicRomLoaded()
        let hasKernalRom = c64.isKernalRomLoaded()
        let hasCharacterRom = c64.isCharRomLoaded()
        let hasVc1541Rom = c64.isVC1541RomLoaded()
       
        let basicURL = parent.basicRomURL
        let characterURL = parent.charRomURL
        let kernalURL = parent.kernalRomURL
        let vc1541URL = parent.vc1541RomURL
        
        let basicHash = parent.c64.basicRomFingerprint()
        let kernalHash = parent.c64.kernalRomFingerprint()
        let characterHash = parent.c64.charRomFingerprint()
        let vc1541Hash = parent.c64.vc1541RomFingerprint()
        
        // Header image and description
        if c64.isRunnable() {
            romHeaderImage.image = NSImage.init(named: "AppIcon")
            romHeaderText.stringValue = "All required ROMs are loaded. VirtualC64 is ready to run."
            romHeaderSubText.stringValue = "Click OK to get startet."
            romOkButton.title = "OK"
        } else {
            let numMissing = (hasBasicRom ? 0 : 1)
                + (hasKernalRom ? 0 : 1)
                + (hasCharacterRom ? 0 : 1)
                + (hasVc1541Rom ? 0 : 1)
            let nrStr = [ "zero", "one", "two", "three", "four" ][numMissing]
            let romStr = numMissing == 1 ? "ROM is" : "ROMs are"
            romHeaderImage.image = NSImage.init(named: "NSCaution")
            romHeaderText.stringValue = "VirtualC64 cannot run because \(nrStr) \(romStr) missing."
            romHeaderSubText.stringValue = "Use drag and drop to add ROM images."
            romOkButton.title = "Quit"
        }
        
        // Basic Rom
        romBasicImage.image = hasBasicRom ? romImage : romImageLight
        romBasicDragImage.isHidden = hasBasicRom
        romBasicHashText.isHidden = !hasBasicRom
        romBasicHashText.stringValue = String(format: "Hash: %llX", basicHash)
        romBasicPathText.isHidden = !hasBasicRom
        romBasicPathText.stringValue = basicURL.path
        romBasicButton.isHidden = !hasBasicRom
        if let description = knownBasicRoms[basicHash] {
            romBasicDescription.stringValue = description
            romBasicDescription.textColor = NSColor.textColor
        } else {
            romBasicDescription.stringValue = "An unknown, possibly patched Basic ROM."
            romBasicDescription.textColor = .red
        }
        
        // Kernal Rom
        romKernalImage.image = hasKernalRom ? romImage : romImageLight
        romKernalDragImage.isHidden = hasKernalRom
        romKernalHashText.isHidden = !hasKernalRom
        romKernalHashText.stringValue = String(format: "Hash: %llX", kernalHash)
        romKernalPathText.isHidden = !hasKernalRom
        romKernalPathText.stringValue = kernalURL.path
        romKernelButton.isHidden = !hasKernalRom
        if let description = knownKernalRoms[kernalHash] {
            romKernalDescription.stringValue = description
            romKernalDescription.textColor = NSColor.textColor
        } else {
            romKernalDescription.stringValue = "An unknown, possibly patched Kernal ROM."
            romKernalDescription.textColor = .red
        }
        
        // Character Rom
        romCharImage.image = hasCharacterRom ? romImage : romImageLight
        romCharDragImage.isHidden = hasCharacterRom
        romCharHashText.isHidden = !hasCharacterRom
        romCharHashText.stringValue = String(format: "Hash: %llX", characterHash)
        romCharDescription.textColor = NSColor.textColor
        romCharPathText.isHidden = !hasCharacterRom
        romCharPathText.stringValue = characterURL.path
        romCharButton.isHidden = !hasCharacterRom
        if let description = knownCharacterRoms[characterHash] {
            romCharDescription.stringValue = description
            romCharDescription.textColor = NSColor.textColor
        } else {
            romCharDescription.stringValue = "An unknown, possibly patched Character ROM."
            romCharDescription.textColor = .red
        }
        
        // VC1541 Rom
        romVc1541Image.image = hasVc1541Rom ? romImage : romImageLight
        romVc1541DragImage.isHidden = hasVc1541Rom
        romVc1541HashText.isHidden = !hasVc1541Rom
        romVc1541HashText.stringValue = String(format: "Hash: %llX", vc1541Hash)
        romVc1541Description.textColor = NSColor.textColor
        romVc1541PathText.isHidden = !hasVc1541Rom
        romVc1541PathText.stringValue = vc1541URL.path
        romVc1541Button.isHidden = !hasVc1541Rom
        if let description = knownVc1541Roms[vc1541Hash] {
            romVc1541Description.stringValue = description
            romVc1541Description.textColor = .textColor
        } else {
            romVc1541Description.stringValue = "An unknown, possibly patched 15xx ROM."
            romVc1541Description.textColor = .red
        }
        
        track()
    }
    
    //
    // Action methods
    //
    
    @IBAction func romDeleteBasicAction(_ sender: Any!)
    {
        parent.basicRomURL = URL(fileURLWithPath: "/")
        parent.c64.halt()
        parent.c64.mem.deleteBasicRom()
        refresh()
    }
    
    @IBAction func romDeleteCharAction(_ sender: Any!)
    {
        parent.charRomURL = URL(fileURLWithPath: "/")
        parent.c64.halt()
        parent.c64.mem.deleteCharacterRom()
        refresh()
    }
    
    @IBAction func romDeleteKernalAction(_ sender: Any!)
    {
        parent.kernalRomURL = URL(fileURLWithPath: "/")
        parent.c64.halt()
        parent.c64.mem.deleteKernalRom()
        refresh()
    }
    
    @IBAction func romDeleteVC1541Action(_ sender: Any!)
    {
        parent.vc1541RomURL = URL(fileURLWithPath: "/")
        parent.c64.halt()
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
}
