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
        let upscalers = parent.renderer.upscalerGallery
        for i in 0 ..< upscalers.count {
            if let item = vidUpscalerPopUp.menu?.item(withTag: i) {
                item.isEnabled = (upscalers[i] != nil)
            }
        }
        
        // Create dot mask textures
        parent.renderer.buildDotMasks()
        updatePalettePreviewImages()
    }
    
    func refreshVideoTab() {
        
        let renderer = parent.renderer!
        
        // Video
        vidUpscalerPopUp.selectItem(withTag: config.upscaler)
        vidPalettePopUp.selectItem(withTag: config.palette)
        vidBrightnessSlider.doubleValue = config.brightness
        vidContrastSlider.doubleValue = config.contrast
        vidSaturationSlider.doubleValue = config.saturation
        
        // Effects
        vidBlurPopUp.selectItem(withTag: Int(config.blur))
        vidBlurRadiusSlider.floatValue = config.blurRadius
        vidBlurRadiusSlider.isEnabled = config.blur > 0
        
        vidBloomPopUp.selectItem(withTag: Int(config.bloom))
        vidBloomRadiusRSlider.floatValue = config.bloomRadiusR
        vidBloomRadiusRSlider.isEnabled = config.bloom > 0
        vidBloomRadiusGSlider.floatValue = config.bloomRadiusG
        vidBloomRadiusGSlider.isEnabled = config.bloom > 1
        vidBloomRadiusBSlider.floatValue = config.bloomRadiusB
        vidBloomRadiusBSlider.isEnabled = config.bloom > 1
        vidBloomBrightnessSlider.floatValue = config.bloomBrightness
        vidBloomBrightnessSlider.isEnabled = config.bloom > 0
        vidBloomWeightSlider.floatValue = config.bloomWeight
        vidBloomWeightSlider.isEnabled = config.bloom > 0
        
        vidDotMaskPopUp.selectItem(withTag: Int(config.dotMask))
        for i in 0 ... 4 {
            vidDotMaskPopUp.item(at: i)?.image = renderer.dotmaskImages[i]
        }
        vidDotMaskBrightnessSlider.floatValue = config.dotMaskBrightness
        vidDotMaskBrightnessSlider.isEnabled = config.dotMask > 0
        
        vidScanlinesPopUp.selectItem(withTag: Int(config.scanlines))
        vidScanlineBrightnessSlider.floatValue = config.scanlineBrightness
        vidScanlineBrightnessSlider.isEnabled = config.scanlines > 0
        vidScanlineWeightSlider.floatValue = config.scanlineWeight
        vidScanlineWeightSlider.isEnabled = config.scanlines == 2
        
        vidMisalignmentPopUp.selectItem(withTag: Int(config.disalignment))
        vidMisalignmentXSlider.floatValue = config.disalignmentH
        vidMisalignmentXSlider.isEnabled = config.disalignment > 0
        vidMisalignmentYSlider.floatValue = config.disalignmentV
        vidMisalignmentYSlider.isEnabled = config.disalignment > 0

        // Geometry
        vidHCenter.floatValue = config.hCenter * 1000
        vidVCenter.floatValue = config.vCenter * 1000
        vidHZoom.floatValue = config.hZoom * 1000
        vidVZoom.floatValue = config.vZoom * 1000

        // Power button
        vidPowerButton.isHidden = !bootable
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
                let p = Palette(rawValue: palette)
                let rgba = c64.vic.rgbaColor(n, palette: p)
                ptr[n] = rgba
            }
            
            // Create image
            let image = NSImage.make(data: mask, rect: size)
            let resizedImage = image?.resizeSharp(width: 64, height: 12)
            vidPalettePopUp.item(at: palette)?.image = resizedImage
        }
    }

    //
    // Action methods (Colors)
    //
    
    @IBAction func vidPaletteAction(_ sender: NSPopUpButton!) {
        
        config.palette = sender.selectedTag()
        refresh()
    }
    
    @IBAction func vidBrightnessAction(_ sender: NSSlider!) {
        
        config.brightness = sender.doubleValue
        refresh()
    }
    
    @IBAction func vidContrastAction(_ sender: NSSlider!) {
        
        config.contrast = sender.doubleValue
        refresh()
    }
    
    @IBAction func vidSaturationAction(_ sender: NSSlider!) {
        
        config.saturation = sender.doubleValue
        refresh()
    }

    //
    // Action methods (Effects)
    //
    
    @IBAction func vidUpscalerAction(_ sender: NSPopUpButton!) {
        
        config.upscaler = sender.selectedTag()
        refresh()
    }
    
    @IBAction func vidBlurAction(_ sender: NSPopUpButton!) {
        
        config.blur = Int32(sender.selectedTag())
        refresh()
    }
    
    @IBAction func vidBlurRadiusAction(_ sender: NSSlider!) {
        
        config.blurRadius = sender.floatValue
        refresh()
    }
    
    @IBAction func vidBloomAction(_ sender: NSPopUpButton!) {
        
        config.bloom = Int32(sender.selectedTag())
        refresh()
    }
    
    @IBAction func vidBloomRadiusRAction(_ sender: NSSlider!) {
        
        config.bloomRadiusR = sender.floatValue
        refresh()
    }

    @IBAction func vidBloomRadiusGAction(_ sender: NSSlider!) {
        
        config.bloomRadiusG = sender.floatValue
        refresh()
    }

    @IBAction func vidBloomRadiusBAction(_ sender: NSSlider!) {
        
        config.bloomRadiusB = sender.floatValue
        refresh()
    }

    @IBAction func vidBloomBrightnessAction(_ sender: NSSlider!) {
        
        config.bloomBrightness = sender.floatValue
        refresh()
    }
    
    @IBAction func vidBloomWeightAction(_ sender: NSSlider!) {
        
        config.bloomWeight = sender.floatValue
        refresh()
    }
    
    @IBAction func vidDotMaskAction(_ sender: NSPopUpButton!) {
        
        config.dotMask = Int32(sender.selectedTag())
        refresh()
    }
    
    @IBAction func vidDotMaskBrightnessAction(_ sender: NSSlider!) {
        
        config.dotMaskBrightness = sender.floatValue
        refresh()
    }
    
    @IBAction func vidScanlinesAction(_ sender: NSPopUpButton!) {
        
        config.scanlines = Int32(sender.selectedTag())
        refresh()
    }
    
    @IBAction func vidScanlineBrightnessAction(_ sender: NSSlider!) {
        
        config.scanlineBrightness = sender.floatValue
        refresh()
    }
    
    @IBAction func vidScanlineWeightAction(_ sender: NSSlider!) {
        
        config.scanlineWeight = sender.floatValue
        refresh()
    }
    
    @IBAction func vidDisalignmentAction(_ sender: NSPopUpButton!) {
        
        config.disalignment = Int32(sender.selectedTag())
        refresh()
    }
    
    @IBAction func vidDisalignmentHAction(_ sender: NSSlider!) {
        
        config.disalignmentH = sender.floatValue
        refresh()
    }
    
    @IBAction func vidDisalignmentVAction(_ sender: NSSlider!) {
        
        config.disalignmentV = sender.floatValue
        refresh()
    }
    
    //
    // Action methods (Geometry)
    //
    
    @IBAction func vidHCenterAction(_ sender: NSSlider!) {
        
        config.hCenter = sender.floatValue / 1000
        refresh()
    }
    
    @IBAction func vidVCenterAction(_ sender: NSSlider!) {
        
        config.vCenter = sender.floatValue / 1000
        refresh()
    }
    
    @IBAction func vidHZoomAction(_ sender: NSSlider!) {
        
        config.hZoom = sender.floatValue / 1000
        refresh()
    }
    
    @IBAction func vidVZoomAction(_ sender: NSSlider!) {
        
        config.vZoom = sender.floatValue / 1000
        refresh()
    }
    
    //
    // Action methods (Misc)
    //
        
    @IBAction func vidPresetAction(_ sender: NSPopUpButton!) {
        
        UserDefaults.resetVideoUserDefaults()
        
        switch sender.selectedTag() {
        case 0: config.loadVideoDefaults(VideoDefaults.tft)
        case 1: config.loadVideoDefaults(VideoDefaults.crt)
        default: fatalError()
        }
        
        updatePalettePreviewImages()
        refresh()
    }
    
    @IBAction func vidDefaultsAction(_ sender: NSButton!) {
        
        track()
        config.saveVideoUserDefaults()
    }
}
