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
    static let basicRom             = "VC64BasicRomFileKey"
    static let charRom              = "VC64CharRomFileKey"
    static let kernalRom            = "VC64KernelRomFileKey"
    static let vc1541Rom            = "VC64VC1541RomFileKey"

    static let inputDevice1         = "VC64InputDevice1"
    static let inputDevice2         = "VC64InputDevice2"

    // Keyboard
    static let mapKeysByPosition    = "VC64MapKeysByPosition"
    static let keyMap               = "VC64KeyMap"
    
    // Devices preferences dialog
    static let joyKeyMap1           = "VC64JoyKeyMap1"
    static let joyKeyMap2           = "VC64JoyKeyMap2"
    static let disconnectKeys       = "VC64DisconnectKeys"
    
    static let autofire             = "VC64Autofire"
    static let autofireBullets      = "VC64AutofireBullets"
    static let autofireFrequency    = "VC64AutofireFrequency"
    
    static let mouseModel           = "VC64MouseModelKey"
    
    // Video preferences dialog
    static let videoUpscaler        = "VC64UpscalerKey"
    static let videoPalette         = "VC64PaletteKey"
    static let brightness           = "VC64BrightnessKey"
    static let contrast             = "VC64ContrastKey"
    static let saturation           = "VC64SaturationKey"

    static let shaderOptions        = "VC64ShaderOptionsKey"
    
    static let aspectRatio          = "VC64FullscreenKeepAspectRatioKey"
    static let eyeX                 = "VC64EyeX"
    static let eyeY                 = "VC64EyeY"
    static let eyeZ                 = "VC64EyeZ"
    
    // Emulator preferences dialog
    static let warpLoad             = "VC64WarpLoadKey"
    static let driveNoise           = "VC64DriveNoiseKey"
    
    static let screenshotResolution = "VC64ScreenshotResolutionKey"
    static let screenshotFormat     = "VC64ScreenshotFormatKey"

    static let autoMount            = "VC64AutoMount"
    static let closeWithoutAsking   = "VC64CloseWithoutAsking"
    static let ejectWithoutAsking   = "VC64EjectWithoutAsking"

    static let pauseInBackground    = "VC64PauseInBackground"
    static let snapshotInterval     = "VC64SnapshotInterval"

    // Hardware preferences dialog
    static let vicChip              = "VC64VICChipModelKey"
    static let grayDotBug           = "VC64VICGrayDotBugKey"

    static let ciaChip              = "VC64CIAChipModelKey"
    static let timerBBug            = "VC64CIATimerBBugKey"

    static let reSID                = "VC64SIDReSIDKey"
    static let audioChip            = "VC64SIDChipModelKey"
    static let audioFilter          = "VC64SIDFilterKey"
    static let samplingMethod       = "VC64SIDSamplingMethodKey"

    static let glueLogic            = "VC64GlueLogicKey"
    static let initPattern          = "VC64InitPatternKey"
}

/// This class extension handles the UserDefaults management

extension MyController {
    
    //
    // Default values
    //

    /// Registers the default values of all user definable properties
    static func registerUserDefaults() {
        
        track()
        registerDevicesUserDefaults()
        registerVideoUserDefaults()
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
    
    /// Registers default values for all devices dialog properties
    static func registerDevicesUserDefaults() {
        
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

    /// Registers default values for all video dialog properties
    static func registerVideoUserDefaults() {
        
        track()
        let dictionary : [String:Any] = [
            
            // Video
            VC64Keys.videoUpscaler: EmulatorDefaults.upscaler,
            VC64Keys.videoPalette: EmulatorDefaults.palette,
            VC64Keys.brightness: EmulatorDefaults.brightness,
            VC64Keys.contrast: EmulatorDefaults.contrast,
            VC64Keys.saturation: EmulatorDefaults.saturation,
            
            VC64Keys.aspectRatio: false,
            VC64Keys.eyeX: EmulatorDefaults.eyeX,
            VC64Keys.eyeY: EmulatorDefaults.eyeY,
            VC64Keys.eyeZ: EmulatorDefaults.eyeZ,
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
        
        if let encoded = try? PropertyListEncoder().encode(ShaderDefaultsTFT) {
            UserDefaults.standard.register(defaults: [VC64Keys.shaderOptions: encoded])
        }
    }
    
    /// Registers default values for all emulator dialog properties
    static func registerEmulatorUserDefaults() {
        
        track()
        let dictionary : [String:Any] = [
            
            VC64Keys.warpLoad: true,
            VC64Keys.driveNoise: true,
            
            VC64Keys.disconnectKeys: true,
            VC64Keys.pauseInBackground: false,
            VC64Keys.snapshotInterval: 3,
            VC64Keys.autoMount: false,
            VC64Keys.closeWithoutAsking: false,
            VC64Keys.ejectWithoutAsking: false
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
    }
    
    /// Registers default values for all hardware dialog properties
    static func registerHardwareUserDefaults() {
        
        track()
        let dictionary : [String:Any] = [
        
            VC64Keys.vicChip: PAL_8565.rawValue,
            VC64Keys.grayDotBug: true,

            VC64Keys.ciaChip: MOS_6526.rawValue,
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
        loadDevicesUserDefaults()
        loadVideoUserDefaults()
        loadEmulatorUserDefaults()
        loadHardwareUserDefaults()
        loadKeyMapUserDefaults()
        c64.resume()
    }
    
    /// Loads the user defaults for all properties that are set in the joystick dialog
    func loadDevicesUserDefaults() {
        
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
        c64.mouse.setModel(defaults.integer(forKey: VC64Keys.mouseModel))
        
        c64.resume()
    }
    
    /// Loads the user defaults for all properties that are set in the video preferences dialog
    func loadVideoUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        
        c64.suspend()
        
        // Video
        metalScreen.videoUpscaler = defaults.integer(forKey: VC64Keys.videoUpscaler)
        c64.vic.setVideoPalette(defaults.integer(forKey: VC64Keys.videoPalette))
        c64.vic.setBrightness(defaults.double(forKey: VC64Keys.brightness))
        c64.vic.setContrast(defaults.double(forKey: VC64Keys.contrast))
        c64.vic.setSaturation(defaults.double(forKey: VC64Keys.saturation))
        
        // Effects
        if let data = defaults.value(forKey: VC64Keys.shaderOptions) as? Data {
            if let options = try? PropertyListDecoder().decode(ShaderOptions.self, from: data) {
                metalScreen.shaderOptions = options
                metalScreen.buildDotMasks()
            }
        }
        
        // Geometry
        metalScreen.fullscreenKeepAspectRatio = defaults.bool(forKey: VC64Keys.aspectRatio)
        metalScreen.setEyeX(defaults.float(forKey: VC64Keys.eyeX))
        metalScreen.setEyeY(defaults.float(forKey: VC64Keys.eyeY))
        metalScreen.setEyeZ(defaults.float(forKey: VC64Keys.eyeZ))
        
        c64.resume()
    }
    
    /// Loads the user defaults for all properties that are set in the emulator preferences dialog
    func loadEmulatorUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard

        c64.suspend()
        
        // Drive
        c64.setWarpLoad(defaults.bool(forKey: VC64Keys.warpLoad))
        c64.drive1.setSendSoundMessages(defaults.bool(forKey: VC64Keys.driveNoise))
        c64.drive2.setSendSoundMessages(defaults.bool(forKey: VC64Keys.driveNoise))

        // Screenshots
        screenshotResolution = defaults.integer(forKey: VC64Keys.screenshotResolution)
        screenshotFormat = defaults.string(forKey: VC64Keys.screenshotFormat) ?? "png"
        
        // User Dialogs
        autoMount = defaults.bool(forKey: VC64Keys.autoMount)
        closeWithoutAsking = defaults.bool(forKey: VC64Keys.closeWithoutAsking)
        ejectWithoutAsking = defaults.bool(forKey: VC64Keys.ejectWithoutAsking)

        // Misc
        pauseInBackground = defaults.bool(forKey: VC64Keys.pauseInBackground)
        c64.setSnapshotInterval(defaults.integer(forKey: VC64Keys.snapshotInterval))
        
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

    /// Saves the user defaults for all properties that are set in the devices dialog
    func saveDevicesUserDefaults() {
     
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
        defaults.set(c64.mouse.model(), forKey: VC64Keys.mouseModel)
    }
 
    func saveKeyMapUserDefaults() {
        
        let defaults = UserDefaults.standard
        if let keyMap = try? JSONEncoder().encode(keyboardcontroller.keyMap) {
            defaults.set(keyMap, forKey: VC64Keys.keyMap)
        }
    }
    
    /// Saves the user defaults for all properties that are set in the video preferences dialog
    func saveVideoUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        
        // Video
        defaults.set(metalScreen.videoUpscaler, forKey: VC64Keys.videoUpscaler)
        defaults.set(c64.vic.videoPalette(), forKey: VC64Keys.videoPalette)
        defaults.set(c64.vic.brightness(), forKey: VC64Keys.brightness)
        defaults.set(c64.vic.contrast(), forKey: VC64Keys.contrast)
        defaults.set(c64.vic.saturation(), forKey: VC64Keys.saturation)
        
        // Effects
        UserDefaults.standard.set(try?
            PropertyListEncoder().encode(metalScreen.shaderOptions),
                                  forKey: VC64Keys.shaderOptions)
        
        // Geometry
        defaults.set(metalScreen.fullscreenKeepAspectRatio, forKey: VC64Keys.aspectRatio)
        defaults.set(metalScreen.eyeX(), forKey: VC64Keys.eyeX)
        defaults.set(metalScreen.eyeY(), forKey: VC64Keys.eyeY)
        defaults.set(metalScreen.eyeZ(), forKey: VC64Keys.eyeZ)
    }
    
    /// Saves the user defaults for all properties that are set in the emulator preferences dialog
    func saveEmulatorUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        
        // Drive
        defaults.set(c64.warpLoad(), forKey: VC64Keys.warpLoad)
        defaults.set(c64.drive1.sendSoundMessages(), forKey: VC64Keys.driveNoise)
        defaults.set(c64.drive2.sendSoundMessages(), forKey: VC64Keys.driveNoise)
        
        // Screenshots
        defaults.set(screenshotResolution, forKey: VC64Keys.screenshotResolution)
        defaults.set(screenshotFormat, forKey: VC64Keys.screenshotFormat)
        
        // User Dialogs
        defaults.set(autoMount, forKey: VC64Keys.autoMount)
        defaults.set(closeWithoutAsking, forKey: VC64Keys.closeWithoutAsking)
        defaults.set(ejectWithoutAsking, forKey: VC64Keys.ejectWithoutAsking)
        
        // Misc
        defaults.set(pauseInBackground, forKey: VC64Keys.pauseInBackground)
        defaults.set(c64.snapshotInterval(), forKey: VC64Keys.snapshotInterval)
    }
    
    /// Saves the user defaults for all properties that are set in the hardware preferences dialog
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
