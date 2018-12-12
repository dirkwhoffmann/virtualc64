//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

class RomPrefsController : UserDialogController {
    
    let romImage = NSImage.init(named: "rom")
    let romImageLight = NSImage.init(named: "rom_light")
    let romImageMedium = NSImage.init(named: "rom_medium")

    var opendOnAppLaunch = false
    
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
    @IBOutlet weak var cancelButton: NSButton!

    override func awakeFromNib()
    {
        // Determine if the dialog was opened on start up. This happens when
        // the emulator can't run because of missing Roms.
        opendOnAppLaunch = !c64.isRunnable()

        // If the dialog was opend on start up, we only want to show a Quit
        // button to the user.
        cancelButton.isHidden = opendOnAppLaunch
        
        // The user might delete Roms. Hence, we better pause emulation while
        // the dialog is open.
        c64.halt()
        
        refresh()
    }

    override func refresh()
    {
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
        basicRomPath.stringValue = basicURL.path
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
        kernalRomPath.stringValue = kernalURL.path
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
        characterRomPath.stringValue = characterURL.path
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
        vc1541RomPath.stringValue = vc1541URL.path
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
        parent.basicRomURL = URL(fileURLWithPath: "/")
        parent.c64.halt()
        parent.c64.mem.deleteBasicRom()
        refresh()
    }
    
    @IBAction func deleteCharacterRom(_ sender: Any!)
    {
        parent.charRomURL = URL(fileURLWithPath: "/")
        parent.c64.halt()
        parent.c64.mem.deleteCharacterRom()
        refresh()
    }
    
    @IBAction func deleteKernalRom(_ sender: Any!)
    {
        parent.kernalRomURL = URL(fileURLWithPath: "/")
        parent.c64.halt()
        parent.c64.mem.deleteKernalRom()
        refresh()
    }
    
    @IBAction func deleteVC1541Rom(_ sender: Any!)
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
    
    @IBAction override func cancelAction(_ sender: Any!) {
        
        track()
        
        parent.loadRomUserDefaults()
        hideSheet()
    }
    
    @IBAction func okAction(_ sender: Any!)
    {
        parent.saveRomUserDefaults()
        hideSheet()
        
        if (c64.isRunnable()) {
            c64.run()
        } else {
           NSApp.terminate(self)
        }
    }
    
}

