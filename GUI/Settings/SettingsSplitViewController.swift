// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Cocoa

class SettingsSplitViewController: NSSplitViewController {

    func instantiate(_ identifier: String) -> Any {

        let main = NSStoryboard(name: "Settings", bundle: nil)
        return main.instantiateController(withIdentifier: identifier)
    }

    var windowController: SettingsWindowController? {
        view.window?.windowController as? SettingsWindowController
    }

    private var sidebarVC: SidebarViewController? {
        return splitViewItems.first?.viewController as? SidebarViewController
    }

    lazy var generalVC = instantiate("General") as! GeneralSettingsViewController
    lazy var keyboardVC = instantiate("Keyboard") as! KeyboardSettingsViewController
    lazy var capturesVC = instantiate("Captures") as! CapturesSettingsViewController
    lazy var controlsVC = instantiate("Controls") as! ControlsSettingsViewController
    lazy var devicesVC = instantiate("Devices") as! DevicesSettingsViewController
    lazy var romsVC = instantiate("Roms") as! RomSettingsViewController
    lazy var hardwareVC = instantiate("Hardware") as! HardwareSettingsViewController
    lazy var peripheralsVC = instantiate("Peripherals") as! PeripheralsSettingsViewController
    lazy var performanceVC = instantiate("Performance") as! PerformanceSettingsViewController
    lazy var audioVC = instantiate("Audio") as! AudioSettingsViewController
    lazy var videoVC = instantiate("Video") as! VideoSettingsViewController
    lazy var serversVC = instantiate("Servers") as! ServerSettingsViewController

    var current: SettingsViewController?

    override func viewDidLoad() {

        super.viewDidLoad()
        sidebarVC?.selectionHandler = { [weak self] item in
            self?.showContent(for: item)
        }
        splitView.delegate = self
    }

    override func splitView(_ splitView: NSSplitView,
                            canCollapseSubview subview: NSView) -> Bool {
        return false
    }

    override func splitView(_ splitView: NSSplitView,
                            constrainSplitPosition proposedPosition: CGFloat,
                            ofSubviewAt dividerIndex: Int) -> CGFloat {

        return splitView.subviews[dividerIndex].frame.size.width
    }

    func showContent(for item: SidebarItem) {

        showContent(title: item.title)
    }

    func showContent(title: String) {

        switch title {

        case "Emulator":        current = generalVC
        case "Virtual Machine": current = romsVC

        case "General":         current = generalVC
        case "Captures":        current = capturesVC
        case "Keyboard":        current = keyboardVC
        case "Controls":        current = controlsVC
        case "Devices":         current = devicesVC
        case "Roms":            current = romsVC
        case "Hardware":        current = hardwareVC
        case "Peripherals":     current = peripheralsVC
        case "Performance":     current = performanceVC
        case "Audio":           current = audioVC
        case "Video":           current = videoVC
        case "Servers":         current = serversVC

        default:            fatalError()
        }

        // Remove the old content pane
        removeSplitViewItem(splitViewItems[1])

        // Create a new split view item for the new content
        let newItem = NSSplitViewItem(viewController: current!)
        addSplitViewItem(newItem)
        current!.activate()
    }

    override func keyDown(with event: NSEvent) {

        print("keyDown: \(event)")
        current?.keyDown(with: event)
    }

    override func flagsChanged(with event: NSEvent) {

        print("flagsChanged: \(event)")
        current?.flagsChanged(with: event)
    }

    @IBAction func presetAction(_ sender: NSPopUpButton) {

        print("presetAction")
        current?.preset(tag: sender.selectedTag())
        current?.refresh()
    }

    @IBAction func saveAction(_ sender: Any) {

        print("saveAction")
        current?.save()
        current?.refresh()
    }
}
