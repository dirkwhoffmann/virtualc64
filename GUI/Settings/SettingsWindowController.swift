// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Cocoa

class SettingsWindowController: NSWindowController {

    var splitViewController: SettingsSplitViewController? {
        self.contentViewController as? SettingsSplitViewController
    }
    var romsVC: RomSettingsViewController? { splitViewController?.romsVC }
    var devicesVC: DevicesSettingsViewController? { splitViewController?.devicesVC }
    var currentVC: SettingsViewController? { splitViewController?.current }
    var isVisible: Bool { window?.isVisible ?? false }

    required init?(coder: NSCoder) {

        super.init(coder: coder)
    }

    override func windowDidLoad() {

        super.windowDidLoad()
    }

    func show() {

        self.showWindow(nil)
        self.window?.makeKeyAndOrderFront(nil)
        NSApp.activate(ignoringOtherApps: true)
    }

    func refresh() {

        currentVC?.refresh()
    }
}

