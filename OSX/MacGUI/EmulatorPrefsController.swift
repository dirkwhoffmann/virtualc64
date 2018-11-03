//
//  PropertiesController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 21.02.18.
//

import Foundation

struct Defaults {
    static let palette = Int(COLOR_PALETTE.rawValue)
    static let upscaler = 0
    static let filter = 0
    static let brightness = Double(50.0)
    static let contrast = Double(100.0)
    static let saturation = Double(50.0)
    static let blurFactor = Float(1.5)
    static let scanlines = 1
    static let dotMask = 1
    static let scanlineBrightness = Float(0.12)
    static let scanlineWeight = Float(6.0)
    static let bloomingFactor = Float(1.5)
    static let dotMaskBrightness = Float(0.7)
    static let eyeX = Float(0.0)
    static let eyeY = Float(0.0)
    static let eyeZ = Float(0.0)
    static let fullscreenAspectRatio = false
    static let warpLoad = true
    static let sendSoundMessages = true
    static let pauseInBackground = false
    static let snapshotInterval = 3
    static let autoMount = false
}

class EmulatorPrefsController : UserDialogController {
    
    // Color synthesizer
    @IBOutlet weak var palette: NSPopUpButton!
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
    
    // Texture processor
    @IBOutlet weak var upscaler: NSPopUpButton!
    @IBOutlet weak var filter: NSPopUpButton!
    @IBOutlet weak var brightnessSlider: NSSlider!
    @IBOutlet weak var contrastSlider: NSSlider!
    @IBOutlet weak var saturationSlider: NSSlider!
    @IBOutlet weak var blurSlider: NSSlider!

    // Effect engine
    @IBOutlet weak var scanlines: NSPopUpButton!
    @IBOutlet weak var dotMask: NSPopUpButton!
    @IBOutlet weak var scanlineBrightnessSlider: NSSlider!
    @IBOutlet weak var scanlineWeightSlider: NSSlider!
    @IBOutlet weak var bloomingSlider: NSSlider!
    @IBOutlet weak var dotMaskBrightnessSlider: NSSlider!

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
        var kernels = parent.metalScreen.upscalers
        for i in 0 ... kernels.count - 1 {
            upscaler.menu!.item(withTag: i)?.isEnabled = (kernels[i] != nil)
        }
        
        // Check for available filters
        kernels = parent.metalScreen.filters
        for i in 0 ... kernels.count - 1 {
            filter.menu!.item(withTag: i)?.isEnabled = (kernels[i] != nil)
        }
        
        update()
    }
    
    func update() {
       
        let document = parent.document as! MyDocument
        
        // Color synthesizer
        palette.selectItem(withTag: document.c64.vic.videoPalette())
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

        // Texture processor
        upscaler.selectItem(withTag: parent.metalScreen.videoUpscaler)
        filter.selectItem(withTag: parent.metalScreen.videoFilter)
        brightnessSlider.doubleValue = document.c64.vic.brightness()
        contrastSlider.doubleValue = document.c64.vic.contrast()
        saturationSlider.doubleValue = document.c64.vic.saturation()
        blurSlider.doubleValue = Double(parent.metalScreen.blurFactor)

        // Effect engine
        scanlines.selectItem(withTag: parent.metalScreen.scanlines)
        dotMask.selectItem(withTag: parent.metalScreen.dotMask)
        scanlineBrightnessSlider.floatValue = parent.metalScreen.scanlineBrightness
        scanlineWeightSlider.floatValue = parent.metalScreen.scanlineWeight
        bloomingSlider.floatValue = parent.metalScreen.bloomingFactor
        dotMaskBrightnessSlider.floatValue = parent.metalScreen.dotMaskBrightness

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
    
    
    //
    // Action methods (Color synthesizer)
    //

    @IBAction func paletteAction(_ sender: NSPopUpButton!) {
        
        let document = parent.document as! MyDocument
        document.c64.vic.setVideoPalette(sender.selectedTag())
        update()
    }

 
    //
    // Action methods (Texture processor)
    //
    
    @IBAction func upscalerAction(_ sender: NSPopUpButton!) {
    
        parent.metalScreen.videoUpscaler = sender.selectedTag()
        update()
    }
    
    @IBAction func filterAction(_ sender: NSPopUpButton!) {
    
        track()
        parent.metalScreen.videoFilter = sender.selectedTag()
        update()
    }
    
    @IBAction func brightnessAction(_ sender: NSSlider!) {
        
        track("New brightness = \(sender.doubleValue)")
        let document = parent.document as! MyDocument
        document.c64.vic.setBrightness(sender.doubleValue)
        update()
    }
    
    @IBAction func contrastAction(_ sender: NSSlider!) {
        
        track("New contrast = \(sender.doubleValue)")
        let document = parent.document as! MyDocument
        document.c64.vic.setContrast(sender.doubleValue)
        update()
    }
    
    @IBAction func saturationAction(_ sender: NSSlider!) {
        
        track("New staturation = \(sender.doubleValue)")
        let document = parent.document as! MyDocument
        document.c64.vic.setSaturation(sender.doubleValue)
        update()
    }
    
    @IBAction func blurAction(_ sender: NSSlider!) {
        
        track("New blur factor = \(sender.doubleValue)")
        parent.metalScreen.blurFactor = sender.floatValue

        let gaussFilter = parent.metalScreen.filters[1] as! GaussFilter
        gaussFilter.sigma = sender.floatValue
        update()
    }

    
    //
    // Action methods (Effect engine)
    //
    
    @IBAction func scanlinesAction(_ sender: NSPopUpButton!) {
        
        track("Scanlines = \(sender.selectedTag())")
        parent.metalScreen.scanlines = sender.selectedTag()
        update()
    }

    @IBAction func dotMaskAction(_ sender: NSPopUpButton!) {
        
        track("dotMask = \(sender.selectedTag())")
        parent.metalScreen.dotMask = sender.selectedTag()
        update()
    }
    
    @IBAction func scanlineBrightnessAction(_ sender: NSSlider!) {
        
        track("New scanline brightness = \(sender.doubleValue)")
        parent.metalScreen.scanlineBrightness = sender.floatValue
        update()
    }

    @IBAction func scanlineWeightAction(_ sender: NSSlider!) {
        
        track("New scanline weight = \(sender.doubleValue)")
        parent.metalScreen.scanlineWeight = sender.floatValue
        update()
    }

    @IBAction func bloomingAction(_ sender: NSSlider!) {
        
        track("New blooming factor = \(sender.doubleValue)")
        parent.metalScreen.bloomingFactor = sender.floatValue
        update()
    }

    @IBAction func dotMaskBrightnessAction(_ sender: NSSlider!) {
        
        track("New blooming factor = \(sender.doubleValue)")
        parent.metalScreen.dotMaskBrightness = sender.floatValue
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
    
    @IBAction func factorySettingsAction(_ sender: Any!) {
        
        // Color synthesizer
        c64.vic.setVideoPalette(Defaults.palette)

        // Texture processor
        parent.metalScreen.videoUpscaler = Defaults.upscaler
        parent.metalScreen.videoFilter = Defaults.filter
        c64.vic.setBrightness(Defaults.brightness)
        c64.vic.setContrast(Defaults.contrast)
        c64.vic.setSaturation(Defaults.saturation)
        parent.metalScreen.blurFactor = Defaults.blurFactor
        
        // Effect engine
        parent.metalScreen.scanlines = Defaults.scanlines
        parent.metalScreen.dotMask = Defaults.dotMask
        parent.metalScreen.scanlineBrightness = Defaults.scanlineBrightness
        parent.metalScreen.scanlineWeight = Defaults.scanlineWeight
        parent.metalScreen.bloomingFactor = Defaults.bloomingFactor
        parent.metalScreen.dotMaskBrightness = Defaults.dotMaskBrightness
            
        // Geometry
        parent.metalScreen.setEyeX(Defaults.eyeX)
        parent.metalScreen.setEyeY(Defaults.eyeY)
        parent.metalScreen.setEyeZ(Defaults.eyeZ)
        parent.metalScreen.fullscreenKeepAspectRatio = Defaults.fullscreenAspectRatio
        
        // VC1541
        c64.setWarpLoad(Defaults.warpLoad)
        c64.drive1.setSendSoundMessages(Defaults.sendSoundMessages)
        c64.drive2.setSendSoundMessages(Defaults.sendSoundMessages)
        
        // Misc
        parent.pauseInBackground = Defaults.pauseInBackground
        c64.setSnapshotInterval(Defaults.snapshotInterval)
        parent.autoMount = Defaults.autoMount

        update()
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        parent.saveEmulatorUserDefaults()
        hideSheet()
    }
}

