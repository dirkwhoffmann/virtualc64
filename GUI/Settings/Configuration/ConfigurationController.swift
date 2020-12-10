// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class ConfigurationController: DialogController {

    var config: Configuration { return parent.config }
    
    @IBOutlet weak var prefTabView: NSTabView!
    
    //
    // Rom preferences
    //
        
    @IBOutlet weak var basicDropView: RomDropView!
    @IBOutlet weak var basicTitle: NSTextField!
    @IBOutlet weak var basicSubtitle: NSTextField!
    @IBOutlet weak var basicSubsubtitle: NSTextField!
    @IBOutlet weak var basicDeleteButton: NSButton!
    
    @IBOutlet weak var kernalDropView: RomDropView!
    @IBOutlet weak var kernalTitle: NSTextField!
    @IBOutlet weak var kernalSubtitle: NSTextField!
    @IBOutlet weak var kernalSubsubtitle: NSTextField!
    @IBOutlet weak var kernalDeleteButton: NSButton!
    
    @IBOutlet weak var charDropView: RomDropView!
    @IBOutlet weak var charTitle: NSTextField!
    @IBOutlet weak var charSubtitle: NSTextField!
    @IBOutlet weak var charSubsubtitle: NSTextField!
    @IBOutlet weak var charDeleteButton: NSButton!
    
    @IBOutlet weak var vc1541DropView: RomDropView!
    @IBOutlet weak var vc1541Title: NSTextField!
    @IBOutlet weak var vc1541Subtitle: NSTextField!
    @IBOutlet weak var vc1541Subsubtitle: NSTextField!
    @IBOutlet weak var vc1541DeleteButton: NSButton!
    
    @IBOutlet weak var romInstallButton: NSButton!

    @IBOutlet weak var romLockImage: NSButton!
    @IBOutlet weak var romLockText: NSTextField!
    @IBOutlet weak var romLockSubText: NSTextField!

    @IBOutlet weak var romOkButton: NSButton!
    @IBOutlet weak var romPowerButton: NSButton!

    //
    // Hardware preferences
    //
    
    // VIC
    @IBOutlet weak var hwVicModelPopup: NSPopUpButton!
    @IBOutlet weak var hwVicIcon: NSImageView!
    @IBOutlet weak var hwVicGrayDotBug: NSButton!
    
    // CIA
    @IBOutlet weak var hwCiaModelPopup: NSPopUpButton!
    @IBOutlet weak var hwCiaTimerBBug: NSButton!
    
    // SID
    @IBOutlet weak var hwSidModelPopup: NSPopUpButton!
    @IBOutlet weak var hwSidEnable1: NSButton!
    @IBOutlet weak var hwSidEnable2: NSButton!
    @IBOutlet weak var hwSidEnable3: NSButton!
    @IBOutlet weak var hwSidAddress1: NSPopUpButton!
    @IBOutlet weak var hwSidAddress2: NSPopUpButton!
    @IBOutlet weak var hwSidAddress3: NSPopUpButton!

    // Board logic
    @IBOutlet weak var hwGlueLogicPopup: NSPopUpButton!

    // Startup
    @IBOutlet weak var hwRamPatternPopup: NSPopUpButton!

    // Drive
    @IBOutlet weak var hwDrive8Type: NSPopUpButton!
    @IBOutlet weak var hwDrive8Connect: NSButton!
    @IBOutlet weak var hwDrive9Type: NSPopUpButton!
    @IBOutlet weak var hwDrive9Connect: NSButton!

    // Ports
    @IBOutlet weak var hwGameDevice1: NSPopUpButton!
    @IBOutlet weak var hwGameDevice2: NSPopUpButton!
    
    // Lock
    @IBOutlet weak var hwLockImage: NSButton!
    @IBOutlet weak var hwLockText: NSTextField!
    @IBOutlet weak var hwLockSubText: NSTextField!

    // Buttons
    @IBOutlet weak var hwOkButton: NSButton!
    @IBOutlet weak var hwPowerButton: NSButton!

    //
    // Audio preferences
    //
    
    // Engine
    @IBOutlet weak var audEngine: NSPopUpButton!
    @IBOutlet weak var audFilter: NSButton!
    @IBOutlet weak var audSampling: NSPopUpButton!
    
    // In
    @IBOutlet weak var audVol0: NSSlider!
    @IBOutlet weak var audVol1: NSSlider!
    @IBOutlet weak var audVol2: NSSlider!
    @IBOutlet weak var audVol3: NSSlider!
    @IBOutlet weak var audPan0: NSSlider!
    @IBOutlet weak var audPan1: NSSlider!
    @IBOutlet weak var audPan2: NSSlider!
    @IBOutlet weak var audPan3: NSSlider!
    
    // Out
    @IBOutlet weak var audVolL: NSSlider!
    @IBOutlet weak var audVolR: NSSlider!

    // Buttons
    @IBOutlet weak var audOKButton: NSButton!
    @IBOutlet weak var audPowerButton: NSButton!
    
    //
    // Video preferences
    //
    
    // Palette
    @IBOutlet weak var vidPalettePopUp: NSPopUpButton!
    @IBOutlet weak var vidBrightnessSlider: NSSlider!
    @IBOutlet weak var vidContrastSlider: NSSlider!
    @IBOutlet weak var vidSaturationSlider: NSSlider!
    
    // Effects
    @IBOutlet weak var vidUpscalerPopUp: NSPopUpButton!
    @IBOutlet weak var vidBlurPopUp: NSPopUpButton!
    @IBOutlet weak var vidBlurRadiusSlider: NSSlider!
    
    @IBOutlet weak var vidBloomPopUp: NSPopUpButton!
    @IBOutlet weak var vidBloomRadiusRSlider: NSSlider!
    @IBOutlet weak var vidBloomRadiusGSlider: NSSlider!
    @IBOutlet weak var vidBloomRadiusBSlider: NSSlider!
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
    
    // Geometry
    @IBOutlet weak var vidHCenter: NSSlider!
    @IBOutlet weak var vidVCenter: NSSlider!
    @IBOutlet weak var vidHZoom: NSSlider!
    @IBOutlet weak var vidVZoom: NSSlider!

    // Buttons
    @IBOutlet weak var vidOkButton: NSButton!
    @IBOutlet weak var vidPowerButton: NSButton!
    
    var bootable: Bool {
        let off   = c64.isPoweredOff
        let ready = c64.isReady()
        return off && ready
    }

    // The tab to open first
    var firstTab = ""
    
    func showSheet(tab: String) {
        
        firstTab = tab
        showSheet()
        refresh()
    }
    
    override func awakeFromNib() {
    
        awakeVideoPrefsFromNib()
        awakeHardwarePrefsFromNib()
        refresh()
    }
    
    override func sheetWillShow() {
        
        track()
        if firstTab != "" { prefTabView?.selectTabViewItem(withIdentifier: firstTab) }
        refresh()
    }
    
    func refresh() {
        
        if let id = prefTabView.selectedTabViewItem?.identifier as? String {
            
            switch id {
            case "Roms": refreshRomTab()
            case "Hardware": refreshHardwareTab()
            case "Audio": refreshAudioTab()
            case "Video": refreshVideoTab()
            default: break
            }
        }
    }
    
    @IBAction func unlockAction(_ sender: Any!) {
        
        track()
        
        c64.powerOff()
        refresh()
    }
    
    @IBAction override func okAction(_ sender: Any!) {
        
        hideSheet()
    }
    
    @IBAction func powerAction(_ sender: Any!) {
        
        hideSheet()
        c64.run()
    }
}

extension ConfigurationController: NSTabViewDelegate {

    func tabView(_ tabView: NSTabView, didSelect tabViewItem: NSTabViewItem?) {

        refresh()
    }
}
