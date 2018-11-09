//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

struct VC64Keys {
    
    // General
    static let basicRom           = "VC64BasicRomFileKey"
    static let charRom            = "VC64CharRomFileKey"
    static let kernalRom          = "VC64KernelRomFileKey"
    static let vc1541Rom          = "VC64VC1541RomFileKey"

    // Keyboard
    static let mapKeysByPosition  = "VC64MapKeysByPosition"
    static let keyMap             = "VC64KeyMap"
    
    // Devices preferences dialog
    static let joyKeyMap1         = "VC64JoyKeyMap1"
    static let joyKeyMap2         = "VC64JoyKeyMap2"
    static let disconnectKeys     = "VC64DisconnectKeys"
    
    static let autofire           = "VC64Autofire"
    static let autofireBullets    = "VC64AutofireBullets"
    static let autofireFrequency  = "VC64AutofireFrequency"
    
    static let mouseModel         = "VC64MouseModelKey"
    
    // Emulator preferences dialog
    static let videoPalette       = "VC64PaletteKey"

    static let videoUpscaler      = "VC64UpscalerKey"
    static let videoFilter        = "VC64FilterKey"
    static let brightness         = "VC64BrightnessKey"
    static let contrast           = "VC64ContrastKey"
    static let saturation         = "VC64SaturationKey"
    static let blur               = "VC64BlurKey"
    
    static let scanlines          = "VC64ScanlinesKey"
    static let scanlineBrightness = "VC64ScanlineBrightness"
    static let scanlineWeight     = "VC64ScanlineWeight"
    static let bloomFactor        = "VC64BloomFactor"
    static let mask               = "VC64Mask"
    static let maskBrightness     = "VC64MaskBrightness"

    static let aspectRatio        = "VC64FullscreenKeepAspectRatioKey"
    static let eyeX               = "VC64EyeX"
    static let eyeY               = "VC64EyeY"
    static let eyeZ               = "VC64EyeZ"
    
    static let warpLoad           = "VC64WarpLoadKey"
    static let driveNoise         = "VC64DriveNoiseKey"
    
    static let pauseInBackground  = "VC64PauseInBackground"
    static let snapshotInterval   = "VC64SnapshotInterval"
    static let autoMount          = "VC64AutoMount"

    // Hardware preferences dialog
    static let vicChip            = "VC64VICChipModelKey"
    static let grayDotBug         = "VC64VICGrayDotBugKey"

    static let ciaChip            = "VC64CIAChipModelKey"
    static let timerBBug          = "VC64CIATimerBBugKey"

    static let reSID              = "VC64SIDReSIDKey"
    static let audioChip          = "VC64SIDChipModelKey"
    static let audioFilter        = "VC64SIDFilterKey"
    static let samplingMethod     = "VC64SIDSamplingMethodKey"

    static let glueLogic          = "VC64GlueLogicKey"
    static let initPattern        = "VC64InitPatternKey"
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
            VC64Keys.autofireFrequency: 2.5,
            
            VC64Keys.mouseModel: 0
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
    }

    /// Registers the default values for all emulator dialog properties
    static func registerEmulatorUserDefaults() {
        
        track()
        let dictionary : [String:Any] = [
            
            VC64Keys.videoPalette: EmulatorDefaults.palette,
            
            VC64Keys.videoUpscaler: EmulatorDefaults.upscaler,
            VC64Keys.videoFilter: EmulatorDefaults.filter,
            VC64Keys.brightness: EmulatorDefaults.brightness,
            VC64Keys.contrast: EmulatorDefaults.contrast,
            VC64Keys.saturation: EmulatorDefaults.saturation,
            VC64Keys.blur: EmulatorDefaults.blur,
            
            VC64Keys.scanlines: EmulatorDefaults.scanlines,
            VC64Keys.scanlineBrightness: EmulatorDefaults.scanlineBrightness,
            VC64Keys.scanlineWeight: EmulatorDefaults.scanlineWeight,
            VC64Keys.bloomFactor: EmulatorDefaults.bloomFactor,
            VC64Keys.mask: EmulatorDefaults.dotMask,
            VC64Keys.maskBrightness: EmulatorDefaults.maskBrightness,
        
            VC64Keys.aspectRatio: false,
            VC64Keys.eyeX: EmulatorDefaults.eyeX,
            VC64Keys.eyeY: EmulatorDefaults.eyeY,
            VC64Keys.eyeZ: EmulatorDefaults.eyeZ,
            
            VC64Keys.warpLoad: true,
            VC64Keys.driveNoise: true,
            
            VC64Keys.disconnectKeys: true,
            VC64Keys.pauseInBackground: false,
            VC64Keys.snapshotInterval: 3,
            VC64Keys.autoMount: false
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
            VC64Keys.initPattern: INIT_PATTERN_C64.rawValue
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
        
        c64.suspend()
        loadJoystickUserDefaults()
        loadEmulatorUserDefaults()
        loadHardwareUserDefaults()
        loadKeyMapUserDefaults()
        c64.resume()
    }
    
    /// Loads the user defaults for all properties that are set in the joystick dialog
    func loadJoystickUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        
        c64.suspend()

        // Joystick emulation keys
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
        
        // Joystick buttons
        c64.port1.setAutofire(defaults.bool(forKey: VC64Keys.autofire))
        c64.port2.setAutofire(defaults.bool(forKey: VC64Keys.autofire))
        c64.port1.setAutofireBullets(defaults.integer(forKey: VC64Keys.autofireBullets))
        c64.port2.setAutofireBullets(defaults.integer(forKey: VC64Keys.autofireBullets))
        c64.port1.setAutofireFrequency(defaults.float(forKey: VC64Keys.autofireFrequency))
        c64.port2.setAutofireFrequency(defaults.float(forKey: VC64Keys.autofireFrequency))
        
        // Mouse
        c64.setMouseModel(defaults.integer(forKey: VC64Keys.mouseModel))
        
        c64.resume()
    }
    
    /// Loads the user defaults for all properties that are set in the hardware dialog
    func loadEmulatorUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard

        c64.suspend()
        
        // Colors
        c64.vic.setVideoPalette(defaults.integer(forKey: VC64Keys.videoPalette))
        
        // Texture
        metalScreen.videoUpscaler = defaults.integer(forKey: VC64Keys.videoUpscaler)
        metalScreen.videoFilter = defaults.integer(forKey: VC64Keys.videoFilter)
        c64.vic.setBrightness(defaults.double(forKey: VC64Keys.brightness))
        c64.vic.setContrast(defaults.double(forKey: VC64Keys.contrast))
        c64.vic.setSaturation(defaults.double(forKey: VC64Keys.saturation))
        metalScreen.blurFactor = defaults.float(forKey: VC64Keys.blur)
        
        // Effects
        metalScreen.scanlines = defaults.integer(forKey: VC64Keys.scanlines)
        metalScreen.scanlineBrightness = defaults.float(forKey: VC64Keys.scanlineBrightness)
        metalScreen.scanlineWeight = defaults.float(forKey: VC64Keys.scanlineWeight)
        metalScreen.bloomFactor = defaults.float(forKey: VC64Keys.bloomFactor)
        metalScreen.dotMask = defaults.integer(forKey: VC64Keys.mask)
        metalScreen.maskBrightness = defaults.float(forKey: VC64Keys.maskBrightness)
        
        // Geometry
        metalScreen.fullscreenKeepAspectRatio = defaults.bool(forKey: VC64Keys.aspectRatio)
        metalScreen.setEyeX(defaults.float(forKey: VC64Keys.eyeX))
        metalScreen.setEyeY(defaults.float(forKey: VC64Keys.eyeY))
        metalScreen.setEyeZ(defaults.float(forKey: VC64Keys.eyeZ))
        
        // Drive
        c64.setWarpLoad(defaults.bool(forKey: VC64Keys.warpLoad))
        c64.drive1.setSendSoundMessages(defaults.bool(forKey: VC64Keys.driveNoise))
        c64.drive2.setSendSoundMessages(defaults.bool(forKey: VC64Keys.driveNoise))
        
        // Misc
        pauseInBackground = defaults.bool(forKey: VC64Keys.pauseInBackground)
        c64.setSnapshotInterval(defaults.integer(forKey: VC64Keys.snapshotInterval))
        autoMount = defaults.bool(forKey: VC64Keys.autoMount)
        
        c64.resume()
    }
    
    /// Loads the user defaults for all properties that are set in the hardware dialog
    func loadHardwareUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        
        c64.suspend()
        
        // VICII
        c64.vic.setModel(defaults.integer(forKey: VC64Keys.vicChip))
        c64.vic.setEmulateGrayDotBug(defaults.bool(forKey: VC64Keys.grayDotBug))

        // CIA
        c64.cia1.setModel(defaults.integer(forKey: VC64Keys.ciaChip))
        c64.cia2.setModel(defaults.integer(forKey: VC64Keys.ciaChip))
        c64.cia1.setEmulateTimerBBug(defaults.bool(forKey: VC64Keys.timerBBug))
        c64.cia2.setEmulateTimerBBug(defaults.bool(forKey: VC64Keys.timerBBug))

        // SID
        c64.sid.setReSID(defaults.bool(forKey: VC64Keys.reSID))
        c64.sid.setModel(defaults.integer(forKey: VC64Keys.audioChip))
        c64.sid.setAudioFilter(defaults.bool(forKey: VC64Keys.audioFilter))
        c64.sid.setSamplingMethod(defaults.integer(forKey: VC64Keys.samplingMethod))

        /*
        let model = c64.sid.chipModel()
        let method = c64.sid.samplingMethod()
        if (model == MOS_8580.rawValue && method == SID_SAMPLE_FAST.rawValue) {
            showResidSamplingMethodAlert()
            c64.sid.setSamplingMethod(Int(SID_SAMPLE_INTERPOLATE.rawValue))
        }
        */
        
        // Board
        c64.vic.setGlueLogic(defaults.integer(forKey: VC64Keys.glueLogic))
        c64.mem.setRamInitPattern(defaults.integer(forKey: VC64Keys.initPattern))
        
        c64.resume()
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
        
        // Joystick emulation keys
        if let keyMap = try? JSONEncoder().encode(gamePadManager.gamePads[0]?.keyMap) {
            defaults.set(keyMap, forKey: VC64Keys.joyKeyMap1)
        }
        if let keyMap = try? JSONEncoder().encode(gamePadManager.gamePads[1]?.keyMap) {
            defaults.set(keyMap, forKey: VC64Keys.joyKeyMap2)
        }
        defaults.set(keyboardcontroller.disconnectEmulationKeys, forKey: VC64Keys.disconnectKeys)
        
        // Joystick buttons
        assert(c64.port1.autofire() == c64.port2.autofire())
        assert(c64.port1.autofireBullets() == c64.port2.autofireBullets())
        assert(c64.port1.autofireFrequency() == c64.port2.autofireFrequency())
        defaults.set(c64.port1.autofire(), forKey: VC64Keys.autofire)
        defaults.set(c64.port1.autofireBullets(), forKey: VC64Keys.autofireBullets)
        defaults.set(c64.port1.autofireFrequency(), forKey: VC64Keys.autofireFrequency)
        
        // Mouse
        defaults.set(c64.mouseModel(), forKey: VC64Keys.mouseModel)
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
        
        // Colors
        defaults.set(c64.vic.videoPalette(), forKey: VC64Keys.videoPalette)
        
        // Texture
        defaults.set(metalScreen.videoUpscaler, forKey: VC64Keys.videoUpscaler)
        defaults.set(metalScreen.videoFilter, forKey: VC64Keys.videoFilter)
        defaults.set(c64.vic.brightness(), forKey: VC64Keys.brightness)
        defaults.set(c64.vic.contrast(), forKey: VC64Keys.contrast)
        defaults.set(c64.vic.saturation(), forKey: VC64Keys.saturation)
        defaults.set(metalScreen.blurFactor, forKey: VC64Keys.blur)

        // Effects
        defaults.set(metalScreen.scanlines, forKey: VC64Keys.scanlines)
        defaults.set(metalScreen.scanlineBrightness, forKey: VC64Keys.scanlineBrightness)
        defaults.set(metalScreen.scanlineWeight, forKey: VC64Keys.scanlineWeight)
        defaults.set(metalScreen.bloomFactor, forKey: VC64Keys.bloomFactor)
        defaults.set(metalScreen.dotMask, forKey: VC64Keys.mask)
        defaults.set(metalScreen.scanlines, forKey: VC64Keys.scanlines)
        defaults.set(metalScreen.maskBrightness, forKey: VC64Keys.maskBrightness)
        
        // Geometry
        defaults.set(metalScreen.fullscreenKeepAspectRatio, forKey: VC64Keys.aspectRatio)
        defaults.set(metalScreen.eyeX(), forKey: VC64Keys.eyeX)
        defaults.set(metalScreen.eyeY(), forKey: VC64Keys.eyeY)
        defaults.set(metalScreen.eyeZ(), forKey: VC64Keys.eyeZ)
        
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
        defaults.set(c64.vic.model(), forKey: VC64Keys.vicChip)
        defaults.set(c64.vic.emulateGrayDotBug(), forKey: VC64Keys.grayDotBug)

        // CIA
        defaults.set(c64.cia1.model(), forKey: VC64Keys.ciaChip)
        defaults.set(c64.cia1.emulateTimerBBug(), forKey: VC64Keys.timerBBug)

        // SID
        defaults.set(c64.sid.reSID(), forKey: VC64Keys.reSID)
        defaults.set(c64.sid.model(), forKey: VC64Keys.audioChip)
        defaults.set(c64.sid.audioFilter(), forKey: VC64Keys.audioFilter)
        defaults.set(c64.sid.samplingMethod(), forKey: VC64Keys.samplingMethod)
        
        // Board
        defaults.set(c64.vic.glueLogic(), forKey: VC64Keys.glueLogic)
        defaults.set(c64.mem.ramInitPattern(), forKey: VC64Keys.initPattern)
    }
}
