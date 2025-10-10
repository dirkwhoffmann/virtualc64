// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Cocoa

@MainActor
class SettingsOutlineView: NSOutlineView {

    @IBOutlet var sidebarViewController: SidebarViewController!

    var current: SettingsViewController? {
        sidebarViewController.splitViewController?.current
    }

    override func keyDown(with event: NSEvent) {

        current?.keyDown(with: event)
    }

    override func flagsChanged(with event: NSEvent) {

        current?.flagsChanged(with: event)
    }
}
