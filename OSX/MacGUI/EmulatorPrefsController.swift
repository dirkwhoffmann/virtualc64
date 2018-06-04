//
//  PropertiesController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 21.02.18.
//

import Foundation

class EmulatorPrefsWindow : NSWindow {
    
    // @IBOutlet weak var controller: EmulatorPrefsController!
    
    func respondToEvents() {
        DispatchQueue.main.async {
            self.makeFirstResponder(self)
        }
    }
    
    override func awakeFromNib() {
        respondToEvents()
    }
    
    override func keyDown(with event: NSEvent) {
        
        let controller = delegate as! EmulatorPrefsController
        controller.keyDown(with: MacKey.init(with: event))
    }
    
    override func flagsChanged(with event: NSEvent) {
        
        let controller = delegate as! EmulatorPrefsController
        if event.modifierFlags.contains(.shift) {
            controller.keyDown(with: MacKey.shift)
        } else if event.modifierFlags.contains(.control) {
            controller.keyDown(with: MacKey.control)
        } else if event.modifierFlags.contains(.option) {
            controller.keyDown(with: MacKey.option)
        }
    }
}

class EmulatorPrefsController : UserDialogController {
    
    /// Indicates if a keycode should be recorded for keyset 1
    var recordKey1: JoystickDirection?
    
    /// Indicates if a keycode should be recorded for keyset 1
    var recordKey2: JoystickDirection?
    
    // Video
    @IBOutlet weak var eyeXSlider: NSSlider!
    @IBOutlet weak var eyeYSlider: NSSlider!
    @IBOutlet weak var eyeZSlider: NSSlider!
    @IBOutlet weak var colorScheme: NSPopUpButton!
    @IBOutlet weak var upscaler: NSPopUpButton!
    @IBOutlet weak var filter: NSPopUpButton!
    @IBOutlet weak var colorWell0: NSColorWell!
    @IBOutlet weak var colorWell1: NSColorWell!
    @IBOutlet weak var colorWell2: NSColorWell!
    @IBOutlet weak var colorWell3: NSColorWell!
    @IBOutlet weak var colorWell4: NSColorWell!
    @IBOutlet weak var colorWell5: NSColorWell!
    @IBOutlet weak var colorWell6: NSColorWell!
    @IBOutlet weak var colorWell7: NSColorWell!
    @IBOutlet weak var colorWell8: NSColorWell!
    @IBOutlet weak var colorWell9: NSColorWell!
    @IBOutlet weak var colorWell10: NSColorWell!
    @IBOutlet weak var colorWell11: NSColorWell!
    @IBOutlet weak var colorWell12: NSColorWell!
    @IBOutlet weak var colorWell13: NSColorWell!
    @IBOutlet weak var colorWell14: NSColorWell!
    @IBOutlet weak var colorWell15: NSColorWell!
    @IBOutlet weak var aspectRatioButton: NSButton!
    
    // Joystick
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
    
    // Documents
    @IBOutlet weak var autoMount: NSButton!

    override func awakeFromNib() {
        update()
    }
    
    func update() {
       
        // Video
        upscaler.selectItem(withTag: parent.metalScreen.videoUpscaler)
        filter.selectItem(withTag: parent.metalScreen.videoFilter)
        colorScheme.selectItem(withTag: Int(c64.vic.colorScheme().rawValue))
        eyeXSlider.floatValue = parent.metalScreen.eyeX()
        eyeYSlider.floatValue = parent.metalScreen.eyeY()
        eyeZSlider.floatValue = parent.metalScreen.eyeZ()
        aspectRatioButton.state = parent.metalScreen.fullscreenKeepAspectRatio ? .on : .off
        colorWell0.color = c64.vic.color(0)
        colorWell1.color = c64.vic.color(1)
        colorWell2.color = c64.vic.color(2)
        colorWell3.color = c64.vic.color(3)
        colorWell4.color = c64.vic.color(4)
        colorWell5.color = c64.vic.color(5)
        colorWell6.color = c64.vic.color(6)
        colorWell7.color = c64.vic.color(7)
        colorWell8.color = c64.vic.color(8)
        colorWell9.color = c64.vic.color(9)
        colorWell10.color = c64.vic.color(10)
        colorWell11.color = c64.vic.color(11)
        colorWell12.color = c64.vic.color(12)
        colorWell13.color = c64.vic.color(13)
        colorWell14.color = c64.vic.color(14)
        colorWell15.color = c64.vic.color(15)
        
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
        
        // Documents
        autoMount.state = parent.autoMount ? .on : .off
    }
    
    func updateKeyMap(_ nr: Int, direction: JoystickDirection, button: NSButton, txt: NSTextField) {
     
        precondition(nr == 0 || nr == 1)
     
        let keyMap = parent.gamePadManager.gamePads[nr]?.keyMap //  ?? [:]
        
        // Which MacKey is assigned to this joystick action?
        var macKey: MacKey?
        var macKeyCode: String = ""
        var macKeyDesc: String = ""
        for (key, dir) in keyMap! {
            if dir == direction.rawValue {
                macKey = key
                macKeyCode = NSString(format: "%02X", macKey!.keyCode) as String
                macKeyDesc = macKey?.description?.uppercased() ?? ""
                break
            }
        }
    
        // Update text and button image
        let recordKey = (nr == 0) ? recordKey1 : recordKey2
        if (recordKey == direction) {
            button.title = ""
            button.image = NSImage(named: NSImage.Name(rawValue: "keyRed"))
            button.imageScaling = .scaleAxesIndependently
        } else {
            button.image = NSImage(named: NSImage.Name(rawValue: "key"))
            button.imageScaling = .scaleAxesIndependently
        }
        button.title = macKeyCode
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
    // Action methods (Video settings)
    //
    
    @IBAction func setUpscalerAction(_ sender: Any!) {
    
        let sender = sender as! NSPopUpButton
        parent.metalScreen.videoUpscaler = sender.selectedTag()
        update()
    }
    
    @IBAction func setFilterAction(_ sender: Any!) {
    
        track()
        let sender = sender as! NSPopUpButton
        parent.metalScreen.videoFilter = sender.selectedTag()
        update()
    }
    
    @IBAction func changeColorScheme(_ sender: Any!) {
        
        track()
        let sender = sender as! NSPopUpButton
        c64.vic.setColorScheme(ColorScheme(rawValue: UInt32(sender.selectedTag())))
        update()
    }

    @IBAction func setEyeXAction(_ sender: Any!) {
    
        let sender = sender as! NSSlider
        parent.metalScreen.setEyeX(sender.floatValue)
        update()
    }
    
    @IBAction func setEyeYAction(_ sender: Any!) {
    
        let sender = sender as! NSSlider
        parent.metalScreen.setEyeY(sender.floatValue)
        update()
    }
    
    @IBAction func setEyeZAction(_ sender: Any!) {
    
        let sender = sender as! NSSlider
        parent.metalScreen.setEyeZ(sender.floatValue)
        update()
    }
    
    @IBAction func setFullscreenAspectRatio(_ sender: Any!) {
    
        let sender = sender as! NSButton
        parent.metalScreen.fullscreenKeepAspectRatio = (sender.state == .on)
        update()
    }
    
    //
    // Action methods (Joystick emulation settings)
    //
    
    @IBAction func recordKeyAction(_ sender: Any!) {

        let tag = UInt32((sender as! NSButton).tag)
        
        if tag >= 0 && tag <= 4 {
            recordKey1 = JoystickDirection(rawValue: tag)
            recordKey2 = nil
        } else if tag >= 10 && tag <= 14 {
            recordKey1 = nil
            recordKey2 = JoystickDirection(rawValue: (tag - 10))
        } else {
            assert(false);
        }
        
        update()
    }
    
    @IBAction func disconnectKeysAction(_ sender: Any!) {
    
        let sender = sender as! NSButton
        parent.keyboardcontroller.disconnectEmulationKeys = (sender.state == .on)
        update()
    }
    
    @IBAction func autoMountAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        parent.autoMount =  (sender.state == .on)
        update()
    }
    
    //
    // Action methods (General)
    //
    
    @IBAction func helpAction(_ sender: Any!) {
        
        if let url = URL.init(string: "http://www.dirkwhoffmann.de/virtualc64/ROMs.html") {
            NSWorkspace.shared.open(url)
        }
    }
    
    @IBAction override func cancelAction(_ sender: Any!) {
        
        parent.loadEmulatorUserDefaults()
        hideSheet()
    }
    
    @IBAction func factorySettingsAction(_ sender: Any!) {
        
        // Display
        parent.metalScreen.setEyeX(0.0)
        parent.metalScreen.setEyeY(0.0)
        parent.metalScreen.setEyeZ(0.0)
        parent.metalScreen.videoUpscaler = 1
        parent.metalScreen.videoFilter = 2
        c64.vic.setColorScheme(VICE)
        parent.metalScreen.fullscreenKeepAspectRatio = false
        
        // Joystick emulation
        parent.gamePadManager.restoreFactorySettings()
        parent.keyboardcontroller.disconnectEmulationKeys = true
        
        // File handling
        parent.autoMount = false
    
        update()
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        parent.saveEmulatorUserDefaults()
        hideSheet()
    }
}

