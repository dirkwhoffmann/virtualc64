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
        var kernels = parent.metalScreen.upscalerGallery
        for i in 0 ... kernels.count - 1 {
            vidUpscalerPopup.menu!.item(withTag: i)?.isEnabled = (kernels[i] != nil)
        }
        
        parent.metalScreen.buildDotMasks()
        updatePalettePreviewImages()
    }
    
    func refreshVideoTab() {
        
        track()
        
        let document = parent.document as! MyDocument
        let shaderOptions = parent.metalScreen.shaderOptions
        
        // Video
        vidUpscalerPopup.selectItem(withTag: parent.metalScreen.upscaler)
        vidPalettePopup.selectItem(withTag: document.c64.vic.videoPalette())
        vidBrightnessSlider.doubleValue = document.c64.vic.brightness()
        vidContrastSlider.doubleValue = document.c64.vic.contrast()
        vidSaturationSlider.doubleValue = document.c64.vic.saturation()
        
        // Effects
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
            vidDotMaskPopUp.item(at: i)?.image = parent.metalScreen.dotmaskImages[i]
        }
        vidDotMaskBrightnessSlider.floatValue = shaderOptions.dotMaskBrightness
        vidDotMaskBrightnessSlider.isEnabled = shaderOptions.dotMask > 0
        
        vidScanlinesPopUp.selectItem(withTag: Int(shaderOptions.scanlines))
        vidScanlineBrightnessSlider.floatValue = shaderOptions.scanlineBrightness
        vidScanlineBrightnessSlider.isEnabled = shaderOptions.scanlines > 0
        vidScanlineWeightSlider.floatValue = shaderOptions.scanlineWeight
        vidScanlineWeightSlider.isEnabled = shaderOptions.scanlines == 2
        
        // Geometry
        vidAspectRatioButton.state = parent.metalScreen.keepAspectRatio ? .on : .off
        vidEyeXSlider.floatValue = parent.metalScreen.eyeX()
        vidEyeYSlider.floatValue = parent.metalScreen.eyeY()
        vidEyeZSlider.floatValue = parent.metalScreen.eyeZ()
        
        vidOkButton.title = parent.c64.isRunnable() ? "OK" : "Quit"
    }
    
    func updatePalettePreviewImages() {
        
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
                let rgba = parent.c64.vic.rgbaColor(n, palette: p)
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
        
        let document = parent.document as! MyDocument
        document.c64.vic.setVideoPalette(sender.selectedTag())
        updatePalettePreviewImages()
        refresh()
    }
    
    @IBAction func vidBrightnessAction(_ sender: NSSlider!) {
        
        let document = parent.document as! MyDocument
        document.c64.vic.setBrightness(sender.doubleValue)
        updatePalettePreviewImages()
        refresh()
    }
    
    @IBAction func vidContrastAction(_ sender: NSSlider!) {
        
        let document = parent.document as! MyDocument
        document.c64.vic.setContrast(sender.doubleValue)
        updatePalettePreviewImages()
        refresh()
    }
    
    @IBAction func vidSaturationAction(_ sender: NSSlider!) {
        
        let document = parent.document as! MyDocument
        document.c64.vic.setSaturation(sender.doubleValue)
        updatePalettePreviewImages()
        refresh()
    }
    
    
    //
    // Action methods (Effects)
    //
    
    @IBAction func vidUpscalerAction(_ sender: NSPopUpButton!) {
        
        parent.metalScreen.upscaler = sender.selectedTag()
        refresh()
    }
    
    @IBAction func vidBlurAction(_ sender: NSPopUpButton!)
    {
        track("\(sender.selectedTag())")
        parent.metalScreen.shaderOptions.blur = Int32(sender.selectedTag())
        refresh()
    }
    
    @IBAction func vidBlurRadiusAction(_ sender: NSSlider!)
    {
        track("\(sender.floatValue)")
        parent.metalScreen.shaderOptions.blurRadius = sender.floatValue
        refresh()
    }
    
    @IBAction func vidBloomAction(_ sender: NSPopUpButton!)
    {
        track("\(sender.selectedTag())")
        parent.metalScreen.shaderOptions.bloom = Int32(sender.selectedTag())
        refresh()
    }

    @IBAction func vidBloomRadiusRAction(_ sender: NSSlider!)
    {
        track("\(sender.floatValue)")
        parent.metalScreen.shaderOptions.bloomRadius = sender.floatValue
        refresh()
    }

    @IBAction func vidBloomRadiusGAction(_ sender: NSSlider!)
    {
        track("\(sender.floatValue)")
        parent.metalScreen.shaderOptions.bloomRadius = sender.floatValue
        refresh()
    }
    
    @IBAction func vidBloomRadiusBAction(_ sender: NSSlider!)
    {
        track("\(sender.floatValue)")
        parent.metalScreen.shaderOptions.bloomRadius = sender.floatValue
        refresh()
    }
    
    
    @IBAction func vidBloomBrightnessAction(_ sender: NSSlider!)
    {
        track("\(sender.floatValue)")
        parent.metalScreen.shaderOptions.bloomBrightness = sender.floatValue
        refresh()
    }
    
    @IBAction func vidBloomWeightAction(_ sender: NSSlider!)
    {
        track("\(sender.floatValue)")
        parent.metalScreen.shaderOptions.bloomWeight = sender.floatValue
        refresh()
    }
    
    @IBAction func vidDotMaskAction(_ sender: NSPopUpButton!)
    {
        track("\(sender.selectedTag())")
        parent.metalScreen.shaderOptions.dotMask = Int32(sender.selectedTag())
        parent.metalScreen.buildDotMasks()
        refresh()
    }
    
    @IBAction func vidDotMaskBrightnessAction(_ sender: NSSlider!)
    {
        track("\(sender.floatValue)")
        parent.metalScreen.shaderOptions.dotMaskBrightness = sender.floatValue
        parent.metalScreen.buildDotMasks()
        refresh()
    }
    
    @IBAction func vidScanlinesAction(_ sender: NSPopUpButton!)
    {
        track("\(sender.selectedTag())")
        parent.metalScreen.shaderOptions.scanlines = Int32(sender.selectedTag())
        refresh()
    }
    @IBAction func vidScanlineBrightnessAction(_ sender: NSSlider!)
    {
        track("\(sender.floatValue)")
        parent.metalScreen.shaderOptions.scanlineBrightness = sender.floatValue
        refresh()
    }
    
    @IBAction func vidScanlineWeightAction(_ sender: NSSlider!)
    {
        track("\(sender.floatValue)")
        parent.metalScreen.shaderOptions.scanlineWeight = sender.floatValue
        refresh()
    }
    
    
    //
    // Action methods (Geometry)
    //
    
    @IBAction func vidAspectRatioAction(_ sender: NSButton!) {
        
        parent.metalScreen.keepAspectRatio = (sender.state == .on)
        refresh()
    }
    
    @IBAction func vidEyeXAction(_ sender: NSSlider!) {
        
        parent.metalScreen.setEyeX(sender.floatValue)
        refresh()
    }
    
    @IBAction func vidEyeYAction(_ sender: NSSlider!) {
        
        parent.metalScreen.setEyeY(sender.floatValue)
        refresh()
    }
    
    @IBAction func vidEyeZAction(_ sender: NSSlider!) {
        
        parent.metalScreen.setEyeZ(sender.floatValue)
        refresh()
    }
    
    //
    // Action methods (Misc)
    //
    
    func vidFactorySettingsAction() {
        
        parent.resetVideoUserDefaults()
        refresh()
        updatePalettePreviewImages()
    }
    
    @IBAction func vidFactorySettingsActionTFT(_ sender: Any!)
    {
        parent.metalScreen.shaderOptions = ShaderDefaultsTFT
        vidFactorySettingsAction()
        
    }
    
    @IBAction func vidFactorySettingsActionCRT(_ sender: Any!)
    {
        parent.metalScreen.shaderOptions = ShaderDefaultsCRT
        vidFactorySettingsAction()
    }
}
