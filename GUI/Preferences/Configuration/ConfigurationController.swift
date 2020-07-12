// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class ConfigurationController: DialogController {

    @IBOutlet weak var prefTabView: NSTabView!
    
    //
    // Rom preferences
    //
    
    @IBOutlet weak var romHeaderImage: NSImageView!
    @IBOutlet weak var romHeaderText: NSTextField!
    @IBOutlet weak var romHeaderSubText: NSTextField!
    
    @IBOutlet weak var romBasicImage: RomDropView!
    @IBOutlet weak var romBasicHashText: NSTextField!
    @IBOutlet weak var romBasicPathText: NSTextField!
    @IBOutlet weak var romBasicDescription: NSTextField!
    @IBOutlet weak var romBasicButton: NSButton!
    
    @IBOutlet weak var romKernalImage: RomDropView!
    @IBOutlet weak var romKernalHashText: NSTextField!
    @IBOutlet weak var romKernalPathText: NSTextField!
    @IBOutlet weak var romKernalDescription: NSTextField!
    @IBOutlet weak var romKernelButton: NSButton!
    
    @IBOutlet weak var romCharImage: RomDropView!
    @IBOutlet weak var romCharHashText: NSTextField!
    @IBOutlet weak var romCharPathText: NSTextField!
    @IBOutlet weak var romCharDescription: NSTextField!
    @IBOutlet weak var romCharButton: NSButton!
    
    @IBOutlet weak var romVc1541Image: RomDropView!
    @IBOutlet weak var romVc1541HashText: NSTextField!
    @IBOutlet weak var romVc1541PathText: NSTextField!
    @IBOutlet weak var romVc1541Description: NSTextField!
    @IBOutlet weak var romVc1541Button: NSButton!
    
    @IBOutlet weak var romOkButton: NSButton!
    @IBOutlet weak var romCancelButton: NSButton!

    //
    // Hardware preferences
    //
    
    // VIC
    @IBOutlet weak var hwVicModelPopup: NSPopUpButton!
    @IBOutlet weak var hwVicIcon: NSImageView!
    @IBOutlet weak var hwVicDescription: NSTextField!
    @IBOutlet weak var hwVicGrayDotBug: NSButton!
    
    // CIA
    @IBOutlet weak var hwCiaModelPopup: NSPopUpButton!
    @IBOutlet weak var hwCiaTimerBBug: NSButton!
    
    // SID
    @IBOutlet weak var hwSidModelPopup: NSPopUpButton!
    @IBOutlet weak var hwSidFilter: NSButton!
    @IBOutlet weak var hwSidEnginePopup: NSPopUpButton!
    @IBOutlet weak var hwSidSamplingPopup: NSPopUpButton!
    
    // Board logic
    @IBOutlet weak var hwGlueLogicPopup: NSPopUpButton!
    @IBOutlet weak var hwRamInitPatternPopup: NSPopUpButton!
    @IBOutlet weak var hwInfoText: NSTextField!

    @IBOutlet weak var hwOkButton: NSButton!
    @IBOutlet weak var hwCancelButton: NSButton!
    
    //
    // Video preferences
    //
    
    // Palette
    @IBOutlet weak var vidPalettePopup: NSPopUpButton!
    @IBOutlet weak var vidBrightnessSlider: NSSlider!
    @IBOutlet weak var vidContrastSlider: NSSlider!
    @IBOutlet weak var vidSaturationSlider: NSSlider!
    
    // Effects
    @IBOutlet weak var vidUpscalerPopup: NSPopUpButton!
    @IBOutlet weak var vidBlurPopUp: NSPopUpButton!
    @IBOutlet weak var vidBlurRadiusSlider: NSSlider!
    
    @IBOutlet weak var vidBloomPopup: NSPopUpButton!
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
    @IBOutlet weak var vidAspectRatioButton: NSButton!
    @IBOutlet weak var vidEyeXSlider: NSSlider!
    @IBOutlet weak var vidEyeYSlider: NSSlider!
    @IBOutlet weak var vidEyeZSlider: NSSlider!
    
    @IBOutlet weak var vidOkButton: NSButton!
    @IBOutlet weak var vidCancelButton: NSButton!
    
    //
    // Emulator preferences
    //
    
    // VC1541
    @IBOutlet weak var emuWarpLoad: NSButton!
    @IBOutlet weak var emuDriveSounds: NSButton!
    
    // Screenshots
    @IBOutlet weak var emuScreenshotSourcePopup: NSPopUpButton!
    @IBOutlet weak var emuScreenshotTargetPopup: NSPopUpButton!
    
    // User Dialogs
    @IBOutlet weak var emuAutoMountButton: NSButton!
    @IBOutlet weak var emuCloseWithoutAskingButton: NSButton!
    @IBOutlet weak var emuEjectWithoutAskingButton: NSButton!
    
    // Misc
    @IBOutlet weak var emuPauseInBackground: NSButton!
    @IBOutlet weak var emuAutoSnapshots: NSButton!
    @IBOutlet weak var emuSnapshotInterval: NSTextField!
    
    @IBOutlet weak var emuOkButton: NSButton!
    @IBOutlet weak var emuCancelButton: NSButton!
    
    // Media files
    @IBOutlet weak var emuD64Popup: NSPopUpButton!
    @IBOutlet weak var emuPrgPopup: NSPopUpButton!
    @IBOutlet weak var emuT64Popup: NSPopUpButton!
    @IBOutlet weak var emuTapPopup: NSPopUpButton!
    @IBOutlet weak var emuCrtPopup: NSPopUpButton!

    @IBOutlet weak var emuD64AutoTypeButton: NSButton!
    @IBOutlet weak var emuPrgAutoTypeButton: NSButton!
    @IBOutlet weak var emuT64AutoTypeButton: NSButton!
    @IBOutlet weak var emuTapAutoTypeButton: NSButton!
    @IBOutlet weak var emuCrtAutoTypeButton: NSButton!

    @IBOutlet weak var emuD64AutoTypeText: NSTextField!
    @IBOutlet weak var emuPrgAutoTypeText: NSTextField!
    @IBOutlet weak var emuT64AutoTypeText: NSTextField!
    @IBOutlet weak var emuTapAutoTypeText: NSTextField!
    @IBOutlet weak var emuCrtAutoTypeText: NSTextField!

    //
    // Devices preferences
    //
    
    /// Indicates if a keycode should be recorded for keyset 1
    var devRecordKey1: JoystickDirection?
    
    /// Indicates if a keycode should be recorded for keyset 1
    var devRecordKey2: JoystickDirection?
    
    /// Joystick emulation keys
    @IBOutlet weak var devLeft1: NSTextField!
    @IBOutlet weak var devLeft1button: NSButton!
    @IBOutlet weak var devRight1: NSTextField!
    @IBOutlet weak var devRight1button: NSButton!
    @IBOutlet weak var devUp1: NSTextField!
    @IBOutlet weak var devUp1button: NSButton!
    @IBOutlet weak var devDown1: NSTextField!
    @IBOutlet weak var devDown1button: NSButton!
    @IBOutlet weak var devFire1: NSTextField!
    @IBOutlet weak var devFire1button: NSButton!
    @IBOutlet weak var devLeft2: NSTextField!
    @IBOutlet weak var devLeft2button: NSButton!
    @IBOutlet weak var devRight2: NSTextField!
    @IBOutlet weak var devRight2button: NSButton!
    @IBOutlet weak var devUp2: NSTextField!
    @IBOutlet weak var devUp2button: NSButton!
    @IBOutlet weak var devDown2: NSTextField!
    @IBOutlet weak var devDown2button: NSButton!
    @IBOutlet weak var devFire2: NSTextField!
    @IBOutlet weak var devFire2button: NSButton!
    @IBOutlet weak var devDisconnectKeys: NSButton!
    
    // Joystick buttons
    @IBOutlet weak var devAutofire: NSButton!
    @IBOutlet weak var devAutofireCease: NSButton!
    @IBOutlet weak var devAutofireCeaseText: NSTextField!
    @IBOutlet weak var devAutofireBullets: NSTextField!
    @IBOutlet weak var devAutofireFrequency: NSSlider!
    
    // Mouse
    @IBOutlet weak var devMouseModel: NSPopUpButton!
    @IBOutlet weak var devMouseInfo: NSTextField!

    @IBOutlet weak var devOkButton: NSButton!
    @IBOutlet weak var devCancelButton: NSButton!
    
    //
    // Keymap preferences
    //
    
    @IBOutlet weak var info: NSTextField!
    @IBOutlet weak var keyMappingPopup: NSPopUpButton!
    @IBOutlet weak var keyMatrixScrollView: NSScrollView!
    @IBOutlet weak var keyMatrixCollectionView: NSCollectionView!

    // Double array of key images, indexed by their row and column number
    var keyImage = Array(repeating: Array(repeating: nil as NSImage?, count: 8), count: 8)
    
    // Selected C64 key
    var selectedKey: C64Key?
    
    @IBOutlet weak var keyOkButton: NSButton!
    @IBOutlet weak var keyCancelButton: NSButton!
    
    override func awakeFromNib() {
    
        awakeVideoPrefsFromNib()
        refresh()
    }
    
    func refresh() {
        
        if let id = prefTabView.selectedTabViewItem?.identifier as? String {
            
            switch id {
            case "Roms": refreshRomTab()
            case "Hardware": refreshHardwareTab()
            case "Video": refreshVideoTab()
            case "Emulator": refreshEmulatorTab()
            case "Devices": refreshDevicesTab()
            case "Keyboard": refreshKeyboardTab()
            default: break
            }
        }
    }
    
    override func keyDown(with key: MacKey) {
        
        if let id = prefTabView.selectedTabViewItem?.identifier as? String {
            
            switch id {
            case "Devices": devKeyDown(with: key)
            case "Keyboard": mapKeyDown(with: key)
            default: break
            }
        }
    }
    
    @IBAction override func cancelAction(_ sender: Any!) {
        
        track()
        
        hideSheet()
        myController?.loadUserDefaults()
        
    }
    
    @IBAction override func okAction(_ sender: Any!) {
        
        track()
        
        hideSheet()
        myController?.saveUserDefaults()
        
        if proxy == nil || !proxy!.isReady() {
            NSApp.terminate(self)
        }
    }
}

extension ConfigurationController: NSTabViewDelegate {

    func tabView(_ tabView: NSTabView, didSelect tabViewItem: NSTabViewItem?) {

        refresh()
    }
}

extension ConfigurationController: NSTextFieldDelegate {
    
    func controlTextDidChange(_ obj: Notification) {
        
        track()
        
        if let view = obj.object as? NSTextField {
            
            let formatter = view.formatter as? NumberFormatter
            
            switch view {
                
            case emuSnapshotInterval:
                
                if formatter?.number(from: view.stringValue) != nil {
                    emuSnapshotIntervalAction(view)
                }
                
            case emuD64AutoTypeText, emuPrgAutoTypeText, emuT64AutoTypeText,
                 emuTapAutoTypeText, emuCrtAutoTypeText:
                
                emuAutoTypeTextAction(view)
                
            case devAutofireBullets:
                
                if formatter?.number(from: view.stringValue) != nil {
                    devAutofireBulletsAction(view)
                }
                
            default:
                break
            }
        }
    }
}
