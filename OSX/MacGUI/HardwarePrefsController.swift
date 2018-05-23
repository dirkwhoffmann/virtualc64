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
    
    // Audio
    @IBOutlet weak var SIDChipModel: NSPopUpButton!
    @IBOutlet weak var SIDFilter: NSButton!
    @IBOutlet weak var SIDEngine: NSPopUpButton!
    @IBOutlet weak var SIDSamplingMethod: NSPopUpButton!

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
        
        // System
        if c64.isPAL() {
            machineType.selectItem(withTag: 0)
            flag.image = NSImage(named: NSImage.Name(rawValue: "flag_eu.png"))
            systemText.stringValue = "PAL"
            systemText2.stringValue = "0.985 MHz"
        } else {
            machineType.selectItem(withTag: 1)
            flag.image = NSImage(named: NSImage.Name(rawValue: "flag_usa.png"))
            systemText.stringValue = "NTSC"
            systemText2.stringValue = "1.023 MHz"
        }
        
        // Audio
        SIDChipModel.selectItem(withTag: c64.chipModel())
        SIDFilter.state = c64.audioFilter() ? .on : .off
        SIDEngine.selectItem(withTag: (c64.reSID() ? 1 : 0))
        SIDSamplingMethod.isEnabled = c64.reSID()
        SIDSamplingMethod.selectItem(withTag: c64.samplingMethod())
        
        // VC1541
        warpLoad.state = c64.warpLoad() ? .on : .off
        driveNoise.state = c64.vc1541.soundMessagesEnabled() ? .on : .off
        
        // Mouse
        let model = c64.mouseModel()
        mouseModel.selectItem(withTag: model)
        mouseInfo.isHidden = (model == Int(MOUSE1350.rawValue))
    }
    
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
    
    @IBAction func SIDFilterAction(_ sender: Any!) {
    
        let sender = sender as! NSButton
        c64.setAudioFilter(sender.state == .on)
        update()
    }
    
    @IBAction func SIDEngineAction(_ sender: Any!) {
    
        let sender = sender as! NSPopUpButton
        c64.setReSID(sender.selectedTag() == 1)
        // parent.waveformView.initAutoScaler()
        update()
    }
    
    @IBAction func SIDSamplingMethodAction(_ sender: Any!) {
    
        let sender = sender as! NSPopUpButton
        c64.setSamplingMethod(sender.selectedTag())
        update()
    }
    
    @IBAction func SIDChipModelAction(_ sender: Any!) {
    
        let sender = sender as! NSPopUpButton
        c64.setChipModel(sender.selectedTag())
        update()
    }
    
    @IBAction func warpLoadAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        c64.setWarpLoad(sender.state == .on)
        update()
    }
    
    @IBAction func driveNoiseAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        c64.vc1541.setSendSoundMessages(sender.state == .on)
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
        
        // VIC
        c64.setNTSC(false)
        
        // SID
        c64.setReSID(true)
        c64.setChipModel(1)
        c64.setAudioFilter(false)
        c64.setSamplingMethod(0)

        // VC1541
        c64.setWarpLoad(true)
        c64.vc1541.setSendSoundMessages(true)
        
        // Mouse
        c64.setMouseModel(0)

        update()
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        parent.saveHardwareUserDefaults()
        hideSheet()
    }
}
