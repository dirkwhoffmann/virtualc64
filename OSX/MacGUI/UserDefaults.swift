//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation


//
// User defaults (all)
//

extension MyController {
    
    static func registerUserDefaults() {
        
        track()
        
        registerGeneralUserDefaults()
        registerRomUserDefaults()
        registerKeyMapUserDefaults()
        registerDevicesUserDefaults()
        registerVideoUserDefaults()
        registerEmulatorUserDefaults()
        registerHardwareUserDefaults()
    }
    
    func loadUserDefaults() {
        
        track()
        
        c64.suspend()
        
        loadGeneralUserDefaults()
        loadRomUserDefaults()
        loadKeyMapUserDefaults()
        loadDevicesUserDefaults()
        loadVideoUserDefaults()
        loadEmulatorUserDefaults()
        loadHardwareUserDefaults()
        
        c64.resume()
    }
    
    func loadUserDefaults(url: URL) {
        
        if let fileContents = NSDictionary(contentsOf: url) {
            
            if let dict = fileContents as? Dictionary<String,Any> {
                
                let filteredDict = dict.filter { $0.0.hasPrefix("VC64") }
                
                let defaults = UserDefaults.standard
                defaults.setValuesForKeys(filteredDict)
                
                loadUserDefaults()
            }
        }
    }
    
    func saveUserDefaults() {
        
        track()
        
        saveGeneralUserDefaults()
        saveRomUserDefaults()
        saveKeyMapUserDefaults()
        saveDevicesUserDefaults()
        saveVideoUserDefaults()
        saveEmulatorUserDefaults()
        saveHardwareUserDefaults()
    }

    func saveUserDefaults(url: URL) {
        
        track()
        
        let dict = UserDefaults.standard.dictionaryRepresentation()
        let filteredDict = dict.filter { $0.0.hasPrefix("VC64") }
        let nsDict = NSDictionary.init(dictionary: filteredDict)
        nsDict.write(to: url, atomically: true)
    }
    
}

//
// User defaults (general)
//

struct VC64Keys {
    
    // Control ports
    static let inputDevice1      = "VC64InputDevice1"
    static let inputDevice2      = "VC64InputDevice2"
    
    // Keyboard mapping mode
    static let mapKeysByPosition = "VC64MapKeysByPosition"
}

struct Defaults {
    
    // Control ports
    static let inputDevice1 = -1
    static let inputDevice2 = -1
    
    // Keyboard mapping mode
    static let mapKeysByPosition = false
}

extension MyController {

    static func registerGeneralUserDefaults() {
        
        let dictionary : [String:Any] = [
            
            VC64Keys.inputDevice1: Defaults.inputDevice1,
            VC64Keys.inputDevice2: Defaults.inputDevice2,
            
            VC64Keys.mapKeysByPosition: Defaults.mapKeysByPosition
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
    }
    
    func loadGeneralUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        c64.suspend()
        
        inputDevice1 = UserDefaults.standard.integer(forKey: VC64Keys.inputDevice1)
        inputDevice2 = UserDefaults.standard.integer(forKey: VC64Keys.inputDevice2)
        keyboardcontroller.mapKeysByPosition = defaults.bool(forKey: VC64Keys.mapKeysByPosition)
        
        c64.resume()
    }
    
    func saveGeneralUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        defaults.set(inputDevice1, forKey: VC64Keys.inputDevice1)
        defaults.set(inputDevice2, forKey: VC64Keys.inputDevice2)
        defaults.set(keyboardcontroller.mapKeysByPosition, forKey: VC64Keys.mapKeysByPosition)
    }
}

//
// User defaults (Roms)
//

extension VC64Keys {
    
    static let basicRom          = "VC64BasicRomFileKey"
    static let charRom           = "VC64CharRomFileKey"
    static let kernalRom         = "VC64KernelRomFileKey"
    static let vc1541Rom         = "VC64VC1541RomFileKey"
}

extension Defaults {
    
    static let basicRom = URL(fileURLWithPath: "/")
    static let charRom = URL(fileURLWithPath: "/")
    static let kernalRom = URL(fileURLWithPath: "/")
    static let vc1541Rom = URL(fileURLWithPath: "/")
}

extension MyController {
    
    static func registerRomUserDefaults() {
        
        let dictionary : [String:Any] = [
            
            VC64Keys.basicRom: Defaults.basicRom,
            VC64Keys.charRom: Defaults.charRom,
            VC64Keys.kernalRom: Defaults.kernalRom,
            VC64Keys.vc1541Rom: Defaults.vc1541Rom,
            ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
    }

    func loadRomUserDefaults() {
        
        let defaults = UserDefaults.standard

        c64.suspend()
        
        loadRom(defaults.url(forKey: VC64Keys.basicRom))
        loadRom(defaults.url(forKey: VC64Keys.charRom))
        loadRom(defaults.url(forKey: VC64Keys.kernalRom))
        loadRom(defaults.url(forKey: VC64Keys.vc1541Rom))
        
        c64.resume()
    }
    
    func saveRomUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        defaults.set(basicRomURL, forKey: VC64Keys.basicRom)
        defaults.set(charRomURL, forKey: VC64Keys.charRom)
        defaults.set(kernalRomURL, forKey: VC64Keys.kernalRom)
        defaults.set(vc1541RomURL, forKey: VC64Keys.vc1541Rom)
    }
}

//
// User defaults (Keymap)
//

extension VC64Keys {
    static let keyMap = "VC64KeyMap"
}

extension Defaults {
    static let keyMap = KeyboardController.standardKeyMap
}

extension MyController {
    
    static func registerKeyMapUserDefaults() {
        
        if let data = try? JSONEncoder().encode(Defaults.keyMap) {
            UserDefaults.standard.register(defaults: [VC64Keys.keyMap: data])
        }
    }
    
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
    
    // Mouse
    static let mouseModel        = "VC64MouseModelKey"

    // Joysticks
    static let disconnectJoyKeys = "VC64DisconnectKeys"
    static let autofire          = "VC64Autofire"
    static let autofireBullets   = "VC64AutofireBullets"
    static let autofireFrequency = "VC64AutofireFrequency"
    static let joyKeyMap1        = "VC64JoyKeyMap1"
    static let joyKeyMap2        = "VC64JoyKeyMap2"
}

extension Defaults {
    
    // Mouse
    static let mouseModel        = MOUSE1350
    
    // Joysticks
    static let disconnectJoyKeys = true
    static let autofire          = false
    static let autofireBullets   = -3
    static let autofireFrequency = Float(2.5)
    
    static let joyKeyMap1 = [
        MacKey.curLeft:  JOYSTICK_LEFT.rawValue,
        MacKey.curRight: JOYSTICK_RIGHT.rawValue,
        MacKey.curUp:    JOYSTICK_UP.rawValue,
        MacKey.curDown:  JOYSTICK_DOWN.rawValue,
        MacKey.space:    JOYSTICK_FIRE.rawValue
    ]
    static let joyKeyMap2 = [
        MacKey.ansi.s:   JOYSTICK_LEFT.rawValue,
        MacKey.ansi.d:   JOYSTICK_RIGHT.rawValue,
        MacKey.ansi.e:   JOYSTICK_UP.rawValue,
        MacKey.ansi.x:   JOYSTICK_DOWN.rawValue,
        MacKey.ansi.c:   JOYSTICK_FIRE.rawValue
    ]
}
    
extension MyController {
    
    static func registerDevicesUserDefaults() {
        
        let dictionary : [String:Any] = [
            VC64Keys.mouseModel: Int(Defaults.mouseModel.rawValue),
            VC64Keys.disconnectJoyKeys: Defaults.disconnectJoyKeys,
            VC64Keys.autofire: Defaults.autofire,
            VC64Keys.autofireBullets: Defaults.autofireBullets,
            VC64Keys.autofireFrequency: Defaults.autofireFrequency
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
        
        if let data = try? JSONEncoder().encode(Defaults.joyKeyMap1) {
            UserDefaults.standard.register(defaults: [VC64Keys.joyKeyMap1: data])
        }
        if let data = try? JSONEncoder().encode(Defaults.joyKeyMap2) {
            UserDefaults.standard.register(defaults: [VC64Keys.joyKeyMap2: data])
        }
    }

    func loadDevicesUserDefaults() {
        
        let defaults = UserDefaults.standard
    
        c64.suspend()
        
        c64.mouse.setModel(defaults.integer(forKey: VC64Keys.mouseModel))
        keyboardcontroller.disconnectJoyKeys = defaults.bool(forKey: VC64Keys.disconnectJoyKeys)
        c64.port1.setAutofire(defaults.bool(forKey: VC64Keys.autofire))
        c64.port2.setAutofire(defaults.bool(forKey: VC64Keys.autofire))
        c64.port1.setAutofireBullets(defaults.integer(forKey: VC64Keys.autofireBullets))
        c64.port2.setAutofireBullets(defaults.integer(forKey: VC64Keys.autofireBullets))
        c64.port1.setAutofireFrequency(defaults.float(forKey: VC64Keys.autofireFrequency))
        c64.port2.setAutofireFrequency(defaults.float(forKey: VC64Keys.autofireFrequency))
        
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
        
        let defaults = UserDefaults.standard
        
        defaults.set(c64.mouse.model(), forKey: VC64Keys.mouseModel)
        defaults.set(keyboardcontroller.disconnectJoyKeys, forKey: VC64Keys.disconnectJoyKeys)
        defaults.set(c64.port1.autofire(), forKey: VC64Keys.autofire)
        defaults.set(c64.port1.autofireBullets(), forKey: VC64Keys.autofireBullets)
        defaults.set(c64.port1.autofireFrequency(), forKey: VC64Keys.autofireFrequency)
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
    
    static let palette         = "VC64PaletteKey"
    static let brightness      = "VC64BrightnessKey"
    static let contrast        = "VC64ContrastKey"
    static let saturation      = "VC64SaturationKey"
    static let upscaler        = "VC64UpscalerKey"

    // Geometry
    static let keepAspectRatio = "VC64FullscreenKeepAspectRatioKey"
    static let eyeX            = "VC64EyeX"
    static let eyeY            = "VC64EyeY"
    static let eyeZ            = "VC64EyeZ"
    
    // GPU options
    static let shaderOptions   = "VC64ShaderOptionsKey"
}

extension Defaults {
    
    static let palette = COLOR_PALETTE
    static let brightness = Double(50.0)
    static let contrast = Double(100.0)
    static let saturation = Double(50.0)
    static let upscaler = 0
    
    // Geometry
    static let keepAspectRatio = false
    static let eyeX = Float(0.0)
    static let eyeY = Float(0.0)
    static let eyeZ = Float(0.0)
    
    // GPU options
    static let shaderOptions = ShaderDefaultsTFT
}

extension MyController {
    
    static func registerVideoUserDefaults() {
        
        let dictionary : [String:Any] = [
            
            VC64Keys.palette: Int(Defaults.palette.rawValue),
            VC64Keys.brightness: Defaults.brightness,
            VC64Keys.contrast: Defaults.contrast,
            VC64Keys.saturation: Defaults.saturation,
            VC64Keys.upscaler: Defaults.upscaler,

            VC64Keys.keepAspectRatio: Defaults.keepAspectRatio,
            VC64Keys.eyeX: Defaults.eyeX,
            VC64Keys.eyeY: Defaults.eyeY,
            VC64Keys.eyeZ: Defaults.eyeZ,
            ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
        
        if let data = try? PropertyListEncoder().encode(Defaults.shaderOptions) {
            UserDefaults.standard.register(defaults: [VC64Keys.shaderOptions: data])
        }
    }
    
    func loadVideoUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        c64.suspend()
        
        metalScreen.upscaler = defaults.integer(forKey: VC64Keys.upscaler)
        c64.vic.setVideoPalette(defaults.integer(forKey: VC64Keys.palette))
        c64.vic.setBrightness(defaults.double(forKey: VC64Keys.brightness))
        c64.vic.setContrast(defaults.double(forKey: VC64Keys.contrast))
        c64.vic.setSaturation(defaults.double(forKey: VC64Keys.saturation))

        metalScreen.keepAspectRatio = defaults.bool(forKey: VC64Keys.keepAspectRatio)
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
        
        let defaults = UserDefaults.standard
        
        defaults.set(metalScreen.upscaler, forKey: VC64Keys.upscaler)
        defaults.set(c64.vic.videoPalette(), forKey: VC64Keys.palette)
        defaults.set(c64.vic.brightness(), forKey: VC64Keys.brightness)
        defaults.set(c64.vic.contrast(), forKey: VC64Keys.contrast)
        defaults.set(c64.vic.saturation(), forKey: VC64Keys.saturation)
        
        defaults.set(metalScreen.keepAspectRatio, forKey: VC64Keys.keepAspectRatio)
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
    static let screenshotSource     = "VC64ScreenshotSourceKey"
    static let screenshotTarget     = "VC64ScreenshotTargetKey"
    
    // User dialogs
    static let autoMount            = "VC64AutoMount"
    static let closeWithoutAsking   = "VC64CloseWithoutAsking"
    static let ejectWithoutAsking   = "VC64EjectWithoutAsking"
    
    // Miscellaneous
    static let pauseInBackground    = "VC64PauseInBackground"
    static let snapshotInterval     = "VC64SnapshotInterval"
}

extension Defaults {
   
    // Drives
    static let warpLoad             = true
    static let driveNoise           = true
    
    // Screenshots
    static let screenshotSource     = 0
    static let screenshotTarget     = NSBitmapImageRep.FileType.png
    
    // User dialogs
    static let autoMount            = false
    static let closeWithoutAsking   = false
    static let ejectWithoutAsking   = false
    
    // Miscellaneous
    static let pauseInBackground    = false
    static let snapshotInterval     = 3
}

extension MyController {
    
    static func registerEmulatorUserDefaults() {
        
        let dictionary : [String:Any] = [
            
            VC64Keys.warpLoad: Defaults.warpLoad,
            VC64Keys.driveNoise: Defaults.driveNoise,
            
            VC64Keys.screenshotSource: Defaults.screenshotSource,
            VC64Keys.screenshotTarget: Int(Defaults.screenshotTarget.rawValue),

            VC64Keys.autoMount: Defaults.autoMount,
            VC64Keys.closeWithoutAsking: Defaults.closeWithoutAsking,
            VC64Keys.ejectWithoutAsking: Defaults.ejectWithoutAsking,

            VC64Keys.pauseInBackground: Defaults.pauseInBackground,
            VC64Keys.snapshotInterval: Defaults.snapshotInterval
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
    }
    
    func loadEmulatorUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        c64.suspend()
        
        c64.setWarpLoad(defaults.bool(forKey: VC64Keys.warpLoad))
        c64.drive1.setSendSoundMessages(defaults.bool(forKey: VC64Keys.driveNoise))
        c64.drive2.setSendSoundMessages(defaults.bool(forKey: VC64Keys.driveNoise))
    
        screenshotSource = defaults.integer(forKey: VC64Keys.screenshotSource)
        screenshotTargetIntValue = defaults.integer(forKey: VC64Keys.screenshotTarget)
    
        autoMount = defaults.bool(forKey: VC64Keys.autoMount)
        closeWithoutAsking = defaults.bool(forKey: VC64Keys.closeWithoutAsking)
        ejectWithoutAsking = defaults.bool(forKey: VC64Keys.ejectWithoutAsking)

        pauseInBackground = defaults.bool(forKey: VC64Keys.pauseInBackground)
        c64.setSnapshotInterval(defaults.integer(forKey: VC64Keys.snapshotInterval))
        
        c64.resume()
    }
    
    func saveEmulatorUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        defaults.set(c64.warpLoad(), forKey: VC64Keys.warpLoad)
        defaults.set(c64.drive1.sendSoundMessages(), forKey: VC64Keys.driveNoise)
        defaults.set(c64.drive2.sendSoundMessages(), forKey: VC64Keys.driveNoise)

        defaults.set(screenshotSource, forKey: VC64Keys.screenshotSource)
        defaults.set(screenshotTargetIntValue, forKey: VC64Keys.screenshotTarget)
        
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
    static let vicChip        = "VC64VICChipModelKey"
    static let grayDotBug     = "VC64VICGrayDotBugKey"
    
    // CIAs
    static let ciaChip        = "VC64CIAChipModelKey"
    static let timerBBug      = "VC64CIATimerBBugKey"
    
    // SID
    static let reSID          = "VC64SIDReSIDKey"
    static let audioChip      = "VC64SIDChipModelKey"
    static let audioFilter    = "VC64SIDFilterKey"
    static let samplingMethod = "VC64SIDSamplingMethodKey"
    
    // Logic board and RAM
    static let glueLogic      = "VC64GlueLogicKey"
    static let initPattern    = "VC64InitPatternKey"
}

extension Defaults {
 
    //VICII
    static let vicChip        = PAL_8565
    static let grayDotBug     = true
    
    // CIAs
    static let ciaChip        = PAL_8565
    static let timerBBug      = true
    
    // SID
    static let reSID          = true
    static let audioChip      = MOS_8580
    static let audioFilter    = false
    static let samplingMethod = 0
    
    // Logic board and RAM
    static let glueLogic      = GLUE_DISCRETE
    static let initPattern    = INIT_PATTERN_C64
}

extension MyController {
    
    static func registerHardwareUserDefaults() {
        
        let dictionary : [String:Any] = [
        
            VC64Keys.vicChip: Int(Defaults.vicChip.rawValue),
            VC64Keys.grayDotBug: Defaults.grayDotBug,

            VC64Keys.ciaChip: Int(Defaults.ciaChip.rawValue),
            VC64Keys.timerBBug: Defaults.timerBBug,

            VC64Keys.reSID: Defaults.reSID,
            VC64Keys.audioChip: Int(Defaults.audioChip.rawValue),
            VC64Keys.audioFilter: false,
            VC64Keys.samplingMethod: 0,

            VC64Keys.glueLogic: Int(Defaults.glueLogic.rawValue),
            VC64Keys.initPattern: Int(Defaults.initPattern.rawValue)
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
    }
    
    func loadHardwareUserDefaults() {
        
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
