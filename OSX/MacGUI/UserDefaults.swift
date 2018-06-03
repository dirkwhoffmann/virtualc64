//
//  UserDefaults.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 10.02.18.
//

import Foundation

struct VC64Keys {
    
    // General
    static let basicRom  = "VC64BasicRomFileKey"
    static let charRom   = "VC64CharRomFileKey"
    static let kernalRom = "VC64KernelRomFileKey"
    static let vc1541Rom = "VC64VC1541RomFileKey"

    // Keyboard
    static let mapKeysByPosition = "VC64MapKeysByPosition"
    static let keyMap = "VC64KeyMap"
    
    // Emulator preferences dialog
    static let eyeX = "VC64EyeX"
    static let eyeY = "VC64EyeY"
    static let eyeZ = "VC64EyeZ"
    
    static let colorScheme    = "VC64ColorSchemeKey"
    static let videoUpscaler  = "VC64VideoUpscalerKey"
    static let videoFilter    = "VC64VideoFilterKey"
    static let aspectRatio    = "VC64FullscreenKeepAspectRatioKey"

    static let joyKeyMap1     = "VC64JoyKeyMap1"
    static let joyKeyMap2     = "VC64JoyKeyMap2"
    static let disconnectKeys = "VC64DisconnectKeys"
    
    static let autoMount      = "VC64AutoMount"
    
    // Hardware preferences dialog
    static let ntsc = "VC64PALorNTSCKey"

    static let reSID          = "VC64SIDReSIDKey"
    static let audioChip      = "VC64SIDChipModelKey"
    static let audioFilter    = "VC64SIDFilterKey"
    static let samplingMethod = "VC64SIDSamplingMethodKey"

    static let warpLoad    = "VC64WarpLoadKey"
    static let driveNoise  = "VC64DriveNoiseKey"

    static let mouseModel = "VC64MouseModelKey"
}

/// This class extension handles the UserDefaults management

extension MyController {
    
    // --------------------------------------------------------------------------------
    //                                 Default values
    // --------------------------------------------------------------------------------


    /// Registers the default values of all user definable properties
    static func registerUserDefaults() {
        
        track()
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
    static func registerEmulatorUserDefaults() {
        
        track()
        let dictionary : [String:Any] = [
            
            VC64Keys.eyeX: 0.0,
            VC64Keys.eyeY: 0.0,
            VC64Keys.eyeZ: 0.0,
        
            VC64Keys.colorScheme: VICE.rawValue,
            VC64Keys.videoUpscaler: 1,
            VC64Keys.videoFilter: 2,
            VC64Keys.aspectRatio: false,
        
            VC64Keys.disconnectKeys: true,
            VC64Keys.autoMount: false
]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
    }
    
    /// Registers the default values for all hardware dialog properties
    static func registerHardwareUserDefaults() {
        
        track()
        let dictionary : [String:Any] = [
        
            VC64Keys.ntsc: false,
            
            VC64Keys.reSID: true,
            VC64Keys.audioChip: 1,
            VC64Keys.audioFilter: false,
            VC64Keys.samplingMethod: 0,

            VC64Keys.warpLoad: true,
            VC64Keys.driveNoise: true,

            VC64Keys.mouseModel: 0
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
    }
    
    // --------------------------------------------------------------------------------
    //                                  Loading
    // --------------------------------------------------------------------------------

    /// Loads all user defaults from database
    func loadUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        keyboardcontroller.mapKeysByPosition = defaults.bool(forKey: VC64Keys.mapKeysByPosition)
        
        loadEmulatorUserDefaults()
        loadHardwareUserDefaults()
        loadKeyMapUserDefaults()
    }
    
    /// Loads the user defaults for all properties that are set in the hardware dialog
    func loadEmulatorUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        metalScreen.setEyeX(defaults.float(forKey: VC64Keys.eyeX))
        metalScreen.setEyeY(defaults.float(forKey: VC64Keys.eyeY))
        metalScreen.setEyeZ(defaults.float(forKey: VC64Keys.eyeZ))
        c64.vic.setColorScheme(defaults.integer(forKey: VC64Keys.colorScheme))
        metalScreen.videoUpscaler = defaults.integer(forKey: VC64Keys.videoUpscaler)
        metalScreen.videoFilter = defaults.integer(forKey: VC64Keys.videoFilter)
        metalScreen.fullscreenKeepAspectRatio = defaults.bool(forKey: VC64Keys.aspectRatio)
        
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
        autoMount = defaults.bool(forKey: VC64Keys.autoMount)
    }
    
    /// Loads the user defaults for all properties that are set in the hardware dialog
    func loadHardwareUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        c64.setNTSC(defaults.bool(forKey: VC64Keys.ntsc))
        c64.setReSID(defaults.bool(forKey: VC64Keys.reSID))
        c64.setChipModel(defaults.integer(forKey: VC64Keys.audioChip))
        c64.setAudioFilter(defaults.bool(forKey: VC64Keys.audioFilter))
        c64.setSamplingMethod(defaults.integer(forKey: VC64Keys.samplingMethod))
        c64.setWarpLoad(defaults.bool(forKey: VC64Keys.warpLoad))
        c64.vc1541.setSendSoundMessages(defaults.bool(forKey: VC64Keys.driveNoise))
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
    
    // --------------------------------------------------------------------------------
    //                                  Saving
    // --------------------------------------------------------------------------------

    /// Saves all user defaults from database
    func saveUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        defaults.set(keyboardcontroller.mapKeysByPosition, forKey: VC64Keys.mapKeysByPosition)
        
        saveEmulatorUserDefaults()
        saveHardwareUserDefaults()
        saveKeyMapUserDefaults()
    }
    
    /// Saves the user defaults for all properties that are set in the hardware dialog
    func saveEmulatorUserDefaults() {
     
        track()
        let defaults = UserDefaults.standard
        defaults.set(metalScreen.eyeX(), forKey: VC64Keys.eyeX)
        defaults.set(metalScreen.eyeY(), forKey: VC64Keys.eyeY)
        defaults.set(metalScreen.eyeZ(), forKey: VC64Keys.eyeZ)
        defaults.set(c64.vic.colorScheme(), forKey: VC64Keys.colorScheme)
        defaults.set(metalScreen.videoUpscaler, forKey: VC64Keys.videoUpscaler)
        defaults.set(metalScreen.videoFilter, forKey: VC64Keys.videoFilter)
        defaults.set(metalScreen.fullscreenKeepAspectRatio, forKey: VC64Keys.aspectRatio)
        
        if let keyMap = try? JSONEncoder().encode(gamePadManager.gamePads[0]?.keyMap) {
            defaults.set(keyMap, forKey: VC64Keys.joyKeyMap1)
        }
        if let keyMap = try? JSONEncoder().encode(gamePadManager.gamePads[1]?.keyMap) {
            defaults.set(keyMap, forKey: VC64Keys.joyKeyMap2)
        }
        defaults.set(keyboardcontroller.disconnectEmulationKeys, forKey: VC64Keys.disconnectKeys)
        defaults.set(autoMount, forKey: VC64Keys.autoMount)
    }
    
    /// Saves the user defaults for all properties that are set in the hardware dialog
    func saveHardwareUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        defaults.set(c64.isNTSC(), forKey: VC64Keys.ntsc)
        defaults.set(c64.reSID(), forKey: VC64Keys.reSID)
        defaults.set(c64.chipModel(), forKey: VC64Keys.audioChip)
        defaults.set(c64.audioFilter(), forKey: VC64Keys.audioFilter)
        defaults.set(c64.samplingMethod(), forKey: VC64Keys.samplingMethod)
        defaults.set(c64.warpLoad(), forKey: VC64Keys.warpLoad)
        defaults.set(c64.vc1541.soundMessagesEnabled(), forKey: VC64Keys.driveNoise)
        defaults.set(c64.mouseModel(), forKey: VC64Keys.mouseModel)
    }
    // ◉◎▪▫▮▯01
    func saveKeyMapUserDefaults() {
        
        let defaults = UserDefaults.standard
        if let keyMap = try? JSONEncoder().encode(keyboardcontroller.keyMap) {
            defaults.set(keyMap, forKey: VC64Keys.keyMap)
        }
    }
}
