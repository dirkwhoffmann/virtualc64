// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class PreferencesController: DialogController {
    
    var pref: Preferences { return parent.prefs }
    var gamePadManager: GamePadManager { return parent.gamePadManager! }
    var myAppDelegate: MyAppDelegate { return NSApp.delegate as! MyAppDelegate }
    
    @IBOutlet weak var tabView: NSTabView!
    
    //
    // General preferences
    //
    
    
    
    
    
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

    func refresh() {
        
        /*
        if let id = tabView.selectedTabViewItem?.identifier as? String {
            
            switch id {
            case "General": refreshGeneralTab()
            case "Devices": refreshDevicesTab()
            default: fatalError()
            }
        }
        */
    }
    
    @discardableResult
    func keyDown(with key: MacKey) -> Bool {
        
        /*
        if let id = tabView.selectedTabViewItem?.identifier as? String {
            
            switch id {
            case "Devices": return devKeyDown(with: key)
            default: break
            }
        }
        */
        return false
    }
    
    @IBAction override func okAction(_ sender: Any!) {
        
        // pref.saveGeneralUserDefaults()
        // pref.saveDevicesUserDefaults()
        hideSheet()
    }
}

extension PreferencesController: NSTabViewDelegate {
    
    func tabView(_ tabView: NSTabView, didSelect tabViewItem: NSTabViewItem?) {
        
        refresh()
    }
}

extension PreferencesController: NSTextFieldDelegate {
    
    func controlTextDidChange(_ obj: Notification) {
        
        track()
        
        if let view = obj.object as? NSTextField {
            
            // let formatter = view.formatter as? NumberFormatter
            
            /*
             switch view {
             
             case emuSnapshotInterval:
             
             if formatter?.number(from: view.stringValue) != nil {
             emuSnapshotIntervalAction(view)
             }
             
             case devAutofireBullets:
             
             if formatter?.number(from: view.stringValue) != nil {
             devAutofireBulletsAction(view)
             }
             
             default:
             break
             }
             */
        }
    }
}
