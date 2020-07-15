// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension ConfigurationController {
    
    func awakeVideoPrefsFromNib() {
        
        // Check for available upscalers
        if let kernels = myController?.renderer.upscalerGallery {
            
            for i in 0 ... kernels.count - 1 {
                vidUpscalerPopup.menu!.item(withTag: i)?.isEnabled = (kernels[i] != nil)
            }
            myController?.renderer.buildDotMasks()
            updatePalettePreviewImages()
        }
    }
    
    func refreshVideoTab() {
        
        // guard let doc = myDocument else { return }
        guard let con = myController else { return }
        guard let renderer = con.renderer else { return }
        guard let c64 = proxy else { return }

        track()
        
        // Video
        vidUpscalerPopup.selectItem(withTag: renderer.upscaler)
        vidPalettePopup.selectItem(withTag: c64.vic.videoPalette())
        vidBrightnessSlider.doubleValue = c64.vic.brightness()
        vidContrastSlider.doubleValue = c64.vic.contrast()
        vidSaturationSlider.doubleValue = c64.vic.saturation()
        
        // Effects
        let shaderOptions = renderer.shaderOptions
        vidBlurPopUp.selectItem(withTag: Int(shaderOptions.blur))
        vidBlurRadiusSlider.floatValue = shaderOptions.blurRadius
        vidBlurRadiusSlider.isEnabled = shaderOptions.blur > 0
        
        vidBloomPopup.selectItem(withTag: Int(shaderOptions.bloom))
        vidBloomRadiusRSlider.floatValue = shaderOptions.bloomRadiusR
        vidBloomRadiusRSlider.isEnabled = shaderOptions.bloom > 0
        vidBloomRadiusGSlider.floatValue = shaderOptions.bloomRadiusG
        vidBloomRadiusGSlider.isEnabled = shaderOptions.bloom > 1
        vidBloomRadiusBSlider.floatValue = shaderOptions.bloomRadiusB
        vidBloomRadiusBSlider.isEnabled = shaderOptions.bloom > 1
        vidBloomBrightnessSlider.floatValue = shaderOptions.bloomBrightness
        vidBloomBrightnessSlider.isEnabled = shaderOptions.bloom > 0
        vidBloomWeightSlider.floatValue = shaderOptions.bloomWeight
        vidBloomWeightSlider.isEnabled = shaderOptions.bloom > 0
        
        vidDotMaskPopUp.selectItem(withTag: Int(shaderOptions.dotMask))
        for i in 0 ... 4 {
            vidDotMaskPopUp.item(at: i)?.image = renderer.dotmaskImages[i]
        }
        vidDotMaskBrightnessSlider.floatValue = shaderOptions.dotMaskBrightness
        vidDotMaskBrightnessSlider.isEnabled = shaderOptions.dotMask > 0
        
        vidScanlinesPopUp.selectItem(withTag: Int(shaderOptions.scanlines))
        vidScanlineBrightnessSlider.floatValue = shaderOptions.scanlineBrightness
        vidScanlineBrightnessSlider.isEnabled = shaderOptions.scanlines > 0
        vidScanlineWeightSlider.floatValue = shaderOptions.scanlineWeight
        vidScanlineWeightSlider.isEnabled = shaderOptions.scanlines == 2
        
        vidMisalignmentPopUp.selectItem(withTag: Int(shaderOptions.disalignment))
        vidMisalignmentXSlider.floatValue = shaderOptions.disalignmentH
        vidMisalignmentXSlider.isEnabled = shaderOptions.disalignment > 0
        vidMisalignmentYSlider.floatValue = shaderOptions.disalignmentV
        vidMisalignmentYSlider.isEnabled = shaderOptions.disalignment > 0

        // Geometry
        vidAspectRatioButton.state = renderer.keepAspectRatio ? .on : .off
        vidEyeXSlider.floatValue = renderer.shiftX.current
        vidEyeYSlider.floatValue = renderer.shiftY.current
        vidEyeZSlider.floatValue = renderer.shiftZ.current
        
        // Power button
        romPowerButton.isHidden = !bootable
    }
    
    func updatePalettePreviewImages() {
        
        guard let c64 = proxy else { return }
        
        // Create image representation in memory
        let size = CGSize.init(width: 16, height: 1)
        let cap = Int(size.width) * Int(size.height)
        let mask = calloc(cap, MemoryLayout<UInt32>.size)!
        let ptr = mask.bindMemory(to: UInt32.self, capacity: cap)
        
        // For all palettes ...
        for palette: Int in 0 ... 5 {
            
            // Create image data
            for n in 0 ... 15 {
                let p = VICPalette(rawValue: UInt32(palette))
                let rgba = c64.vic.rgbaColor(n, palette: p)
                ptr[n] = rgba
            }
            
            // Create image
            let image = NSImage.make(data: mask, rect: size)
            let resizedImage = image?.resizeImageSharp(width: 64, height: 12)
            vidPalettePopup.item(at: palette)?.image = resizedImage
        }
    }

    //
    // Action methods (Colors)
    //
    
    @IBAction func vidPaletteAction(_ sender: NSPopUpButton!) {
        
        proxy?.vic.setVideoPalette(sender.selectedTag())
        updatePalettePreviewImages()
        refresh()
    }
    
    @IBAction func vidBrightnessAction(_ sender: NSSlider!) {
        
        proxy?.vic.setBrightness(sender.doubleValue)
        updatePalettePreviewImages()
        refresh()
    }
    
    @IBAction func vidContrastAction(_ sender: NSSlider!) {
        
        proxy?.vic.setContrast(sender.doubleValue)
        updatePalettePreviewImages()
        refresh()
    }
    
    @IBAction func vidSaturationAction(_ sender: NSSlider!) {
        
        proxy?.vic.setSaturation(sender.doubleValue)
        updatePalettePreviewImages()
        refresh()
    }

    //
    // Action methods (Effects)
    //
    
    @IBAction func vidUpscalerAction(_ sender: NSPopUpButton!) {

        if let renderer = myController?.renderer {
            track("\(sender.selectedTag())")
            renderer.upscaler = sender.selectedTag()
            refresh()
        }
    }
    
    @IBAction func vidBlurAction(_ sender: NSPopUpButton!) {
        
        if let renderer = myController?.renderer {
            track("\(sender.selectedTag())")
            renderer.shaderOptions.blur = Int32(sender.selectedTag())
            refresh()
        }
    }
    
    @IBAction func vidBlurRadiusAction(_ sender: NSSlider!) {
        
        if let renderer = myController?.renderer {
            track("\(sender.floatValue)")
            renderer.shaderOptions.blurRadius = sender.floatValue
            refresh()
        }
    }
    
    @IBAction func vidBloomAction(_ sender: NSPopUpButton!) {
        
        if let renderer = myController?.renderer {

            renderer.shaderOptions.bloom = Int32(sender.selectedTag())
            vidBloomRadiusRAction(vidBloomRadiusRSlider)
        }
    }
    
    @IBAction func vidBloomRadiusRAction(_ sender: NSSlider!) {
        
        if let renderer = myController?.renderer {
            
            renderer.shaderOptions.bloomRadiusR = sender.floatValue
            
            if vidBloomPopup.selectedTag() == 1 {
                
                // Use this value for the other channels, too
                renderer.shaderOptions.bloomRadiusG = sender.floatValue
                renderer.shaderOptions.bloomRadiusB = sender.floatValue
            }
            refresh()
        }
    }

    @IBAction func vidBloomRadiusGAction(_ sender: NSSlider!) {
        
        if let renderer = myController?.renderer {
            
            renderer.shaderOptions.bloomRadiusG = sender.floatValue
            
            if vidBloomPopup.selectedTag() == 1 {
                
                // Use this value for the other channels, too
                renderer.shaderOptions.bloomRadiusR = sender.floatValue
                renderer.shaderOptions.bloomRadiusB = sender.floatValue
            }
            refresh()
        }
    }
    
    @IBAction func vidBloomRadiusBAction(_ sender: NSSlider!) {
        
        if let renderer = myController?.renderer {
            
            renderer.shaderOptions.bloomRadiusB = sender.floatValue
            
            if vidBloomPopup.selectedTag() == 1 {
                
                // Use this value for the other channels, too
                renderer.shaderOptions.bloomRadiusR = sender.floatValue
                renderer.shaderOptions.bloomRadiusG = sender.floatValue
            }
            refresh()
        }
    }
    
    @IBAction func vidBloomBrightnessAction(_ sender: NSSlider!) {
        
        if let renderer = myController?.renderer {
            renderer.shaderOptions.bloomBrightness = sender.floatValue
            refresh()
        }
    }
    
    @IBAction func vidBloomWeightAction(_ sender: NSSlider!) {
        
        if let renderer = myController?.renderer {
            renderer.shaderOptions.bloomWeight = sender.floatValue
            refresh()
        }
    }
    
    @IBAction func vidDotMaskAction(_ sender: NSPopUpButton!) {
        
        if let renderer = myController?.renderer {
            renderer.shaderOptions.dotMask = Int32(sender.selectedTag())
            myController?.renderer.buildDotMasks()
            refresh()
        }
    }
    
    @IBAction func vidDotMaskBrightnessAction(_ sender: NSSlider!) {
        
        if let renderer = myController?.renderer {
            renderer.shaderOptions.dotMaskBrightness = sender.floatValue
            myController?.renderer.buildDotMasks()
            refresh()
        }
    }
    
    @IBAction func vidScanlinesAction(_ sender: NSPopUpButton!) {
        
        if let renderer = myController?.renderer {
            renderer.shaderOptions.scanlines = Int32(sender.selectedTag())
            refresh()
        }
    }
    @IBAction func vidScanlineBrightnessAction(_ sender: NSSlider!) {
        
        if let renderer = myController?.renderer {
            renderer.shaderOptions.scanlineBrightness = sender.floatValue
            refresh()
        }
    }
    
    @IBAction func vidScanlineWeightAction(_ sender: NSSlider!) {

        if let renderer = myController?.renderer {
            renderer.shaderOptions.scanlineWeight = sender.floatValue
            refresh()
        }
    }
    
    @IBAction func vidDisalignmentAction(_ sender: NSPopUpButton!) {
        
        if let renderer = myController?.renderer {
            renderer.shaderOptions.disalignment = Int32(sender.selectedTag())
            refresh()
        }
    }
    @IBAction func vidDisalignmentHAction(_ sender: NSSlider!) {
        
        if let renderer = myController?.renderer {
            renderer.shaderOptions.disalignmentH = sender.floatValue
            refresh()
        }
    }
    
    @IBAction func vidDisalignmentVAction(_ sender: NSSlider!) {

        if let renderer = myController?.renderer {
            renderer.shaderOptions.disalignmentV = sender.floatValue
            refresh()
        }
    }
    
    //
    // Action methods (Geometry)
    //
    
    @IBAction func vidAspectRatioAction(_ sender: NSButton!) {
        
        if let renderer = myController?.renderer {
            renderer.keepAspectRatio = (sender.state == .on)
            refresh()
        }
    }
    
    @IBAction func vidEyeXAction(_ sender: NSSlider!) {
        
        /*
        if let renderer = myController?.renderer {
            renderer.setEyeX(sender.floatValue)
            refresh()
            
        }
        */
    }
    
    @IBAction func vidEyeYAction(_ sender: NSSlider!) {
        
        /*
        if let renderer = myController?.renderer {
            renderer.setEyeY(sender.floatValue)
            refresh()
        }
        */
    }
    
    @IBAction func vidEyeZAction(_ sender: NSSlider!) {
        
        /*
        if let renderer = myController?.renderer {
            renderer.setEyeZ(sender.floatValue)
            refresh()
        }
        */
    }
    
    //
    // Action methods (Misc)
    //
    
    /*
    func vidFactorySettingsAction() {
        
        UserDefaults.resetVideoUserDefaults()
        updatePalettePreviewImages()
        refresh()
    }
    */
    
    /*
    @IBAction func vidFactorySettingsActionTFT(_ sender: Any!) {

        UserDefaults.resetVideoUserDefaults()
        myController?.renderer.shaderOptions = ShaderDefaultsTFT
        updatePalettePreviewImages()
        refresh()
    }
    
    @IBAction func vidFactorySettingsActionCRT(_ sender: Any!) {
        
        UserDefaults.resetVideoUserDefaults()
        myController?.renderer.shaderOptions = ShaderDefaultsCRT
        myController?.renderer.buildDotMasks()
        updatePalettePreviewImages()
        refresh()
    }
    */
    
    @IBAction func vidPresetAction(_ sender: NSPopUpButton!) {
        
        UserDefaults.resetVideoUserDefaults()
        
        switch sender.selectedTag() {
        case 0: myController?.renderer.shaderOptions = ShaderDefaultsTFT
        case 1: myController?.renderer.shaderOptions = ShaderDefaultsCRT
        default: fatalError()
        }
        
        updatePalettePreviewImages()
        refresh()
    }
    
    @IBAction func vidDefaultsAction(_ sender: NSButton!) {
        
        track()
        // config.saveVideoUserDefaults()
    }
}
