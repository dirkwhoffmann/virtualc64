//
//  MyToolbar.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 05.07.20.
//

class MyToolbar: NSToolbar {
    
    @IBOutlet weak var parent: MyController!
    
    // Toolbar items
    @IBOutlet weak var controlPort1: NSPopUpButton!
    @IBOutlet weak var controlPort2: NSPopUpButton!
    @IBOutlet weak var powerButton: NSToolbarItem!
    @IBOutlet weak var pauseButton: NSToolbarItem!
    @IBOutlet weak var resetButton: NSToolbarItem!
    @IBOutlet weak var keyboardButton: NSToolbarItem!
    @IBOutlet weak var snapshotSegCtrl: NSSegmentedControl!
    
    override func validateVisibleItems() {

        track()
        
        validateToolbarItems()
        
        /*
        let c64 = parent.c64!
        let pause = pauseButton.view as? NSButton
        let reset = resetButton.view as? NSButton
        let kb = keyboardButton.view as? NSButton
        
        // Disable the keyboard button, if the virtual keyboard is open
        if let window = parent.virtualKeyboard?.window {
            kb?.isEnabled = !window.isVisible
        } else {
            kb?.isEnabled = true
        }
        
        // Disable the Pause and Reset button if the emulator if powered off
        let poweredOn = true //  c64.isPoweredOn()
        pause?.isEnabled = poweredOn
        reset?.isEnabled = poweredOn

        // Adjust the appearance of the Pause button
        if c64.isRunning() {
            pause?.image = NSImage.init(named: "pauseTemplate")
            pauseButton.label = "Pause"
        } else {
            pause?.image = NSImage.init(named: "continueTemplate")
            pauseButton.label = "Run"
        }
        
        // Change the label of reset button. If we don't do that, the
        // label color does not change (at least in macOS Mojave)
        resetButton.label = ""
        resetButton.label = "Reset"

        // Update input device selectors
        parent.gamePadManager.refresh(popup: controlPort1)
        parent.gamePadManager.refresh(popup: controlPort2)
        controlPort1.selectItem(withTag: parent.config.gameDevice1)
        controlPort2.selectItem(withTag: parent.config.gameDevice2)
        */
    }
    
    func validateToolbarItems() {
         
        let isRunning = true // c64.isRunning()
        
         let button = pauseButton.view as? NSButton
         if isRunning {
             button?.image = NSImage.init(named: "pauseTemplate")
             pauseButton.label = "Pause"
         } else {
             button?.image = NSImage.init(named: "continueTemplate")
             pauseButton.label = "Run"
         }
         
         validateJoystickToolbarItems()
     }
    
    func validateJoystickToolbarItems() {
        
        let c64 = parent.c64!

        validateJoystickToolbarItem(controlPort1,
                                    selectedSlot: parent.inputDevice1,
                                    port: c64.port1)
        validateJoystickToolbarItem(controlPort2,
                                    selectedSlot: parent.inputDevice2,
                                    port: c64.port2)
    }
    
    func validateJoystickToolbarItem(_ popup: NSPopUpButton, selectedSlot: Int, port: ControlPortProxy!) {
        
        let manager = parent.gamePadManager!
        
        let menu =  popup.menu
        let item3 = menu?.item(withTag: InputDevice.joystick1)
        let item4 = menu?.item(withTag: InputDevice.joystick2)
        
        // USB joysticks
        item3?.title = manager.gamePads[3]?.name ?? "USB Device 1"
        item4?.title = manager.gamePads[4]?.name ?? "USB Device 2"
        item3?.isEnabled = !manager.slotIsEmpty(InputDevice.joystick1)
        item4?.isEnabled = !manager.slotIsEmpty(InputDevice.joystick2)
        
        // Mark game pad connected to port
        popup.selectItem(withTag: selectedSlot)
    }
}
