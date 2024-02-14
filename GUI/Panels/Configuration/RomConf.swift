// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension ConfigurationController {

    func refreshRomTab() {
        
        let basicRom = c64.basicRom
        let charRom = c64.charRom
        let kernalRom = c64.kernalRom
        let vc1541Rom = c64.vc1541Rom

        let poweredOff         = c64.poweredOff
        
        let hasBasic           = basicRom.crc32 != 0 // basicIdentifier != .ROM_MISSING
        let hasCommodoreBasic  = basicRom.isCommodoreRom // c64.isCommodoreRom(basicIdentifier)
        let hasMega65Basic     = basicRom.isMega65Rom // c64.hasMega65Rom(.BASIC)
        let hasPatchedBasic    = basicRom.isPatchedRom // c64.isPatchedRom(basicIdentifier)

        let hasChar            = charRom.crc32 != 0 // charIdentifier != .ROM_MISSING
        let hasCommodoreChar   = charRom.isCommodoreRom // c64.isCommodoreRom(charIdentifier)
        let hasMega65Char      = charRom.isMega65Rom // c64.hasMega65Rom(.CHAR)
        let hasPatchedChar     = charRom.isPatchedRom // c64.isPatchedRom(charIdentifier)

        let hasKernal          = kernalRom.crc32 != 0 //  kernalIdentifier != .ROM_MISSING
        let hasCommodoreKernal = kernalRom.isCommodoreRom // c64.isCommodoreRom(kernalIdentifier)
        let hasMega65Kernal    = kernalRom.isMega65Rom // c64.hasMega65Rom(.KERNAL)
        let hasPatchedKernal   = kernalRom.isPatchedRom // c64.isPatchedRom(kernalIdentifier)

        let hasVC1541          = vc1541Rom.crc32 != 0 // vc1541Identifier != .ROM_MISSING
        let hasCommodoreVC1541 = vc1541Rom.isCommodoreRom //  c64.isCommodoreRom(vc1541Identifier)
        let hasPatchedVC1541   = vc1541Rom.isPatchedRom // c64.isPatchedRom(vc1541Identifier)

        let romMissing = NSImage(named: "rom_missing")
        let romOrig    = NSImage(named: "rom_original")
        let romMega    = NSImage(named: "rom_mega65")
        let romPatched = NSImage(named: "rom_patched")
        let romUnknown = NSImage(named: "rom_unknown")
        
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
        basicTitle.stringValue = hasBasic ? String(cString: basicRom.title) : "Basic Rom"
        basicSubtitle.stringValue = hasBasic ? String(cString: basicRom.subtitle) : "Required"
        basicSubsubtitle.stringValue = String(cString: basicRom.revision)

        charTitle.stringValue = hasChar ? String(cString: charRom.title) : "Character Rom"
        charSubtitle.stringValue = hasChar ? String(cString: charRom.subtitle) : "Required"
        charSubsubtitle.stringValue = String(cString: charRom.revision)

        kernalTitle.stringValue = hasKernal ? String(cString: kernalRom.title) : "Kernal Rom"
        kernalSubtitle.stringValue = hasKernal ? String(cString: kernalRom.subtitle) : "Required"
        kernalSubsubtitle.stringValue = String(cString: kernalRom.revision)

        vc1541Title.stringValue = hasVC1541 ? String(cString: vc1541Rom.title) : "VC1541 Rom"
        vc1541Subtitle.stringValue = hasVC1541 ? String(cString: vc1541Rom.subtitle) : "Optional"
        vc1541Subsubtitle.stringValue = String(cString: vc1541Rom.revision)

        // Hide some controls
        basicDeleteButton.isHidden = !hasBasic
        charDeleteButton.isHidden = !hasChar
        kernalDeleteButton.isHidden = !hasKernal
        vc1541DeleteButton.isHidden = !hasVC1541

        // Lock symbol and explanation
        if poweredOff {
            romLockImage.image = NSImage(named: "NSInfo")
            romLockText.stringValue = "To add a Rom, drag a Rom image file onto one of the four chip icons."
            romLockSubText.stringValue = "Original Roms are protected by copyright. Please obey legal regulations."
        } else {
            romLockImage.image = NSImage(named: "lockIcon")
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

        c64.powerOff()
        c64.deleteRom(.BASIC)
        refresh()
    }
    
    @IBAction func romDeleteCharAction(_ sender: Any!) {

        c64.powerOff()
        c64.deleteRom(.CHAR)
        refresh()
    }
    
    @IBAction func romDeleteKernalAction(_ sender: Any!) {

        c64.powerOff()
        c64.deleteRom(.KERNAL)
        refresh()
    }
    
    @IBAction func romDeleteVC1541Action(_ sender: Any!) {

        c64.powerOff()
        c64.deleteRom(.VC1541)
        refresh()
    }
        
    @IBAction func romInstallAction(_ sender: NSButton!) {
        
        let b = NSData(data: NSDataAsset(name: "basic_generic")!.data)
        let c = NSData(data: NSDataAsset(name: "chargen_openroms")!.data)
        let k = NSData(data: NSDataAsset(name: "kernal_generic")!.data)

        if let rom = try? RomFileProxy.makeWith(buffer: b.bytes, length: b.length) {
            c64.loadRom(rom)
        }
        if let rom = try? RomFileProxy.makeWith(buffer: c.bytes, length: c.length) {
            c64.loadRom(rom)
        }
        if let rom = try? RomFileProxy.makeWith(buffer: k.bytes, length: k.length) {
            c64.loadRom(rom)
        }
        
        refresh()
    }

    @IBAction func romDefaultsAction(_ sender: NSButton!) {
        
        do {

            try config.saveRomUserDefaults()

        } catch {

            parent.showAlert(.cantSaveRoms, error: error, window: window)
        }
    }
}
