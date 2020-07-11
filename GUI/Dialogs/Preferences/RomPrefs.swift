// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// swiftlint:disable colon

struct Rom {

    static let missing                 = 0x0000000000000000 as UInt64

    // Basic Roms
    static let basic_commodore         = 0x20765FEA67A8762D as UInt64
    static let basic_mega65_generic    = 0xF5E925DCABE2D3F0 as UInt64

    // Character Roms
    static let char_commodore          = 0xACC576F7B332AC15 as UInt64
    static let char_swedish_C2D007     = 0x3CA9D37AA3DE0969 as UInt64
    static let char_swedish            = 0x6E3827A65FFF116F as UInt64
    static let char_spanish_C64C       = 0x623CDF045B74B691 as UInt64
    static let char_danish             = 0x79A236A3B3645231 as UInt64
    static let char_atari_800          = 0x1130C1CE287876DD as UInt64
    static let char_msx                = 0x975546A5B6168FFD as UInt64
    static let char_zx_spectrum        = 0x7C74107C9365F735 as UInt64
    static let char_amstrad_cpc        = 0xAFFE8B0EE2176CBD as UInt64
    static let char_a500_topaz_broken  = 0xD14C5BE4FEE17705 as UInt64
    static let char_a500_topaz_v2      = 0xA2C6A6E2C0477981 as UInt64
    static let char_a1200_topaz_broken = 0x3BF55C821EE80365 as UInt64
    static let char_a1200_topaz_v2     = 0x19F0DD3F3F9C4FE9 as UInt64
    static let char_teletext           = 0xE527AD3E0DDE930D as UInt64
    static let char_mega65_generic     = 0x4D31ECBF4F967DC3 as UInt64

    // Kernal Roms
    static let kernal_1st              = 0xFB166E49AF709AB8 as UInt64
    static let kernal_2nd              = 0x4232D81CCD24FAAE as UInt64
    static let kernal_3rd              = 0x4AF60EE54BEC9701 as UInt64
    static let kernal_danish_3rd       = 0x429EA22675CAB478 as UInt64
    static let kernal_sx64             = 0x8C4548E2202CB366 as UInt64
    static let kernal_sx64_scand       = 0x746EB1BC008B07E1 as UInt64
    static let kernal_64_jiffy         = 0xDE7F07008B787040 as UInt64
    static let kernal_sx64_jiffy       = 0xA9D2AD1A4E5F782C as UInt64
    static let kernal_turbo_rom        = 0x750617B8DE6DBA82 as UInt64
    static let kernal_datel_v32        = 0x7E0A124C3F192818 as UInt64
    static let kernal_exos_v3          = 0x211EAC45AB03A2CA as UInt64
    static let kernal_turbo_tape       = 0xF2A39FF166D338AE as UInt64
    static let kernal_mega65_generic   = 0xCB54D7934B99ADAC as UInt64

    // VC1541 Roms
    static let vc1541_II_1987          = 0x44BBA0EAC5898597 as UInt64
    static let vc1541_II_newtronic     = 0xA1D36980A17C8756 as UInt64
    static let vc1541_old_white        = 0x361A1EC48F04F5A4 as UInt64
    static let vc1541_1541C            = 0xB938E2DA07F4FE40 as UInt64
    static let vc1541_II_reloc_patch   = 0x47CBA55F16FB3E09 as UInt64
    static let vc1541_II_jiffy         = 0x8B2A523E29BED889 as UInt64
    static let vc1541_II_jiffy_v600    = 0xF7F4D931219DBB5D as UInt64
}

let knownBasicRoms: [UInt64: String] = [

    Rom.missing:                 "This 8 KB Rom contains Commodore's Basic interpreter.",
    Rom.basic_commodore:         "Commodore 64 Basic V2",
    Rom.basic_mega65_generic:    "M.E.G.A. C64 OpenROM"
]

let knownCharacterRoms: [UInt64: String] = [

    Rom.missing:                 "This 4 KB Rom contains the C64's character set.",
    Rom.char_commodore:          "The Commodore 64 character set",
    Rom.char_swedish_C2D007:     "Character ROM from a Swedish C64 (C2D007)",
    Rom.char_swedish:            "Character ROM from a Swedish C64",
    Rom.char_spanish_C64C:       "Character ROM from a Spanish C64C",
    Rom.char_danish:             "Character rom from a Danish C64",
    Rom.char_atari_800:          "Atari 800 character set",
    Rom.char_msx:                "MSX character set",
    Rom.char_zx_spectrum:        "ZX Spectrum character set",
    Rom.char_amstrad_cpc:        "Amstrad CPC character set",
    Rom.char_a500_topaz_broken:  "Amiga 500 Topaz character set (broken)",
    Rom.char_a500_topaz_v2:      "Amiga 500 Topaz character set V2",
    Rom.char_a1200_topaz_broken: "Amiga 1200 Topaz character set (broken)",
    Rom.char_a1200_topaz_v2:     "Amiga 1200 Topaz character set V2",
    Rom.char_teletext:           "Teletext character set",
    Rom.char_mega65_generic:     "M.E.G.A. C64 OpenROM"
]

let knownKernalRoms: [UInt64: String] = [

    Rom.missing:                 "This 8 KB Rom contains the low-level operating system.",
    Rom.kernal_1st:              "Commodore 64 Kernal (1st revision)",
    Rom.kernal_2nd:              "Commodore 64 Kernal (2nd revision)",
    Rom.kernal_3rd:              "Commodore 64 Kernal (3rd revision)",
    Rom.kernal_danish_3rd:       "Commodore 64 Kernal (3rd revision, Danish)",
    Rom.kernal_sx64:             "Commodore SX-64 Kernal",
    Rom.kernal_sx64_scand:       "Commodore SX-64 Kernal (Scandinavian)",
    Rom.kernal_64_jiffy:         "Commodore 64 Kernel (JiffyDOS patch)",
    Rom.kernal_sx64_jiffy:       "Commodore SX-64 Kernal (JiffyDOS patch)",
    Rom.kernal_turbo_rom:        "Cockroach Turbo-ROM V1",
    Rom.kernal_datel_v32:        "Datel Electronics Turbo Rom II 3.2+",
    Rom.kernal_exos_v3:          "Exos Kernal ROM V3",
    Rom.kernal_turbo_tape:       "Turbo Tape ROM V0.1 (2007)",
    Rom.kernal_mega65_generic:   "M.E.G.A. C64 OpenROM"
]

let knownVc1541Roms: [UInt64: String] = [

    Rom.missing:                 "This 16 KB Rom contains the firmware of Commodore's floppy drive.",
    Rom.vc1541_II_1987:          "1541-II firmware released in 1987",
    Rom.vc1541_II_newtronic:     "1541-II firmware appearing in drives with a modern Newtronics Motor",
    Rom.vc1541_old_white:        "Firmware of the old-style 1541 in white case",
    Rom.vc1541_1541C:            "Upgraded firmware for the 1541C drive",
    Rom.vc1541_II_reloc_patch:   "1541-II firmware with relocation patch",
    Rom.vc1541_II_jiffy:         "1541-II firmware (JiffyDOS patch)",
    Rom.vc1541_II_jiffy_v600:    "1541-II firmware (JiffyDOS patch 6.00)"
]

let megaRoms: [UInt64] = [

    Rom.basic_mega65_generic,
    Rom.char_mega65_generic,
    Rom.kernal_mega65_generic
]

extension PreferencesController {

    func refreshRomTab() {
        
        guard let con = myController else { return }
        guard let c64 = proxy else { return }

        track()

        let dragInfo = "To add a Rom, drag a Rom image file onto the icon on the left."
        let romImage = NSImage.init(named: "rom")
        let romImageMega = NSImage.init(named: "rom_mega65")
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

        let hasMegaBasic = megaRoms.contains(basicHash)
        let hasMegaChar = megaRoms.contains(characterHash)
        let hasMegaKernal = megaRoms.contains(kernalHash)

        let basicRomImage = hasMegaBasic ? romImageMega : romImage
        let charRomImage = hasMegaChar ? romImageMega : romImage
        let kernalRomImage = hasMegaKernal ? romImageMega : romImage

        // Header image and description
        if c64.isReady() {
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
            romBasicImage.image = basicRomImage
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
            romKernalImage.image = kernalRomImage
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
            romCharImage.image = charRomImage
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
        
        romOkButton.title = c64.isReady() ? "OK" : "Quit"
    }
    
    //
    // Action methods
    //
    
    @IBAction func romDeleteBasicAction(_ sender: Any!) {

        myController?.basicRomURL = URL(fileURLWithPath: "/")
        proxy?.powerOff()
        proxy?.mem.deleteBasicRom()
        refresh()
    }
    
    @IBAction func romDeleteCharAction(_ sender: Any!) {

        myController?.charRomURL = URL(fileURLWithPath: "/")
        proxy?.powerOff()
        proxy?.mem.deleteCharacterRom()
        refresh()
    }
    
    @IBAction func romDeleteKernalAction(_ sender: Any!) {

        myController?.kernalRomURL = URL(fileURLWithPath: "/")
        proxy?.powerOff()
        proxy?.mem.deleteKernalRom()
        refresh()
    }
    
    @IBAction func romDeleteVC1541Action(_ sender: Any!) {

        myController?.vc1541RomURL = URL(fileURLWithPath: "/")
        proxy?.powerOff()
        proxy?.drive1.deleteRom()
        proxy?.drive2.deleteRom()
        refresh()
    }
    
    @IBAction func helpAction(_ sender: Any!) {
        
        if let url = URL.init(string: "http://www.dirkwhoffmann.de/virtualc64/ROMs.html") {
            NSWorkspace.shared.open(url)
        }
    }
    
    @IBAction func romDefaultsAction(_ sender: NSButton!) {
        
        track()
        // config.saveRomUserDefaults()
    }
}
