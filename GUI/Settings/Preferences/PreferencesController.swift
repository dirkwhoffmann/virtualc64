// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class PreferencesController: DialogController {
    
    var pref: Preferences { return parent.pref }
    var gamePadManager: GamePadManager { return parent.gamePadManager! }
    var myAppDelegate: MyAppDelegate { return NSApp.delegate as! MyAppDelegate }
    
    @IBOutlet weak var tabView: NSTabView!
    
    //
    // General preferences
    //
        
    // Drive
    @IBOutlet weak var genDriveBlankDiskFormat: NSPopUpButton!
    @IBOutlet weak var genEjectUnasked: NSButton!
    @IBOutlet weak var genDriveSounds: NSButton!
    @IBOutlet weak var genDriveSoundPan: NSPopUpButton!
    @IBOutlet weak var genDriveInsertSound: NSButton!
    @IBOutlet weak var genDriveEjectSound: NSButton!
    @IBOutlet weak var genDriveHeadSound: NSButton!
    @IBOutlet weak var genDriveConnectSound: NSButton!

    // Fullscreen
    @IBOutlet weak var genAspectRatioButton: NSButton!
    @IBOutlet weak var genExitOnEscButton: NSButton!

    // Snapshots and Screenshots
    @IBOutlet weak var genAutoSnapshots: NSButton!
    @IBOutlet weak var genSnapshotInterval: NSTextField!
    @IBOutlet weak var genScreenshotSourcePopup: NSPopUpButton!
    @IBOutlet weak var genScreenshotTargetPopup: NSPopUpButton!
        
    // Warp mode
    @IBOutlet weak var genWarpMode: NSPopUpButton!

    // Misc
    @IBOutlet weak var genPauseInBackground: NSButton!
    @IBOutlet weak var genCloseWithoutAskingButton: NSButton!
    
    //
    // Controls
    //
    
    // Tag of the button that is currently being recorded
    var conRecordedKey: Int?
    
    // Joystick emulation keys
    @IBOutlet weak var conLeft1: NSTextField!
    @IBOutlet weak var conLeft1button: NSButton!
    @IBOutlet weak var conRight1: NSTextField!
    @IBOutlet weak var conRight1button: NSButton!
    @IBOutlet weak var conUp1: NSTextField!
    @IBOutlet weak var conUp1button: NSButton!
    @IBOutlet weak var conDown1: NSTextField!
    @IBOutlet weak var conDown1button: NSButton!
    @IBOutlet weak var conFire1: NSTextField!
    @IBOutlet weak var conFire1button: NSButton!
    @IBOutlet weak var conLeft2: NSTextField!
    @IBOutlet weak var conLeft2button: NSButton!
    @IBOutlet weak var conRight2: NSTextField!
    @IBOutlet weak var conRight2button: NSButton!
    @IBOutlet weak var conUp2: NSTextField!
    @IBOutlet weak var conUp2button: NSButton!
    @IBOutlet weak var conDown2: NSTextField!
    @IBOutlet weak var conDown2button: NSButton!
    @IBOutlet weak var conFire2: NSTextField!
    @IBOutlet weak var conFire2button: NSButton!
    @IBOutlet weak var conDisconnectKeys: NSButton!
    
    // Joystick buttons
    @IBOutlet weak var conAutofire: NSButton!
    @IBOutlet weak var conAutofireCease: NSButton!
    @IBOutlet weak var conAutofireCeaseText: NSTextField!
    @IBOutlet weak var conAutofireBullets: NSTextField!
    @IBOutlet weak var conAutofireFrequency: NSSlider!
    
    // Mouse
    @IBOutlet weak var conMouseModel: NSPopUpButton!
    @IBOutlet weak var conMouseInfo: NSTextField!
    
    @IBOutlet weak var conOkButton: NSButton!
    @IBOutlet weak var conPowerButton: NSButton!
    
    //
    // Devices
    //
    
    @IBOutlet weak var devSelector: NSSegmentedControl!
    
    // Property Box
    @IBOutlet weak var devInfoBox: NSBox!
    @IBOutlet weak var devInfoBoxTitle: NSTextField!
    @IBOutlet weak var devManufacturer: NSTextField!
    @IBOutlet weak var devProduct: NSTextField!
    @IBOutlet weak var devVersion: NSTextField!
    @IBOutlet weak var devVendorID: NSTextField!
    @IBOutlet weak var devProductID: NSTextField!
    @IBOutlet weak var devTransport: NSTextField!
    @IBOutlet weak var devUsage: NSTextField!
    @IBOutlet weak var devUsagePage: NSTextField!
    @IBOutlet weak var devLocationID: NSTextField!
    @IBOutlet weak var devUniqueID: NSTextField!
    @IBOutlet weak var devActivity: NSTextField!
    @IBOutlet weak var devActivity2: NSTextField!

    // Controller
    @IBOutlet weak var devImage: NSImageView!
    @IBOutlet weak var devLeftText: NSTextField!
    @IBOutlet weak var devRightText: NSTextField!
    @IBOutlet weak var devHatText: NSTextField!
    @IBOutlet weak var devLeftScheme: NSPopUpButton!
    @IBOutlet weak var devRightScheme: NSPopUpButton!
    @IBOutlet weak var devHatScheme: NSPopUpButton!
    
    //
    // Keymap
    //
    
    @IBOutlet weak var keyMappingText: NSTextField!
    @IBOutlet weak var keyMappingPopup: NSPopUpButton!
    @IBOutlet weak var keyMapText: NSTextField!
    @IBOutlet weak var keyBox: NSBox!

    // Array holding a reference to the view of each key button
    var keyView = Array(repeating: nil as RecordButton?, count: 66)

    // Arrays holding the key caps for each record key
    var keyImage = Array(repeating: nil as NSImage?, count: 66)
    var pressedKeyImage = Array(repeating: nil as NSImage?, count: 66)
    var mappedKeyImage = Array(repeating: nil as NSImage?, count: 66)
    
    // The C64 key that has been selected to be mapped
    var selectedKey: C64Key?
    
    @IBOutlet weak var keyOkButton: NSButton!
    @IBOutlet weak var keyPowerButton: NSButton!
    
    // The tab to open first
    var firstTab: String?

    func showSheet(tab: String) {

        firstTab = tab
        showSheet()
    }

    override func awakeFromNib() {

        super.awakeFromNib()
        
        if let id = firstTab { tabView.selectTabViewItem(withIdentifier: id) }
        refresh()
    }

    override func sheetDidShow() {

        if let id = firstTab { tabView.selectTabViewItem(withIdentifier: id) }
    }

    override func cleanup() {
     
        track()
        parent.gamePadManager.gamePads[3]?.notify = false
        parent.gamePadManager.gamePads[4]?.notify = false
    }
    
    func refresh() {
        
        if let id = tabView.selectedTabViewItem?.identifier as? String {
            
            switch id {
            case "General": refreshGeneralTab()
            case "Controls": refreshControlsTab()
            case "Devices": refreshDevicesTab()
            case "Keyboard": refreshKeyboardTab()
            default: break
            }
        }
    }
    
    func select() {
        
        if let id = tabView.selectedTabViewItem?.identifier as? String {
            
            switch id {
            case "General": selectGeneralTab()
            case "Controls": selectControlsTab()
            case "Devices": selectDevicesTab()
            case "Keyboard": selectKeyboardTab()
            default: fatalError()
            }
        }
    }
    
    @discardableResult
    func keyDown(with key: MacKey) -> Bool {
        
        if let id = tabView.selectedTabViewItem?.identifier as? String {
            
            switch id {
            case "Controls": return conKeyDown(with: key)
            case "Keyboard": return mapSelectedKey(to: key)
            default: break
            }
        }
        return false
    }
    
    @IBAction override func okAction(_ sender: Any!) {
        
        track()
        pref.saveGeneralUserDefaults()
        pref.saveControlsUserDefaults()
        pref.saveKeyboardUserDefaults()
        myAppDelegate.database.save()
        hideSheet()
    }
}

extension PreferencesController: NSTabViewDelegate {
    
    func tabView(_ tabView: NSTabView, didSelect tabViewItem: NSTabViewItem?) {
        
        track()
        select()
    }
}

extension PreferencesController: NSWindowDelegate {
    
    func windowWillClose(_ notification: Notification) {
         
        cleanup()
    }
        
    func windowDidBecomeKey(_ notification: Notification) {
        
        track()
        select()
    }
}

extension PreferencesController: NSTextFieldDelegate {
    
    func controlTextDidChange(_ obj: Notification) {
        
        track()
        
        if let view = obj.object as? NSTextField {
            
            let formatter = view.formatter as? NumberFormatter
            
            switch view {
                
            case genSnapshotInterval:
                
                if formatter?.number(from: view.stringValue) != nil {
                    genSnapshotIntervalAction(view)
                }

            case conAutofireBullets:
                
                if formatter?.number(from: view.stringValue) != nil {
                    conAutofireBulletsAction(view)
                }
                
            default:
                break
            }
        }
    }
}
