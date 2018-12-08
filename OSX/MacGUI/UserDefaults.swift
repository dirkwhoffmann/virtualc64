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
    
    // Roms
    static let basicRom             = "VC64BasicRomFileKey"
    static let charRom              = "VC64CharRomFileKey"
    static let kernalRom            = "VC64KernelRomFileKey"
    static let vc1541Rom            = "VC64VC1541RomFileKey"

    // Control ports
    static let inputDevice1         = "VC64InputDevice1"
    static let inputDevice2         = "VC64InputDevice2"
    
    // Keyboard
    static let mapKeysByPosition    = "VC64MapKeysByPosition"
    static let keyMap               = "VC64KeyMap"
}

//
// User defaults (all)
//

extension MyController {
    
    static func registerUserDefaults() {
        
        track()
        
        let dictionary : [String:Any] = [
            
            VC64Keys.basicRom: "",
            VC64Keys.charRom: "",
            VC64Keys.kernalRom: "",
            VC64Keys.vc1541Rom: "",
            
            VC64Keys.inputDevice1: -1,
            VC64Keys.inputDevice2: -1,

            VC64Keys.mapKeysByPosition: false
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
        
        registerDevicesUserDefaults()
        registerVideoUserDefaults()
        registerEmulatorUserDefaults()
        registerHardwareUserDefaults()
    }
    
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
}

extension MyController {
    
    func saveKeyMapUserDefaults() {
        
        let defaults = UserDefaults.standard
        if let keyMap = try? JSONEncoder().encode(keyboardcontroller.keyMap) {
            defaults.set(keyMap, forKey: VC64Keys.keyMap)
        }
    }
    
    func loadKeyMapUserDefaults() {
        
        let defaults = UserDefaults.standard
        if let data = defaults.data(forKey: VC64Keys.keyMap) {
            if let keyMap = try? JSONDecoder().decode([MacKey:C64Key].self, from: data) {
                keyboardcontroller.keyMap = keyMap
            }
        }
    }
}

//
// User defaults (Devices)
//

extension VC64Keys {
    
    // Joysticks
    static let joyKeyMap1           = "VC64JoyKeyMap1"
    static let joyKeyMap2           = "VC64JoyKeyMap2"
    static let disconnectKeys       = "VC64DisconnectKeys"
    
    static let autofire             = "VC64Autofire"
    static let autofireBullets      = "VC64AutofireBullets"
    static let autofireFrequency    = "VC64AutofireFrequency"
    
    // Mouse
    static let mouseModel           = "VC64MouseModelKey"
}

extension MyController {
    
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

    func loadDevicesUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        
        c64.suspend()
        keyboardcontroller.disconnectEmulationKeys = defaults.bool(forKey: VC64Keys.disconnectKeys)
        c64.port1.setAutofire(defaults.bool(forKey: VC64Keys.autofire))
        c64.port2.setAutofire(defaults.bool(forKey: VC64Keys.autofire))
        c64.port1.setAutofireBullets(defaults.integer(forKey: VC64Keys.autofireBullets))
        c64.port2.setAutofireBullets(defaults.integer(forKey: VC64Keys.autofireBullets))
        c64.port1.setAutofireFrequency(defaults.float(forKey: VC64Keys.autofireFrequency))
        c64.port2.setAutofireFrequency(defaults.float(forKey: VC64Keys.autofireFrequency))
        
        c64.mouse.setModel(defaults.integer(forKey: VC64Keys.mouseModel))
        
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
        c64.resume()
    }
    
    func saveDevicesUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        
        defaults.set(keyboardcontroller.disconnectEmulationKeys, forKey: VC64Keys.disconnectKeys)
        defaults.set(c64.port1.autofire(), forKey: VC64Keys.autofire)
        defaults.set(c64.port1.autofireBullets(), forKey: VC64Keys.autofireBullets)
        defaults.set(c64.port1.autofireFrequency(), forKey: VC64Keys.autofireFrequency)
        
        defaults.set(c64.mouse.model(), forKey: VC64Keys.mouseModel)
        
        if let keyMap = try? JSONEncoder().encode(gamePadManager.gamePads[0]?.keyMap) {
            defaults.set(keyMap, forKey: VC64Keys.joyKeyMap1)
        }
        if let keyMap = try? JSONEncoder().encode(gamePadManager.gamePads[1]?.keyMap) {
            defaults.set(keyMap, forKey: VC64Keys.joyKeyMap2)
        }
    }
}

//
// User defaults (Video)
//

extension VC64Keys {
    
    static let videoUpscaler        = "VC64UpscalerKey"
    static let videoPalette         = "VC64PaletteKey"
    static let brightness           = "VC64BrightnessKey"
    static let contrast             = "VC64ContrastKey"
    static let saturation           = "VC64SaturationKey"
    
    // Geometry
    static let aspectRatio          = "VC64FullscreenKeepAspectRatioKey"
    static let eyeX                 = "VC64EyeX"
    static let eyeY                 = "VC64EyeY"
    static let eyeZ                 = "VC64EyeZ"
    
    // GPU options
    static let shaderOptions        = "VC64ShaderOptionsKey"
}

extension MyController {
    
    static func registerVideoUserDefaults() {
        
        track()
        let dictionary : [String:Any] = [
            
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
    
    func loadVideoUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        
        c64.suspend()
        
        metalScreen.videoUpscaler = defaults.integer(forKey: VC64Keys.videoUpscaler)
        c64.vic.setVideoPalette(defaults.integer(forKey: VC64Keys.videoPalette))
        c64.vic.setBrightness(defaults.double(forKey: VC64Keys.brightness))
        c64.vic.setContrast(defaults.double(forKey: VC64Keys.contrast))
        c64.vic.setSaturation(defaults.double(forKey: VC64Keys.saturation))

        metalScreen.fullscreenKeepAspectRatio = defaults.bool(forKey: VC64Keys.aspectRatio)
        metalScreen.setEyeX(defaults.float(forKey: VC64Keys.eyeX))
        metalScreen.setEyeY(defaults.float(forKey: VC64Keys.eyeY))
        metalScreen.setEyeZ(defaults.float(forKey: VC64Keys.eyeZ))

        if let data = defaults.value(forKey: VC64Keys.shaderOptions) as? Data {
            if let options = try? PropertyListDecoder().decode(ShaderOptions.self, from: data) {
                metalScreen.shaderOptions = options
                metalScreen.buildDotMasks()
            }
        }
        c64.resume()
    }
    
    func saveVideoUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        
        defaults.set(metalScreen.videoUpscaler, forKey: VC64Keys.videoUpscaler)
        defaults.set(c64.vic.videoPalette(), forKey: VC64Keys.videoPalette)
        defaults.set(c64.vic.brightness(), forKey: VC64Keys.brightness)
        defaults.set(c64.vic.contrast(), forKey: VC64Keys.contrast)
        defaults.set(c64.vic.saturation(), forKey: VC64Keys.saturation)
        
        defaults.set(metalScreen.fullscreenKeepAspectRatio, forKey: VC64Keys.aspectRatio)
        defaults.set(metalScreen.eyeX(), forKey: VC64Keys.eyeX)
        defaults.set(metalScreen.eyeY(), forKey: VC64Keys.eyeY)
        defaults.set(metalScreen.eyeZ(), forKey: VC64Keys.eyeZ)
        
        UserDefaults.standard.set(try?
            PropertyListEncoder().encode(metalScreen.shaderOptions),
                                  forKey: VC64Keys.shaderOptions)
    }
}


//
// User defaults (Emulator)
//

extension VC64Keys {
    
    // Drives
    static let warpLoad             = "VC64WarpLoadKey"
    static let driveNoise           = "VC64DriveNoiseKey"
    
    // Screenshots
    static let screenshotResolution = "VC64ScreenshotResolutionKey"
    static let screenshotFormat     = "VC64ScreenshotFormatKey"
    
    // User dialogs
    static let autoMount            = "VC64AutoMount"
    static let closeWithoutAsking   = "VC64CloseWithoutAsking"
    static let ejectWithoutAsking   = "VC64EjectWithoutAsking"
    
    // Miscellaneous
    static let pauseInBackground    = "VC64PauseInBackground"
    static let snapshotInterval     = "VC64SnapshotInterval"
}

extension MyController {
    
    static func registerEmulatorUserDefaults() {
        
        track()
        let dictionary : [String:Any] = [
            
            VC64Keys.warpLoad: true,
            VC64Keys.driveNoise: true,
            
            VC64Keys.screenshotResolution: 0,
            VC64Keys.screenshotFormat: Int(NSBitmapImageRep.FileType.png.rawValue),

            VC64Keys.autoMount: false,
            VC64Keys.closeWithoutAsking: false,
            VC64Keys.ejectWithoutAsking: false,

            VC64Keys.pauseInBackground: false,
            VC64Keys.snapshotInterval: 3
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
    }
    
    func loadEmulatorUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        
        c64.suspend()
        c64.setWarpLoad(defaults.bool(forKey: VC64Keys.warpLoad))
        c64.drive1.setSendSoundMessages(defaults.bool(forKey: VC64Keys.driveNoise))
        c64.drive2.setSendSoundMessages(defaults.bool(forKey: VC64Keys.driveNoise))
    
        screenshotResolution = defaults.integer(forKey: VC64Keys.screenshotResolution)
        screenshotFormatIntValue = defaults.integer(forKey: VC64Keys.screenshotFormat)
    
        autoMount = defaults.bool(forKey: VC64Keys.autoMount)
        closeWithoutAsking = defaults.bool(forKey: VC64Keys.closeWithoutAsking)
        ejectWithoutAsking = defaults.bool(forKey: VC64Keys.ejectWithoutAsking)

        pauseInBackground = defaults.bool(forKey: VC64Keys.pauseInBackground)
        c64.setSnapshotInterval(defaults.integer(forKey: VC64Keys.snapshotInterval))
        c64.resume()
    }
    
    func saveEmulatorUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        
        defaults.set(c64.warpLoad(), forKey: VC64Keys.warpLoad)
        defaults.set(c64.drive1.sendSoundMessages(), forKey: VC64Keys.driveNoise)
        defaults.set(c64.drive2.sendSoundMessages(), forKey: VC64Keys.driveNoise)

        defaults.set(screenshotResolution, forKey: VC64Keys.screenshotResolution)
        defaults.set(screenshotFormatIntValue, forKey: VC64Keys.screenshotFormat)
        
        defaults.set(autoMount, forKey: VC64Keys.autoMount)
        defaults.set(closeWithoutAsking, forKey: VC64Keys.closeWithoutAsking)
        defaults.set(ejectWithoutAsking, forKey: VC64Keys.ejectWithoutAsking)
        
        defaults.set(pauseInBackground, forKey: VC64Keys.pauseInBackground)
        defaults.set(c64.snapshotInterval(), forKey: VC64Keys.snapshotInterval)
    }
}


//
// User defaults (Hardware)
//

extension VC64Keys {
    
    //VICII
    static let vicChip              = "VC64VICChipModelKey"
    static let grayDotBug           = "VC64VICGrayDotBugKey"
    
    // CIAs
    static let ciaChip              = "VC64CIAChipModelKey"
    static let timerBBug            = "VC64CIATimerBBugKey"
    
    // SID
    static let reSID                = "VC64SIDReSIDKey"
    static let audioChip            = "VC64SIDChipModelKey"
    static let audioFilter          = "VC64SIDFilterKey"
    static let samplingMethod       = "VC64SIDSamplingMethodKey"
    
    // Logic board and RAM
    static let glueLogic            = "VC64GlueLogicKey"
    static let initPattern          = "VC64InitPatternKey"
}

extension MyController {
    
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
    
    func loadHardwareUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        
        c64.suspend()
        c64.vic.setModel(defaults.integer(forKey: VC64Keys.vicChip))
        c64.vic.setEmulateGrayDotBug(defaults.bool(forKey: VC64Keys.grayDotBug))

        c64.cia1.setModel(defaults.integer(forKey: VC64Keys.ciaChip))
        c64.cia2.setModel(defaults.integer(forKey: VC64Keys.ciaChip))
        c64.cia1.setEmulateTimerBBug(defaults.bool(forKey: VC64Keys.timerBBug))
        c64.cia2.setEmulateTimerBBug(defaults.bool(forKey: VC64Keys.timerBBug))

        c64.sid.setReSID(defaults.bool(forKey: VC64Keys.reSID))
        c64.sid.setModel(defaults.integer(forKey: VC64Keys.audioChip))
        c64.sid.setAudioFilter(defaults.bool(forKey: VC64Keys.audioFilter))
        c64.sid.setSamplingMethod(defaults.integer(forKey: VC64Keys.samplingMethod))
        
        c64.vic.setGlueLogic(defaults.integer(forKey: VC64Keys.glueLogic))
        c64.mem.setRamInitPattern(defaults.integer(forKey: VC64Keys.initPattern))
        c64.resume()
    }

    func saveHardwareUserDefaults() {
        
        track()
        let defaults = UserDefaults.standard
        
        defaults.set(c64.vic.model(), forKey: VC64Keys.vicChip)
        defaults.set(c64.vic.emulateGrayDotBug(), forKey: VC64Keys.grayDotBug)

        defaults.set(c64.cia1.model(), forKey: VC64Keys.ciaChip)
        defaults.set(c64.cia1.emulateTimerBBug(), forKey: VC64Keys.timerBBug)

        defaults.set(c64.sid.reSID(), forKey: VC64Keys.reSID)
        defaults.set(c64.sid.model(), forKey: VC64Keys.audioChip)
        defaults.set(c64.sid.audioFilter(), forKey: VC64Keys.audioFilter)
        defaults.set(c64.sid.samplingMethod(), forKey: VC64Keys.samplingMethod)
        
        defaults.set(c64.vic.glueLogic(), forKey: VC64Keys.glueLogic)
        defaults.set(c64.mem.ramInitPattern(), forKey: VC64Keys.initPattern)
    }
}
