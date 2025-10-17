// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class RomSettingsViewController: SettingsViewController {

    @IBOutlet weak var basicDropView: RomDropView!
    @IBOutlet weak var basicTitle: NSTextField!
    @IBOutlet weak var basicSubtitle: NSTextField!
    @IBOutlet weak var basicSubsubtitle: NSTextField!
    @IBOutlet weak var basicDeleteButton: NSButton!

    @IBOutlet weak var kernalDropView: RomDropView!
    @IBOutlet weak var kernalTitle: NSTextField!
    @IBOutlet weak var kernalSubtitle: NSTextField!
    @IBOutlet weak var kernalSubsubtitle: NSTextField!
    @IBOutlet weak var kernalDeleteButton: NSButton!

    @IBOutlet weak var charDropView: RomDropView!
    @IBOutlet weak var charTitle: NSTextField!
    @IBOutlet weak var charSubtitle: NSTextField!
    @IBOutlet weak var charSubsubtitle: NSTextField!
    @IBOutlet weak var charDeleteButton: NSButton!

    @IBOutlet weak var vc1541DropView: RomDropView!
    @IBOutlet weak var vc1541Title: NSTextField!
    @IBOutlet weak var vc1541Subtitle: NSTextField!
    @IBOutlet weak var vc1541Subsubtitle: NSTextField!
    @IBOutlet weak var vc1541DeleteButton: NSButton!

    @IBOutlet weak var romLockImage: NSButton!
    @IBOutlet weak var romLockText: NSTextField!
    @IBOutlet weak var romLockSubText: NSTextField!

    @IBOutlet weak var romOkButton: NSButton!
    @IBOutlet weak var romPowerButton: NSButton!

    override var showLock: Bool { true }

    override func viewDidLoad() {

        log(.lifetime)

        basicDropView.parent = self
        kernalDropView.parent = self
        charDropView.parent = self
        vc1541DropView.parent = self
    }

    override func refresh() {

        super.refresh()
        
        guard let emu = emu else { return }

        let basicRom = emu.c64.basicRom
        let charRom = emu.c64.charRom
        let kernalRom = emu.c64.kernalRom
        let vc1541Rom = emu.c64.vc1541Rom

        let poweredOff         = emu.poweredOff

        let hasBasic           = basicRom.crc != 0
        let hasCommodoreBasic  = basicRom.vendor == .COMMODORE
        let hasMega65Basic     = basicRom.vendor == .MEGA65
        let hasUnknownBasic    = basicRom.vendor == .UNKNOWN && hasBasic
        let hasPatchedBasic    = basicRom.patched

        let hasChar            = charRom.crc != 0
        let hasCommodoreChar   = charRom.vendor == .COMMODORE
        let hasMega65Char      = charRom.vendor == .MEGA65
        let hasUnknownChar     = charRom.vendor == .UNKNOWN && hasChar
        let hasPatchedChar     = charRom.patched

        let hasKernal          = kernalRom.crc != 0
        let hasCommodoreKernal = kernalRom.vendor == .COMMODORE
        let hasMega65Kernal    = kernalRom.vendor == .MEGA65
        let hasUnknownKernal   = kernalRom.vendor == .UNKNOWN && hasKernal
        let hasPatchedKernal   = kernalRom.patched

        let hasVC1541          = vc1541Rom.crc != 0
        let hasCommodoreVC1541 = vc1541Rom.vendor == .COMMODORE
        let hasUnknownVC1541   = vc1541Rom.vendor == .UNKNOWN && hasVC1541
        let hasPatchedVC1541   = vc1541Rom.patched

        let romMissing = NSImage(named: "rom_missing")
        let romOrig    = NSImage(named: "rom_original")
        let romMega    = NSImage(named: "rom_mega65")
        let romPatched = NSImage(named: "rom_patched")
        let romUnknown = NSImage(named: "rom_unknown")

        // Lock controls if emulator is powered on
        presetPopup.isEnabled = poweredOff
        basicDropView.isEnabled = poweredOff
        basicDeleteButton.isEnabled = poweredOff
        charDropView.isEnabled = poweredOff
        charDeleteButton.isEnabled = poweredOff
        kernalDropView.isEnabled = poweredOff
        kernalDeleteButton.isEnabled = poweredOff
        vc1541DropView.isEnabled = poweredOff
        vc1541DeleteButton.isEnabled = poweredOff

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
        if hasUnknownBasic { basicSubtitle.stringValue = "\(String(format: "%0lx", basicRom.fnv))" }

        charTitle.stringValue = hasChar ? String(cString: charRom.title) : "Character Rom"
        charSubtitle.stringValue = hasChar ? String(cString: charRom.subtitle) : "Required"
        charSubsubtitle.stringValue = String(cString: charRom.revision)
        if hasUnknownChar { charSubtitle.stringValue = "\(String(format: "%0lx", charRom.fnv))" }

        kernalTitle.stringValue = hasKernal ? String(cString: kernalRom.title) : "Kernal Rom"
        kernalSubtitle.stringValue = hasKernal ? String(cString: kernalRom.subtitle) : "Required"
        kernalSubsubtitle.stringValue = String(cString: kernalRom.revision)
        if hasUnknownKernal { kernalSubtitle.stringValue = "\(String(format: "%0lx", kernalRom.fnv))" }

        vc1541Title.stringValue = hasVC1541 ? String(cString: vc1541Rom.title) : "VC1541 Rom"
        vc1541Subtitle.stringValue = hasVC1541 ? String(cString: vc1541Rom.subtitle) : "Optional"
        vc1541Subsubtitle.stringValue = String(cString: vc1541Rom.revision)
        if hasUnknownVC1541 { vc1541Subtitle.stringValue = "\(String(format: "%0lx", vc1541Rom.fnv))" }

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
    }

    //
    // Action methods
    //

    @IBAction func romDeleteBasicAction(_ sender: Any!) {

        if let emu = emu {

            emu.powerOff()
            emu.delete(.BASIC)
            refresh()
        }
    }

    @IBAction func romDeleteCharAction(_ sender: Any!) {

        if let emu = emu {

            emu.powerOff()
            emu.delete(.CHAR)
            refresh()
        }
    }

    @IBAction func romDeleteKernalAction(_ sender: Any!) {

        if let emu = emu {

            emu.powerOff()
            emu.delete(.KERNAL)
            refresh()
        }
    }

    @IBAction func romDeleteVC1541Action(_ sender: Any!) {

        if let emu = emu {

            emu.powerOff()
            emu.delete(.VC1541)
            refresh()
        }
    }

    @IBAction func romInstallAction(_ sender: NSButton!) {

        emu?.installOpenRoms()
        refresh()
    }

    //
    // Presets and Saving
    //
    override func preset(tag: Int) {

        switch tag {

        case 0: config?.loadRomUserDefaults()
        case 1: emu?.installOpenRoms()
        default: fatalError()
        }
        refresh()
    }

    override func save() {

        do {

            try config?.saveRomUserDefaults()

        } catch {

            controller?.showAlert(.cantSaveRoms,
                                  error: error,
                                  window: controller!.window)
        }
    }
}
