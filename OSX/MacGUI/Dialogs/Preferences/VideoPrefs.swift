//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

extension PreferencesController {
    
    func awakeVideoPrefsFromNib() {
        
        // Check for available upscalers
        if let kernels = myController?.metalScreen.upscalerGallery {
            
            for i in 0 ... kernels.count - 1 {
                vidUpscalerPopup.menu!.item(withTag: i)?.isEnabled = (kernels[i] != nil)
            }
            myController?.metalScreen.buildDotMasks()
            updatePalettePreviewImages()
        }
    }
    
    func refreshVideoTab() {
        
        // guard let doc = myDocument else { return }
        guard let con = myController else { return }
        guard let metal = con.metalScreen else { return }
        guard let c64 = proxy else { return }

        track()
        
        // Video
        vidUpscalerPopup.selectItem(withTag: metal.upscaler)
        vidPalettePopup.selectItem(withTag: c64.vic.videoPalette())
        vidBrightnessSlider.doubleValue = c64.vic.brightness()
        vidContrastSlider.doubleValue = c64.vic.contrast()
        vidSaturationSlider.doubleValue = c64.vic.saturation()
        
        // Effects
        let shaderOptions = metal.shaderOptions
        vidBlurPopUp.selectItem(withTag: Int(shaderOptions.blur))
        vidBlurRadiusSlider.floatValue = shaderOptions.blurRadius
        vidBlurRadiusSlider.isEnabled = shaderOptions.blur > 0
        
        vidBloomPopup.selectItem(withTag: Int(shaderOptions.bloom))
        vidBloomRadiusRSlider.floatValue = shaderOptions.bloomRadius
        vidBloomRadiusRSlider.isEnabled = shaderOptions.bloom > 0
        vidBloomRadiusGSlider.floatValue = shaderOptions.bloomRadius
        vidBloomRadiusGSlider.isEnabled = shaderOptions.bloom > 0
        vidBloomRadiusBSlider.floatValue = shaderOptions.bloomRadius
        vidBloomRadiusBSlider.isEnabled = shaderOptions.bloom > 0
        vidBloomBrightnessSlider.floatValue = shaderOptions.bloomBrightness
        vidBloomBrightnessSlider.isEnabled = shaderOptions.bloom > 0
        vidBloomWeightSlider.floatValue = shaderOptions.bloomWeight
        vidBloomWeightSlider.isEnabled = shaderOptions.bloom > 0
        
        vidDotMaskPopUp.selectItem(withTag: Int(shaderOptions.dotMask))
        for i in 0 ... 4 {
            vidDotMaskPopUp.item(at: i)?.image = metal.dotmaskImages[i]
        }
        vidDotMaskBrightnessSlider.floatValue = shaderOptions.dotMaskBrightness
        vidDotMaskBrightnessSlider.isEnabled = shaderOptions.dotMask > 0
        
        vidScanlinesPopUp.selectItem(withTag: Int(shaderOptions.scanlines))
        vidScanlineBrightnessSlider.floatValue = shaderOptions.scanlineBrightness
        vidScanlineBrightnessSlider.isEnabled = shaderOptions.scanlines > 0
        vidScanlineWeightSlider.floatValue = shaderOptions.scanlineWeight
        vidScanlineWeightSlider.isEnabled = shaderOptions.scanlines == 2
        
        // Geometry
        vidAspectRatioButton.state = metal.keepAspectRatio ? .on : .off
        vidEyeXSlider.floatValue = metal.eyeX()
        vidEyeYSlider.floatValue = metal.eyeY()
        vidEyeZSlider.floatValue = metal.eyeZ()
        
        vidOkButton.title = c64.isRunnable() ? "OK" : "Quit"
    }
    
    func updatePalettePreviewImages() {
        
        guard let c64 = proxy else { return }
        
        // Create image representation in memory
        let size = CGSize.init(width: 16, height: 1)
        let cap = Int(size.width) * Int(size.height)
        let mask = calloc(cap, MemoryLayout<UInt32>.size)!
        let ptr = mask.bindMemory(to: UInt32.self, capacity: cap)
        
        // For all palettes ...
        for palette : Int in 0 ... 5 {
            
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

        if let metal = myController?.metalScreen {
            track("\(sender.selectedTag())")
            metal.upscaler = sender.selectedTag()
            refresh()
        }
    }
    
    @IBAction func vidBlurAction(_ sender: NSPopUpButton!) {
        
        if let metal = myController?.metalScreen {
            track("\(sender.selectedTag())")
            metal.shaderOptions.blur = Int32(sender.selectedTag())
            refresh()
        }
    }
    
    @IBAction func vidBlurRadiusAction(_ sender: NSSlider!) {
        
        if let metal = myController?.metalScreen {
            track("\(sender.floatValue)")
            metal.shaderOptions.blurRadius = sender.floatValue
            refresh()
        }
    }
    
    @IBAction func vidBloomAction(_ sender: NSPopUpButton!) {
        
        if let metal = myController?.metalScreen {
            track("\(sender.selectedTag())")
            metal.shaderOptions.bloom = Int32(sender.selectedTag())
            refresh()
        }
    }

    @IBAction func vidBloomRadiusRAction(_ sender: NSSlider!) {
        
        if let metal = myController?.metalScreen {
            track("\(sender.floatValue)")
            metal.shaderOptions.bloomRadius = sender.floatValue
            refresh()
        }
    }

    @IBAction func vidBloomRadiusGAction(_ sender: NSSlider!) {
        
        if let metal = myController?.metalScreen {
            track("\(sender.floatValue)")
            metal.shaderOptions.bloomRadius = sender.floatValue
            refresh()
        }
    }
    
    @IBAction func vidBloomRadiusBAction(_ sender: NSSlider!) {
        
        if let metal = myController?.metalScreen {
            track("\(sender.floatValue)")
            metal.shaderOptions.bloomRadius = sender.floatValue
            refresh()
        }
    }
    
    
    @IBAction func vidBloomBrightnessAction(_ sender: NSSlider!) {
        
        if let metal = myController?.metalScreen {
            track("\(sender.floatValue)")
            metal.shaderOptions.bloomBrightness = sender.floatValue
            refresh()
        }
    }
    
    @IBAction func vidBloomWeightAction(_ sender: NSSlider!) {
        
        if let metal = myController?.metalScreen {
            track("\(sender.floatValue)")
            metal.shaderOptions.bloomWeight = sender.floatValue
            refresh()
        }
    }
    
    @IBAction func vidDotMaskAction(_ sender: NSPopUpButton!) {
        
        if let metal = myController?.metalScreen {
            track("\(sender.selectedTag())")
            metal.shaderOptions.dotMask = Int32(sender.selectedTag())
            metal.buildDotMasks()
            refresh()
        }
    }
    
    @IBAction func vidDotMaskBrightnessAction(_ sender: NSSlider!) {
        
        if let metal = myController?.metalScreen {
            track("\(sender.floatValue)")
            metal.shaderOptions.dotMaskBrightness = sender.floatValue
            metal.buildDotMasks()
            refresh()
        }
    }
    
    @IBAction func vidScanlinesAction(_ sender: NSPopUpButton!) {
        
        if let metal = myController?.metalScreen {
            track("\(sender.selectedTag())")
            metal.shaderOptions.scanlines = Int32(sender.selectedTag())
            refresh()
        }
    }
    @IBAction func vidScanlineBrightnessAction(_ sender: NSSlider!) {
        
        if let metal = myController?.metalScreen {
            track("\(sender.floatValue)")
            metal.shaderOptions.scanlineBrightness = sender.floatValue
            refresh()
        }
    }
    
    @IBAction func vidScanlineWeightAction(_ sender: NSSlider!)
    {
        if let metal = myController?.metalScreen {
            track("\(sender.floatValue)")
            metal.shaderOptions.scanlineWeight = sender.floatValue
            refresh()
        }
    }
    
    
    //
    // Action methods (Geometry)
    //
    
    @IBAction func vidAspectRatioAction(_ sender: NSButton!) {
        
        if let metal = myController?.metalScreen {
            metal.keepAspectRatio = (sender.state == .on)
            refresh()
        }
    }
    
    @IBAction func vidEyeXAction(_ sender: NSSlider!) {
        
        if let metal = myController?.metalScreen {
            metal.setEyeX(sender.floatValue)
            refresh()
            
        }
    }
    
    @IBAction func vidEyeYAction(_ sender: NSSlider!) {
        
        if let metal = myController?.metalScreen {
            metal.setEyeY(sender.floatValue)
            refresh()
        }
    }
    
    @IBAction func vidEyeZAction(_ sender: NSSlider!) {
        
        if let metal = myController?.metalScreen {
            metal.setEyeZ(sender.floatValue)
            refresh()
        }
    }
    
    //
    // Action methods (Misc)
    //
    
    func vidFactorySettingsAction() {
        
        myController?.resetVideoUserDefaults()
        refresh()
        updatePalettePreviewImages()
    }
    
    @IBAction func vidFactorySettingsActionTFT(_ sender: Any!)
    {
        myController?.metalScreen.shaderOptions = ShaderDefaultsTFT
        vidFactorySettingsAction()
        
    }
    
    @IBAction func vidFactorySettingsActionCRT(_ sender: Any!)
    {
        myController?.metalScreen.shaderOptions = ShaderDefaultsCRT
        vidFactorySettingsAction()
    }
}
