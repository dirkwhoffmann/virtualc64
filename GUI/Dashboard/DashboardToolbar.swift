// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension NSToolbarItem.Identifier {

    static let dashboardPanel = NSToolbarItem.Identifier("Dashboard.Progress")
    static let newDashboard = NSToolbarItem.Identifier("Dashboard.New")
}

@MainActor
class DashboardToolbar: NSToolbar, NSToolbarDelegate {

    var dashboard: DashboardViewController!
    var panel: MyToolbarMenuItem!
    var newDashboard: MyToolbarItem!

    // Set to true to gray out all toolbar items
    var globalDisable = false

    init() {

        super.init(identifier: "DashboardToolbar")
        self.delegate = self
        self.allowsUserCustomization = false
        self.displayMode = .iconOnly
    }

    override init(identifier: NSToolbar.Identifier) {

        super.init(identifier: identifier)
        self.delegate = self
        self.allowsUserCustomization = false
        self.displayMode = .iconOnly
    }

    convenience init(dashboard: DashboardViewController) {

        self.init(identifier: "DashboardToolbar")
        self.dashboard = dashboard
    }

    func toolbarAllowedItemIdentifiers(_ toolbar: NSToolbar) -> [NSToolbarItem.Identifier] {

        return [ .dashboardPanel,
                 .newDashboard,
                 .flexibleSpace ]
    }

    func toolbarDefaultItemIdentifiers(_ toolbar: NSToolbar) -> [NSToolbarItem.Identifier] {

        return [ .dashboardPanel,
                 .newDashboard,
                 .flexibleSpace ]
    }

    func toolbar(_ toolbar: NSToolbar,
                 itemForItemIdentifier itemIdentifier: NSToolbarItem.Identifier,
                 willBeInsertedIntoToolbar flag: Bool) -> NSToolbarItem? {

        switch itemIdentifier {

        case .dashboardPanel:

            let menuItems = [ (SFSymbol.magnifyingglass3, "Overview", 0),
                              (SFSymbol.magnifyingglass3, "CIA1 Activity", 1),
                              (SFSymbol.magnifyingglass3, "CIA2 Activity", 2),
                              (SFSymbol.magnifyingglass3, "C64 Clock Frequency", 3),
                              (SFSymbol.magnifyingglass3, "C64 Refresh Rate", 4),
                              (SFSymbol.magnifyingglass3, "Host CPU Load", 5),
                              (SFSymbol.magnifyingglass3, "Host Refresh Rate", 6),
                              (SFSymbol.magnifyingglass3, "Audio Buffer Fill Level", 7),
                              (SFSymbol.magnifyingglass3, "Waveform", 8) ]

            panel = MyToolbarMenuItem(identifier: .dashboardPanel,
                                      menuItems: menuItems,
                                      image: .magnifyingglass2,
                                      action: #selector(panelAction(_:)),
                                      target: self,
                                      label: "Panel")
            return panel

        case .newDashboard:

            newDashboard = MyToolbarItem(identifier: .newDashboard,
                                         image: .plus,
                                         action: #selector(plusAction),
                                         target: self,
                                         label: "New")
            return newDashboard

        default:
            return nil
        }
    }

    override func validateVisibleItems() {

    }

    func updateToolbar(full: Bool) {

        if full {

            // Take care of the global disable flag
            for item in items { item.isEnabled = !globalDisable }
        }
    }

    //
    // Action methods
    //


    @objc private func panelAction(_ sender: NSMenuItem) {

        dashboard.type = PanelType(rawValue: sender.tag)
    }

    @objc private func plusAction(_ sender: NSButton) {

        var next = PanelType.Combined
        if let type = dashboard.type?.rawValue {
            next = PanelType(rawValue: type + 1) ?? PanelType.Combined
        }

        dashboard.myController?.addDashboard(type: next)
    }
}

/*
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
*/
