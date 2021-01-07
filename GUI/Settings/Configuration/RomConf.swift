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
    Rom.basic_commodore:         "Commodore 64 Basic V2"
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
    Rom.kernal_64_jiffy:         "Commodore 64 Kernal (JiffyDOS patch)",
    Rom.kernal_sx64_jiffy:       "Commodore SX-64 Kernal (JiffyDOS patch)",
    Rom.kernal_turbo_rom:        "Cockroach Turbo-ROM V1",
    Rom.kernal_datel_v32:        "Datel Electronics Turbo Rom II 3.2+",
    Rom.kernal_exos_v3:          "Exos Kernal ROM V3",
    Rom.kernal_turbo_tape:       "Turbo Tape ROM V0.1 (2007)"
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

extension ConfigurationController {

    func refreshRomTab() {
        
        track()

        let poweredOff         = c64.poweredOff
        
        let basicIdentifier    = c64.romIdentifier(.BASIC)
        let hasBasic           = basicIdentifier != .ROM_MISSING
        let hasCommodoreBasic  = c64.isCommodoreRom(basicIdentifier)
        let hasMega65Basic     = c64.hasMega65BasicRom()
        let hasPatchedBasic    = c64.isPatchedRom(basicIdentifier)

        let charIdentifier     = c64.romIdentifier(.CHAR)
        let hasChar            = charIdentifier != .ROM_MISSING
        let hasCommodoreChar   = c64.isCommodoreRom(charIdentifier)
        let hasMega65Char      = c64.hasMega65CharRom()
        let hasPatchedChar     = c64.isPatchedRom(charIdentifier)

        let kernalIdentifier   = c64.romIdentifier(.KERNAL)
        let hasKernal          = kernalIdentifier != .ROM_MISSING
        let hasCommodoreKernal = c64.isCommodoreRom(kernalIdentifier)
        let hasMega65Kernal    = c64.hasMega65KernelRom()
        let hasPatchedKernal   = c64.isPatchedRom(kernalIdentifier)

        let vc1541Identifier   = c64.romIdentifier(.VC1541)
        let hasVC1541          = vc1541Identifier != .ROM_MISSING
        let hasCommodoreVC1541 = c64.isCommodoreRom(vc1541Identifier)
        let hasPatchedVC1541   = c64.isPatchedRom(vc1541Identifier)

        let romMissing = NSImage.init(named: "rom_missing")
        let romOrig    = NSImage.init(named: "rom_original")
        let romMega    = NSImage.init(named: "rom_mega65")
        let romPatched = NSImage.init(named: "rom_unknown")
        let romUnknown = NSImage.init(named: "rom_unknown")
        
        // Lock controls if emulator is powered on
        basicDropView.isEnabled = poweredOff
        basicDeleteButton.isEnabled = poweredOff
        charDropView.isEnabled = poweredOff
        charDeleteButton.isEnabled = poweredOff
        kernalDropView.isEnabled = poweredOff
        kernalDeleteButton.isEnabled = poweredOff
        vc1541DropView.isEnabled = poweredOff
        vc1541DeleteButton.isEnabled = poweredOff
        romInstallButton.isEnabled = poweredOff
        
        // Icons
        basicDropView.image =
            hasMega65Basic     ? romMega :
            hasCommodoreBasic  ? romOrig :
            hasPatchedBasic    ? romPatched :
            hasBasic           ? romUnknown : romMissing

        charDropView.image =
            hasMega65Char      ? romMega :
            hasCommodoreChar   ? romOrig :
            hasPatchedChar     ? romPatched :
            hasChar            ? romUnknown : romMissing

        kernalDropView.image =
            hasMega65Kernal    ? romMega :
            hasCommodoreKernal ? romOrig :
            hasPatchedKernal   ? romPatched :
            hasKernal          ? romUnknown : romMissing

        vc1541DropView.image =
            hasCommodoreVC1541 ? romOrig :
            hasPatchedVC1541   ? romPatched :
            hasVC1541          ? romUnknown : romMissing

        // Titles and subtitles
        basicTitle.stringValue = hasBasic ? c64.romTitle(.BASIC) : "Basic Rom"
        basicSubtitle.stringValue = hasBasic ? c64.romSubTitle(.BASIC) : "Required"
        basicSubsubtitle.stringValue = c64.romRevision(.BASIC)

        charTitle.stringValue = hasChar ? c64.romTitle(.CHAR) : "Character Rom"
        charSubtitle.stringValue = hasChar ? c64.romSubTitle(.CHAR) : "Required"
        charSubsubtitle.stringValue = c64.romRevision(.CHAR)

        kernalTitle.stringValue = hasKernal ? c64.romTitle(.KERNAL) : "Kernal Rom"
        kernalSubtitle.stringValue = hasKernal ? c64.romSubTitle(.KERNAL) : "Required"
        kernalSubsubtitle.stringValue = c64.romRevision(.KERNAL)

        vc1541Title.stringValue = hasVC1541 ? c64.romTitle(.VC1541) : "VC1541 Rom"
        vc1541Subtitle.stringValue = hasVC1541 ? c64.romSubTitle(.VC1541) : "Optional"
        vc1541Subsubtitle.stringValue = c64.romRevision(.VC1541)

        // Hide some controls
        basicDeleteButton.isHidden = !hasBasic
        charDeleteButton.isHidden = !hasChar
        kernalDeleteButton.isHidden = !hasKernal
        vc1541DeleteButton.isHidden = !hasVC1541

        // Lock symbol and explanation
        if poweredOff {
            romLockImage.image = NSImage.init(named: "NSInfo")
            romLockText.stringValue = "To add a Rom, drag a Rom image file onto one of the four chip icons."
            romLockSubText.stringValue = "Original Roms are protected by copyright. Please obey legal regulations."
        } else {
            romLockImage.image = NSImage.init(named: "lockIcon")
            romLockText.stringValue = "The settings are locked because the emulator is running."
            romLockSubText.stringValue = "Click the lock to power down the emulator."
        }

        // Boot button
        romPowerButton.isHidden = !bootable
    }
    
    //
    // Action methods
    //
    
    @IBAction func romDeleteBasicAction(_ sender: Any!) {

        proxy?.powerOff()
        proxy?.deleteBasicRom()
        refresh()
    }
    
    @IBAction func romDeleteCharAction(_ sender: Any!) {

        proxy?.powerOff()
        proxy?.deleteCharRom()
        refresh()
    }
    
    @IBAction func romDeleteKernalAction(_ sender: Any!) {

        proxy?.powerOff()
        proxy?.deleteKernalRom()
        refresh()
    }
    
    @IBAction func romDeleteVC1541Action(_ sender: Any!) {

        proxy?.powerOff()
        proxy?.deleteVC1541Rom()
        refresh()
    }
        
    @IBAction func romInstallAction(_ sender: NSButton!) {

        track()
        
        let basicRom = NSDataAsset(name: "basic_generic")?.data
        let charRom = NSDataAsset(name: "chargen_openroms")?.data
        let kernalRom = NSDataAsset(name: "kernal_generic")?.data
        
        // Install OpenROMs
        try? c64.loadRom(type: .BASIC, data: basicRom)
        try? c64.loadRom(type: .CHAR, data: charRom)
        try? c64.loadRom(type: .KERNAL, data: kernalRom)
        
        refresh()
    }

    @IBAction func romDefaultsAction(_ sender: NSButton!) {
        
        track()
        config.saveRomUserDefaults()
    }
}
