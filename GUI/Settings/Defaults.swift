// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// swiftlint:disable colon

import Carbon.HIToolbox

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
    
    // Decodes an item of generic type 'Decodable'
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
        registerControlsUserDefaults()
        registerKeyboardUserDefaults()
        
        registerRomUserDefaults()
        registerHardwareUserDefaults()
        registerPeripheralsUserDefaults()
        registerAudioUserDefaults()
        registerVideoUserDefaults()
    }
}

extension MyController {
    
    func loadUserDefaults() {
        
        track()
        
        c64.suspend()
        
        pref.loadGeneralUserDefaults()
        pref.loadControlsUserDefaults()
        pref.loadKeyboardUserDefaults()
        
        config.loadRomUserDefaults()
        config.loadHardwareUserDefaults()
        config.loadPeripheralsUserDefaults()
        config.loadAudioUserDefaults()
        config.loadVideoUserDefaults()
        
        c64.resume()
    }
    
    func loadUserDefaults(url: URL, prefixes: [String]) {
        
        if let fileContents = NSDictionary(contentsOf: url) {
            
            if let dict = fileContents as? [String: Any] {
                
                let filteredDict = dict.filter { prefixes.contains(where: $0.0.hasPrefix) }
                
                let defaults = UserDefaults.standard
                defaults.setValuesForKeys(filteredDict)
                
                loadUserDefaults()
            }
        }
    }
    
    func saveUserDefaults(url: URL, prefixes: [String]) {
        
        track()
        
        let dict = UserDefaults.standard.dictionaryRepresentation()
        let filteredDict = dict.filter { prefixes.contains(where: $0.0.hasPrefix) }
        let nsDict = NSDictionary.init(dictionary: filteredDict)
        nsDict.write(to: url, atomically: true)
    }
}

//
// User defaults (Emulator)
//

struct Keys {
    
    struct Gen {
        
        // Drives
        static let driveBlankDiskFormat   = "VC64_GEN_DriveBlankDiskFormat"
        static let driveEjectUnasked      = "VC64_GEN_EjectUnasked"
        static let driveSounds            = "VC64_GEN_DriveSounds"
        static let driveSoundPan          = "VC64_GEN_DriveSoundPan"
        static let driveInsertSound       = "VC64_GEN_DriveInsertSound"
        static let driveEjectSound        = "VC64_GEN_DriveEjectSound"
        static let driveHeadSound         = "VC64_GEN_DriveHeadSound"
        static let driveConnectSound      = "VC64_GEN_DriveConnectSound"
        
        // Snapshots and screenshots
        static let autoSnapshots          = "VC64_GEN_AutoSnapshots"
        static let autoSnapshotInterval   = "VC64_GEN_ScreenshotInterval"
        static let autoScreenshots        = "VC64_GEN_AutoScreenshots"
        static let screenshotSource       = "VC64_GEN_ScreenshotSource"
        static let screenshotTarget       = "VC64_GEN_ScreenshotTarget"
        
        // Fullscreen
        static let keepAspectRatio        = "VC64_GEN_FullscreenKeepAspectRatio"
        static let exitOnEsc              = "VC64_GEN_FullscreenExitOnEsc"
        
        // User dialogs
        static let closeWithoutAsking     = "VC64_GEN_CloseWithoutAsking"
        
        // Warp mode
        static let warpMode               = "VC64_GEN_WarpMode"
        
        // Miscellaneous
        static let pauseInBackground      = "VC64_GEN_PauseInBackground"
    }
}

struct GeneralDefaults {
    
    // Drives
    let driveBlankDiskFormat: DOSType
    let driveEjectUnasked: Bool
    let driveSounds: Bool
    let driveSoundPan: Double
    let driveInsertSound: Bool
    let driveEjectSound: Bool
    let driveHeadSound: Bool
    let driveConnectSound: Bool
    
    // Snapshots and Screenshots
    let autoSnapshots: Bool
    let autoSnapshotInterval: Int
    let autoScreenshots: Bool
    let screenshotSource: Int
    let screenshotTarget: NSBitmapImageRep.FileType
    
    // Fullscreen
    let keepAspectRatio: Bool
    let exitOnEsc: Bool
    
    // Warp mode
    let warpMode: WarpMode
    
    // Miscellaneous
    let pauseInBackground: Bool
    let closeWithoutAsking: Bool
    
    //
    // Schemes
    //
    
    static let std = GeneralDefaults.init(
        
        driveBlankDiskFormat: .CBM,
        driveEjectUnasked: false,
        driveSounds: true,
        driveSoundPan: 1.0,
        driveInsertSound: false,
        driveEjectSound: false,
        driveHeadSound: true,
        driveConnectSound: false,
        
        autoSnapshots: false,
        autoSnapshotInterval: 20,
        autoScreenshots: false,
        screenshotSource: 0,
        screenshotTarget: .png,
        
        keepAspectRatio: false,
        exitOnEsc: true,
        
        warpMode: .auto,
        
        pauseInBackground: false,
        closeWithoutAsking: false
    )
}

extension UserDefaults {

    static func registerGeneralUserDefaults() {
        
        let defaults = GeneralDefaults.std
        let dictionary: [String: Any] = [
            
            Keys.Gen.driveBlankDiskFormat: Int(defaults.driveBlankDiskFormat.rawValue),
            Keys.Gen.driveEjectUnasked: defaults.driveEjectUnasked,
            Keys.Gen.driveSounds: defaults.driveSounds,
            Keys.Gen.driveSoundPan: defaults.driveSoundPan,
            Keys.Gen.driveInsertSound: defaults.driveInsertSound,
            Keys.Gen.driveEjectSound: defaults.driveEjectSound,
            Keys.Gen.driveHeadSound: defaults.driveHeadSound,
            Keys.Gen.driveConnectSound: defaults.driveConnectSound,
            
            Keys.Gen.autoSnapshots: defaults.autoSnapshots,
            Keys.Gen.autoSnapshotInterval: defaults.autoSnapshotInterval,
            Keys.Gen.autoScreenshots: defaults.autoScreenshots,
            Keys.Gen.screenshotSource: defaults.screenshotSource,
            Keys.Gen.screenshotTarget: Int(defaults.screenshotTarget.rawValue),
            
            Keys.Gen.keepAspectRatio: defaults.keepAspectRatio,
            Keys.Gen.exitOnEsc: defaults.exitOnEsc,
            
            Keys.Gen.warpMode: Int(defaults.warpMode.rawValue),
            
            Keys.Gen.pauseInBackground: defaults.pauseInBackground,
            Keys.Gen.closeWithoutAsking: defaults.closeWithoutAsking
        ]
        
        let userDefaults = UserDefaults.standard
        
        userDefaults.register(defaults: dictionary)
    }
    
    static func resetGeneralUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        let keys = [ Keys.Gen.driveBlankDiskFormat,
                     Keys.Gen.driveEjectUnasked,
                     Keys.Gen.driveSounds,
                     Keys.Gen.driveSoundPan,
                     Keys.Gen.driveInsertSound,
                     Keys.Gen.driveEjectSound,
                     Keys.Gen.driveHeadSound,
                     Keys.Gen.driveConnectSound,
                     
                     Keys.Gen.autoSnapshots,
                     Keys.Gen.autoSnapshotInterval,
                     Keys.Gen.autoScreenshots,
                     Keys.Gen.screenshotSource,
                     Keys.Gen.screenshotTarget,
                     
                     Keys.Gen.keepAspectRatio,
                     Keys.Gen.exitOnEsc,
                     
                     Keys.Gen.warpMode,
                     
                     Keys.Gen.pauseInBackground,
                     Keys.Gen.closeWithoutAsking
        ]
        
        for key in keys { defaults.removeObject(forKey: key) }
    }
}
    
//
// User defaults (Controls)
//

extension Keys {
    
    struct Con {
        
        // Emulation keys
        static let mouseKeyMap       = "VC64_CON_MouseKeyMap"
        static let joyKeyMap1        = "VC64_CON_JoyKeyMap1"
        static let joyKeyMap2        = "VC64_CON_JoyKeyMap2"
        static let disconnectJoyKeys = "VC64_CON_DisconnectKeys"
        
        // Joysticks
        static let autofire          = "VC64_CON_Autofire"
        static let autofireBullets   = "VC64_CON_AutofireBullets"
        static let autofireFrequency = "VC64_CON_AutofireFrequency"
        
        // Mouse
        static let mouseModel        = "VC64_CON_MouseModel"
    }
}

struct ControlsDefaults {
    
    // Joysticks
    let mouseKeyMap: [MacKey: Int]
    let joyKeyMap1: [MacKey: Int]
    let joyKeyMap2: [MacKey: Int]
    let disconnectJoyKeys: Bool
    let autofire: Bool
    let autofireBullets: Int
    let autofireFrequency: Float
    
    // Mouse
    let mouseModel: MouseModel
    
    //
    // Schemes
    //
    
    static let stdKeyMap1 = [
        
        MacKey.init(keyCode: kVK_LeftArrow): GamePadAction.PULL_LEFT.rawValue,
        MacKey.init(keyCode: kVK_RightArrow): GamePadAction.PULL_RIGHT.rawValue,
        MacKey.init(keyCode: kVK_UpArrow): GamePadAction.PULL_UP.rawValue,
        MacKey.init(keyCode: kVK_DownArrow): GamePadAction.PULL_DOWN.rawValue,
        MacKey.init(keyCode: kVK_Space): GamePadAction.PRESS_FIRE.rawValue
    ]
    static let stdKeyMap2 = [
        
        MacKey.init(keyCode: kVK_ANSI_S): GamePadAction.PULL_LEFT.rawValue,
        MacKey.init(keyCode: kVK_ANSI_D): GamePadAction.PULL_RIGHT.rawValue,
        MacKey.init(keyCode: kVK_ANSI_E): GamePadAction.PULL_UP.rawValue,
        MacKey.init(keyCode: kVK_ANSI_X): GamePadAction.PULL_DOWN.rawValue,
        MacKey.init(keyCode: kVK_ANSI_C): GamePadAction.PRESS_FIRE.rawValue
    ]
    
    static let std = ControlsDefaults.init(
        
        mouseKeyMap: [:],
        joyKeyMap1: stdKeyMap1,
        joyKeyMap2: stdKeyMap2,
        disconnectJoyKeys: true,
        autofire: false,
        autofireBullets: -3,
        autofireFrequency: 2.5,
        
        mouseModel: .MOUSE1350
    )
}

extension UserDefaults {
    
    static func registerControlsUserDefaults() {
        
        let defaults = ControlsDefaults.std
        let dictionary: [String: Any] = [

            // Joysticks
            Keys.Con.disconnectJoyKeys: defaults.disconnectJoyKeys,
            Keys.Con.autofire: defaults.autofire,
            Keys.Con.autofireBullets: defaults.autofireBullets,
            Keys.Con.autofireFrequency: defaults.autofireFrequency,
            
            // Mouse
            Keys.Con.mouseModel: defaults.mouseModel.rawValue
        ]
        
        let userDefaults = UserDefaults.standard
        userDefaults.register(defaults: dictionary)
        userDefaults.register(encodableItem: defaults.joyKeyMap1, forKey: Keys.Con.joyKeyMap1)
        userDefaults.register(encodableItem: defaults.joyKeyMap2, forKey: Keys.Con.joyKeyMap2)
    }
    
    static func resetControlsUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        let keys = [ Keys.Con.joyKeyMap1,
                     Keys.Con.joyKeyMap2,
                     Keys.Con.disconnectJoyKeys,
                     Keys.Con.autofire,
                     Keys.Con.autofireBullets,
                     Keys.Con.autofireFrequency,
                     
                     Keys.Con.mouseModel ]
        
        for key in keys { defaults.removeObject(forKey: key) }
    }
}

//
// User defaults (Keyboard)
//

extension Keys {
    
    struct Kbd {
        
        // Key map
        static let keyMap            = "VC64KeyMap"
        static let mapKeysByPosition = "VC64MapKeysByPosition"
    }
}

struct KeyboardDefaults {
    
    // Key map
    let keyMap: [MacKey: C64Key]
    let mapKeysByPosition: Bool
    
    //
    // Schemes
    //
    
    static let symbolicMapping = KeyboardDefaults.init(
        
        keyMap: KeyboardController.standardKeyMap,
        mapKeysByPosition: false
    )

    static let positionalMapping = KeyboardDefaults.init(
        
        keyMap: KeyboardController.standardKeyMap,
        mapKeysByPosition: true
    )
}

extension UserDefaults {
    
    static func registerKeyboardUserDefaults() {
        
        track()
        
        let defaults = KeyboardDefaults.symbolicMapping
        let dictionary: [String: Any] = [
            
            Keys.Kbd.mapKeysByPosition: defaults.mapKeysByPosition
        ]
        
        track("\(defaults.mapKeysByPosition)")
        let userDefaults = UserDefaults.standard
        userDefaults.register(defaults: dictionary)
        userDefaults.register(encodableItem: defaults.keyMap, forKey: Keys.Kbd.keyMap)
    }
    
    static func resetKeyMapUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        let keys = [ Keys.Kbd.mapKeysByPosition,
                     Keys.Kbd.keyMap
        ]
        
        for key in keys { defaults.removeObject(forKey: key) }
    }
}

//
// User defaults (Roms)
//

extension UserDefaults {
    
    static func romUrl(name: String) -> URL? {
        
        let folder = try? URL.appSupportFolder("Roms")
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
            track("Deleting Drive Rom")
            try? fm.removeItem(at: url)
        }
    }
}

//
// User defaults (Hardware)
//

extension Keys {
    
    struct Hwd {
        
        //VICII
        static let vicRevision    = "VC64_HW_VicRev"
        static let vicGrayDotBug  = "VC64_HW_VicGrayDotBug"
        
        // CIAs
        static let ciaRevision    = "VC64_HW_CiaRev"
        static let ciaTimerBBug   = "VC64_HW_CiaTimerBBug"
        
        // SID
        static let sidRevision    = "VC64_HW_SidRev"
        static let sidFilter      = "VC64_HW_SidFilter"
        static let sidEnable1     = "VC64_HW_SidEnable1"
        static let sidEnable2     = "VC64_HW_SidEnable2"
        static let sidEnable3     = "VC64_HW_SidEnable3"
        static let sidAddress1    = "VC64_HW_SidAddress1"
        static let sidAddress2    = "VC64_HW_SidAddress2"
        static let sidAddress3    = "VC64_HW_SidAddress3"
        
        // Logic board and RAM
        static let glueLogic      = "VC64_HW_GlueLogic"
        static let ramPattern     = "VC64_HW_RamPattern"
    }
}

struct HardwareDefaults {
    
    var vicRevision: VICRevision
    var vicGrayDotBug: Bool
    
    var ciaRevision: CIARevision
    var ciaTimerBBug: Bool
    
    var sidRevision: SIDRevision
    var sidFilter: Bool
    let sidEnable1: Bool
    let sidEnable2: Bool
    let sidEnable3: Bool
    let sidAddress1: Int
    let sidAddress2: Int
    let sidAddress3: Int
    
    var glueLogic: GlueLogic
    var ramPattern: RamPattern
    
    //
    // Schemes
    //
    
    static let C64_PAL = HardwareDefaults.init(
        
        vicRevision:   .PAL_6569_R3,
        vicGrayDotBug: false,
        
        ciaRevision:   .MOS_6526,
        ciaTimerBBug:  true,
        
        sidRevision:   .MOS_6581,
        sidFilter:     true,
        sidEnable1:    false,
        sidEnable2:    false,
        sidEnable3:    false,
        sidAddress1:   0xD420,
        sidAddress2:   0xD440,
        sidAddress3:   0xD460,
        
        glueLogic:     .GLUE_DISCRETE,
        ramPattern:    .RAM_PATTERN_C64
    )
    
    static let C64_II_PAL = HardwareDefaults.init(
        
        vicRevision:   .PAL_8565,
        vicGrayDotBug: true,
        
        ciaRevision:   .MOS_8521,
        ciaTimerBBug:  false,
        
        sidRevision:   .MOS_8580,
        sidFilter:     true,
        sidEnable1:    false,
        sidEnable2:    false,
        sidEnable3:    false,
        sidAddress1:   0xD420,
        sidAddress2:   0xD440,
        sidAddress3:   0xD460,
        
        glueLogic:     .GLUE_CUSTOM_IC,
        ramPattern:    .RAM_PATTERN_C64C
    )
    
    static let C64_OLD_PAL = HardwareDefaults.init(
        
        vicRevision:   .PAL_6569_R1,
        vicGrayDotBug: false,
        
        ciaRevision:   .MOS_6526,
        ciaTimerBBug:  true,
        
        sidRevision:   .MOS_6581,
        sidFilter:     true,
        sidEnable1:    false,
        sidEnable2:    false,
        sidEnable3:    false,
        sidAddress1:   0xD420,
        sidAddress2:   0xD440,
        sidAddress3:   0xD460,
        
        glueLogic:     .GLUE_DISCRETE,
        ramPattern:    .RAM_PATTERN_C64
    )

    static let C64_NTSC = HardwareDefaults.init(
        
        vicRevision:   .NTSC_6567,
        vicGrayDotBug: false,
        
        ciaRevision:   .MOS_6526,
        ciaTimerBBug:  false,
        
        sidRevision:   .MOS_6581,
        sidFilter:     true,
        sidEnable1:    false,
        sidEnable2:    false,
        sidEnable3:    false,
        sidAddress1:   0xD420,
        sidAddress2:   0xD440,
        sidAddress3:   0xD460,
        
        glueLogic:     .GLUE_DISCRETE,
        ramPattern:    .RAM_PATTERN_C64
    )
    
    static let C64_II_NTSC = HardwareDefaults.init(
        
        vicRevision:   .NTSC_8562,
        vicGrayDotBug: true,
        
        ciaRevision:   .MOS_8521,
        ciaTimerBBug:  true,
        
        sidRevision:   .MOS_8580,
        sidFilter:     true,
        sidEnable1:    false,
        sidEnable2:    false,
        sidEnable3:    false,
        sidAddress1:   0xD420,
        sidAddress2:   0xD440,
        sidAddress3:   0xD460,
        
        glueLogic:     .GLUE_CUSTOM_IC,
        ramPattern:    .RAM_PATTERN_C64C
    )
    
    static let C64_OLD_NTSC = HardwareDefaults.init(
        
        vicRevision:   .NTSC_6567_R56A,
        vicGrayDotBug: false,
        
        ciaRevision:   .MOS_6526,
        ciaTimerBBug:  false,
        
        sidRevision:   .MOS_6581,
        sidFilter:     true,
        sidEnable1:    false,
        sidEnable2:    false,
        sidEnable3:    false,
        sidAddress1:   0xD420,
        sidAddress2:   0xD440,
        sidAddress3:   0xD460,
        
        glueLogic:     .GLUE_DISCRETE,
        ramPattern:    .RAM_PATTERN_C64
    )
}

extension UserDefaults {
    
    static func registerHardwareUserDefaults() {
        
        let defaults = HardwareDefaults.C64_PAL
        let dictionary: [String: Any] = [
            
            Keys.Hwd.vicRevision:   defaults.vicRevision.rawValue,
            Keys.Hwd.vicGrayDotBug: defaults.vicGrayDotBug,
            
            Keys.Hwd.ciaRevision:   defaults.ciaRevision.rawValue,
            Keys.Hwd.ciaTimerBBug:  defaults.ciaTimerBBug,
            
            Keys.Hwd.sidRevision:   defaults.sidRevision.rawValue,
            Keys.Hwd.sidEnable1:    defaults.sidEnable1,
            Keys.Hwd.sidEnable2:    defaults.sidEnable2,
            Keys.Hwd.sidEnable3:    defaults.sidEnable3,
            Keys.Hwd.sidAddress1:   defaults.sidAddress1,
            Keys.Hwd.sidAddress2:   defaults.sidAddress2,
            Keys.Hwd.sidAddress3:   defaults.sidAddress3,

            Keys.Hwd.glueLogic:     defaults.glueLogic.rawValue,
            Keys.Hwd.ramPattern:    defaults.ramPattern.rawValue
        ]
        
        let userDefaults = UserDefaults.standard
        userDefaults.register(defaults: dictionary)
    }
    
    static func resetHardwareUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        let keys = [Keys.Hwd.vicRevision,
                    Keys.Hwd.vicGrayDotBug,
                    
                    Keys.Hwd.ciaRevision,
                    Keys.Hwd.ciaTimerBBug,
                    
                    Keys.Hwd.sidRevision,
                    Keys.Hwd.sidEnable1,
                    Keys.Hwd.sidEnable2,
                    Keys.Hwd.sidEnable3,
                    Keys.Hwd.sidAddress1,
                    Keys.Hwd.sidAddress2,
                    Keys.Hwd.sidAddress3,
                    
                    Keys.Hwd.glueLogic,
                    Keys.Hwd.ramPattern
        ]

        for key in keys { defaults.removeObject(forKey: key) }
    }
}

//
// User defaults (Peripherals)
//

extension Keys {
    
    struct Per {
                
        // Drive
        static let drive8Connect  = "VC64_PER_Drive8Connect"
        static let drive8Type     = "VC64_PER_Drive8Type"
        static let drive9Connect  = "VC64_PER_Drive9Connect"
        static let drive9Type     = "VC64_PER_Drive9Type"
        
        // Ports
        static let gameDevice1    = "VC64_PER_ControlPort1"
        static let gameDevice2    = "VC64_PER_ControlPort2"
    }
}

struct PeripheralsDefaults {
    
    var driveConnect: [Bool]
    var driveType: [DriveType]
    
    var gameDevice1: Int
    var gameDevice2: Int
    
    //
    // Schemes
    //
    
    static let std = PeripheralsDefaults.init(
        
        driveConnect:  [true, false],
        driveType:     [.DRIVE_VC1541II, .DRIVE_VC1541II],
        
        gameDevice1:   -1,
        gameDevice2:   -1
    )
}

extension UserDefaults {
    
    static func registerPeripheralsUserDefaults() {
        
        let defaults = PeripheralsDefaults.std
        let dictionary: [String: Any] = [
                        
            Keys.Per.drive8Connect: defaults.driveConnect[0],
            Keys.Per.drive8Type:    defaults.driveType[0].rawValue,
            Keys.Per.drive9Connect: defaults.driveConnect[1],
            Keys.Per.drive9Type:    defaults.driveType[1].rawValue,
            
            Keys.Per.gameDevice1:   defaults.gameDevice1,
            Keys.Per.gameDevice2:   defaults.gameDevice2
        ]
        
        let userDefaults = UserDefaults.standard
        userDefaults.register(defaults: dictionary)
    }
    
    static func resetPeripheralsUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        let keys = [Keys.Per.drive8Connect,
                    Keys.Per.drive8Type,
                    Keys.Per.drive9Connect,
                    Keys.Per.drive9Type,
                    
                    Keys.Per.gameDevice1,
                    Keys.Per.gameDevice2
        ]

        for key in keys { defaults.removeObject(forKey: key) }
    }
}

//
// User defaults (Audio)
//

extension Keys {
    
    struct Aud {
        
        // Engine
        static let sidEngine          = "VC64_AUD_SidEngine"
        static let sidSampling        = "VC64_AUD_Sampling"
        
        // In
        static let vol0               = "VAMIGA_AUD_Volume0"
        static let vol1               = "VAMIGA_AUD_Volume1"
        static let vol2               = "VAMIGA_AUD_Volume2"
        static let vol3               = "VAMIGA_AUD_Volume3"
        static let pan0               = "VAMIGA_AUD_Pan0"
        static let pan1               = "VAMIGA_AUD_Pan1"
        static let pan2               = "VAMIGA_AUD_Pan2"
        static let pan3               = "VAMIGA_AUD_Pan3"
        
        // Out
        static let volL               = "VAMIGA_AUD_VolumeL"
        static let volR               = "VAMIGA_AUD_VolumeR"
    }
}

struct AudioDefaults {
    
    // Audio Engine
    let sidEngine: SIDEngine
    let sidSampling: SamplingMethod
    
    // In
    let vol0: Int
    let vol1: Int
    let vol2: Int
    let vol3: Int
    let pan0: Int
    let pan1: Int
    let pan2: Int
    let pan3: Int
    
    // Out
    let volL: Int
    let volR: Int
    
    //
    // Schemes
    //
    
    static let mono = AudioDefaults.init(
        
        sidEngine: .ENGINE_RESID,
        sidSampling: .SID_SAMPLE_INTERPOLATE,
        
        vol0: 400,
        vol1: 400,
        vol2: 400,
        vol3: 400,
        pan0: 0,
        pan1: 0,
        pan2: 0,
        pan3: 0,
        
        volL: 50,
        volR: 50
    )
    
    static let stereo = AudioDefaults.init(
        
        sidEngine: .ENGINE_RESID,
        sidSampling: .SID_SAMPLE_INTERPOLATE,

        vol0: 400,
        vol1: 400,
        vol2: 400,
        vol3: 400,
        pan0: 150,
        pan1: 50,
        pan2: 150,
        pan3: 50,
        
        volL: 50,
        volR: 50
    )
}

extension UserDefaults {

    static func registerAudioUserDefaults() {

        let defaults = AudioDefaults.mono
        let dictionary: [String: Any] = [

            Keys.Aud.sidEngine: Int(defaults.sidEngine.rawValue),
            Keys.Aud.sidSampling: Int(defaults.sidSampling.rawValue),

            Keys.Aud.vol0: defaults.vol0,
            Keys.Aud.vol1: defaults.vol1,
            Keys.Aud.vol2: defaults.vol2,
            Keys.Aud.vol3: defaults.vol3,
            Keys.Aud.pan0: defaults.pan0,
            Keys.Aud.pan1: defaults.pan1,
            Keys.Aud.pan2: defaults.pan2,
            Keys.Aud.pan3: defaults.pan3,
            
            Keys.Aud.volL: defaults.volL,
            Keys.Aud.volR: defaults.volR
        ]

        let userDefaults = UserDefaults.standard
        userDefaults.register(defaults: dictionary)
    }

    static func resetAudioUserDefaults() {

        let userDefaults = UserDefaults.standard
        
        let keys = [ Keys.Aud.sidEngine,
                     Keys.Aud.sidSampling,
                     
                     Keys.Aud.vol0,
                     Keys.Aud.vol1,
                     Keys.Aud.vol2,
                     Keys.Aud.vol3,
                     Keys.Aud.pan0,
                     Keys.Aud.pan1,
                     Keys.Aud.pan2,
                     Keys.Aud.pan3,
                     
                     Keys.Aud.volL,
                     Keys.Aud.volR]

        for key in keys { userDefaults.removeObject(forKey: key) }
    }
}

//
// User defaults (Video)
//

extension Keys {
    
    struct Vid {
        
        // Colors
        static let palette            = "VC64_VID_Palette"
        static let brightness         = "VC64_VID_Brightness"
        static let contrast           = "VC64_VID_Contrast"
        static let saturation         = "VC64_VID_Saturation"
        
        // Geometry
        static let hCenter            = "VC64_VID_HCenter"
        static let vCenter            = "VC64_VID_VCenter"
        static let hZoom              = "VC64_VID_HZoom"
        static let vZoom              = "VC64_VID_VZoom"
        
        // Upscalers
        static let upscaler           = "VC64_VID_Upscaler"
        
        // Shader options
        static let blur               = "VC64_VID_Blur"
        static let blurRadius         = "VC64_VID_BlurRadius"
        static let bloom              = "VC64_VID_Bloom"
        static let bloomRadiusR       = "VC64_VID_BloonRadiusR"
        static let bloomRadiusG       = "VC64_VID_BloonRadiusG"
        static let bloomRadiusB       = "VC64_VID_BloonRadiusB"
        static let bloomBrightness    = "VC64_VID_BloomBrightness"
        static let bloomWeight        = "VC64_VID_BloomWeight"
        static let flicker            = "VC64_VID_Flicker"
        static let flickerWeight      = "VC64_VID_FlickerWeight"
        static let dotMask            = "VC64_VID_DotMask"
        static let dotMaskBrightness  = "VC64_VID_DotMaskBrightness"
        static let scanlines          = "VC64_VID_Scanlines"
        static let scanlineBrightness = "VC64_VID_ScanlineBrightness"
        static let scanlineWeight     = "VC64_VID_ScanlineWeight"
        static let disalignment       = "VC64_VID_Disalignment"
        static let disalignmentH      = "VC64_VID_DisalignmentH"
        static let disalignmentV      = "VC64_VID_DisalignmentV"
    }
}

struct VideoDefaults {
    
    // Colors
    let palette: Palette
    let brightness: Double
    let contrast: Double
    let saturation: Double
    
    // Geometry
    let hCenter: Float
    let vCenter: Float
    let hZoom: Float
    let vZoom: Float
    
    // Upscalers
    let upscaler: Int
    
    // Shader options
    let blur: Int32
    let blurRadius: Float
    let bloom: Int32
    let bloomRadiusR: Float
    let bloomRadiusG: Float
    let bloomRadiusB: Float
    let bloomBrightness: Float
    let bloomWeight: Float
    let flicker: Int32
    let flickerWeight: Float
    let dotMask: Int32
    let dotMaskBrightness: Float
    let scanlines: Int32
    let scanlineBrightness: Float
    let scanlineWeight: Float
    let disalignment: Int32
    let disalignmentH: Float
    let disalignmentV: Float
    
    //
    // Schemes
    //
    
    // TFT monitor appearance
    static let tft = VideoDefaults.init(
        
        palette: COLOR_PALETTE,
        brightness: 50.0,
        contrast: 100.0,
        saturation: 50.0,
        
        hCenter: 0,
        vCenter: 0,
        hZoom: 0,
        vZoom: 0.046,
        
        upscaler: 0,
        
        blur: 1,
        blurRadius: 0,
        bloom: 0,
        bloomRadiusR: 1.0,
        bloomRadiusG: 1.0,
        bloomRadiusB: 1.0,
        bloomBrightness: 0.4,
        bloomWeight: 1.21,
        flicker: 1,
        flickerWeight: 0.5,
        dotMask: 0,
        dotMaskBrightness: 0.7,
        scanlines: 0,
        scanlineBrightness: 0.55,
        scanlineWeight: 0.11,
        disalignment: 0,
        disalignmentH: 0.001,
        disalignmentV: 0.001
    )
    
    // CRT monitor appearance
    static let crt = VideoDefaults.init(
        
        palette: COLOR_PALETTE,
        brightness: 50.0,
        contrast: 100.0,
        saturation: 50.0,
        
        hCenter: 0,
        vCenter: 0,
        hZoom: 0,
        vZoom: 0.046,
        
        upscaler: 0,
        
        blur: 1,
        blurRadius: 1.5,
        bloom: 1,
        bloomRadiusR: 1.0,
        bloomRadiusG: 1.0,
        bloomRadiusB: 1.0,
        bloomBrightness: 0.4,
        bloomWeight: 1.21,
        flicker: 1,
        flickerWeight: 0.5,
        dotMask: 1,
        dotMaskBrightness: 0.5,
        scanlines: 2,
        scanlineBrightness: 0.55,
        scanlineWeight: 0.11,
        disalignment: 0,
        disalignmentH: 0.001,
        disalignmentV: 0.001
    )
}

extension UserDefaults {
    
    static func registerVideoUserDefaults() {
        
        let defaults = VideoDefaults.tft
        let dictionary: [String: Any] = [
            
            Keys.Vid.palette: Int(defaults.palette.rawValue),
            Keys.Vid.brightness: defaults.brightness,
            Keys.Vid.contrast: defaults.contrast,
            Keys.Vid.saturation: defaults.saturation,
            
            Keys.Vid.hCenter: defaults.hCenter,
            Keys.Vid.vCenter: defaults.vCenter,
            Keys.Vid.hZoom: defaults.hZoom,
            Keys.Vid.vZoom: defaults.vZoom,
            
            Keys.Vid.upscaler: defaults.upscaler,
            
            Keys.Vid.blur: defaults.blur,
            Keys.Vid.blurRadius: defaults.blurRadius,
            Keys.Vid.bloom: defaults.bloom,
            Keys.Vid.bloomRadiusR: defaults.bloomRadiusR,
            Keys.Vid.bloomRadiusG: defaults.bloomRadiusG,
            Keys.Vid.bloomRadiusB: defaults.bloomRadiusB,
            Keys.Vid.bloomBrightness: defaults.bloomBrightness,
            Keys.Vid.bloomWeight: defaults.bloomWeight,
            Keys.Vid.flicker: defaults.flicker,
            Keys.Vid.flickerWeight: defaults.flickerWeight,
            Keys.Vid.dotMask: defaults.dotMask,
            Keys.Vid.dotMaskBrightness: defaults.dotMaskBrightness,
            Keys.Vid.scanlines: defaults.scanlines,
            Keys.Vid.scanlineBrightness: defaults.scanlineBrightness,
            Keys.Vid.scanlineWeight: defaults.scanlineWeight,
            Keys.Vid.disalignment: defaults.disalignment,
            Keys.Vid.disalignmentH: defaults.disalignmentH,
            Keys.Vid.disalignmentV: defaults.disalignmentV
        ]
        
        let userDefaults = UserDefaults.standard
        userDefaults.register(defaults: dictionary)
    }
    
    static func resetVideoUserDefaults() {
        
        let defaults = UserDefaults.standard
                
        let keys = [ Keys.Vid.palette,
                     Keys.Vid.brightness,
                     Keys.Vid.contrast,
                     Keys.Vid.saturation,
                     
                     Keys.Vid.hCenter,
                     Keys.Vid.vCenter,
                     Keys.Vid.hZoom,
                     Keys.Vid.vZoom,
                     
                     Keys.Vid.upscaler,
                     
                     Keys.Vid.blur,
                     Keys.Vid.blurRadius,
                     Keys.Vid.bloom,
                     Keys.Vid.bloomRadiusR,
                     Keys.Vid.bloomRadiusG,
                     Keys.Vid.bloomRadiusB,
                     Keys.Vid.bloomBrightness,
                     Keys.Vid.bloomWeight,
                     Keys.Vid.flicker,
                     Keys.Vid.flickerWeight,
                     Keys.Vid.dotMask,
                     Keys.Vid.dotMaskBrightness,
                     Keys.Vid.scanlines,
                     Keys.Vid.scanlineBrightness,
                     Keys.Vid.scanlineWeight,
                     Keys.Vid.disalignment,
                     Keys.Vid.disalignmentH,
                     Keys.Vid.disalignmentV
        ]
        
        for key in keys { defaults.removeObject(forKey: key) }
    }
}
