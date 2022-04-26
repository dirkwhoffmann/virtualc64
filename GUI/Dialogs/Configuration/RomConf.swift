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
        
        let poweredOff         = c64.poweredOff
        
        let basicIdentifier    = c64.romIdentifier(.BASIC)
        let hasBasic           = basicIdentifier != .ROM_MISSING
        let hasCommodoreBasic  = c64.isCommodoreRom(basicIdentifier)
        let hasMega65Basic     = c64.hasMega65Rom(.BASIC)
        let hasPatchedBasic    = c64.isPatchedRom(basicIdentifier)

        let charIdentifier     = c64.romIdentifier(.CHAR)
        let hasChar            = charIdentifier != .ROM_MISSING
        let hasCommodoreChar   = c64.isCommodoreRom(charIdentifier)
        let hasMega65Char      = c64.hasMega65Rom(.CHAR)
        let hasPatchedChar     = c64.isPatchedRom(charIdentifier)

        let kernalIdentifier   = c64.romIdentifier(.KERNAL)
        let hasKernal          = kernalIdentifier != .ROM_MISSING
        let hasCommodoreKernal = c64.isCommodoreRom(kernalIdentifier)
        let hasMega65Kernal    = c64.hasMega65Rom(.KERNAL)
        let hasPatchedKernal   = c64.isPatchedRom(kernalIdentifier)

        let vc1541Identifier   = c64.romIdentifier(.VC1541)
        let hasVC1541          = vc1541Identifier != .ROM_MISSING
        let hasCommodoreVC1541 = c64.isCommodoreRom(vc1541Identifier)
        let hasPatchedVC1541   = c64.isPatchedRom(vc1541Identifier)

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

        track()
        
        let b = NSData(data: NSDataAsset(name: "basic_generic")!.data)
        let c = NSData(data: NSDataAsset(name: "chargen_openroms")!.data)
        let k = NSData(data: NSDataAsset(name: "kernal_generic")!.data)

        if let rom = try? Proxy.make(buffer: b.bytes, length: b.length) as RomFileProxy {
            c64.loadRom(rom)
        }
        if let rom = try? Proxy.make(buffer: c.bytes, length: c.length) as RomFileProxy {
            c64.loadRom(rom)
        }
        if let rom = try? Proxy.make(buffer: k.bytes, length: k.length) as RomFileProxy {
            c64.loadRom(rom)
        }
        
        refresh()
    }

    @IBAction func romDefaultsAction(_ sender: NSButton!) {
        
        track()
        config.saveRomUserDefaults()
    }
}
