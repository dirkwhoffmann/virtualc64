// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension ConfigurationController {
    
    func refreshAudioTab() {
                                
        // Engine
        audEngine.selectItem(withTag: config.sidEngine)
        audFilter.state = config.sidFilter ? .on : .off
        audSampling.isEnabled = config.sidEngine == vc64.SIDEngine.RESID.rawValue
        audSampling.selectItem(withTag: config.sidSampling)

        // In
        audVol0.integerValue = config.vol0
        audVol1.integerValue = config.vol1
        audVol2.integerValue = config.vol2
        audVol3.integerValue = config.vol3
        audPan0.integerValue = config.pan0
        audPan1.integerValue = config.pan1
        audPan2.integerValue = config.pan2
        audPan3.integerValue = config.pan3
 
        // Out
        audVolL.integerValue = config.volL
        audVolR.integerValue = config.volR

        // Drives
        audStepVolume.integerValue = config.stepVolume
        audInsertVolume.integerValue = config.insertVolume
        audEjectVolume.integerValue = config.ejectVolume
        audDrive8Pan.integerValue = config.drive8Pan
        audDrive9Pan.integerValue = config.drive9Pan

        // Buttons
        audPowerButton.isHidden = !bootable
    }

    @IBAction func audVol0Action(_ sender: NSSlider!) {

        config.vol0 = sender.integerValue
    }
    
    @IBAction func audVol1Action(_ sender: NSSlider!) {

        config.vol1 = sender.integerValue
    }
    
    @IBAction func audVol2Action(_ sender: NSSlider!) {

        config.vol2 = sender.integerValue
    }
    
    @IBAction func audVol3Action(_ sender: NSSlider!) {

        config.vol3 = sender.integerValue
    }
    
    @IBAction func audPan0Action(_ sender: NSSlider!) {
        
        config.pan0 = sender.integerValue
    }
    
    @IBAction func audPan1Action(_ sender: NSSlider!) {
        
        config.pan1 = sender.integerValue
    }
    
    @IBAction func audPan2Action(_ sender: NSSlider!) {
        
        config.pan2 = sender.integerValue
    }
    
    @IBAction func audPan3Action(_ sender: NSSlider!) {
        
        config.pan3 = sender.integerValue
    }

    @IBAction func audVolLAction(_ sender: NSSlider!) {
        
        config.volL = sender.integerValue
    }

    @IBAction func audVolRAction(_ sender: NSSlider!) {
        
        config.volR = sender.integerValue
    }

    @IBAction func audStepVolumeAction(_ sender: NSSlider!) {

        config.stepVolume = sender.integerValue
    }

    @IBAction func audInsertVolumeAction(_ sender: NSSlider!) {

        config.insertVolume = sender.integerValue
    }

    @IBAction func audEjectVolumeAction(_ sender: NSSlider!) {

        config.ejectVolume = sender.integerValue
    }
    
    @IBAction func audDrive8PanAction(_ sender: NSSlider!) {
        
        config.drive8Pan = sender.integerValue
    }

    @IBAction func audDrive9PanAction(_ sender: NSSlider!) {
        
        config.drive9Pan = sender.integerValue
    }

    @IBAction func audEngineAction(_ sender: NSPopUpButton!) {
        
        config.sidEngine = sender.selectedTag()
    }

    @IBAction func audSamplingAction(_ sender: NSPopUpButton!) {
        
        config.sidSampling = sender.selectedTag()
    }
    
    @IBAction func audFilterAction(_ sender: NSButton!) {
        
        config.sidFilter = sender.state == .on
    }
    
    @IBAction func audPresetAction(_ sender: NSPopUpButton!) {
              
        if let emu = emu {
            
            emu.suspend()

            // Revert to standard settings
            EmulatorProxy.defaults.removeAudioUserDefaults()

            // Update the configuration
            config.applyAudioUserDefaults()

            // Override some options
            switch sender.selectedTag() {

            case 0: // Mono
                config.pan0 = 0
                config.pan1 = 0
                config.pan2 = 0
                config.pan3 = 0
                config.drive8Pan = 0
                config.drive9Pan = 0

            case 1: // Stereo
                config.pan0 = 100
                config.pan1 = 300
                config.pan2 = 300
                config.pan3 = 100
                config.drive8Pan = 100
                config.drive9Pan = 300

            default:
                fatalError()
            }

            emu.resume()
        }
    }
    
    @IBAction func audDefaultsAction(_ sender: NSButton!) {
        
        config.saveAudioUserDefaults()
    }
}
