// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class VideoSettingsViewController: SettingsViewController {

    // Palette
    @IBOutlet weak var vidPalettePopUp: NSPopUpButton!
    @IBOutlet weak var vidBrightnessSlider: NSSlider!
    @IBOutlet weak var vidContrastSlider: NSSlider!
    @IBOutlet weak var vidSaturationSlider: NSSlider!

    // Geometry
    @IBOutlet weak var vidZoom: NSPopUpButton!
    @IBOutlet weak var vidHZoom: NSSlider!
    @IBOutlet weak var vidVZoom: NSSlider!
    @IBOutlet weak var vidHZoomLabel: NSTextField!
    @IBOutlet weak var vidVZoomLabel: NSTextField!
    @IBOutlet weak var vidCenter: NSPopUpButton!
    @IBOutlet weak var vidHCenter: NSSlider!
    @IBOutlet weak var vidVCenter: NSSlider!
    @IBOutlet weak var vidHCenterLabel: NSTextField!
    @IBOutlet weak var vidVCenterLabel: NSTextField!

    // Effects
    @IBOutlet weak var vidUpscalerPopUp: NSPopUpButton!
    @IBOutlet weak var vidBlurPopUp: NSPopUpButton!
    @IBOutlet weak var vidBlurRadiusSlider: NSSlider!

    @IBOutlet weak var vidBloomPopUp: NSPopUpButton!
    @IBOutlet weak var vidBloomRadiusSlider: NSSlider!
    @IBOutlet weak var vidBloomBrightnessSlider: NSSlider!
    @IBOutlet weak var vidBloomWeightSlider: NSSlider!

    @IBOutlet weak var vidDotMaskPopUp: NSPopUpButton!
    @IBOutlet weak var vidDotMaskBrightnessSlider: NSSlider!

    @IBOutlet weak var vidScanlinesPopUp: NSPopUpButton!
    @IBOutlet weak var vidScanlineBrightnessSlider: NSSlider!
    @IBOutlet weak var vidScanlineWeightSlider: NSSlider!

    @IBOutlet weak var vidMisalignmentPopUp: NSPopUpButton!
    @IBOutlet weak var vidMisalignmentXSlider: NSSlider!
    @IBOutlet weak var vidMisalignmentYSlider: NSSlider!

    // Buttons
    @IBOutlet weak var vidOkButton: NSButton!
    @IBOutlet weak var vidPowerButton: NSButton!

    override var showLock: Bool { true }

    override func viewDidLoad() {

        log(.lifetime)
    }

    override func refresh() {

        guard let config = config, let renderer = renderer else { return }

        // Colors
        vidPalettePopUp.selectItem(withTag: config.palette)
        vidBrightnessSlider.integerValue = config.brightness
        vidContrastSlider.integerValue = config.contrast
        vidSaturationSlider.integerValue = config.saturation

        // Geometry
        vidZoom.selectItem(withTag: 0)
        vidHZoom.integerValue = config.hZoom
        vidVZoom.integerValue = config.vZoom
        vidHZoom.isEnabled = true
        vidVZoom.isEnabled = true
        vidHZoomLabel.textColor = .labelColor
        vidVZoomLabel.textColor = .labelColor
        vidCenter.selectItem(withTag: 0)
        vidHCenter.integerValue = config.hCenter
        vidVCenter.integerValue = config.vCenter
        vidHCenter.isEnabled = true
        vidVCenter.isEnabled = true
        vidHCenterLabel.textColor = .labelColor
        vidVCenterLabel.textColor = .labelColor

        // Upscalers
        vidUpscalerPopUp.selectItem(withTag: config.upscaler)

        // Effects
        vidBlurPopUp.selectItem(withTag: Int(config.blur))
        vidBlurRadiusSlider.integerValue = config.blurRadius
        vidBlurRadiusSlider.isEnabled = config.blur > 0

        vidBloomPopUp.selectItem(withTag: Int(config.bloom))
        vidBloomRadiusSlider.integerValue = config.bloomRadius
        vidBloomRadiusSlider.isEnabled = config.bloom > 0
        vidBloomBrightnessSlider.integerValue = config.bloomBrightness
        vidBloomBrightnessSlider.isEnabled = config.bloom > 0
        vidBloomWeightSlider.integerValue = config.bloomWeight
        vidBloomWeightSlider.isEnabled = config.bloom > 0

        vidDotMaskPopUp.selectItem(withTag: Int(config.dotMask))
        for i in 0 ... 4 {
            vidDotMaskPopUp.item(at: i)?.image = renderer.ressourceManager.dotmaskImages[i]
        }
        vidDotMaskBrightnessSlider.integerValue = config.dotMaskBrightness
        vidDotMaskBrightnessSlider.isEnabled = config.dotMask > 0

        vidScanlinesPopUp.selectItem(withTag: Int(config.scanlines))
        vidScanlineBrightnessSlider.integerValue = config.scanlineBrightness
        vidScanlineBrightnessSlider.isEnabled = config.scanlines > 0
        vidScanlineWeightSlider.integerValue = config.scanlineWeight
        vidScanlineWeightSlider.isEnabled = config.scanlines == 2

        vidMisalignmentPopUp.selectItem(withTag: Int(config.disalignment))
        vidMisalignmentXSlider.integerValue = config.disalignmentH
        vidMisalignmentXSlider.isEnabled = config.disalignment > 0
        vidMisalignmentYSlider.integerValue = config.disalignmentV
        vidMisalignmentYSlider.isEnabled = config.disalignment > 0
    }

    func updatePalettePreviewImages() {

        guard let emu = controller?.emu else { return }

        // Create image representation in memory
        let size = CGSize(width: 16, height: 1)
        let cap = Int(size.width) * Int(size.height)
        let mask = calloc(cap, MemoryLayout<UInt32>.size)!
        let ptr = mask.bindMemory(to: UInt32.self, capacity: cap)

        // For all palettes ...
        for palette: Int in 0 ... 5 {

            // Create image data
            for n in 0 ... 15 {
                let p = vc64.Palette(rawValue: palette)!
                let rgba = emu.vic.rgbaColor(n, palette: p)
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

        config?.palette = sender.selectedTag()
    }

    @IBAction func vidBrightnessAction(_ sender: NSSlider!) {

        config?.brightness = sender.integerValue
    }

    @IBAction func vidContrastAction(_ sender: NSSlider!) {

        config?.contrast = sender.integerValue
    }

    @IBAction func vidSaturationAction(_ sender: NSSlider!) {

        config?.saturation = sender.integerValue
    }

    //
    // Action methods (Effects)
    //

    @IBAction func vidUpscalerAction(_ sender: NSPopUpButton!) {

        config?.upscaler = sender.selectedTag()
    }

    @IBAction func vidBlurAction(_ sender: NSPopUpButton!) {

        config?.blur = sender.selectedTag()
    }

    @IBAction func vidBlurRadiusAction(_ sender: NSSlider!) {

        config?.blurRadius = sender.integerValue
    }

    @IBAction func vidBloomAction(_ sender: NSPopUpButton!) {

        config?.bloom = sender.selectedTag()
    }

    @IBAction func vidBloomRadiusAction(_ sender: NSSlider!) {

        config?.bloomRadius = sender.integerValue
    }

    @IBAction func vidBloomBrightnessAction(_ sender: NSSlider!) {

        config?.bloomBrightness = sender.integerValue
    }

    @IBAction func vidBloomWeightAction(_ sender: NSSlider!) {

        config?.bloomWeight = sender.integerValue
    }

    @IBAction func vidDotMaskAction(_ sender: NSPopUpButton!) {

        config?.dotMask = sender.selectedTag()
    }

    @IBAction func vidDotMaskBrightnessAction(_ sender: NSSlider!) {

        config?.dotMaskBrightness = sender.integerValue
    }

    @IBAction func vidScanlinesAction(_ sender: NSPopUpButton!) {

        config?.scanlines = sender.selectedTag()
    }

    @IBAction func vidScanlineBrightnessAction(_ sender: NSSlider!) {

        config?.scanlineBrightness = sender.integerValue
    }

    @IBAction func vidScanlineWeightAction(_ sender: NSSlider!) {

        config?.scanlineWeight = sender.integerValue
    }

    @IBAction func vidDisalignmentAction(_ sender: NSPopUpButton!) {

        config?.disalignment = sender.selectedTag()
    }

    @IBAction func vidDisalignmentHAction(_ sender: NSSlider!) {

        config?.disalignmentH = sender.integerValue
    }

    @IBAction func vidDisalignmentVAction(_ sender: NSSlider!) {

        config?.disalignmentV = sender.integerValue
    }

    //
    // Action methods (Geometry)
    //

    @IBAction func vidHCenterAction(_ sender: NSSlider!) {

        config?.hCenter = sender.integerValue
    }

    @IBAction func vidVCenterAction(_ sender: NSSlider!) {

        config?.vCenter = sender.integerValue
    }

    @IBAction func vidHZoomAction(_ sender: NSSlider!) {

        config?.hZoom = sender.integerValue
    }

    @IBAction func vidVZoomAction(_ sender: NSSlider!) {

        config?.vZoom = sender.integerValue
    }

    //
    // Presets and Saving
    //

    override func preset(tag: Int) {

        let defaults = EmulatorProxy.defaults!

        switch tag {

        case 0: // Recommended settings (all)

            EmulatorProxy.defaults.removeVideoUserDefaults()

        case 10: // Recommended settings (geometry)

            EmulatorProxy.defaults.removeGeometryUserDefaults()

        case 20: // Recommended settings (colors + shader)

            EmulatorProxy.defaults.removeColorUserDefaults()
            EmulatorProxy.defaults.removeShaderUserDefaults()

        case 21: // TFT monitor

            EmulatorProxy.defaults.removeColorUserDefaults()
            EmulatorProxy.defaults.removeShaderUserDefaults()

        case 22: // CRT monitor

            EmulatorProxy.defaults.removeColorUserDefaults()
            EmulatorProxy.defaults.removeShaderUserDefaults()

            defaults.set(.MON_DOTMASK, 1)
            defaults.set(.MON_SCANLINES, 2)

        default:
            fatalError()
        }

        config?.applyVideoUserDefaults()
        updatePalettePreviewImages()
    }

    override func save() {

        config?.saveVideoUserDefaults()
    }
}
