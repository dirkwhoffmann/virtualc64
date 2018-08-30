//
//  HardwarePrefsController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 22.02.18.
//

import Foundation

class HardwarePrefsController : UserDialogController {

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
    
    func update() {
        
        // VIC
        let vic = c64.vic.chipModel()
        vicModel.selectItem(withTag: vic)
        switch (UInt32(vic)) {
            
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
            /*
            systemText.stringValue = "NTSC "
            systemText2.stringValue = "1.023 MHz"
            systemText3.stringValue = "63 raster cycles"
            */
            
        default:
            assert(false)
        }
        grayDotBug.state = c64.vic.emulateGrayDotBug() ? .on : .off
        grayDotBug.isEnabled = c64.vic.hasGrayDotBug()
        
        // CIA
        assert(c64.cia1.chipModel() == c64.cia2.chipModel());
        assert(c64.cia1.timerBBug() == c64.cia2.timerBBug());
        ciaModel.selectItem(withTag: c64.cia1.chipModel())
        timerBBug.state = c64.cia1.timerBBug() ? .on : .off
        
        // Audio
        let sidModel = c64.sid.chipModel()
        sidChipModel.selectItem(withTag: sidModel)
        sidFilter.state = c64.sid.audioFilter() ? .on : .off
        sidEngine.selectItem(withTag: (c64.sid.reSID() ? 1 : 0))
        sidSamplingMethod.isEnabled = c64.sid.reSID()
        sidSamplingMethod.selectItem(withTag: c64.sid.samplingMethod())
        let sampleFast = sidSamplingMethod.item(at: 0)
        sampleFast?.isHidden = (sidModel == Int(MOS_8580.rawValue))
        
        // Logic board
        glueLogic.selectItem(withTag: c64.vic.glueLogic())
        ramInitPattern.selectItem(withTag: c64.mem.ramInitPattern())
    }
    
    @IBAction func vicChipModelAction(_ sender: NSMenuItem!) {
        
        c64.vic.setChipModel(sender.tag)
        update()
    }
    
    @IBAction func vicGrayDotBugAction(_ sender: NSButton!) {
        
        c64.vic.setEmulateGrayDotBug(sender.state == .on)
        update()
    }
    
    @IBAction func ciaChipModelAction(_ sender: NSMenuItem!) {
        
        c64.cia1.setChipModel(sender.tag)
        c64.cia2.setChipModel(sender.tag)
        update()
    }
    
    @IBAction func ciaTimerBBugAction(_ sender: NSButton!) {
        
        c64.cia1.setTimerBBug(sender.state == .on)
        c64.cia2.setTimerBBug(sender.state == .on)
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
        c64.sid.setChipModel(sender.selectedTag())
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
    
    @IBAction func factorySettingsAction(_ sender: Any!) {
        
        c64.suspend()

        // VICII
        c64.vic.setChipModel(Int(PAL_8565.rawValue))
        c64.vic.setEmulateGrayDotBug(true)
        
        // CIA
        c64.cia1.setChipModel(Int(MOS_6526_OLD.rawValue))
        c64.cia2.setChipModel(Int(MOS_6526_OLD.rawValue))
        c64.cia1.setTimerBBug(true);
        c64.cia2.setTimerBBug(true);

        // SID
        c64.sid.setReSID(true)
        c64.sid.setChipModel(0)
        c64.sid.setAudioFilter(true)
        c64.sid.setSamplingMethod(0)

        // Logic board
        c64.vic.setGlueLogic(Int(GLUE_DISCRETE.rawValue))
        c64.mem.setRamInitPattern(Int(INIT_PATTERN_C64.rawValue))

        c64.resume()
        update()
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        parent.saveHardwareUserDefaults()
        hideSheet()
    }
}
