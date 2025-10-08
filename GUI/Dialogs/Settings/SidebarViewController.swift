// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Cocoa

class SidebarItem {

    let title: String
    let iconName: String
    let children: [SidebarItem]

    init(title: String, icon: String, children: [SidebarItem] = []) {
        self.title = title
        self.iconName = icon
        self.children = children
    }
}

class SidebarViewController: NSViewController {

    @IBOutlet weak var outlineView: SettingsOutlineView!

    var splitViewController: SettingsSplitViewController? {
        parent as? SettingsSplitViewController
    }

    let items: [SidebarItem] = [

        SidebarItem(title: "Emulator", icon: "house", children: [

            SidebarItem(title: "General",       icon: "gear"),
            SidebarItem(title: "Captures",      icon: "binoculars.fill"),
            SidebarItem(title: "Controls",      icon: "arrowkeys"),
            SidebarItem(title: "Devices",       icon: "gamecontroller")
        ]),
        SidebarItem(title: "Virtual Machine", icon: "sparkles.tv", children: [

            SidebarItem(title: "Roms",          icon: "memorychip"),
            SidebarItem(title: "Hardware",      icon: "engine.combustion"),
            SidebarItem(title: "Peripherals",   icon: "externaldrive"),
            SidebarItem(title: "Performance",   icon: "hare"),
            SidebarItem(title: "Compatibility", icon: "compass.drawing"),
            SidebarItem(title: "Audio",         icon: "waveform"),
            SidebarItem(title: "Video",         icon: "eye")
        ])
    ]

    var selectionHandler: ((SidebarItem) -> Void)?

    override func viewDidLoad() {

        super.viewDidLoad()

        outlineView.delegate = self
        outlineView.dataSource = self
        outlineView.usesAutomaticRowHeights = false
        outlineView.rowSizeStyle = .custom
        outlineView.backgroundColor = .clear
        outlineView.usesAlternatingRowBackgroundColors = false
        outlineView.reloadData()

        // Select first item by default
        // outlineView.selectRowIndexes(IndexSet(integer: 0), byExtendingSelection: false)
        outlineView.selectRowIndexes(IndexSet(integer: 1), byExtendingSelection: false)

        // Expand all items
        for item in self.items { outlineView.expandItem(item, expandChildren: true) }
    }
}

extension SidebarViewController: NSOutlineViewDataSource {

    func outlineView(_ outlineView: NSOutlineView, numberOfChildrenOfItem item: Any?) -> Int {

        if let sidebarItem = item as? SidebarItem {
            return sidebarItem.children.count
        } else {
            return items.count
        }
    }

    func outlineView(_ outlineView: NSOutlineView, heightOfRowByItem item: Any) -> CGFloat {

        return 32 // 48
    }

    func outlineView(_ outlineView: NSOutlineView, isItemExpandable item: Any) -> Bool {

        if let sidebarItem = item as? SidebarItem {
            return !sidebarItem.children.isEmpty
        }
        return false
    }

    func outlineView(_ outlineView: NSOutlineView, child index: Int, ofItem item: Any?) -> Any {

        if let sidebarItem = item as? SidebarItem {
            return sidebarItem.children[index]
        } else {
            return items[index]
        }
    }
}

extension SidebarViewController: NSOutlineViewDelegate {

    func outlineView(_ outlineView: NSOutlineView, viewFor tableColumn: NSTableColumn?, item: Any) -> NSView? {

        let cell = outlineView.makeView(withIdentifier: NSUserInterfaceItemIdentifier("SidebarCell"), owner: self) as? NSTableCellView

        if let sidebarItem = item as? SidebarItem {
            cell?.textField?.stringValue = sidebarItem.title
            cell?.imageView?.image = NSImage(systemSymbolName: sidebarItem.iconName,
                                             accessibilityDescription: sidebarItem.title)
        } else {
            cell?.textField?.stringValue = "???"
            cell?.imageView?.image = nil
        }
        return cell
    }

    func outlineViewSelectionDidChange(_ notification: Notification) {

        let selectedIndex = outlineView.selectedRow
        if selectedIndex >= 0 {
            if let item = outlineView.item(atRow: selectedIndex) as? SidebarItem {
                selectionHandler?(item)
            }
        }
        /*
        let selectedIndex = outlineView.selectedRow
        if selectedIndex >= 0 {
            selectionHandler?(items[selectedIndex])
        }
        */
    }
}
