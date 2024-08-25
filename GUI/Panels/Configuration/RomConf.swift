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

        if let emu = emu {

            let basicRom = emu.c64.basicRom
            let charRom = emu.c64.charRom
            let kernalRom = emu.c64.kernalRom
            let vc1541Rom = emu.c64.vc1541Rom

            let poweredOff         = emu.poweredOff

            let hasBasic           = basicRom.crc != 0
            let hasCommodoreBasic  = basicRom.vendor == .COMMODORE
            let hasMega65Basic     = basicRom.vendor == .MEGA65
            let hasPatchedBasic    = basicRom.patched

            let hasChar            = charRom.crc != 0
            let hasCommodoreChar   = charRom.vendor == .COMMODORE
            let hasMega65Char      = charRom.vendor == .MEGA65
            let hasPatchedChar     = charRom.patched

            let hasKernal          = kernalRom.crc != 0
            let hasCommodoreKernal = kernalRom.vendor == .COMMODORE
            let hasMega65Kernal    = kernalRom.vendor == .MEGA65
            let hasPatchedKernal   = kernalRom.patched

            let hasVC1541          = vc1541Rom.crc != 0
            let hasCommodoreVC1541 = vc1541Rom.vendor == .COMMODORE
            let hasPatchedVC1541   = vc1541Rom.patched

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
    }

    //
    // Action methods
    //
    
    @IBAction func romDeleteBasicAction(_ sender: Any!) {

        if let emu = emu {

            emu.powerOff()
            emu.deleteRom(.BASIC)
            refresh()
        }
    }
    
    @IBAction func romDeleteCharAction(_ sender: Any!) {

        if let emu = emu {

            emu.powerOff()
            emu.deleteRom(.CHAR)
            refresh()
        }
    }
    
    @IBAction func romDeleteKernalAction(_ sender: Any!) {

        if let emu = emu {

            emu.powerOff()
            emu.deleteRom(.KERNAL)
            refresh()
        }
    }
    
    @IBAction func romDeleteVC1541Action(_ sender: Any!) {

        if let emu = emu {

            emu.powerOff()
            emu.deleteRom(.VC1541)
            refresh()
        }
    }
        
    @IBAction func romInstallAction(_ sender: NSButton!) {
        
        emu?.installOpenRoms()
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
