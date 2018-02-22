//
//  HardwarePrefsController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 22.02.18.
//

import Foundation

class HardwarePrefsController : UserDialogController {

    // System
    @IBOutlet weak var machineType: NSPopUpButton!
    @IBOutlet weak var flag: NSImageView!
    @IBOutlet weak var systemText: NSTextField!
    @IBOutlet weak var systemText2: NSTextField!
    
    // VC1541
    @IBOutlet weak var warpLoad: NSButton!
    @IBOutlet weak var warpText: NSTextField!
    @IBOutlet weak var driveNoise: NSButton!
    
    // Audio
    @IBOutlet weak var SIDFilter: NSButton!
    @IBOutlet weak var SIDUseReSID: NSButton!
    @IBOutlet weak var SIDChipModel: NSPopUpButton!
    @IBOutlet weak var SIDSamplingMethod: NSPopUpButton!

    override func awakeFromNib() {
        update()
    }
    
    func update() {
        
        // System
        if c64.isPAL() {
            machineType.selectItem(withTag: 0)
            flag.image = NSImage(named: NSImage.Name(rawValue: "flag_eu.png"))
            systemText.stringValue = "PAL machine"
            systemText2.stringValue = "63 cycles per rasterline"
        } else {
            machineType.selectItem(withTag: 1)
            flag.image = NSImage(named: NSImage.Name(rawValue: "flag_usa.png"))
            systemText.stringValue = "NTSC machine"
            systemText2.stringValue = "65 cycles per rasterline"
        }
        
        // VC1541
        warpLoad.state = c64.warpLoad() ? .on : .off
        driveNoise.state = c64.vc1541.soundMessagesEnabled() ? .on : .off
        
        // Audio
        SIDUseReSID.state = c64.reSID() ? .on : .off
        SIDFilter.isEnabled = c64.reSID()
        SIDFilter.state = c64.audioFilter() ? .on : .off
        SIDChipModel.isEnabled = c64.reSID()
        SIDChipModel.selectItem(withTag: Int(c64.chipModel()))
        SIDSamplingMethod.isEnabled = c64.reSID()
        SIDSamplingMethod.selectItem(withTag: Int(c64.samplingMethod()))
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
    
    @IBAction func SIDFilterAction(_ sender: Any!) {
    
        let sender = sender as! NSButton
        c64.setAudioFilter(sender.state == .on)
        update()
    }
    
    @IBAction func SIDReSIDAction(_ sender: Any!) {
    
        let sender = sender as! NSButton
        c64.setReSID(sender.state == .on)
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
    
    @IBAction override func cancelAction(_ sender: Any!) {
        
        parent.loadHardwareUserDefaults()
        hideSheet()
    }
    
    @IBAction func factorySettingsAction(_ sender: Any!) {
        
        parent.restoreHardwareUserDefaults()
        update()
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        parent.saveHardwareUserDefaults()
        hideSheet()
    }
}
