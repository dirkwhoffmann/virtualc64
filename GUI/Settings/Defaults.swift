// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// swiftlint:disable colon

//
// Convenience extensions to UserDefaults
//

extension UserDefaults {
    
    // Registers an item of generic type 'Encodable'
    func register<T: Encodable>(encodableItem item: T, forKey key: String) {
        
        if let data = try? PropertyListEncoder().encode(item) {
            register(defaults: [key: data])
        }
    }

    // Encodes an item of generic type 'Encodable'
    func encode<T: Encodable>(_ item: T, forKey key: String) {
        
        if let encoded = try? PropertyListEncoder().encode(item) {
            set(encoded, forKey: key)
        } else {
            track("Failed to encode \(key)")
        }
    }
    
    // Encodes an item of generic type 'Decodable'
    func decode<T: Decodable>(_ item: inout T, forKey key: String) {
        
        if let data = data(forKey: key) {
            if let decoded = try? PropertyListDecoder().decode(T.self, from: data) {
                item = decoded
            } else {
                track("Failed to decode \(key)")
            }
        }
    }
}

//
// User defaults (all)
//

extension UserDefaults {
    
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
    
    static func resetUserDefaults() {
        
        track()
                
        resetGeneralUserDefaults()
        resetRomUserDefaults()
        resetKeyMapUserDefaults()
        resetDevicesUserDefaults()
        resetVideoUserDefaults()
        resetEmulatorUserDefaults()
        resetHardwareUserDefaults()        
    }
}

extension MyController {
    
    func loadUserDefaults() {
        
        track()
        
        c64.suspend()
        
        loadGeneralUserDefaults()
        loadDevicesUserDefaults()
        loadKeyMapUserDefaults()

        config.loadRomUserDefaults()
        config.loadHardwareUserDefaults()
        loadVideoUserDefaults()
        loadEmulatorUserDefaults()
        
        c64.resume()
    }
    
    func loadUserDefaults(url: URL) {
        
        if let fileContents = NSDictionary(contentsOf: url) {
            
            if let dict = fileContents as? [String: Any] {
                
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
        saveDevicesUserDefaults()
        saveKeyMapUserDefaults()

        config.saveRomUserDefaults()
        config.saveHardwareUserDefaults()
        saveVideoUserDefaults()
        saveEmulatorUserDefaults()
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

struct Keys {
    
    // Control ports
    static let inputDevice1      = "VC64InputDevice1"
    static let inputDevice2      = "VC64InputDevice2"
}

struct Defaults {
    
    // Control ports
    static let inputDevice1 = -1
    static let inputDevice2 = -1
}

extension UserDefaults {

    static func registerGeneralUserDefaults() {
        
        let dictionary: [String: Any] = [
            
            Keys.inputDevice1: Defaults.inputDevice1,
            Keys.inputDevice2: Defaults.inputDevice2
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
    }
    
    static func resetGeneralUserDefaults() {
        
        let defaults = UserDefaults.standard

        let keys = [ Keys.inputDevice1,
                     Keys.inputDevice2,
                     
                     Keys.mapKeysByPosition
        ]

        for key in keys { defaults.removeObject(forKey: key) }
    }
}

extension MyController {
        
    func loadGeneralUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        c64.suspend()
        
        setPort1(defaults.integer(forKey: Keys.inputDevice1))
        setPort2(defaults.integer(forKey: Keys.inputDevice2))
        
        // inputDevice1 = defaults.integer(forKey: VC64Keys.inputDevice1)
        // inputDevice2 = defaults.integer(forKey: VC64Keys.inputDevice2)
        
        c64.resume()
    }
    
    func saveGeneralUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        defaults.set(inputDevice1, forKey: Keys.inputDevice1)
        defaults.set(inputDevice2, forKey: Keys.inputDevice2)
    }
}

//
// User defaults (Roms)
//

extension UserDefaults {
    
    static func romUrl(name: String) -> URL? {
        
        let folder = URL.appSupportFolder("Roms")
        return folder?.appendingPathComponent(name)
    }
    
    static var basicRomUrl:  URL? { return romUrl(name: "basic.bin") }
    static var charRomUrl:   URL? { return romUrl(name: "char.bin") }
    static var kernalRomUrl: URL? { return romUrl(name: "kernal.bin") }
    static var vc1541RomUrl: URL? { return romUrl(name: "vc1541.bin") }
    
    static func registerRomUserDefaults() {
        
    }

    static func resetRomUserDefaults() {
        
        // Delete previously saved Rom files
        let fm = FileManager.default
        
        if let url = basicRomUrl {
            track("Deleting Basic Rom")
            try? fm.removeItem(at: url)
        }
        if let url = charRomUrl {
            track("Deleting Character Rom")
            try? fm.removeItem(at: url)
        }
        if let url = kernalRomUrl {
            track("Deleting Kernal Rom")
            try? fm.removeItem(at: url)
        }
        if let url = vc1541RomUrl {
            track("Deleting VC1541 Rom")
            try? fm.removeItem(at: url)
        }
    }
}

//
// User defaults (Keymap)
//

extension Keys {
    static let keyMap = "VC64KeyMap"
    static let mapKeysByPosition = "VC64MapKeysByPosition"
}

extension Defaults {
    static let keyMap = KeyboardController.standardKeyMap
    static let mapKeysByPosition = false
}

extension UserDefaults {
    
    static func registerKeyMapUserDefaults() {
        
        let dictionary: [String: Any] = [
            
            Keys.mapKeysByPosition: Defaults.mapKeysByPosition
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
        defaults.register(encodableItem: Defaults.keyMap, forKey: Keys.keyMap)
    }
    
    static func resetKeyMapUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        let keys = [ Keys.mapKeysByPosition,
                     Keys.keyMap
        ]

        for key in keys { defaults.removeObject(forKey: key) }
    }
}

extension MyController {
        
    func loadKeyMapUserDefaults() {
        
        let defaults = UserDefaults.standard
        keyboard.mapKeysByPosition = defaults.bool(forKey: Keys.mapKeysByPosition)
        defaults.decode(&keyboard.keyMap, forKey: Keys.keyMap)
    }
    
    func saveKeyMapUserDefaults() {
        
        let defaults = UserDefaults.standard
        defaults.encode(keyboard.keyMap, forKey: Keys.keyMap)
        defaults.set(keyboard.mapKeysByPosition, forKey: Keys.mapKeysByPosition)
    }
}

//
// User defaults (Devices)
//

extension Keys {
    
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
        MacKey.curLeft: JOYSTICK_LEFT.rawValue,
        MacKey.curRight: JOYSTICK_RIGHT.rawValue,
        MacKey.curUp: JOYSTICK_UP.rawValue,
        MacKey.curDown: JOYSTICK_DOWN.rawValue,
        MacKey.space: JOYSTICK_FIRE.rawValue
    ]
    static let joyKeyMap2 = [
        MacKey.Ansi.s: JOYSTICK_LEFT.rawValue,
        MacKey.Ansi.d: JOYSTICK_RIGHT.rawValue,
        MacKey.Ansi.e: JOYSTICK_UP.rawValue,
        MacKey.Ansi.x: JOYSTICK_DOWN.rawValue,
        MacKey.Ansi.c: JOYSTICK_FIRE.rawValue
    ]
}
    
extension UserDefaults {
    
    static func registerDevicesUserDefaults() {
        
        let dictionary: [String: Any] = [
            Keys.mouseModel: Int(Defaults.mouseModel.rawValue),
            Keys.disconnectJoyKeys: Defaults.disconnectJoyKeys,
            Keys.autofire: Defaults.autofire,
            Keys.autofireBullets: Defaults.autofireBullets,
            Keys.autofireFrequency: Defaults.autofireFrequency
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
        defaults.register(encodableItem: Defaults.joyKeyMap1, forKey: Keys.joyKeyMap1)
        defaults.register(encodableItem: Defaults.joyKeyMap2, forKey: Keys.joyKeyMap2)
    }

    static func resetDevicesUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        let keys = [ Keys.mouseModel,
                     Keys.disconnectJoyKeys,
                     Keys.autofire,
                     Keys.autofireBullets,
                     Keys.autofireFrequency,
                     
                     Keys.joyKeyMap1,
                     Keys.joyKeyMap2
        ]

        for key in keys { defaults.removeObject(forKey: key) }
    }
}
 
extension MyController {
    
    func loadDevicesUserDefaults() {
        
        let defaults = UserDefaults.standard
    
        c64.suspend()
        
        c64.mouse.setModel(defaults.integer(forKey: Keys.mouseModel))
        keyboard.disconnectJoyKeys = defaults.bool(forKey: Keys.disconnectJoyKeys)
        c64.port1.setAutofire(defaults.bool(forKey: Keys.autofire))
        c64.port2.setAutofire(defaults.bool(forKey: Keys.autofire))
        c64.port1.setAutofireBullets(defaults.integer(forKey: Keys.autofireBullets))
        c64.port2.setAutofireBullets(defaults.integer(forKey: Keys.autofireBullets))
        c64.port1.setAutofireFrequency(defaults.float(forKey: Keys.autofireFrequency))
        c64.port2.setAutofireFrequency(defaults.float(forKey: Keys.autofireFrequency))
        defaults.decode(&gamePadManager.gamePads[0]!.keyMap, forKey: Keys.joyKeyMap1)
        defaults.decode(&gamePadManager.gamePads[1]!.keyMap, forKey: Keys.joyKeyMap2)
 
        c64.resume()
    }
    
    func saveDevicesUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        defaults.set(c64.mouse.model(), forKey: Keys.mouseModel)
        defaults.set(keyboard.disconnectJoyKeys, forKey: Keys.disconnectJoyKeys)
        defaults.set(c64.port1.autofire(), forKey: Keys.autofire)
        defaults.set(c64.port1.autofireBullets(), forKey: Keys.autofireBullets)
        defaults.set(c64.port1.autofireFrequency(), forKey: Keys.autofireFrequency)
        defaults.encode(gamePadManager.gamePads[0]!.keyMap, forKey: Keys.joyKeyMap1)
        defaults.encode(gamePadManager.gamePads[1]!.keyMap, forKey: Keys.joyKeyMap2)
    }
}

//
// User defaults (Video)
//

extension Keys {
    
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

extension UserDefaults {
    
    static func registerVideoUserDefaults() {
        
        let dictionary: [String: Any] = [
            
            Keys.palette: Int(Defaults.palette.rawValue),
            Keys.brightness: Defaults.brightness,
            Keys.contrast: Defaults.contrast,
            Keys.saturation: Defaults.saturation,
            Keys.upscaler: Defaults.upscaler,

            Keys.keepAspectRatio: Defaults.keepAspectRatio,
            Keys.eyeX: Defaults.eyeX,
            Keys.eyeY: Defaults.eyeY,
            Keys.eyeZ: Defaults.eyeZ
        ]
        
        let defaults = UserDefaults.standard
        defaults.register(defaults: dictionary)
        defaults.register(encodableItem: Defaults.shaderOptions, forKey: Keys.shaderOptions)
    }
}

extension UserDefaults {
    
    static func resetVideoUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        let keys = [ Keys.palette,
                     Keys.brightness,
                     Keys.contrast,
                     Keys.saturation,
                     Keys.upscaler,
                     
                     Keys.keepAspectRatio,
                     Keys.eyeX,
                     Keys.eyeY,
                     Keys.eyeZ,
                     
                     Keys.shaderOptions
        ]

        for key in keys { defaults.removeObject(forKey: key) }
    }
}

extension MyController {
        
    func loadVideoUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        c64.suspend()
        
        renderer.upscaler = defaults.integer(forKey: Keys.upscaler)
        c64.vic.setVideoPalette(defaults.integer(forKey: Keys.palette))
        c64.vic.setBrightness(defaults.double(forKey: Keys.brightness))
        c64.vic.setContrast(defaults.double(forKey: Keys.contrast))
        c64.vic.setSaturation(defaults.double(forKey: Keys.saturation))

        renderer.keepAspectRatio = defaults.bool(forKey: Keys.keepAspectRatio)
        /*
        renderer.setEyeX(defaults.float(forKey: VC64Keys.eyeX))
        renderer.setEyeY(defaults.float(forKey: VC64Keys.eyeY))
        renderer.setEyeZ(defaults.float(forKey: VC64Keys.eyeZ))
        */
        
        defaults.decode(&renderer.shaderOptions, forKey: Keys.shaderOptions)
        renderer.buildDotMasks()
 
        c64.resume()
    }
    
    func saveVideoUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        defaults.set(renderer.upscaler, forKey: Keys.upscaler)
        defaults.set(c64.vic.videoPalette(), forKey: Keys.palette)
        defaults.set(c64.vic.brightness(), forKey: Keys.brightness)
        defaults.set(c64.vic.contrast(), forKey: Keys.contrast)
        defaults.set(c64.vic.saturation(), forKey: Keys.saturation)
        
        defaults.set(renderer.keepAspectRatio, forKey: Keys.keepAspectRatio)
        /*
        defaults.set(renderer.eyeX(), forKey: VC64Keys.eyeX)
        defaults.set(renderer.eyeY(), forKey: VC64Keys.eyeY)
        defaults.set(renderer.eyeZ(), forKey: VC64Keys.eyeZ)
        */
        defaults.encode(renderer.shaderOptions, forKey: Keys.shaderOptions)
    }
}

//
// User defaults (Emulator)
//

extension Keys {
    
    // Drives
    static let warpLoad             = "VC64WarpLoadKey"
    static let driveNoise           = "VC64DriveNoiseKey"
    
    // Screenshots
    static let screenshotSource     = "VC64ScreenshotSourceKey"
    static let screenshotTarget     = "VC64ScreenshotTargetKey"
    
    // User dialogs
    static let closeWithoutAsking   = "VC64CloseWithoutAsking"
    static let ejectWithoutAsking   = "VC64EjectWithoutAsking"
    
    // Miscellaneous
    static let pauseInBackground    = "VC64PauseInBackground"
    static let autoSnapshots        = "VC64AutoSnapshots"
    static let autoSnapshotInterval = "VC64SnapshotInterval"
    
    // Media files
    static let autoMountAction      = "VC64AutoMountAction"
    static let autoType             = "VC64AutoType"
    static let autoTypeText         = "VC64AutoTypeText"
}

extension Defaults {
   
    // Drives
    static let warpLoad             = true
    static let driveNoise           = true
    
    // Screenshots
    static let screenshotSource     = 0
    static let screenshotTarget     = NSBitmapImageRep.FileType.png
    
    // User dialogs
    static let closeWithoutAsking   = false
    static let ejectWithoutAsking   = false
    
    // Miscellaneous
    static let pauseInBackground    = false
    static let autoSnapshots        = true
    static let autoSnapshotInterval = 3
    
    // Media files
    static let autoMountAction      = [ "D64": AutoMountAction.openBrowser,
                                        "PRG": AutoMountAction.openBrowser,
                                        "T64": AutoMountAction.openBrowser,
                                        "TAP": AutoMountAction.openBrowser,
                                        "CRT": AutoMountAction.openBrowser ]
    static let autoType             = [ "D64": true,
                                        "PRG": true,
                                        "T64": true,
                                        "TAP": true,
                                        "CRT": false ]
    static let autoTypeText         = [ "D64": "LOAD \"*\",8,1:",
                                        "PRG": "RUN",
                                        "T64": "RUN",
                                        "TAP": "LOAD",
                                        "CRT": "" ]
}

extension UserDefaults {
    
    static func registerEmulatorUserDefaults() {
        
        let dictionary: [String: Any] = [
            
            Keys.warpLoad: Defaults.warpLoad,
            Keys.driveNoise: Defaults.driveNoise,
            
            Keys.screenshotSource: Defaults.screenshotSource,
            Keys.screenshotTarget: Int(Defaults.screenshotTarget.rawValue),

            Keys.closeWithoutAsking: Defaults.closeWithoutAsking,
            Keys.ejectWithoutAsking: Defaults.ejectWithoutAsking,

            Keys.pauseInBackground: Defaults.pauseInBackground,
            Keys.autoSnapshots: Defaults.autoSnapshots,
            Keys.autoSnapshotInterval: Defaults.autoSnapshotInterval
        ]
        
        let defaults = UserDefaults.standard
        
        defaults.register(defaults: dictionary)
        
        defaults.register(encodableItem: Defaults.autoMountAction, forKey: Keys.autoMountAction)
        defaults.register(encodableItem: Defaults.autoType, forKey: Keys.autoType)
        defaults.register(encodableItem: Defaults.autoTypeText, forKey: Keys.autoTypeText)
    }

    static func resetEmulatorUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        let keys = [Keys.warpLoad,
                    Keys.driveNoise,
                    
                    Keys.screenshotSource,
                    Keys.screenshotTarget,
                    
                    Keys.closeWithoutAsking,
                    Keys.ejectWithoutAsking,
                    
                    Keys.pauseInBackground,
                    Keys.autoSnapshots,
                    Keys.autoSnapshotInterval,
                    
                    Keys.autoMountAction,
                    Keys.autoType,
                    Keys.autoTypeText
        ]

        for key in keys { defaults.removeObject(forKey: key) }
    }
}

extension MyController {
        
    func loadEmulatorUserDefaults() {
        
        let defaults = UserDefaults.standard
            
        c64.suspend()
        
        c64.setWarpLoad(defaults.bool(forKey: Keys.warpLoad))
        c64.drive1.setSendSoundMessages(defaults.bool(forKey: Keys.driveNoise))
        c64.drive2.setSendSoundMessages(defaults.bool(forKey: Keys.driveNoise))
    
        screenshotSource = defaults.integer(forKey: Keys.screenshotSource)
        screenshotTargetIntValue = defaults.integer(forKey: Keys.screenshotTarget)
    
        closeWithoutAsking = defaults.bool(forKey: Keys.closeWithoutAsking)
        ejectWithoutAsking = defaults.bool(forKey: Keys.ejectWithoutAsking)

        pauseInBackground = defaults.bool(forKey: Keys.pauseInBackground)
        c64.setTakeAutoSnapshots(defaults.bool(forKey: Keys.autoSnapshots))
        c64.setSnapshotInterval(defaults.integer(forKey: Keys.autoSnapshotInterval))
        
        defaults.decode(&autoMountAction, forKey: Keys.autoMountAction)
        defaults.decode(&autoType, forKey: Keys.autoType)
        defaults.decode(&autoTypeText, forKey: Keys.autoTypeText)
        
        c64.resume()
    }
    
    func saveEmulatorUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        defaults.set(c64.warpLoad(), forKey: Keys.warpLoad)
        defaults.set(c64.drive1.sendSoundMessages(), forKey: Keys.driveNoise)
        defaults.set(c64.drive2.sendSoundMessages(), forKey: Keys.driveNoise)

        defaults.set(screenshotSource, forKey: Keys.screenshotSource)
        defaults.set(screenshotTargetIntValue, forKey: Keys.screenshotTarget)
        
        defaults.set(closeWithoutAsking, forKey: Keys.closeWithoutAsking)
        defaults.set(ejectWithoutAsking, forKey: Keys.ejectWithoutAsking)
        
        defaults.set(pauseInBackground, forKey: Keys.pauseInBackground)
        defaults.set(c64.takeAutoSnapshots(), forKey: Keys.autoSnapshots)
        defaults.set(c64.snapshotInterval(), forKey: Keys.autoSnapshotInterval)
        
        defaults.encode(autoMountAction, forKey: Keys.autoMountAction)
        defaults.encode(autoType, forKey: Keys.autoType)
        defaults.encode(autoTypeText, forKey: Keys.autoTypeText)
    }
}

//
// User defaults (Hardware)
//

extension Keys {
    
    //VICII
    static let vicRevision    = "VC64VICChipModelKey"
    static let grayDotBug     = "VC64VICGrayDotBugKey"
    
    // CIAs
    static let ciaRevision    = "VC64CIAChipModelKey"
    static let timerBBug      = "VC64CIATimerBBugKey"
    
    // SID
    static let sidRevision    = "VC64SIDChipModelKey"
    static let reSID          = "VC64SIDReSIDKey"
    static let audioFilter    = "VC64SIDFilterKey"
    static let sampling       = "VC64SIDSamplingMethodKey"
    
    // Logic board and RAM
    static let glueLogic      = "VC64GlueLogicKey"
    static let initPattern    = "VC64InitPatternKey"
}

struct HardwareDefaults {
    
    var vicRevision: VICRevision
    var grayDotBug: Bool
    
    var ciaRevision: CIARevision
    var timerBBug: Bool
    
    var sidRevision: SIDRevision
    var reSID: Bool
    var audioFilter: Bool
    var sampling: SamplingMethod
    
    var glueLogic: GlueLogic
    var initPattern: RamInitPattern

    //
    // Schemes
    //
    
    static let C64_PAL = HardwareDefaults.init(
        
        vicRevision: PAL_6569_R3,
        grayDotBug:  false,
        ciaRevision: MOS_6526,
        timerBBug:   true,
        sidRevision: MOS_6581,
        reSID:       true,
        audioFilter: true,
        sampling:    SID_SAMPLE_INTERPOLATE,
        glueLogic:   GLUE_DISCRETE,
        initPattern: INIT_PATTERN_C64
    )
    
    static let C64_II_PAL = HardwareDefaults.init(
        
        vicRevision: PAL_8565,
        grayDotBug:  true,
        ciaRevision: MOS_8521,
        timerBBug:   false,
        sidRevision: MOS_8580,
        reSID:       true,
        audioFilter: true,
        sampling:    SID_SAMPLE_INTERPOLATE,
        glueLogic:   GLUE_CUSTOM_IC,
        initPattern: INIT_PATTERN_C64C
    )
    
    static let C64_OLD_PAL = HardwareDefaults.init(
        
        vicRevision: PAL_6569_R1,
        grayDotBug:  false,
        ciaRevision: MOS_6526,
        timerBBug:   true,
        sidRevision: MOS_6581,
        reSID:       true,
        audioFilter: true,
        sampling:    SID_SAMPLE_INTERPOLATE,
        glueLogic:   GLUE_DISCRETE,
        initPattern: INIT_PATTERN_C64
    )

    static let C64_NTSC = HardwareDefaults.init(
        
        vicRevision: NTSC_6567,
        grayDotBug:  false,
        ciaRevision: MOS_6526,
        timerBBug:   false,
        sidRevision: MOS_6581,
        reSID:       true,
        audioFilter: true,
        sampling:    SID_SAMPLE_INTERPOLATE,
        glueLogic:   GLUE_DISCRETE,
        initPattern: INIT_PATTERN_C64
    )
    
    static let C64_II_NTSC = HardwareDefaults.init(
        
        vicRevision: NTSC_8562,
        grayDotBug:  true,
        ciaRevision: MOS_8521,
        timerBBug:   true,
        sidRevision: MOS_8580,
        reSID:       true,
        audioFilter: true,
        sampling:    SID_SAMPLE_INTERPOLATE,
        glueLogic:   GLUE_CUSTOM_IC,
        initPattern: INIT_PATTERN_C64C
    )
    
    static let C64_OLD_NTSC = HardwareDefaults.init(
        
        vicRevision: NTSC_6567_R56A,
        grayDotBug:  false,
        ciaRevision: MOS_6526,
        timerBBug:   false,
        sidRevision: MOS_6581,
        reSID:       true,
        audioFilter: true,
        sampling:    SID_SAMPLE_INTERPOLATE,
        glueLogic:   GLUE_DISCRETE,
        initPattern: INIT_PATTERN_C64
    )
}

extension UserDefaults {
    
    static func registerHardwareUserDefaults() {
        
        let defaults = HardwareDefaults.C64_PAL
        let dictionary: [String: Any] = [
            
            Keys.vicRevision: defaults.vicRevision.rawValue,
            Keys.grayDotBug:  defaults.grayDotBug,
            
            Keys.ciaRevision: defaults.ciaRevision.rawValue,
            Keys.timerBBug:   defaults.timerBBug,
            
            Keys.sidRevision: defaults.sidRevision.rawValue,
            Keys.reSID:       defaults.reSID,
            Keys.audioFilter: defaults.audioFilter,
            Keys.sampling:    defaults.sampling.rawValue,
            
            Keys.glueLogic:   defaults.glueLogic.rawValue,
            Keys.initPattern: defaults.initPattern.rawValue
        ]
        
        let userDefaults = UserDefaults.standard
        userDefaults.register(defaults: dictionary)
    }
    
    static func resetHardwareUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        let keys = [Keys.vicRevision,
                    Keys.grayDotBug,
                    
                    Keys.ciaRevision,
                    Keys.timerBBug,
                    
                    Keys.sidRevision,
                    Keys.reSID,
                    Keys.audioFilter,
                    Keys.sampling,
                    
                    Keys.glueLogic,
                    Keys.initPattern
        ]

        for key in keys { defaults.removeObject(forKey: key) }
    }
}

/*
extension MyController {
    
    func loadHardwareUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        c64.suspend()
        
        c64.vic.setModel(defaults.integer(forKey: Keys.vicChip))
        c64.vic.setEmulateGrayDotBug(defaults.bool(forKey: Keys.grayDotBug))

        c64.cia1.setModel(defaults.integer(forKey: Keys.ciaChip))
        c64.cia2.setModel(defaults.integer(forKey: Keys.ciaChip))
        c64.cia1.setEmulateTimerBBug(defaults.bool(forKey: Keys.timerBBug))
        c64.cia2.setEmulateTimerBBug(defaults.bool(forKey: Keys.timerBBug))

        c64.sid.setReSID(defaults.bool(forKey: Keys.reSID))
        c64.sid.setModel(defaults.integer(forKey: Keys.audioChip))
        c64.sid.setAudioFilter(defaults.bool(forKey: Keys.audioFilter))
        c64.sid.setSamplingMethod(defaults.integer(forKey: Keys.sampling))
        
        c64.vic.setGlueLogic(defaults.integer(forKey: Keys.glueLogic))
        c64.mem.setRamInitPattern(defaults.integer(forKey: Keys.initPattern))
        
        c64.resume()
    }

    func saveHardwareUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        defaults.set(c64.vic.model(), forKey: Keys.vicChip)
        defaults.set(c64.vic.emulateGrayDotBug(), forKey: Keys.grayDotBug)

        defaults.set(c64.cia1.model(), forKey: Keys.ciaChip)
        defaults.set(c64.cia1.emulateTimerBBug(), forKey: Keys.timerBBug)

        defaults.set(c64.sid.reSID(), forKey: Keys.reSID)
        defaults.set(c64.sid.model(), forKey: Keys.audioChip)
        defaults.set(c64.sid.audioFilter(), forKey: Keys.audioFilter)
        defaults.set(c64.sid.samplingMethod(), forKey: Keys.sampling)
        
        defaults.set(c64.vic.glueLogic(), forKey: Keys.glueLogic)
        defaults.set(c64.mem.ramInitPattern(), forKey: Keys.initPattern)
    }
}
*/
