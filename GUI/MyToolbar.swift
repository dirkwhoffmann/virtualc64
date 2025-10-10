// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension NSToolbarItem.Identifier {

    static let inspectors = NSToolbarItem.Identifier("Inspectors")
    static let snapshots = NSToolbarItem.Identifier("Snapshots")
    static let port1 = NSToolbarItem.Identifier("Port1")
    static let port2 = NSToolbarItem.Identifier("Port2")
    static let keyboard = NSToolbarItem.Identifier("Keyboard")
    static let settings = NSToolbarItem.Identifier("Settings")
    static let preferences = NSToolbarItem.Identifier("Preferences")
    static let controls = NSToolbarItem.Identifier("Controls")
}

class MyToolbar: NSToolbar, NSToolbarDelegate {

    var controller: MyController!
    var emu: EmulatorProxy! { return controller.emu! }

    var inspectors: MyToolbarItemGroup!
    var snapshots: MyToolbarItemGroup!
    var port1: MyToolbarMenuItem!
    var port2: MyToolbarMenuItem!
    var keyboard: MyToolbarItemGroup!
    var settings: MyToolbarItemGroup!
    var controls: MyToolbarItemGroup!

    // Set to true to gray out all toolbar items
    var globalDisable = false

    init() {

        super.init(identifier: "MyToolbar")
        self.delegate = self
        self.allowsUserCustomization = true
        self.displayMode = .iconAndLabel
    }

    override init(identifier: NSToolbar.Identifier) {

        super.init(identifier: identifier)
        self.delegate = self
        self.allowsUserCustomization = true
        self.displayMode = .iconAndLabel
    }

    convenience init(controller: MyController) {

        self.init(identifier: "MyToolbar")
        self.controller = controller
    }
    
    func toolbarAllowedItemIdentifiers(_ toolbar: NSToolbar) -> [NSToolbarItem.Identifier] {

        return [ .inspectors,
                 .snapshots,
                 .port1,
                 .port2,
                 .keyboard,
                 .settings,
                 .controls,
                 .flexibleSpace ]
    }

    func toolbarDefaultItemIdentifiers(_ toolbar: NSToolbar) -> [NSToolbarItem.Identifier] {

        return [ .inspectors,
                 .flexibleSpace,
                 .snapshots,
                 .flexibleSpace,
                 .port1,
                 .port2,
                 .flexibleSpace,
                 .keyboard,
                 .flexibleSpace,
                 .preferences,
                 .flexibleSpace,
                 .settings,
                 .flexibleSpace,
                 .controls ]
    }

    func toolbar(_ toolbar: NSToolbar,
                 itemForItemIdentifier itemIdentifier: NSToolbarItem.Identifier,
                 willBeInsertedIntoToolbar flag: Bool) -> NSToolbarItem? {

        let portItems = [ (SFSymbol.nosign, "None", -1),
                          (SFSymbol.mouse, "Mouse", 0),
                          (SFSymbol.arrowkeys, "Keyset 1", 1),
                          (SFSymbol.arrowkeys, "Keyset 2", 2),
                          (SFSymbol.gamecontroller, "Gamepad 1", 3),
                          (SFSymbol.gamecontroller, "Gamepad 2", 4),
                          (SFSymbol.gamecontroller, "Gamepad 3", 5),
                          (SFSymbol.gamecontroller, "Gamepad 4", 6) ]

        switch itemIdentifier {

        case .inspectors:

            let images: [SFSymbol] = [

                .magnifyingglass,
                .gauge,
                .console
            ]

            let actions: [Selector] = [

                #selector(inspectorAction),
                #selector(dashboardAction),
                #selector(consoleAction)
            ]

            inspectors = MyToolbarItemGroup(identifier: .inspectors,
                                            images: images,
                                            actions: actions,
                                            target: self,
                                            label: "Inspectors")
            return inspectors

        case .snapshots:

            let images: [SFSymbol] = [

                .arrowDown,
                .arrowUp,
                .arrowClock
            ]

            let actions: [Selector] = [

                #selector(takeSnapshotAction),
                #selector(restoreSnapshotAction),
                #selector(browseSnapshotAction)
            ]

            snapshots = MyToolbarItemGroup(identifier: .snapshots,
                                           images: images,
                                           actions: actions,
                                           target: self,
                                           label: "Snapshots")
            return snapshots

        case .port1:

            port1 = MyToolbarMenuItem(identifier: .port1,
                                      menuItems: portItems,
                                      image: .gear,
                                      action: #selector(port1Action(_:)),
                                      target: self,
                                      label: "Port 1")
            return port1

        case .port2:

            port2 = MyToolbarMenuItem(identifier: .port2,
                                      menuItems: portItems,
                                      image: .gear,
                                      action: #selector(port2Action(_:)),
                                      target: self,
                                      label: "Port 2")
            return port2

        case .keyboard:

            keyboard = MyToolbarItemGroup(identifier: .keyboard,
                                          images: [.keyboard],
                                          actions: [#selector(keyboardAction)],
                                          target: self,
                                          label: "Keyboard")
            return keyboard

        case .settings:

            settings = MyToolbarItemGroup(identifier: .settings,
                                          images: [.gear],
                                          actions: [#selector(settingsAction)],
                                          target: self,
                                          label: "Settings")
            return settings

        case .controls:

            let images: [SFSymbol] = [

                .pause,
                .reset,
                .power
            ]

            let actions: [Selector] = [

                #selector(runAction),
                #selector(resetAction),
                #selector(powerAction)
            ]

            controls = MyToolbarItemGroup(identifier: .controls,
                                          images: images,
                                          actions: actions,
                                          target: self,
                                          label: "Controls")
            return controls

        default:
            return nil
        }
    }

    override func validateVisibleItems() {

        // Take care of the global disable flag
        for item in items { item.isEnabled = !globalDisable }

        // Disable the keyboard button if the virtual keyboard is open
        if  controller.virtualKeyboard?.window?.isVisible == true {
            (keyboard.view as? NSButton)?.isEnabled = false
        }

        // Disable the snapshot revert button if no snapshots have been taken
        snapshots.setEnabled(controller.snapshotCount > 0, forSegment: 1)

        // Update input devices
        controller.gamePadManager.refresh(menu: port1.menu)
        controller.gamePadManager.refresh(menu: port2.menu)
        port1.selectItem(withTag: controller.config.gameDevice1)
        port2.selectItem(withTag: controller.config.gameDevice2)

        port1.menuFormRepresentation = nil
        port2.menuFormRepresentation = nil
        keyboard.menuFormRepresentation = nil
        settings.menuFormRepresentation = nil
        controls.menuFormRepresentation = nil
    }

    func updateToolbar() {

        if emu.poweredOn {

            controls.setEnabled(true, forSegment: 0) // Pause
            controls.setEnabled(true, forSegment: 1) // Reset
            controls.setToolTip("Power off", forSegment: 2) // Power

        } else {

            controls.setEnabled(false, forSegment: 0) // Pause
            controls.setEnabled(false, forSegment: 1) // Reset
            controls.setToolTip("Power on", forSegment: 2) // Power
        }

        if emu.running {

            controls.setToolTip("Pause", forSegment: 0)
            controls.setImage(SFSymbol.get(.pause), forSegment: 0)

        } else {

            controls.setToolTip("Run", forSegment: 0)
            controls.setImage(SFSymbol.get(.play), forSegment: 0)
        }
    }

    //
    // Action methods
    //

    @objc private func inspectorAction() {

        controller.inspectorAction(self)
    }

    @objc private func dashboardAction() {

        controller.dashboardAction(self)
    }

    @objc private func consoleAction() {

        controller.consoleAction(self)
    }

    @objc private func takeSnapshotAction() {

        controller.takeSnapshotAction(self)
    }

    @objc private func restoreSnapshotAction() {

        controller.restoreSnapshotAction(self)
    }

    @objc private func browseSnapshotAction() {

        controller.browseSnapshotsAction(self)
    }

    @objc private func port1Action(_ sender: NSMenuItem) {

        controller.config.gameDevice1 = sender.tag
    }

    @objc private func port2Action(_ sender: NSMenuItem) {

        controller.config.gameDevice2 = sender.tag
    }

    @objc private func keyboardAction() {

        if controller.virtualKeyboard == nil {
            controller.virtualKeyboard = VirtualKeyboardController.make(parent: controller)
        }
        if controller.virtualKeyboard?.window?.isVisible == false {
            controller.virtualKeyboard?.showAsSheet()
        }
    }

    @objc private func settingsAction() {

        controller.settingsAction(self)
    }

    @objc private func runAction() {

        controller.stopAndGoAction(self)
    }

    @objc private func resetAction() {

        controller.resetAction(self)
    }

    @objc private func powerAction() {

        controller.powerAction(self)
    }
}


/*
class MyToolbar: NSToolbar {
    
    var emu: EmulatorProxy? { controller.emu }

    @IBOutlet weak var controller: MyController!

    // References to toolbar items
    @IBOutlet weak var controlPort1Item: NSToolbarItem!
    @IBOutlet weak var controlPort2Item: NSToolbarItem!
    @IBOutlet weak var keyboardItem: NSToolbarItem!
    @IBOutlet weak var preferencesItem: NSToolbarItem!
    @IBOutlet weak var controlsItem: NSToolbarItem!

    // Reference to toolbar item objects
    @IBOutlet weak var controlPort1: NSPopUpButton!
    @IBOutlet weak var controlPort2: NSPopUpButton!
    @IBOutlet weak var snapshotSegCtrl: NSSegmentedControl!
    @IBOutlet weak var screenshotSegCtrl: NSSegmentedControl!
    @IBOutlet weak var controlsSegCtrl: NSSegmentedControl!
    
    override func validateVisibleItems() {
                           
        // Disable the keyboard button if the virtual keyboard is open
        let visible = controller.virtualKeyboard?.window?.isVisible ?? false
        let view = keyboardItem.view as? NSButton
        view?.isEnabled = !visible
        
        // Disable the snapshot revert button if no snapshots have been taken
        snapshotSegCtrl.setEnabled(controller.snapshotCount > 0, forSegment: 1)

        // Update input devices
        controller.gamePadManager.refresh(popup: controlPort1)
        controller.gamePadManager.refresh(popup: controlPort2)
        controlPort1.selectItem(withTag: controller.config.gameDevice1)
        controlPort2.selectItem(withTag: controller.config.gameDevice2)

        controlPort1Item.menuFormRepresentation = nil
        controlPort2Item.menuFormRepresentation = nil
        keyboardItem.menuFormRepresentation = nil
        preferencesItem.menuFormRepresentation = nil
        controlsItem.menuFormRepresentation = nil
    }
    
    func updateToolbar() {
        
        if emu != nil {

            if emu!.poweredOn {
                controlsSegCtrl.setEnabled(true, forSegment: 0) // Pause
                controlsSegCtrl.setEnabled(true, forSegment: 1) // Reset
                controlsSegCtrl.setToolTip("Power off", forSegment: 2) // Power
            } else {
                controlsSegCtrl.setEnabled(false, forSegment: 0) // Pause
                controlsSegCtrl.setEnabled(false, forSegment: 1) // Reset
                controlsSegCtrl.setToolTip("Power on", forSegment: 2) // Power
            }
            if emu!.running {
                controlsSegCtrl.setToolTip("Pause", forSegment: 0)
                controlsSegCtrl.setImage(NSImage(named: "pauseTemplate"), forSegment: 0)
            } else {
                controlsSegCtrl.setToolTip("Run", forSegment: 0)
                controlsSegCtrl.setImage(NSImage(named: "runTemplate"), forSegment: 0)
            }
        }
    }
    
    //
    // Action methods
    //
    
    @IBAction func inspectAction(_ sender: NSSegmentedControl) {
        
        switch sender.selectedSegment {
            
        case 0: controller.inspectorAction(sender)
        case 1: controller.dashboardAction(sender)
        case 2: controller.consoleAction(sender)
            
        default: assert(false)
        }
    }
    
    @IBAction func snapshotAction(_ sender: NSSegmentedControl) {
        
        switch sender.selectedSegment {
        
        case 0: controller.takeSnapshotAction(self)
        case 1: controller.restoreSnapshotAction(self)
        case 2: controller.browseSnapshotsAction(self)
            
        default: assert(false)
        }
    }
    
    @IBAction func screenshotAction(_ sender: NSSegmentedControl) {
                
        switch sender.selectedSegment {
            
        case 0: controller.takeScreenshotAction(self)
        case 1: controller.browseScreenshotsAction(self)
            
        default: assert(false)
        }
    }
    
    @IBAction func port1Action(_ sender: Any) {

        if let obj = sender as? NSPopUpButton {
            controller.config.gameDevice1 = obj.selectedTag()
        }
    }
 
    @IBAction func port2Action(_ sender: Any) {

        if let obj = sender as? NSPopUpButton {
            controller.config.gameDevice2 = obj.selectedTag()
        }
    }

    @IBAction func keyboardAction(_ sender: Any!) {
        
        if controller.virtualKeyboard == nil {
            controller.virtualKeyboard =
            VirtualKeyboardController(with: controller, nibName: "VirtualKeyboard")
        }
        if controller.virtualKeyboard?.window?.isVisible == false {
            controller.virtualKeyboard?.showSheet()
        }
    }
    
    @IBAction func preferencesAction(_ sender: Any) {

        if let obj = sender as? NSSegmentedControl {

            switch obj.selectedSegment {
            case 0: controller.preferencesAction(obj)
            case 1: controller.configureAction(obj)
            default: assert(false)
            }
        }
    }
    
    @IBAction func controlsAction(_ sender: Any) {

        if let obj = sender as? NSSegmentedControl {

            switch obj.selectedSegment {
            case 0: controller.stopAndGoAction(self)
            case 1: controller.resetAction(self)
            case 2: controller.powerAction(self)
            default: assert(false)
            }
        }
    }
}
*/
