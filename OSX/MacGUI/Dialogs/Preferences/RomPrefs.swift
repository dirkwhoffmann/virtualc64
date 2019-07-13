//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

let knownBasicRoms: [UInt64: String] = [
    0x0000000000000000:
    "This 8 KB Rom contains Commodore's Basic interpreter.",
    0x20765FEA67A8762D:
    "Commodore 64 Basic V2"
]

let knownCharacterRoms: [UInt64: String] = [
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
    "The Commodore 64 character set",
    0x1130C1CE287876DD:
    "Atari 800 character set",
    0x975546A5B6168FFD:
    "MSX character set",
    0x7C74107C9365F735:
    "ZX Spectrum character set",
    0xAFFE8B0EE2176CBD:
    "Amstrad CPC character set",
    0xD14C5BE4FEE17705:
    "Amiga 500 Topaz character set (broken)",
    0xA2C6A6E2C0477981:
    "Amiga 500 Topaz character set V2",
    0x3BF55C821EE80365:
    "Amiga 1200 Topaz character set (broken)",
    0x19F0DD3F3F9C4FE9:
    "Amiga 1200 Topaz character set V2",
    0xE527AD3E0DDE930D:
    "Teletext character set"    
]

let knownKernalRoms: [UInt64: String] = [
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
    "Commodore SX-64 Kernal (JiffyDOS patch)",
    0x750617B8DE6DBA82:
    "Cockroach Turbo-ROM V1",
    0x7E0A124C3F192818:
    "Datel Electronics Turbo Rom II 3.2+",
    0x211EAC45AB03A2CA:
    "Exos Kernal ROM V3",
    0xF2A39FF166D338AE:
    "Turbo Tape ROM V0.1 (2007)"
]

let knownVc1541Roms: [UInt64: String] = [
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
        
        guard let con = myController else { return }
        guard let c64 = proxy else { return }

        track()

        let dragInfo = "To add a Rom, drag a Rom image file onto the icon on the left."
        let romImage = NSImage.init(named: "rom")
        let romImageLight = NSImage.init(named: "rom_light")
        
        // Gather information about Roms
        let hasBasicRom = c64.isBasicRomLoaded()
        let hasKernalRom = c64.isKernalRomLoaded()
        let hasCharacterRom = c64.isCharRomLoaded()
        let hasVc1541Rom = c64.isVC1541RomLoaded()
       
        let basicURL = con.basicRomURL
        let characterURL = con.charRomURL
        let kernalURL = con.kernalRomURL
        let vc1541URL = con.vc1541RomURL
        
        let basicHash = c64.basicRomFingerprint()
        let kernalHash = c64.kernalRomFingerprint()
        let characterHash = c64.charRomFingerprint()
        let vc1541Hash = c64.vc1541RomFingerprint()
        
        // Header image and description
        if c64.isRunnable() {
            romHeaderImage.image = NSImage.init(named: "AppIcon")
            romHeaderText.stringValue = "All required ROMs are loaded. VirtualC64 is ready to run."
            romHeaderSubText.stringValue = ""
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
        if hasBasicRom {
            romBasicImage.image = romImage
            romBasicHashText.isHidden = false
            romBasicHashText.stringValue = String(format: "Hash: %llX", basicHash)
            romBasicPathText.textColor = .textColor
            romBasicPathText.stringValue = basicURL.path
            romBasicButton.isHidden = false
        } else {
            romBasicImage.image = romImageLight
            romBasicHashText.isHidden = true
            romBasicPathText.textColor = .red
            romBasicPathText.stringValue = dragInfo
            romBasicButton.isHidden = true
        }
        if let description = knownBasicRoms[basicHash] {
            romBasicDescription.stringValue = description
            romBasicDescription.textColor = .textColor
        } else {
            romBasicDescription.stringValue = "An unknown, possibly patched Basic ROM."
            romBasicDescription.textColor = .red
        }
        
        // Kernal Rom
        if hasKernalRom {
            romKernalImage.image = romImage
            romKernalHashText.isHidden = false
            romKernalHashText.stringValue = String(format: "Hash: %llX", kernalHash)
            romKernalPathText.textColor = .textColor
            romKernalPathText.stringValue = kernalURL.path
            romKernelButton.isHidden = false
        } else {
            romKernalImage.image = romImageLight
            romKernalHashText.isHidden = true
            romKernalPathText.textColor = .red
            romKernalPathText.stringValue = dragInfo
            romKernelButton.isHidden = true
        }
        if let description = knownKernalRoms[kernalHash] {
            romKernalDescription.stringValue = description
            romKernalDescription.textColor = .textColor
        } else {
            romKernalDescription.stringValue = "An unknown, possibly patched Kernal ROM."
            romKernalDescription.textColor = .red
        }
        
        // Character Rom
        if hasCharacterRom {
            romCharImage.image = romImage
            romCharHashText.isHidden = false
            romCharHashText.stringValue = String(format: "Hash: %llX", characterHash)
            romCharPathText.textColor = .textColor
            romCharPathText.stringValue = characterURL.path
            romCharButton.isHidden = false
        } else {
            romCharImage.image = romImageLight
            romCharHashText.isHidden = true
            romCharPathText.textColor = .red
            romCharPathText.stringValue = dragInfo
            romCharButton.isHidden = true
        }
        if let description = knownCharacterRoms[characterHash] {
            romCharDescription.stringValue = description
            romCharDescription.textColor = .textColor
        } else {
            romCharDescription.stringValue = "An unknown, possibly patched Character ROM."
            romCharDescription.textColor = .red
        }
        
        // VC1541 Rom
        if hasVc1541Rom {
            romVc1541Image.image = romImage
            romVc1541HashText.isHidden = false
            romVc1541HashText.stringValue = String(format: "Hash: %llX", vc1541Hash)
            romVc1541PathText.textColor = .textColor
            romVc1541PathText.stringValue = vc1541URL.path
            romVc1541Button.isHidden = false
        } else {
            romVc1541Image.image = romImageLight
            romVc1541HashText.isHidden = true
            romVc1541PathText.textColor = .red
            romVc1541PathText.stringValue = dragInfo
            romVc1541Button.isHidden = true
        }
        if let description = knownVc1541Roms[vc1541Hash] {
            romVc1541Description.stringValue = description
            romVc1541Description.textColor = .textColor
        } else {
            romVc1541Description.stringValue = "An unknown, possibly patched 15xx ROM."
            romVc1541Description.textColor = .red
        }
        
        romOkButton.title = c64.isRunnable() ? "OK" : "Quit"
    }
    
    //
    // Action methods
    //
    
    @IBAction func romDeleteBasicAction(_ sender: Any!) {

        myController?.basicRomURL = URL(fileURLWithPath: "/")
        proxy?.halt()
        proxy?.mem.deleteBasicRom()
        refresh()
    }
    
    @IBAction func romDeleteCharAction(_ sender: Any!) {

        myController?.charRomURL = URL(fileURLWithPath: "/")
        proxy?.halt()
        proxy?.mem.deleteCharacterRom()
        refresh()
    }
    
    @IBAction func romDeleteKernalAction(_ sender: Any!) {

        myController?.kernalRomURL = URL(fileURLWithPath: "/")
        proxy?.halt()
        proxy?.mem.deleteKernalRom()
        refresh()
    }
    
    @IBAction func romDeleteVC1541Action(_ sender: Any!) {

        myController?.vc1541RomURL = URL(fileURLWithPath: "/")
        proxy?.halt()
        proxy?.drive1.deleteRom()
        proxy?.drive2.deleteRom()
        refresh()
    }
    
    @IBAction func helpAction(_ sender: Any!) {
        
        if let url = URL.init(string: "http://www.dirkwhoffmann.de/virtualc64/ROMs.html") {
            NSWorkspace.shared.open(url)
        }
    }
}
