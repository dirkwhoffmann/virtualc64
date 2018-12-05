//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

class VideoPrefsController : UserDialogController {
    
    // Video
    @IBOutlet weak var upscalerPopup: NSPopUpButton!
    @IBOutlet weak var palettePopup: NSPopUpButton!
    @IBOutlet weak var brightnessSlider: NSSlider!
    @IBOutlet weak var contrastSlider: NSSlider!
    @IBOutlet weak var saturationSlider: NSSlider!
    
    // Effects
    @IBOutlet weak var blurPopUp: NSPopUpButton!
    @IBOutlet weak var blurRadiusSlider: NSSlider!
    
    @IBOutlet weak var bloomPopup:NSPopUpButton!
    @IBOutlet weak var bloomRadiusSlider: NSSlider!
    @IBOutlet weak var bloomBrightnessSlider: NSSlider!
    @IBOutlet weak var bloomWeightSlider: NSSlider!
    
    @IBOutlet weak var dotMaskPopUp: NSPopUpButton!
    @IBOutlet weak var dotMaskBrightnessSlider: NSSlider!
    
    @IBOutlet weak var scanlinesPopUp: NSPopUpButton!
    @IBOutlet weak var scanlineBrightnessSlider: NSSlider!
    @IBOutlet weak var scanlineWeightSlider: NSSlider!
    
    // Geometry
    @IBOutlet weak var fullscreenAspectRatioButton: NSButton!
    @IBOutlet weak var eyeXSlider: NSSlider!
    @IBOutlet weak var eyeYSlider: NSSlider!
    @IBOutlet weak var eyeZSlider: NSSlider!
    
    override func awakeFromNib() {
        
        // Check for available upscalers
        var kernels = parent.metalScreen.upscalerGallery
        for i in 0 ... kernels.count - 1 {
            upscalerPopup.menu!.item(withTag: i)?.isEnabled = (kernels[i] != nil)
        }
        
        parent.metalScreen.buildDotMasks()
        update()
        updatePalettePreviewImages()
    }
    
    func update() {
        
        let document = parent.document as! MyDocument
        let shaderOptions = parent.metalScreen.shaderOptions
        
        // Video
        upscalerPopup.selectItem(withTag: parent.metalScreen.videoUpscaler)
        palettePopup.selectItem(withTag: document.c64.vic.videoPalette())
        brightnessSlider.doubleValue = document.c64.vic.brightness()
        contrastSlider.doubleValue = document.c64.vic.contrast()
        saturationSlider.doubleValue = document.c64.vic.saturation()
        
        // Effects
        blurPopUp.selectItem(withTag: Int(shaderOptions.blur))
        blurRadiusSlider.floatValue = shaderOptions.blurRadius
        blurRadiusSlider.isEnabled = shaderOptions.blur > 0
        
        bloomPopup.selectItem(withTag: Int(shaderOptions.bloom))
        bloomRadiusSlider.floatValue = shaderOptions.bloomRadius
        bloomRadiusSlider.isEnabled = shaderOptions.bloom > 0
        bloomBrightnessSlider.floatValue = shaderOptions.bloomBrightness
        bloomBrightnessSlider.isEnabled = shaderOptions.bloom > 0
        bloomWeightSlider.floatValue = shaderOptions.bloomWeight
        bloomWeightSlider.isEnabled = shaderOptions.bloom > 0
        
        dotMaskPopUp.selectItem(withTag: Int(shaderOptions.dotMask))
        for i in 0 ... 4 {
            dotMaskPopUp.item(at: i)?.image = parent.metalScreen.dotmaskImages[i]
        }
        dotMaskBrightnessSlider.floatValue = shaderOptions.dotMaskBrightness
        dotMaskBrightnessSlider.isEnabled = shaderOptions.dotMask > 0
        
        scanlinesPopUp.selectItem(withTag: Int(shaderOptions.scanlines))
        scanlineBrightnessSlider.floatValue = shaderOptions.scanlineBrightness
        scanlineBrightnessSlider.isEnabled = shaderOptions.scanlines > 0
        scanlineWeightSlider.floatValue = shaderOptions.scanlineWeight
        scanlineWeightSlider.isEnabled = shaderOptions.scanlines == 2
        
        // Geometry
        fullscreenAspectRatioButton.state = parent.metalScreen.fullscreenKeepAspectRatio ? .on : .off
        eyeXSlider.floatValue = parent.metalScreen.eyeX()
        eyeYSlider.floatValue = parent.metalScreen.eyeY()
        eyeZSlider.floatValue = parent.metalScreen.eyeZ()
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
            // palettePreview[palette] = image?.resizeImageSharp(width: 64, height: 12)
            let resizedImage = image?.resizeImageSharp(width: 64, height: 12)
            palettePopup.item(at: palette)?.image = resizedImage
        }
    }
    
    
    //
    // Action methods (Colors)
    //
    
    @IBAction func paletteAction(_ sender: NSPopUpButton!) {
        
        let document = parent.document as! MyDocument
        document.c64.vic.setVideoPalette(sender.selectedTag())
        update()
        updatePalettePreviewImages()
    }
    
    @IBAction func brightnessAction(_ sender: NSSlider!) {
        
        let document = parent.document as! MyDocument
        document.c64.vic.setBrightness(sender.doubleValue)
        update()
        updatePalettePreviewImages()
    }
    
    @IBAction func contrastAction(_ sender: NSSlider!) {
        
        let document = parent.document as! MyDocument
        document.c64.vic.setContrast(sender.doubleValue)
        update()
        updatePalettePreviewImages()
    }
    
    @IBAction func saturationAction(_ sender: NSSlider!) {
        
        let document = parent.document as! MyDocument
        document.c64.vic.setSaturation(sender.doubleValue)
        update()
        updatePalettePreviewImages()
    }
    
    
    //
    // Action methods (Effects)
    //
    
    @IBAction func upscalerAction(_ sender: NSPopUpButton!) {
        
        parent.metalScreen.videoUpscaler = sender.selectedTag()
        update()
    }
    
    @IBAction func blurAction(_ sender: NSPopUpButton!)
    {
        track("\(sender.selectedTag())")
        parent.metalScreen.shaderOptions.blur = Int32(sender.selectedTag())
        update()
    }
    
    @IBAction func blurRadiusAction(_ sender: NSSlider!)
    {
        track("\(sender.floatValue)")
        parent.metalScreen.shaderOptions.blurRadius = sender.floatValue
        update()
    }
    
    @IBAction func bloomAction(_ sender: NSPopUpButton!)
    {
        track("\(sender.selectedTag())")
        parent.metalScreen.shaderOptions.bloom = Int32(sender.selectedTag())
        update()
    }
    
    @IBAction func bloomRadiusAction(_ sender: NSSlider!)
    {
        track("\(sender.floatValue)")
        parent.metalScreen.shaderOptions.bloomRadius = sender.floatValue
        update()
    }
    
    @IBAction func bloomBrightnessAction(_ sender: NSSlider!)
    {
        track("\(sender.floatValue)")
        parent.metalScreen.shaderOptions.bloomBrightness = sender.floatValue
        update()
    }
    
    @IBAction func bloomWeightAction(_ sender: NSSlider!)
    {
        track("\(sender.floatValue)")
        parent.metalScreen.shaderOptions.bloomWeight = sender.floatValue
        update()
    }
    
    @IBAction func dotMaskAction(_ sender: NSPopUpButton!)
    {
        track("\(sender.selectedTag())")
        parent.metalScreen.shaderOptions.dotMask = Int32(sender.selectedTag())
        parent.metalScreen.buildDotMasks()
        update()
    }
    
    @IBAction func dotMaskBrightnessAction(_ sender: NSSlider!)
    {
        track("\(sender.floatValue)")
        parent.metalScreen.shaderOptions.dotMaskBrightness = sender.floatValue
        parent.metalScreen.buildDotMasks()
        update()
    }
    
    @IBAction func scanlinesAction(_ sender: NSPopUpButton!)
    {
        track("\(sender.selectedTag())")
        parent.metalScreen.shaderOptions.scanlines = Int32(sender.selectedTag())
        update()
    }
    @IBAction func scanlineBrightnessAction(_ sender: NSSlider!)
    {
        track("\(sender.floatValue)")
        parent.metalScreen.shaderOptions.scanlineBrightness = sender.floatValue
        update()
    }
    
    @IBAction func scanlineWeightAction(_ sender: NSSlider!)
    {
        track("\(sender.floatValue)")
        parent.metalScreen.shaderOptions.scanlineWeight = sender.floatValue
        update()
    }
    
    
    //
    // Action methods (Geometry)
    //
    
    @IBAction func fullscreenAspectRatioAction(_ sender: NSButton!) {
        
        parent.metalScreen.fullscreenKeepAspectRatio = (sender.state == .on)
        update()
    }
    
    @IBAction func eyeXAction(_ sender: NSSlider!) {
        
        parent.metalScreen.setEyeX(sender.floatValue)
        update()
    }
    
    @IBAction func eyeYAction(_ sender: NSSlider!) {
        
        parent.metalScreen.setEyeY(sender.floatValue)
        update()
    }
    
    @IBAction func eyeZAction(_ sender: NSSlider!) {
        
        parent.metalScreen.setEyeZ(sender.floatValue)
        update()
    }
    
    
    //
    // Action methods (Misc)
    //
    
    @IBAction override func cancelAction(_ sender: Any!) {
        
        parent.loadVideoUserDefaults()
        hideSheet()
    }
    
    func factorySettingsAction() {
        
        // Video
        parent.metalScreen.videoUpscaler = EmulatorDefaults.upscaler
        c64.vic.setVideoPalette(EmulatorDefaults.palette)
        c64.vic.setBrightness(EmulatorDefaults.brightness)
        c64.vic.setContrast(EmulatorDefaults.contrast)
        c64.vic.setSaturation(EmulatorDefaults.saturation)
        
        // Geometry
        parent.metalScreen.setEyeX(EmulatorDefaults.eyeX)
        parent.metalScreen.setEyeY(EmulatorDefaults.eyeY)
        parent.metalScreen.setEyeZ(EmulatorDefaults.eyeZ)
        parent.metalScreen.fullscreenKeepAspectRatio = EmulatorDefaults.fullscreenAspectRatio
        
        update()
        updatePalettePreviewImages()
    }
    
    @IBAction func factorySettingsActionTFT(_ sender: Any!)
    {
        parent.metalScreen.shaderOptions = ShaderDefaultsTFT
        factorySettingsAction()
        
    }
    
    @IBAction func factorySettingsActionCRT(_ sender: Any!)
    {
        parent.metalScreen.shaderOptions = ShaderDefaultsCRT
        factorySettingsAction()
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        parent.saveVideoUserDefaults()
        hideSheet()
    }
}
