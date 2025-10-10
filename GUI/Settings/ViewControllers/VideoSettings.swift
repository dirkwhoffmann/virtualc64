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
    @IBOutlet weak var palettePopUp: NSPopUpButton!
    @IBOutlet weak var brightnessSlider: NSSlider!
    @IBOutlet weak var contrastSlider: NSSlider!
    @IBOutlet weak var saturationSlider: NSSlider!

    // Geometry
    @IBOutlet weak var zoom: NSPopUpButton!
    @IBOutlet weak var hZoom: NSSlider!
    @IBOutlet weak var vZoom: NSSlider!
    @IBOutlet weak var hZoomLabel: NSTextField!
    @IBOutlet weak var vZoomLabel: NSTextField!
    @IBOutlet weak var center: NSPopUpButton!
    @IBOutlet weak var hCenter: NSSlider!
    @IBOutlet weak var vCenter: NSSlider!
    @IBOutlet weak var hCenterLabel: NSTextField!
    @IBOutlet weak var vCenterLabel: NSTextField!

    // Effects
    @IBOutlet weak var upscalerPopUp: NSPopUpButton!
    @IBOutlet weak var blurPopUp: NSPopUpButton!
    @IBOutlet weak var blurRadiusSlider: NSSlider!

    @IBOutlet weak var bloomPopUp: NSPopUpButton!
    @IBOutlet weak var bloomRadiusSlider: NSSlider!
    @IBOutlet weak var bloomBrightnessSlider: NSSlider!
    @IBOutlet weak var bloomWeightSlider: NSSlider!

    @IBOutlet weak var dotMaskPopUp: NSPopUpButton!
    @IBOutlet weak var dotMaskBrightnessSlider: NSSlider!

    @IBOutlet weak var scanlinesPopUp: NSPopUpButton!
    @IBOutlet weak var scanlineBrightnessSlider: NSSlider!
    @IBOutlet weak var scanlineWeightSlider: NSSlider!

    @IBOutlet weak var misalignmentPopUp: NSPopUpButton!
    @IBOutlet weak var misalignmentXSlider: NSSlider!
    @IBOutlet weak var misalignmentYSlider: NSSlider!

    override var showLock: Bool { true }

    override func viewDidLoad() {

        log(.lifetime)
    }

    override func refresh() {

        super.refresh()
        
        guard let config = config, let renderer = renderer else { return }

        // Colors
        palettePopUp.selectItem(withTag: config.palette)
        brightnessSlider.integerValue = config.brightness
        contrastSlider.integerValue = config.contrast
        saturationSlider.integerValue = config.saturation

        // Geometry
        zoom.selectItem(withTag: 0)
        hZoom.integerValue = config.hZoom
        vZoom.integerValue = config.vZoom
        hZoom.isEnabled = true
        vZoom.isEnabled = true
        hZoomLabel.textColor = .labelColor
        vZoomLabel.textColor = .labelColor
        center.selectItem(withTag: 0)
        hCenter.integerValue = config.hCenter
        vCenter.integerValue = config.vCenter
        hCenter.isEnabled = true
        vCenter.isEnabled = true
        hCenterLabel.textColor = .labelColor
        vCenterLabel.textColor = .labelColor

        // Upscalers
        upscalerPopUp.selectItem(withTag: config.upscaler)

        // Effects
        blurPopUp.selectItem(withTag: Int(config.blur))
        blurRadiusSlider.integerValue = config.blurRadius
        blurRadiusSlider.isEnabled = config.blur > 0

        bloomPopUp.selectItem(withTag: Int(config.bloom))
        bloomRadiusSlider.integerValue = config.bloomRadius
        bloomRadiusSlider.isEnabled = config.bloom > 0
        bloomBrightnessSlider.integerValue = config.bloomBrightness
        bloomBrightnessSlider.isEnabled = config.bloom > 0
        bloomWeightSlider.integerValue = config.bloomWeight
        bloomWeightSlider.isEnabled = config.bloom > 0

        dotMaskPopUp.selectItem(withTag: Int(config.dotMask))
        for i in 0 ... 4 {
            dotMaskPopUp.item(at: i)?.image = renderer.ressourceManager.dotmaskImages[i]
        }
        dotMaskBrightnessSlider.integerValue = config.dotMaskBrightness
        dotMaskBrightnessSlider.isEnabled = config.dotMask > 0

        scanlinesPopUp.selectItem(withTag: Int(config.scanlines))
        scanlineBrightnessSlider.integerValue = config.scanlineBrightness
        scanlineBrightnessSlider.isEnabled = config.scanlines > 0
        scanlineWeightSlider.integerValue = config.scanlineWeight
        scanlineWeightSlider.isEnabled = config.scanlines == 2

        misalignmentPopUp.selectItem(withTag: Int(config.disalignment))
        misalignmentXSlider.integerValue = config.disalignmentH
        misalignmentXSlider.isEnabled = config.disalignment > 0
        misalignmentYSlider.integerValue = config.disalignmentV
        misalignmentYSlider.isEnabled = config.disalignment > 0
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
            palettePopUp.item(at: palette)?.image = resizedImage
        }
    }

    //
    // Action methods (Colors)
    //

    @IBAction func paletteAction(_ sender: NSPopUpButton!) {

        config?.palette = sender.selectedTag()
    }

    @IBAction func brightnessAction(_ sender: NSSlider!) {

        config?.brightness = sender.integerValue
    }

    @IBAction func contrastAction(_ sender: NSSlider!) {

        config?.contrast = sender.integerValue
    }

    @IBAction func saturationAction(_ sender: NSSlider!) {

        config?.saturation = sender.integerValue
    }

    //
    // Action methods (Effects)
    //

    @IBAction func upscalerAction(_ sender: NSPopUpButton!) {

        config?.upscaler = sender.selectedTag()
    }

    @IBAction func blurAction(_ sender: NSPopUpButton!) {

        config?.blur = sender.selectedTag()
    }

    @IBAction func blurRadiusAction(_ sender: NSSlider!) {

        config?.blurRadius = sender.integerValue
    }

    @IBAction func bloomAction(_ sender: NSPopUpButton!) {

        config?.bloom = sender.selectedTag()
    }

    @IBAction func bloomRadiusAction(_ sender: NSSlider!) {

        config?.bloomRadius = sender.integerValue
    }

    @IBAction func bloomBrightnessAction(_ sender: NSSlider!) {

        config?.bloomBrightness = sender.integerValue
    }

    @IBAction func bloomWeightAction(_ sender: NSSlider!) {

        config?.bloomWeight = sender.integerValue
    }

    @IBAction func dotMaskAction(_ sender: NSPopUpButton!) {

        config?.dotMask = sender.selectedTag()
    }

    @IBAction func dotMaskBrightnessAction(_ sender: NSSlider!) {

        config?.dotMaskBrightness = sender.integerValue
    }

    @IBAction func scanlinesAction(_ sender: NSPopUpButton!) {

        config?.scanlines = sender.selectedTag()
    }

    @IBAction func scanlineBrightnessAction(_ sender: NSSlider!) {

        config?.scanlineBrightness = sender.integerValue
    }

    @IBAction func scanlineWeightAction(_ sender: NSSlider!) {

        config?.scanlineWeight = sender.integerValue
    }

    @IBAction func disalignmentAction(_ sender: NSPopUpButton!) {

        config?.disalignment = sender.selectedTag()
    }

    @IBAction func disalignmentHAction(_ sender: NSSlider!) {

        config?.disalignmentH = sender.integerValue
    }

    @IBAction func disalignmentVAction(_ sender: NSSlider!) {

        config?.disalignmentV = sender.integerValue
    }

    //
    // Action methods (Geometry)
    //

    @IBAction func hCenterAction(_ sender: NSSlider!) {

        config?.hCenter = sender.integerValue
    }

    @IBAction func vCenterAction(_ sender: NSSlider!) {

        config?.vCenter = sender.integerValue
    }

    @IBAction func hZoomAction(_ sender: NSSlider!) {

        config?.hZoom = sender.integerValue
    }

    @IBAction func vZoomAction(_ sender: NSSlider!) {

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
