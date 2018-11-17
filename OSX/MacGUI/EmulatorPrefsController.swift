//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

class EmulatorPrefsController : UserDialogController {
    
    // Video
    @IBOutlet weak var upscaler: NSPopUpButton!
    @IBOutlet weak var palettePopup: NSPopUpButton!
    @IBOutlet weak var paletteImage: NSImageView!
    @IBOutlet weak var colorWell0: NSColorWell!
    @IBOutlet weak var colorWell1: NSColorWell!
    @IBOutlet weak var colorWell2: NSColorWell!
    @IBOutlet weak var colorWell3: NSColorWell!
    @IBOutlet weak var colorWell4: NSColorWell!
    @IBOutlet weak var colorWell5: NSColorWell!
    @IBOutlet weak var colorWell6: NSColorWell!
    @IBOutlet weak var colorWell7: NSColorWell!
    @IBOutlet weak var colorWell8: NSColorWell!
    @IBOutlet weak var colorWell9: NSColorWell!
    @IBOutlet weak var colorWell10: NSColorWell!
    @IBOutlet weak var colorWell11: NSColorWell!
    @IBOutlet weak var colorWell12: NSColorWell!
    @IBOutlet weak var colorWell13: NSColorWell!
    @IBOutlet weak var colorWell14: NSColorWell!
    @IBOutlet weak var colorWell15: NSColorWell!
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
    @IBOutlet weak var aspectRatioButton: NSButton!
    @IBOutlet weak var eyeXSlider: NSSlider!
    @IBOutlet weak var eyeYSlider: NSSlider!
    @IBOutlet weak var eyeZSlider: NSSlider!

    // VC1541
    @IBOutlet weak var warpLoad: NSButton!
    @IBOutlet weak var driveNoise: NSButton!
    
    // Misc
    @IBOutlet weak var pauseInBackground: NSButton!
    @IBOutlet weak var autoSnapshots: NSButton!
    @IBOutlet weak var snapshotInterval: NSTextField!
    @IBOutlet weak var autoMount: NSButton!

    override func awakeFromNib() {
        
        // Check for available upscalers
        var kernels = parent.metalScreen.upscalerGallery
        for i in 0 ... kernels.count - 1 {
            upscaler.menu!.item(withTag: i)?.isEnabled = (kernels[i] != nil)
        }
        
        parent.metalScreen.buildDotMasks()
        update()
        updatePaletteImage()
    }
    
    func update() {
       
        let document = parent.document as! MyDocument
        let shaderOptions = parent.metalScreen.shaderOptions
            
        // Video
        upscaler.selectItem(withTag: parent.metalScreen.videoUpscaler)
        palettePopup.selectItem(withTag: document.c64.vic.videoPalette())
        /*
        colorWell0.color = c64.vic.color(0)
        colorWell1.color = c64.vic.color(1)
        colorWell2.color = c64.vic.color(2)
        colorWell3.color = c64.vic.color(3)
        colorWell4.color = c64.vic.color(4)
        colorWell5.color = c64.vic.color(5)
        colorWell6.color = c64.vic.color(6)
        colorWell7.color = c64.vic.color(7)
        colorWell8.color = c64.vic.color(8)
        colorWell9.color = c64.vic.color(9)
        colorWell10.color = c64.vic.color(10)
        colorWell11.color = c64.vic.color(11)
        colorWell12.color = c64.vic.color(12)
        colorWell13.color = c64.vic.color(13)
        colorWell14.color = c64.vic.color(14)
        colorWell15.color = c64.vic.color(15)
 */
        brightnessSlider.doubleValue = document.c64.vic.brightness()
        contrastSlider.doubleValue = document.c64.vic.contrast()
        saturationSlider.doubleValue = document.c64.vic.saturation()

        // Effects
        blurPopUp.selectItem(withTag: shaderOptions.blur)
        blurRadiusSlider.floatValue = shaderOptions.blurRadius
        blurRadiusSlider.isEnabled = shaderOptions.blur > 0

        bloomPopup.selectItem(withTag: shaderOptions.bloom)
        bloomRadiusSlider.floatValue = shaderOptions.bloomRadius
        bloomRadiusSlider.isEnabled = shaderOptions.bloom > 0
        bloomBrightnessSlider.floatValue = shaderOptions.bloomBrightness
        bloomBrightnessSlider.isEnabled = shaderOptions.bloom > 0
        bloomWeightSlider.floatValue = shaderOptions.bloomWeight
        bloomWeightSlider.isEnabled = shaderOptions.bloom > 0
        
        dotMaskPopUp.selectItem(withTag: shaderOptions.dotMask)
        for i in 0 ... 4 {
            dotMaskPopUp.item(at: i)?.image = parent.metalScreen.dotmaskImages[i]
        }
        dotMaskBrightnessSlider.floatValue = shaderOptions.dotMaskBrightness
        dotMaskBrightnessSlider.isEnabled = shaderOptions.dotMask > 0

        scanlinesPopUp.selectItem(withTag: shaderOptions.scanlines)
        scanlineBrightnessSlider.floatValue = shaderOptions.scanlineBrightness
        scanlineBrightnessSlider.isEnabled = shaderOptions.scanlines > 0
        scanlineWeightSlider.floatValue = shaderOptions.scanlineWeight
        scanlineWeightSlider.isEnabled = shaderOptions.scanlines == 2

        // Geometry
        aspectRatioButton.state = parent.metalScreen.fullscreenKeepAspectRatio ? .on : .off
        eyeXSlider.floatValue = parent.metalScreen.eyeX()
        eyeYSlider.floatValue = parent.metalScreen.eyeY()
        eyeZSlider.floatValue = parent.metalScreen.eyeZ()
        
        // VC1541
        warpLoad.state = c64.warpLoad() ? .on : .off
        driveNoise.state = c64.drive1.sendSoundMessages() ? .on : .off
        
        // Miscellanious
        pauseInBackground.state = parent.pauseInBackground ? .on : .off
        autoSnapshots.state = (c64.snapshotInterval() > 0) ? .on : .off
        snapshotInterval.integerValue = Int(c64.snapshotInterval().magnitude)
        snapshotInterval.isEnabled = (c64.snapshotInterval() > 0)
        autoMount.state = parent.autoMount ? .on : .off
    }
    
    func updatePaletteImage() {
        
        // Create image representation in memory
        let size = CGSize.init(width: 16, height: 1)
        let cap = Int(size.width) * Int(size.height)
        let mask = calloc(cap, MemoryLayout<UInt32>.size)!
        let ptr = mask.bindMemory(to: UInt32.self, capacity: cap)
        
        for n in 0 ... 15 {
            
            let color = parent.c64.vic.color(n)!
            let r = UInt8(color.redComponent * 255)
            let g = UInt8(color.greenComponent * 255)
            let b = UInt8(color.blueComponent * 255)
            let rgba = UInt32.init(r: r, g: g, b: b)
            
            ptr[n] = rgba
        }
    
        // Create image
        let image = NSImage.make(data: mask, rect: size)
        paletteImage.image = image?.resizeImageSharp(width: 128, height: 1)
    }
    
    
    //
    // Action methods (Colors)
    //

    @IBAction func paletteAction(_ sender: NSPopUpButton!) {
        
        let document = parent.document as! MyDocument
        document.c64.vic.setVideoPalette(sender.selectedTag())
        update()
        updatePaletteImage()
    }

    @IBAction func brightnessAction(_ sender: NSSlider!) {
        
        let document = parent.document as! MyDocument
        document.c64.vic.setBrightness(sender.doubleValue)
        update()
        updatePaletteImage()
    }
    
    @IBAction func contrastAction(_ sender: NSSlider!) {
        
        let document = parent.document as! MyDocument
        document.c64.vic.setContrast(sender.doubleValue)
        update()
        updatePaletteImage()
    }
    
    @IBAction func saturationAction(_ sender: NSSlider!) {
        
        let document = parent.document as! MyDocument
        document.c64.vic.setSaturation(sender.doubleValue)
        update()
        updatePaletteImage()
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
        parent.metalScreen.shaderOptions.blur = sender.selectedTag()
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
        parent.metalScreen.shaderOptions.bloom = sender.selectedTag()
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
        parent.metalScreen.shaderOptions.dotMask = sender.selectedTag()
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
        parent.metalScreen.shaderOptions.scanlines = sender.selectedTag()
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
    
    @IBAction func setFullscreenAspectRatio(_ sender: NSButton!) {
        
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
    // Action methods (VC1541)
    //
    
    @IBAction func warpLoadAction(_ sender: NSButton!) {
        
        c64.setWarpLoad(sender.state == .on)
        update()
    }
    
    @IBAction func driveNoiseAction(_ sender: NSButton!) {
        
        c64.drive1.setSendSoundMessages(sender.state == .on)
        c64.drive2.setSendSoundMessages(sender.state == .on)
        update()
    }
    
    @IBAction func pauseInBackgroundAction(_ sender: NSButton!) {
        
        parent.pauseInBackground =  (sender.state == .on)
        update()
    }

    @IBAction func autoSnapshotAction(_ sender: NSButton!) {
        
        if sender.state == .on {
            c64.enableAutoSnapshots()
        } else {
            c64.disableAutoSnapshots()
        }
        update()
    }

    @IBAction func snapshotIntervalAction(_ sender: NSTextField!) {
        
        c64.setSnapshotInterval(sender.integerValue)
        update()
    }
    
    @IBAction func autoMountAction(_ sender: NSButton!) {
        
        parent.autoMount = (sender.state == .on)
        update()
    }
    
    
    //
    // Action methods (Misc)
    //
    
    @IBAction override func cancelAction(_ sender: Any!) {
        
        parent.loadEmulatorUserDefaults()
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
        
        // VC1541
        c64.setWarpLoad(EmulatorDefaults.warpLoad)
        c64.drive1.setSendSoundMessages(EmulatorDefaults.sendSoundMessages)
        c64.drive2.setSendSoundMessages(EmulatorDefaults.sendSoundMessages)
        
        // Misc
        parent.pauseInBackground = EmulatorDefaults.pauseInBackground
        c64.setSnapshotInterval(EmulatorDefaults.snapshotInterval)
        parent.autoMount = EmulatorDefaults.autoMount

        update()
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
        
        parent.saveEmulatorUserDefaults()
        hideSheet()
    }
}

