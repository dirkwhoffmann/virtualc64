// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension ConfigurationController {
    
    func refreshAudioTab() {
                
        track()
        
        /*
        let sid1 = config.sidEnable1
        let sid2 = config.sidEnable2
        let sid3 = config.sidEnable3
        */
        
        // Engine
        audEngine.selectItem(withTag: config.sidEngine)
        audFilter.state = config.sidFilter ? .on : .off
        audSampling.isEnabled = config.sidEngine == SIDEngine.ENGINE_RESID.rawValue
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
        /*
        audVol1.isEnabled = sid1
        audVol2.isEnabled = sid2
        audVol3.isEnabled = sid3
        audPan1.isEnabled = sid1
        audPan2.isEnabled = sid2
        audPan3.isEnabled = sid3
        */
        
        // Out
        audVolL.integerValue = config.volL
        audVolR.integerValue = config.volR

        // Buttons
        audPowerButton.isHidden = !bootable
    }

    @IBAction func audVol0Action(_ sender: NSSlider!) {

        track()
        config.vol0 = sender.integerValue
        refresh()
    }
    
    @IBAction func audVol1Action(_ sender: NSSlider!) {

        track()
        config.vol1 = sender.integerValue
        refresh()
    }
    
    @IBAction func audVol2Action(_ sender: NSSlider!) {

        track()
        config.vol2 = sender.integerValue
        refresh()
    }
    
    @IBAction func audVol3Action(_ sender: NSSlider!) {

        track()
        config.vol3 = sender.integerValue
        refresh()
    }
    
    @IBAction func audPan0Action(_ sender: NSSlider!) {
        
        track()
        config.pan0 = sender.integerValue
        refresh()
    }
    
    @IBAction func audPan1Action(_ sender: NSSlider!) {
        
        track()
        config.pan1 = sender.integerValue
        refresh()
    }
    
    @IBAction func audPan2Action(_ sender: NSSlider!) {
        
        track()
        config.pan2 = sender.integerValue
        refresh()
    }
    
    @IBAction func audPan3Action(_ sender: NSSlider!) {
        
        track()
        config.pan3 = sender.integerValue
        refresh()
    }

    @IBAction func audVolLAction(_ sender: NSSlider!) {
        
        track()
        config.volL = sender.integerValue
        refresh()
    }

    @IBAction func audVolRAction(_ sender: NSSlider!) {
        
        config.volR = sender.integerValue
        refresh()
    }

    @IBAction func audEngineAction(_ sender: NSPopUpButton!) {
        
        config.sidEngine = sender.selectedTag()
        refresh()
    }

    @IBAction func audSamplingAction(_ sender: NSPopUpButton!) {
        
        config.sidSampling = sender.selectedTag()
        refresh()
    }
    
    @IBAction func audFilterAction(_ sender: NSButton!) {
        
        config.sidFilter = sender.state == .on
        refresh()
    }
    
    @IBAction func audPresetAction(_ sender: NSPopUpButton!) {
        
        track()
        
        switch sender.selectedTag() {
        case 0: config.loadAudioDefaults(AudioDefaults.mono)
        case 1: config.loadAudioDefaults(AudioDefaults.stereo)
        default: fatalError()
        }
        refresh()
    }
    
    @IBAction func audDefaultsAction(_ sender: NSButton!) {
        
        track()
        config.saveAudioUserDefaults()
    }
}
