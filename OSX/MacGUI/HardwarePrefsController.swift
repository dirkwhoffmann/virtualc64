//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

class HardwarePrefsController : UserDialogController {

    // C64
    // @IBOutlet weak var profile: NSPopUpButton!
    @IBOutlet weak var infoText: NSTextField!
    
    // VIC
    @IBOutlet weak var vicModel: NSPopUpButton!
    @IBOutlet weak var vicIcon: NSImageView!
    @IBOutlet weak var vicDescription: NSTextField!
    @IBOutlet weak var grayDotBug: NSButton!

    // CIA
    @IBOutlet weak var ciaModel: NSPopUpButton!
    @IBOutlet weak var timerBBug: NSButton!
    
    // SID
    @IBOutlet weak var sidChipModel: NSPopUpButton!
    @IBOutlet weak var sidFilter: NSButton!
    @IBOutlet weak var sidEngine: NSPopUpButton!
    @IBOutlet weak var sidSamplingMethod: NSPopUpButton!

    // Board logic
    @IBOutlet weak var glueLogic: NSPopUpButton!
    @IBOutlet weak var ramInitPattern: NSPopUpButton!

    override func awakeFromNib() {
        update()
    }
    
    func updateInfoText() {
        
        var descr = "???"
        
        switch UInt32(c64.model()) {
        case C64_PAL.rawValue:
            descr = "matches a C64 with brown casing (breadbox) and PAL video ouput"

        case C64_II_PAL.rawValue:
            descr = "matches a C64 II (white casing) with PAL video ouput"

        case C64_OLD_PAL.rawValue:
            descr = "matches an early C64 with brown casing (breadbox) and PAL video output"
            
        case C64_NTSC.rawValue:
            descr = "matches a C64 with brown casing (breadbox) and NTSC video ouput"
            
        case C64_II_NTSC.rawValue:
            descr = "matches a C64 II (white casing) with NTSC video ouput"
            
        case C64_OLD_NTSC.rawValue:
            descr = "matches an early C64 with brown casing (breadbox) and NTSC video output"
            
        default:
            descr = "is a custom configuration. It matches no known C64 model"
        }
        infoText.stringValue = "This configuration \(descr)."
    }
    
    func update() {
        
        // Hardware configuration
        updateInfoText()
        
        // VIC
        let model = c64.vic.model()
        vicModel.selectItem(withTag: model)
        
        switch (UInt32(model)) {
            
        case PAL_6569_R1.rawValue,
             PAL_6569_R3.rawValue,
             PAL_8565.rawValue:
            
            vicIcon.image = NSImage(named: NSImage.Name(rawValue: "pref_vicii_pal"))
            vicDescription.stringValue = "PAL 0.985 MHz 65 cycles"
            
        case NTSC_6567_R56A.rawValue:
            
            vicIcon.image = NSImage(named: NSImage.Name(rawValue: "pref_vicii_ntsc"))
            vicDescription.stringValue = "NTSC 1.023 MHz 64 cycles"
            
        case NTSC_6567.rawValue,
             NTSC_8562.rawValue:
            
            vicIcon.image = NSImage(named: NSImage.Name(rawValue: "pref_vicii_ntsc"))
            vicDescription.stringValue = "NTSC 1.023 MHz 63 cycles"
            
        default:
            assert(false)
        }
        grayDotBug.state = c64.vic.emulateGrayDotBug() ? .on : .off
        
        // CIA
        assert(c64.cia1.model() == c64.cia2.model());
        assert(c64.cia1.emulateTimerBBug() == c64.cia2.emulateTimerBBug());
        ciaModel.selectItem(withTag: c64.cia1.model())
        timerBBug.state = c64.cia1.emulateTimerBBug() ? .on : .off

        // Audio
        let sidModel = c64.sid.model()
        sidChipModel.selectItem(withTag: sidModel)
        sidFilter.state = c64.sid.audioFilter() ? .on : .off
        sidEngine.selectItem(withTag: (c64.sid.reSID() ? 1 : 0))
        sidSamplingMethod.isEnabled = c64.sid.reSID()
        sidSamplingMethod.selectItem(withTag: c64.sid.samplingMethod())
        // let sampleFast = sidSamplingMethod.item(at: 0)
        // sampleFast?.isHidden = (sidModel == Int(MOS_8580.rawValue))
        
        // Logic board
        glueLogic.selectItem(withTag: c64.vic.glueLogic())
        ramInitPattern.selectItem(withTag: c64.mem.ramInitPattern())
    }
    
    @IBAction func profileAction(_ sender: NSMenuItem!) {
        
        if sender.tag != C64_CUSTOM.rawValue {
            c64.setModel(sender.tag)
        }
        update()
    }
    
    @IBAction func vicChipModelAction(_ sender: NSMenuItem!) {
        
        c64.vic.setModel(sender.tag)
        update()
    }
    
    @IBAction func vicGrayDotBugAction(_ sender: NSButton!) {
        
        c64.vic.setEmulateGrayDotBug(sender.state == .on)
        update()
    }
    
    @IBAction func ciaChipModelAction(_ sender: NSMenuItem!) {
        
        c64.cia1.setModel(sender.tag)
        c64.cia2.setModel(sender.tag)
        update()
    }
    
    @IBAction func ciaTimerBBugAction(_ sender: NSButton!) {
        
        c64.cia1.setEmulateTimerBBug(sender.state == .on)
        c64.cia2.setEmulateTimerBBug(sender.state == .on)
        update()
    }
    
    @IBAction func SIDFilterAction(_ sender: NSButton!) {
    
        c64.sid.setAudioFilter(sender.state == .on)
        update()
    }
    
    @IBAction func SIDEngineAction(_ sender: NSPopUpButton!) {
    
        c64.sid.setReSID(sender.selectedTag() == 1)
        update()
    }
    
    @IBAction func SIDSamplingMethodAction(_ sender: NSPopUpButton!) {
    
        c64.sid.setSamplingMethod(sender.selectedTag())
        update()
    }
    
    @IBAction func SIDChipModelAction(_ sender: NSPopUpButton!) {
    
        let model = UInt32(sender.selectedTag())
        let method = UInt32(c64.sid.samplingMethod())
        
        track("Model = \(model) method = \(method)")
        if (model == MOS_8580.rawValue && method == SID_SAMPLE_FAST.rawValue) {
            parent.showResidSamplingMethodAlert()
            c64.sid.setSamplingMethod(Int(SID_SAMPLE_INTERPOLATE.rawValue))
        }
        
        c64.sid.setModel(sender.selectedTag())
        update()
    }
    
    @IBAction func glueLogicAction(_ sender: NSMenuItem!) {
        
        c64.vic.setGlueLogic(sender.tag)
        update()
    }
    
    @IBAction func ramInitPatternAction(_ sender: NSMenuItem!) {
        
        c64.mem.setRamInitPattern(sender.tag)
        update()
    }
            
    @IBAction override func cancelAction(_ sender: Any!) {
        
        track()
        parent.loadHardwareUserDefaults()
        hideSheet()
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        parent.saveHardwareUserDefaults()
        hideSheet()
    }
}
