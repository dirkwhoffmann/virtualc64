// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class PreferencesController: DialogController {
    
    var pref: Preferences { return parent.pref }
    var gamePadManager: GamePadManager { return parent.gamePadManager }
    var firstTab: String? // The tab to open first
    var isVisible: Bool { window?.isVisible ?? false }
    
    @IBOutlet weak var tabView: NSTabView!
    
    //
    // General preferences
    //
        
    // Snapshots
    @IBOutlet weak var genAutoSnapshots: NSButton!
    @IBOutlet weak var genSnapshotInterval: NSTextField!
    
    // Screenshots
    @IBOutlet weak var genScreenshotSourcePopup: NSPopUpButton!
    @IBOutlet weak var genScreenshotTargetPopup: NSPopUpButton!
        
    // Screen captures
    @IBOutlet weak var genFFmpegPath: NSComboBox!
    @IBOutlet weak var genSource: NSPopUpButton!
    @IBOutlet weak var genAspectX: NSTextField!
    @IBOutlet weak var genAspectY: NSTextField!
    @IBOutlet weak var genBitRate: NSComboBox!
    
    // Fullscreen
    @IBOutlet weak var genAspectRatioButton: NSButton!
    @IBOutlet weak var genExitOnEscButton: NSButton!

    // Misc
    @IBOutlet weak var genEjectUnasked: NSButton!
    @IBOutlet weak var genPauseInBackground: NSButton!
    @IBOutlet weak var genCloseWithoutAskingButton: NSButton!
    
    //
    // Controls
    //
    
    // Tag of the button that is currently being recorded
    var conRecordedKey: Int?
    
    // Emulation keys
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
    
    @IBOutlet weak var conMouseLeft: NSTextField!
    @IBOutlet weak var conMouseLeftButton: NSButton!
    @IBOutlet weak var conMouseRight: NSTextField!
    @IBOutlet weak var conMouseRightButton: NSButton!

    @IBOutlet weak var conDisconnectKeys: NSButton!
    
    // Mouse
    @IBOutlet weak var conRetainMouseKeyComb: NSPopUpButton!
    @IBOutlet weak var conRetainMouseWithKeys: NSButton!
    @IBOutlet weak var conRetainMouseByClick: NSButton!
    @IBOutlet weak var conRetainMouseByEntering: NSButton!
    @IBOutlet weak var conReleaseMouseKeyComb: NSPopUpButton!
    @IBOutlet weak var conReleaseMouseWithKeys: NSButton!
    @IBOutlet weak var conReleaseMouseByShaking: NSButton!
        
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
        
    //
    // Methods
    //
    
    func showSheet(tab: String) {

        firstTab = tab
        showAsSheet()
    }

    override func awakeFromNib() {

        super.awakeFromNib()
        
        if let id = firstTab { tabView.selectTabViewItem(withIdentifier: id) }
        refresh()
    }

    override func dialogDidShow() {

        if let id = firstTab { tabView.selectTabViewItem(withIdentifier: id) }
    }

    override func cleanup() {
     
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
        
        pref.saveGeneralUserDefaults()
        pref.saveControlsUserDefaults()
        pref.saveDevicesUserDefaults()
        pref.saveKeyboardUserDefaults()
        myAppDelegate.database.save()
        close()
    }
}

extension PreferencesController: NSTabViewDelegate {
    
    func tabView(_ tabView: NSTabView, didSelect tabViewItem: NSTabViewItem?) {
        
        select()
    }
}

extension PreferencesController {
    
    override func windowWillClose(_ notification: Notification) {
         
        cleanup()
    }
        
    override func windowDidBecomeKey(_ notification: Notification) {
        
        select()
    }
}

extension PreferencesController: NSTextFieldDelegate {
    
    func controlTextDidChange(_ obj: Notification) {
                
        if let view = obj.object as? NSTextField {
            
            let formatter = view.formatter as? NumberFormatter
            
            switch view {
                
            case genSnapshotInterval:
                
                if formatter?.number(from: view.stringValue) != nil {
                    genSnapshotIntervalAction(view)
                }

                /*
            case conAutofireBullets:
                
                if formatter?.number(from: view.stringValue) != nil {
                    conAutofireBulletsAction(view)
                }
                */
            default:
                break
            }
        }
    }
}
