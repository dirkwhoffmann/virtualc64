// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

@MainActor
class DashboardToolbar: NSToolbar {
    
    @IBOutlet weak var dashboard: DashboardViewController!
    @IBOutlet weak var selectorPopup: NSPopUpButton!
    @IBOutlet weak var selectorPopupItem: NSToolbarItem!

    override func validateVisibleItems() {
        
        // Disable shrinked popup buttons to prevent macOS from crashing
        selectorPopup.isEnabled = selectorPopupItem.isVisible
        
        // Update controls
        selectorPopup.selectItem(withTag: dashboard.type?.rawValue ?? 0)
    }
    
    //
    // Action methods
    //
    
    @IBAction
    func plusAction(_ sender: NSButton) {

        var next = PanelType.Combined
        if let type = dashboard.type?.rawValue {
            next = PanelType(rawValue: type + 1) ?? PanelType.Combined
        }
        
        dashboard.myController?.addDashboard(type: next)
    }

    @IBAction
    func panelAction(_ sender: Any) {
        
        if let popup = sender as? NSPopUpButton {
            dashboard.type = PanelType(rawValue: popup.selectedTag())!
        }
    }
}
