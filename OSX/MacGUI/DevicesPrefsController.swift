//
//  DevicesPrefsController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 29.08.18.
//

import Foundation

class DevicesPrefsWindow : NSWindow {
    
    func respondToEvents() {
        DispatchQueue.main.async {
            self.makeFirstResponder(self)
        }
    }
    
    override func awakeFromNib() {
        
        respondToEvents()
    }
    
    override func keyDown(with event: NSEvent) {
        
        let controller = delegate as! DevicesPrefsController
        controller.keyDown(with: MacKey.init(with: event))
    }
    
    override func flagsChanged(with event: NSEvent) {
        
        let controller = delegate as! DevicesPrefsController
        if event.modifierFlags.contains(.shift) {
            controller.keyDown(with: MacKey.shift)
        } else if event.modifierFlags.contains(.control) {
            controller.keyDown(with: MacKey.control)
        } else if event.modifierFlags.contains(.option) {
            controller.keyDown(with: MacKey.option)
        }
    }
}

class DevicesPrefsController : UserDialogController {
    
    /// Indicates if a keycode should be recorded for keyset 1
    var recordKey1: JoystickDirection?
    
    /// Indicates if a keycode should be recorded for keyset 1
    var recordKey2: JoystickDirection?
    
    /// Joystick emulation keys
    @IBOutlet weak var left1: NSTextField!
    @IBOutlet weak var left1button: NSButton!
    @IBOutlet weak var right1: NSTextField!
    @IBOutlet weak var right1button: NSButton!
    @IBOutlet weak var up1: NSTextField!
    @IBOutlet weak var up1button: NSButton!
    @IBOutlet weak var down1: NSTextField!
    @IBOutlet weak var down1button: NSButton!
    @IBOutlet weak var fire1: NSTextField!
    @IBOutlet weak var fire1button: NSButton!
    @IBOutlet weak var left2: NSTextField!
    @IBOutlet weak var left2button: NSButton!
    @IBOutlet weak var right2: NSTextField!
    @IBOutlet weak var right2button: NSButton!
    @IBOutlet weak var up2: NSTextField!
    @IBOutlet weak var up2button: NSButton!
    @IBOutlet weak var down2: NSTextField!
    @IBOutlet weak var down2button: NSButton!
    @IBOutlet weak var fire2: NSTextField!
    @IBOutlet weak var fire2button: NSButton!
    @IBOutlet weak var disconnectKeys: NSButton!
    
    // Joystick buttons
    @IBOutlet weak var autofire: NSButton!
    @IBOutlet weak var autofireCease: NSButton!
    @IBOutlet weak var autofireCeaseText: NSTextField!
    @IBOutlet weak var autofireBullets: NSTextField!
    @IBOutlet weak var autofireFrequency: NSSlider!

    // Mouse
    @IBOutlet weak var mouseModel: NSPopUpButton!
    @IBOutlet weak var mouseInfo: NSTextField!

    
    override func awakeFromNib() {
    
        update()
    }
    
    func update() {
        
        // Joystick emulation keys
        updateKeyMap(0, direction: JOYSTICK_UP, button: up1button, txt: up1)
        updateKeyMap(0, direction: JOYSTICK_DOWN, button: down1button, txt: down1)
        updateKeyMap(0, direction: JOYSTICK_LEFT, button: left1button, txt: left1)
        updateKeyMap(0, direction: JOYSTICK_RIGHT, button: right1button, txt: right1)
        updateKeyMap(0, direction: JOYSTICK_FIRE, button: fire1button, txt: fire1)
        updateKeyMap(1, direction: JOYSTICK_UP, button: up2button, txt: up2)
        updateKeyMap(1, direction: JOYSTICK_DOWN, button: down2button, txt: down2)
        updateKeyMap(1, direction: JOYSTICK_LEFT, button: left2button, txt: left2)
        updateKeyMap(1, direction: JOYSTICK_RIGHT, button: right2button, txt: right2)
        updateKeyMap(1, direction: JOYSTICK_FIRE, button: fire2button, txt: fire2)
        disconnectKeys.state = parent.keyboardcontroller.disconnectEmulationKeys ? .on : .off

        assert(c64.port1.autofire() == c64.port2.autofire())
        assert(c64.port1.autofireBullets() == c64.port2.autofireBullets())
        assert(c64.port1.autofireFrequency() == c64.port2.autofireFrequency())

        // Joystick buttons
        autofire.state = c64.port1.autofire() ? .on : .off
        autofireCease.state = c64.port1.autofireBullets() > 0 ? .on : .off
        autofireBullets.integerValue = Int(c64.port1.autofireBullets().magnitude)
        autofireFrequency.floatValue = c64.port1.autofireFrequency()
        autofireCease.isEnabled = autofire.state == .on
        autofireCeaseText.textColor = autofire.state == .on ? NSColor.controlTextColor : NSColor.disabledControlTextColor
        autofireBullets.isEnabled = autofire.state == .on 
        autofireFrequency.isEnabled = autofire.state == .on
        
        // Mouse
        let model = c64.mouseModel()
        mouseModel.selectItem(withTag: model)
        mouseInfo.isHidden = (model == Int(MOUSE1350.rawValue))
    }
    
    func updateKeyMap(_ nr: Int, direction: JoystickDirection, button: NSButton, txt: NSTextField) {
        
        precondition(nr == 0 || nr == 1)
        
        let keyMap = parent.gamePadManager.gamePads[nr]?.keyMap
        
        // Which MacKey is assigned to this joystick action?
        var macKey: MacKey?
        var macKeyCode: NSAttributedString = NSAttributedString.init()
        var macKeyDesc: String = ""
        for (key, dir) in keyMap! {
            if dir == direction.rawValue {
                 let attr = [NSAttributedStringKey.foregroundColor: NSColor.black]
                macKey = key
                // macKeyCode = NSString(format: "%02X", macKey!.keyCode) as String
                let myStr = NSString(format: "%02X", macKey!.keyCode) as String
                macKeyCode = NSAttributedString(string: myStr, attributes: attr)
                macKeyDesc = macKey?.description?.uppercased() ?? ""
                break
            }
        }
        
        // Update text and button image
        let recordKey = (nr == 0) ? recordKey1 : recordKey2
        if (recordKey == direction) {
            button.title = ""
            button.image = NSImage(named: NSImage.Name(rawValue: "key_red"))
            button.imageScaling = .scaleAxesIndependently
        } else {
            button.image = NSImage(named: NSImage.Name(rawValue: "key"))
            button.imageScaling = .scaleAxesIndependently
        }
        button.attributedTitle = macKeyCode
        txt.stringValue = macKeyDesc
    }
    
    //
    // Keyboard events
    //
    
    func keyDown(with macKey: MacKey) {
        
        track()
        
        // Check for ESC key
        if macKey == MacKey.escape {
            cancelAction(self)
            return
        }
        
        if recordKey1 != nil {
            
            parent.gamePadManager.gamePads[0]?.assign(key: macKey, direction: recordKey1!)
            recordKey1 = nil
        }
        if recordKey2 != nil {
            
            parent.gamePadManager.gamePads[1]?.assign(key: macKey, direction: recordKey2!)
            recordKey2 = nil
        }
        
        update()
    }
    
    //
    // Action methods
    //
    
    @IBAction func recordKeyAction(_ sender: NSButton!) {
        
        let tag = UInt32(sender.tag)
        
        if tag >= 0 && tag <= 5 {
            recordKey1 = JoystickDirection(rawValue: tag)
            recordKey2 = nil
        } else if tag >= 10 && tag <= 15 {
            recordKey1 = nil
            recordKey2 = JoystickDirection(rawValue: (tag - 10))
        } else {
            assert(false);
        }
        
        update()
    }

    @IBAction func disconnectKeysAction(_ sender: NSButton!) {
        
        parent.keyboardcontroller.disconnectEmulationKeys = (sender.state == .on)
        update()
    }
    
    @IBAction func autofireAction(_ sender: NSButton!) {
        
        let value = sender.state
        track("value = \(value)")
        c64.port1.setAutofire(sender.state == .on)
        c64.port2.setAutofire(sender.state == .on)
        update()
    }
    
    @IBAction func autofireCeaseAction(_ sender: NSButton!) {
        
        let value = sender.state
        track("value = \(value)")
        
        assert(c64.port1.autofireBullets() == c64.port2.autofireBullets())
        let bullets = Int(c64.port1.autofireBullets().magnitude)
        let sign = sender.state == .on ? 1 : -1;
        c64.port1.setAutofireBullets(bullets * sign)
        c64.port2.setAutofireBullets(bullets * sign)
        update()
    }
    
    @IBAction func autofireBulletsAction(_ sender: NSTextField!) {
        
        let value = sender.integerValue
        track("value = \(value)")
        c64.port1.setAutofireBullets(value)
        c64.port2.setAutofireBullets(value)
        update()
    }
    
    @IBAction func autofireFrequencyAction(_ sender: NSSlider!) {
        
        let value = sender.floatValue
        track("value = \(value)")
        c64.port1.setAutofireFrequency(value)
        c64.port2.setAutofireFrequency(value)
        update()
    }
    
    @IBAction func mouseModelAction(_ sender: NSPopUpButton!) {
        
        c64.setMouseModel(sender.selectedTag())
        update()
    }
    
    @IBAction func helpAction(_ sender: Any!) {
        
        if let url = URL.init(string: "http://www.dirkwhoffmann.de/virtualc64/ROMs.html") {
            NSWorkspace.shared.open(url)
        }
    }
    
    @IBAction override func cancelAction(_ sender: Any!) {
        
        parent.loadJoystickUserDefaults()
        hideSheet()
    }
    
    @IBAction func factorySettingsAction(_ sender: Any!) {
        
        // Joystick emulation keys
        parent.gamePadManager.restoreFactorySettings()
        parent.keyboardcontroller.disconnectEmulationKeys = true

        // Joystick buttons
        c64.port1.setAutofire(false)
        c64.port2.setAutofire(false)
        c64.port1.setAutofireBullets(-3)
        c64.port2.setAutofireBullets(-3)
        c64.port1.setAutofireFrequency(2.5)
        c64.port2.setAutofireFrequency(2.5)
        
        // Mouse
        c64.setMouseModel(0)
        
        update()
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        parent.saveJoystickUserDefaults()
        hideSheet()
    }
}

