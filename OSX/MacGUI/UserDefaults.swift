//
//  UserDefaults.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 10.02.18.
//

import Foundation

struct VC64Keys {
    
    // General
    static let basicRom          = "VC64BasicRomFileKey"
    static let charRom           = "VC64CharRomFileKey"
    static let kernalRom         = "VC64KernelRomFileKey"
    static let vc1541Rom         = "VC64VC1541RomFileKey"

    // Keyboard
    static let mapKeysByPosition = "VC64MapKeysByPosition"
    static let keyMap            = "VC64KeyMap"
    
    // Emulator preferences dialog
    static let eyeX              = "VC64EyeX"
    static let eyeY              = "VC64EyeY"
    static let eyeZ              = "VC64EyeZ"
    static let brightness        = "VC64BrightnessKey"
    static let contrast          = "VC64ContrastKey"
    static let saturation        = "VC64SaturationKey"
    static let videoUpscaler     = "VC64UpscalerKey"
    static let videoFilter       = "VC64FilterKey"
    static let aspectRatio       = "VC64FullscreenKeepAspectRatioKey"

    static let joyKeyMap1        = "VC64JoyKeyMap1"
    static let joyKeyMap2        = "VC64JoyKeyMap2"
    static let disconnectKeys    = "VC64DisconnectKeys"
    static let autofire          = "VC64Autofire"
    static let autofireBullets   = "VC64AutofireBullets"
    static let autofireFrequency = "VC64AutofireFrequency"

    static let pauseInBackground = "VC64PauseInBackground"
    static let snapshotInterval  = "VC64SnapshotInterval"
    static let autoMount         = "VC64AutoMount"

    // Hardware preferences dialog
    static let vicChip           = "VC64VICChipModelKey"
    static let grayDotBug        = "VC64VICGrayDotBugKey"

    static let ciaChip           = "VC64CIAChipModelKey"
    static let timerBBug         = "VC64CIATimerBBugKey"

    static let reSID             = "VC64SIDReSIDKey"
    static let audioChip         = "VC64SIDChipModelKey"
    static let audioFilter       = "VC64SIDFilterKey"
    static let samplingMethod    = "VC64SIDSamplingMethodKey"

    static let glueLogic         = "VC64GlueLogicKey"
    static let initPattern       = "VC64InitPatternKey"

    static let warpLoad          = "VC64WarpLoadKey"
    static let driveNoise        = "VC64DriveNoiseKey"

    static let mouseModel        = "VC64MouseModelKey"
}

/// This class extension handles the UserDefaults management

extension MyController {
    
    //
    // Default values
    //

    /// Registers the default values of all user definable properties
    static func registerUserDefaults() {
        
        track()
        registerJoystickUserDefaults()
        registerEmulatorUserDefaults()
        registerHardwareUserDefaults()
        
        let dictionary : [String:Any] = [
        
            VC64Keys.basicRom: "",
            VC64Keys.charRom: "",
            VC64Keys.kernalRom: "",
            VC64Keys.vc1541Rom: "",
            
            VC64Keys.mapKeysByPosition: false
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
    }
    
    /// Registers the default values for all emulator dialog properties
    static func registerJoystickUserDefaults() {
        
        track()
        let dictionary : [String:Any] = [
            
            VC64Keys.disconnectKeys: true,
            VC64Keys.autofire: false,
            VC64Keys.autofireBullets: -3,
            VC64Keys.autofireFrequency: 2.5
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
    }

    /// Registers the default values for all emulator dialog properties
    static func registerEmulatorUserDefaults() {
        
        track()
        let dictionary : [String:Any] = [
            
            VC64Keys.eyeX: 0.0,
            VC64Keys.eyeY: 0.0,
            VC64Keys.eyeZ: 0.0,
            VC64Keys.brightness: 50.0,
            VC64Keys.contrast: 100.0,
            VC64Keys.saturation: 50.0,
            VC64Keys.videoUpscaler: 0,
            VC64Keys.videoFilter: 1,
            VC64Keys.aspectRatio: false,
            
            VC64Keys.disconnectKeys: true,
            VC64Keys.pauseInBackground: false,
            VC64Keys.snapshotInterval: 3,
            VC64Keys.autoMount: false,
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
    }
    
    /// Registers the default values for all hardware dialog properties
    static func registerHardwareUserDefaults() {
        
        track()
        let dictionary : [String:Any] = [
        
            VC64Keys.vicChip: PAL_8565.rawValue,
            VC64Keys.grayDotBug: true,

            VC64Keys.ciaChip: MOS_6526_OLD.rawValue,
            VC64Keys.timerBBug: true,

            VC64Keys.reSID: true,
            VC64Keys.audioChip: MOS_8580.rawValue,
            VC64Keys.audioFilter: false,
            VC64Keys.samplingMethod: 0,

            VC64Keys.glueLogic: GLUE_DISCRETE.rawValue,
            VC64Keys.initPattern: INIT_PATTERN_C64.rawValue,

            VC64Keys.warpLoad: true,
            VC64Keys.driveNoise: true,

            VC64Keys.mouseModel: 0
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
    }
    
    
    //
    // Loading
    //

    /// Loads all user defaults from database
    func loadUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        keyboardcontroller.mapKeysByPosition = defaults.bool(forKey: VC64Keys.mapKeysByPosition)
        
        loadJoystickUserDefaults()
        loadEmulatorUserDefaults()
        loadHardwareUserDefaults()
        loadKeyMapUserDefaults()
    }
    
    /// Loads the user defaults for all properties that are set in the joystick dialog
    func loadJoystickUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        if let data = defaults.data(forKey: VC64Keys.joyKeyMap1) {
            if let keyMap = try? JSONDecoder().decode([MacKey:UInt32].self, from: data) {
                gamePadManager.gamePads[0]?.keyMap = keyMap
            }
        }
        if let data = defaults.data(forKey: VC64Keys.joyKeyMap2) {
            if let keyMap = try? JSONDecoder().decode([MacKey:UInt32].self, from: data) {
                gamePadManager.gamePads[1]?.keyMap = keyMap
            }
        }
        keyboardcontroller.disconnectEmulationKeys = defaults.bool(forKey: VC64Keys.disconnectKeys)
        
        c64.port1.setAutofire(defaults.bool(forKey: VC64Keys.autofire))
        c64.port2.setAutofire(defaults.bool(forKey: VC64Keys.autofire))
        c64.port1.setAutofireBullets(defaults.integer(forKey: VC64Keys.autofireBullets))
        c64.port2.setAutofireBullets(defaults.integer(forKey: VC64Keys.autofireBullets))
        c64.port1.setAutofireFrequency(defaults.float(forKey: VC64Keys.autofireFrequency))
        c64.port2.setAutofireFrequency(defaults.float(forKey: VC64Keys.autofireFrequency))
    }
    
    /// Loads the user defaults for all properties that are set in the hardware dialog
    func loadEmulatorUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard

        // Video
        metalScreen.setEyeX(defaults.float(forKey: VC64Keys.eyeX))
        metalScreen.setEyeY(defaults.float(forKey: VC64Keys.eyeY))
        metalScreen.setEyeZ(defaults.float(forKey: VC64Keys.eyeZ))
        c64.vic.setBrightness(defaults.double(forKey: VC64Keys.brightness))
        c64.vic.setContrast(defaults.double(forKey: VC64Keys.contrast))
        c64.vic.setSaturation(defaults.double(forKey: VC64Keys.saturation))
        metalScreen.videoUpscaler = defaults.integer(forKey: VC64Keys.videoUpscaler)
        metalScreen.videoFilter = defaults.integer(forKey: VC64Keys.videoFilter)
        metalScreen.fullscreenKeepAspectRatio = defaults.bool(forKey: VC64Keys.aspectRatio)
        
        // Drive
        c64.setWarpLoad(defaults.bool(forKey: VC64Keys.warpLoad))
        c64.drive1.setSendSoundMessages(defaults.bool(forKey: VC64Keys.driveNoise))
        c64.drive2.setSendSoundMessages(defaults.bool(forKey: VC64Keys.driveNoise))
        
        // Misc
        pauseInBackground = defaults.bool(forKey: VC64Keys.pauseInBackground)
        c64.setSnapshotInterval(defaults.integer(forKey: VC64Keys.snapshotInterval))
        autoMount = defaults.bool(forKey: VC64Keys.autoMount)
    }
    
    /// Loads the user defaults for all properties that are set in the hardware dialog
    func loadHardwareUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        
        // VICII
        c64.vic.setChipModel(defaults.integer(forKey: VC64Keys.vicChip))
        c64.vic.setEmulateGrayDotBug(defaults.bool(forKey: VC64Keys.grayDotBug))

        // CIA
        c64.cia1.setChipModel(defaults.integer(forKey: VC64Keys.ciaChip))
        c64.cia2.setChipModel(defaults.integer(forKey: VC64Keys.ciaChip))
        c64.cia1.setTimerBBug(defaults.bool(forKey: VC64Keys.timerBBug))
        c64.cia2.setTimerBBug(defaults.bool(forKey: VC64Keys.timerBBug))

        // SID
        c64.sid.setReSID(defaults.bool(forKey: VC64Keys.reSID))
        c64.sid.setChipModel(defaults.integer(forKey: VC64Keys.audioChip))
        c64.sid.setAudioFilter(defaults.bool(forKey: VC64Keys.audioFilter))
        c64.sid.setSamplingMethod(defaults.integer(forKey: VC64Keys.samplingMethod))

        // Board
        c64.vic.setGlueLogic(defaults.integer(forKey: VC64Keys.glueLogic))
        c64.mem.setRamInitPattern(defaults.integer(forKey: VC64Keys.initPattern))

        // Mouse
        c64.setMouseModel(defaults.integer(forKey: VC64Keys.mouseModel))
    }
    
    func loadKeyMapUserDefaults() {
        
        let defaults = UserDefaults.standard
        if let data = defaults.data(forKey: VC64Keys.keyMap) {
            if let keyMap = try? JSONDecoder().decode([MacKey:C64Key].self, from: data) {
                keyboardcontroller.keyMap = keyMap
            }
        }
    }
    
    
    //
    // Saving
    //

    /// Saves all user defaults from database
    /*
    func saveUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        defaults.set(keyboardcontroller.mapKeysByPosition, forKey: VC64Keys.mapKeysByPosition)
        
        saveJoystickUserDefaults()
        saveEmulatorUserDefaults()
        saveHardwareUserDefaults()
        saveKeyMapUserDefaults()
    }
    */
    
    /// Saves the user defaults for all properties that are set in the joystick dialog
    func saveJoystickUserDefaults() {
     
        track()
        let defaults = UserDefaults.standard
        
        if let keyMap = try? JSONEncoder().encode(gamePadManager.gamePads[0]?.keyMap) {
            defaults.set(keyMap, forKey: VC64Keys.joyKeyMap1)
        }
        if let keyMap = try? JSONEncoder().encode(gamePadManager.gamePads[1]?.keyMap) {
            defaults.set(keyMap, forKey: VC64Keys.joyKeyMap2)
        }
        defaults.set(keyboardcontroller.disconnectEmulationKeys, forKey: VC64Keys.disconnectKeys)
        
        assert(c64.port1.autofire() == c64.port2.autofire())
        assert(c64.port1.autofireBullets() == c64.port2.autofireBullets())
        assert(c64.port1.autofireFrequency() == c64.port2.autofireFrequency())
        defaults.set(c64.port1.autofire(), forKey: VC64Keys.autofire)
        defaults.set(c64.port1.autofireBullets(), forKey: VC64Keys.autofireBullets)
        defaults.set(c64.port1.autofireFrequency(), forKey: VC64Keys.autofireFrequency)
    }
 
    func saveKeyMapUserDefaults() {
        
        let defaults = UserDefaults.standard
        if let keyMap = try? JSONEncoder().encode(keyboardcontroller.keyMap) {
            defaults.set(keyMap, forKey: VC64Keys.keyMap)
        }
    }
    
    /// Saves the user defaults for all properties that are set in the hardware dialog
    func saveEmulatorUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        
        // Video
        defaults.set(metalScreen.eyeX(), forKey: VC64Keys.eyeX)
        defaults.set(metalScreen.eyeY(), forKey: VC64Keys.eyeY)
        defaults.set(metalScreen.eyeZ(), forKey: VC64Keys.eyeZ)
        defaults.set(c64.vic.brightness(), forKey: VC64Keys.brightness)
        defaults.set(c64.vic.contrast(), forKey: VC64Keys.contrast)
        defaults.set(c64.vic.saturation(), forKey: VC64Keys.saturation)
        defaults.set(metalScreen.videoUpscaler, forKey: VC64Keys.videoUpscaler)
        defaults.set(metalScreen.videoFilter, forKey: VC64Keys.videoFilter)
        defaults.set(metalScreen.fullscreenKeepAspectRatio, forKey: VC64Keys.aspectRatio)
        
        // Drive
        defaults.set(c64.warpLoad(), forKey: VC64Keys.warpLoad)
        defaults.set(c64.drive1.sendSoundMessages(), forKey: VC64Keys.driveNoise)
        defaults.set(c64.drive2.sendSoundMessages(), forKey: VC64Keys.driveNoise)
        
        // Misc
        defaults.set(pauseInBackground, forKey: VC64Keys.pauseInBackground)
        defaults.set(c64.snapshotInterval(), forKey: VC64Keys.snapshotInterval)
        defaults.set(autoMount, forKey: VC64Keys.autoMount)
    }
    
    /// Saves the user defaults for all properties that are set in the hardware dialog
    func saveHardwareUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        
        // VICII
        defaults.set(c64.vic.chipModel(), forKey: VC64Keys.vicChip)
        defaults.set(c64.vic.emulateGrayDotBug(), forKey: VC64Keys.grayDotBug)

        // CIA
        defaults.set(c64.cia1.chipModel(), forKey: VC64Keys.ciaChip)
        defaults.set(c64.cia1.timerBBug(), forKey: VC64Keys.timerBBug)

        // SID
        defaults.set(c64.sid.reSID(), forKey: VC64Keys.reSID)
        defaults.set(c64.sid.chipModel(), forKey: VC64Keys.audioChip)
        defaults.set(c64.sid.audioFilter(), forKey: VC64Keys.audioFilter)
        defaults.set(c64.sid.samplingMethod(), forKey: VC64Keys.samplingMethod)
        
        // Board
        defaults.set(c64.vic.glueLogic(), forKey: VC64Keys.glueLogic)
        defaults.set(c64.mem.ramInitPattern(), forKey: VC64Keys.initPattern)

        // Mouse
        defaults.set(c64.mouseModel(), forKey: VC64Keys.mouseModel)
    }
}
