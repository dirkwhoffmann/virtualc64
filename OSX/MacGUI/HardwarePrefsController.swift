//
//  HardwarePrefsController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 22.02.18.
//

import Foundation

class HardwarePrefsController : UserDialogController {

    // VIC
    @IBOutlet weak var machineType: NSPopUpButton!
    @IBOutlet weak var flag: NSImageView!
    @IBOutlet weak var systemText: NSTextField!
    @IBOutlet weak var systemText2: NSTextField!
    @IBOutlet weak var systemText3: NSTextField!

    // Audio
    @IBOutlet weak var sidChipModel: NSPopUpButton!
    @IBOutlet weak var sidFilter: NSButton!
    @IBOutlet weak var sidEngine: NSPopUpButton!
    @IBOutlet weak var sidSamplingMethod: NSPopUpButton!

    // VC1541
    @IBOutlet weak var warpLoad: NSButton!
    @IBOutlet weak var driveNoise: NSButton!
    
    // Mouse
    @IBOutlet weak var mouseModel: NSPopUpButton!
    @IBOutlet weak var mouseInfo: NSTextField!

    override func awakeFromNib() {
        update()
    }
    
    func update() {
        
        let vicModel = c64.vic.chipModel()
        machineType.selectItem(withTag: vicModel)
        switch (UInt32(vicModel)) {
            
        case PAL_6569_R1.rawValue,
             PAL_6569_R3.rawValue,
             PAL_8565.rawValue:
            
            flag.image = NSImage(named: NSImage.Name(rawValue: "flag_eu"))
            systemText.stringValue = "PAL"
            systemText2.stringValue = "0.985 MHz"
            systemText3.stringValue = "65 raster cycles"

        case NTSC_6567_R56A.rawValue:
            
            flag.image = NSImage(named: NSImage.Name(rawValue: "flag_usa"))
            systemText.stringValue = "NTSC"
            systemText2.stringValue = "1.023 MHz"
            systemText3.stringValue = "64 raster cycles"
            
        case NTSC_6567.rawValue,
             NTSC_8562.rawValue:
            
            flag.image = NSImage(named: NSImage.Name(rawValue: "flag_usa"))
            systemText.stringValue = "NTSC "
            systemText2.stringValue = "1.023 MHz"
            systemText3.stringValue = "63 raster cycles"
            
        default:
            assert(false)
        }

        // Audio
        let sidModel = c64.sid.chipModel()
        sidChipModel.selectItem(withTag: sidModel)
        sidFilter.state = c64.sid.audioFilter() ? .on : .off
        sidEngine.selectItem(withTag: (c64.sid.reSID() ? 1 : 0))
        sidSamplingMethod.isEnabled = c64.sid.reSID()
        sidSamplingMethod.selectItem(withTag: c64.sid.samplingMethod())
        let sampleFast = sidSamplingMethod.item(at: 0)
        sampleFast?.isHidden = (sidModel == Int(MOS_8580.rawValue))
            
        // VC1541
        warpLoad.state = c64.warpLoad() ? .on : .off
        driveNoise.state = c64.drive1.sendSoundMessages() ? .on : .off
        
        // Mouse
        let model = c64.mouseModel()
        mouseModel.selectItem(withTag: model)
        mouseInfo.isHidden = (model == Int(MOUSE1350.rawValue))
    }
    
    /*
    @IBAction func setPalAction(_ sender: Any!) {
    
        track()
        c64.setPAL()
        update()
    }
    
    @IBAction func setNtscAction(_ sender: Any!) {
    
        track()
        c64.setNTSC()
        update()
    }
    */
    
    @IBAction func vicChipModelAction(_ sender: NSMenuItem!) {
        
        c64.vic.setChipModel(sender.tag)
        update()
    }
    
    @IBAction func SIDFilterAction(_ sender: Any!) {
    
        let sender = sender as! NSButton
        c64.sid.setAudioFilter(sender.state == .on)
        update()
    }
    
    @IBAction func SIDEngineAction(_ sender: Any!) {
    
        let sender = sender as! NSPopUpButton
        c64.sid.setReSID(sender.selectedTag() == 1)
        update()
    }
    
    @IBAction func SIDSamplingMethodAction(_ sender: Any!) {
    
        let sender = sender as! NSPopUpButton
        c64.sid.setSamplingMethod(sender.selectedTag())
        update()
    }
    
    @IBAction func SIDChipModelAction(_ sender: Any!) {
    
        let sender = sender as! NSPopUpButton
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
    
    @IBAction func warpLoadAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        c64.setWarpLoad(sender.state == .on)
        update()
    }
    
    @IBAction func driveNoiseAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        c64.drive1.setSendSoundMessages(sender.state == .on)
        c64.drive2.setSendSoundMessages(sender.state == .on)
        update()
    }
    
    @IBAction func mouseModelAction(_ sender: Any!) {
        
        let sender = sender as! NSPopUpButton
        c64.setMouseModel(sender.selectedTag())
        update()
    }
    
    @IBAction override func cancelAction(_ sender: Any!) {
        
        track()
        parent.loadHardwareUserDefaults()
        hideSheet()
    }
    
    @IBAction func factorySettingsAction(_ sender: Any!) {
        
        c64.suspend()

        // VIC
        c64.vic.setChipModel(Int(PAL_8565.rawValue))
        
        // SID
        c64.sid.setReSID(true)
        c64.sid.setChipModel(0)
        c64.sid.setAudioFilter(true)
        c64.sid.setSamplingMethod(0)

        // VC1541
        c64.setWarpLoad(true)
        c64.drive1.setSendSoundMessages(true)
        c64.drive2.setSendSoundMessages(true)

        // Mouse
        c64.setMouseModel(0)

        c64.resume()
        update()
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        parent.saveHardwareUserDefaults()
        hideSheet()
    }
}
