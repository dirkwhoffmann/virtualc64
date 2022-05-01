// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// swiftlint:disable colon

import Carbon.HIToolbox

//
// Proxy extensions
//

extension DefaultsProxy {

    func resetSearchPaths() {

        set("BASIC_PATH", UserDefaults.basicRomUrl!.path)
        set("CHAR_PATH", UserDefaults.charRomUrl!.path)
        set("KERNAL_PATH", UserDefaults.kernalRomUrl!.path)
        set("VC1541_PATH", UserDefaults.vc1541RomUrl!.path)
    }

    func load(url: URL) throws {

        resetSearchPaths()

        let exception = ExceptionWrapper()
        load(url, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
    }

    func load() {

        log("Loading user defaults")

        do {
            let folder = try URL.appSupportFolder()
            let path = folder.appendingPathComponent("virtualc64.ini")

            do {
                try load(url: path)
                log("Successfully loaded user defaults from file \(path)")
            } catch {
                log("Failed to load user defaults from file \(path)")
            }

        } catch {
            log("Failed to access application support folder")
        }
    }

    func save(url: URL) throws {

        let exception = ExceptionWrapper()
        save(url, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
    }

    func save() {

        log("Saving user defaults")

        do {
            let folder = try URL.appSupportFolder()
            let path = folder.appendingPathComponent("virtualc64.ini")

            do {
                try save(url: path)
                log("Successfully saved user defaults to file \(path)")
            } catch {
                log("Failed to save user defaults file \(path)")
            }

        } catch {
            log("Failed to access application support folder")
        }
    }

    func register(_ key: String, _ val: String) {
        register(key, value: val)
    }
    func register(_ key: String, _ val: Bool) {
        register(key, value: val ? "1" : "0")
    }
    func register(_ key: String, _ val: Int) {
        register(key, value: "\(val)")
    }
    func register(_ key: String, _ val: UInt) {
        register(key, value: "\(val)")
    }
    func register(_ key: String, _ val: Float) {
        register(key, value: "\(val)")
    }
    func register(_ key: String, _ val: Double) {
        register(key, value: "\(val)")
    }
    func remove(_ option: Option, _ nr: Int) {
        remove(option, nr: nr)
    }
    func remove(_ option: Option, _ nr: [Int]) {
        for n in nr { remove(option, nr: n) }
    }
    func set(_ key: String, _ val: String) {
        setKey(key, value: val)
    }
    func set(_ key: String, _ val: Bool) {
        setKey(key, value: val ? "1" : "0")
    }
    func set(_ key: String, _ val: Int) {
        setKey(key, value: "\(val)")
    }
    func set(_ key: String, _ val: UInt) {
        setKey(key, value: "\(val)")
    }
    func set(_ key: String, _ val: Float) {
        setKey(key, value: "\(val)")
    }
    func set(_ key: String, _ val: Double) {
        setKey(key, value: "\(val)")
    }
    func set(_ option: Option, _ val: Int) {
        setOpt(option, value: val)
    }
    func set(_ option: Option, _ val: Bool) {
        setOpt(option, value: val ? 1 : 0)
    }
    func set(_ option: Option, _ nr: Int, _ val: Int) {
        setOpt(option, nr: nr, value: val)
    }
    func set(_ option: Option, _ nr: Int, _ val: Bool) {
        setOpt(option, nr: nr, value: val ? 1 : 0)
    }
    func set(_ option: Option, _ nr: [Int], _ val: Int) {
        for n in nr { setOpt(option, nr: n, value: val) }
    }
    func set(_ option: Option, _ nr: [Int], _ val: Bool) {
        for n in nr { setOpt(option, nr: n, value: val ? 1 : 0) }
    }
    func get(_ option: Option) -> Int {
        return getOpt(option)
    }
    func get(_ option: Option, _ nr: Int) -> Int {
        return getOpt(option, nr: nr)
    }
    func string(_ key: String) -> String {
        return getString(key) ?? ""
    }
    func bool(_ key: String) -> Bool {
        return getInt(key) != 0
    }
    func int(_ key: String) -> Int {
        return getInt(key)
    }
    func float(_ key: String) -> Float {
        return (getString(key) as NSString).floatValue
    }
    func double(_ key: String) -> Double {
        return (getString(key) as NSString).doubleValue
    }

    func register<T: Encodable>(_ key: String, encodable item: T) {

        let jsonData = try? JSONEncoder().encode(item)
        let jsonString = jsonData?.base64EncodedString() ?? ""
        register(key, jsonString)
    }

    func encode<T: Encodable>(_ key: String, _ item: T) {

        let jsonData = try? JSONEncoder().encode(item)
        let jsonString = jsonData?.base64EncodedString() ?? ""
        set(key, jsonString)
    }

    func decode<T: Decodable>(_ key: String, _ item: inout T) {

        if let jsonString = getString(key) {

            log("Decoding jsonString '\(jsonString)'")
            if let data = Data(base64Encoded: jsonString) {

                if let decoded = try? JSONDecoder().decode(T.self, from: data) {
                    item = decoded
                } else {
                    log(warning: "Failed to decode \(jsonString)")
                }
                return
            }
        }
        log("Failed to decode jsonString")
    }
}

//
// Paths
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
}

//
// User defaults (all)
//

extension DefaultsProxy {

    func registerUserDefaults() {

        log("Registering user defaults")

        registerGeneralUserDefaults()
        registerControlsUserDefaults()
        registerDevicesUserDefaults()
        registerKeyboardUserDefaults()

        registerHardwareUserDefaults()
        registerPeripheralsUserDefaults()
        registerCompatibilityUserDefaults()
        registerAudioUserDefaults()
        registerVideoUserDefaults()
    }
}

extension Preferences {

    func applyUserDefaults() {

        log(level: 2)

        applyGeneralUserDefaults()
        applyControlsUserDefaults()
        applyDevicesUserDefaults()
        applyKeyboardUserDefaults()
    }
}

extension Configuration {

    func applyUserDefaults() {

        log(level: 2)

        applyHardwareUserDefaults()
        applyPeripheralsUserDefaults()
        applyCompatibilityUserDefaults()
        applyAudioUserDefaults()
        applyVideoUserDefaults()
    }
}

//
// User defaults (General)
//

struct Keys {
    
    struct Gen {
                
        // Snapshots
        static let autoSnapshots          = "General.AutoSnapshots"
        static let autoSnapshotInterval   = "General.ScreenshotInterval"

        // Screenshots
        static let screenshotSource       = "General.ScreenshotSource"
        static let screenshotTarget       = "General.ScreenshotTarget"
        
        // Screen captures
        static let ffmpegPath             = "General.ffmpegPath"
        static let captureSource          = "General.Source"
        static let bitRate                = "General.BitRate"
        static let aspectX                = "General.AspectX"
        static let aspectY                = "General.AspectY"

        // Fullscreen
        static let keepAspectRatio        = "General.FullscreenKeepAspectRatio"
        static let exitOnEsc              = "General.FullscreenExitOnEsc"
                
        // Warp mode
        static let warpMode               = "General.WarpMode"
        
        // Miscellaneous
        static let ejectWithoutAsking     = "General.EjectWithoutAsking"
        static let closeWithoutAsking     = "General.CloseWithoutAsking"
        static let pauseInBackground      = "General.PauseInBackground"
    }
}

extension DefaultsProxy {

    func registerGeneralUserDefaults() {

        log(level: 2)

        // Snapshots
        register(Keys.Gen.autoSnapshots, false)
        register(Keys.Gen.autoSnapshotInterval, 20)

        // Screenshots
        register(Keys.Gen.screenshotSource, 0)
        register(Keys.Gen.screenshotTarget, NSBitmapImageRep.FileType.png.rawValue)

        // Captures
        register(Keys.Gen.ffmpegPath, "")
        register(Keys.Gen.captureSource, 0)
        register(Keys.Gen.bitRate, 2048)
        register(Keys.Gen.aspectX, 768)
        register(Keys.Gen.aspectY, 702)

        // Fullscreen
        register(Keys.Gen.keepAspectRatio, false)
        register(Keys.Gen.exitOnEsc, true)

        // Warp mode
        register(Keys.Gen.warpMode, WarpMode.off.rawValue)

        // Misc
        register(Keys.Gen.ejectWithoutAsking, false)
        register(Keys.Gen.closeWithoutAsking, false)
        register(Keys.Gen.pauseInBackground, false)
    }

    func removeGeneralUserDefaults() {

        log(level: 2)

        let keys = [ Keys.Gen.autoSnapshots,
                     Keys.Gen.autoSnapshotInterval,

                     Keys.Gen.screenshotSource,
                     Keys.Gen.screenshotTarget,

                     Keys.Gen.ffmpegPath,
                     Keys.Gen.captureSource,
                     Keys.Gen.bitRate,
                     Keys.Gen.aspectX,
                     Keys.Gen.aspectY,

                     Keys.Gen.keepAspectRatio,
                     Keys.Gen.exitOnEsc,

                     Keys.Gen.warpMode,

                     Keys.Gen.ejectWithoutAsking,
                     Keys.Gen.closeWithoutAsking,
                     Keys.Gen.pauseInBackground
        ]

        for key in keys { removeKey(key) }
    }
}

extension Preferences {

    func saveGeneralUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!

        defaults.set(Keys.Gen.autoSnapshots, autoSnapshots)
        defaults.set(Keys.Gen.autoSnapshotInterval, snapshotInterval)

        defaults.set(Keys.Gen.screenshotSource, screenshotSource)
        defaults.set(Keys.Gen.screenshotTarget, screenshotTargetIntValue)

        defaults.set(Keys.Gen.ffmpegPath, ffmpegPath)
        defaults.set(Keys.Gen.captureSource, captureSource)
        defaults.set(Keys.Gen.bitRate, bitRate)
        defaults.set(Keys.Gen.aspectX, aspectX)
        defaults.set(Keys.Gen.aspectY, aspectY)

        defaults.set(Keys.Gen.keepAspectRatio, keepAspectRatio)
        defaults.set(Keys.Gen.exitOnEsc, exitOnEsc)

        defaults.set(Keys.Gen.warpMode, warpModeIntValue)

        defaults.set(Keys.Gen.ejectWithoutAsking, ejectWithoutAsking)
        defaults.set(Keys.Gen.closeWithoutAsking, closeWithoutAsking)
        defaults.set(Keys.Gen.pauseInBackground, pauseInBackground)

        defaults.save()
    }

    func applyGeneralUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!

        autoSnapshots = defaults.bool(Keys.Gen.autoSnapshots)
        snapshotInterval = defaults.int(Keys.Gen.autoSnapshotInterval)

        screenshotSource = defaults.int(Keys.Gen.screenshotSource)
        screenshotTargetIntValue = defaults.int(Keys.Gen.screenshotTarget)

        ffmpegPath = defaults.string(Keys.Gen.ffmpegPath)
        captureSource = defaults.int(Keys.Gen.captureSource)
        bitRate = defaults.int(Keys.Gen.bitRate)
        aspectX = defaults.int(Keys.Gen.aspectX)
        aspectY = defaults.int(Keys.Gen.aspectY)

        keepAspectRatio = defaults.bool(Keys.Gen.keepAspectRatio)
        exitOnEsc = defaults.bool(Keys.Gen.exitOnEsc)

        warpModeIntValue = defaults.int(Keys.Gen.warpMode)

        ejectWithoutAsking = defaults.bool(Keys.Gen.ejectWithoutAsking)
        closeWithoutAsking = defaults.bool(Keys.Gen.closeWithoutAsking)
        pauseInBackground = defaults.bool(Keys.Gen.pauseInBackground)
    }
}

//
// User defaults (Controls)
//

extension Keys {
    
    struct Con {
        
        // Emulation keys
        static let mouseKeyMap           = "Controls.MouseKeyMap"
        static let joyKeyMap1            = "Controls.JoyKeyMap1"
        static let joyKeyMap2            = "Controls.JoyKeyMap2"
        static let disconnectJoyKeys     = "Controls.DisconnectKeys"
        
        // Joysticks
        static let autofire              = "Controls.Autofire"
        static let autofireBullets       = "Controls.AutofireBullets"
        static let autofireFrequency     = "Controls.AutofireFrequency"
        
        // Mouse
        static let retainMouseKeyComb    = "Controls.RetainMouseKeyComb"
        static let retainMouseWithKeys   = "Controls.RetainMouseWithKeys"
        static let retainMouseByClick    = "Controls.RetainMouseByClick"
        static let retainMouseByEntering = "Controls.RetainMouseByEntering"
        static let releaseMouseKeyComb   = "Controls.ReleaseMouseKeyComb"
        static let releaseMouseWithKeys  = "Controls.ReleaseMouseWithKeys"
        static let releaseMouseByShaking = "Controls.ReleaseMouseByShaking"
    }
}

extension DefaultsProxy {

    func registerControlsUserDefaults() {

        log(level: 2)

        let emptyMap: [MacKey: Int] = [:]

        let stdKeyMap1: [MacKey: Int] = [

            MacKey(keyCode: kVK_LeftArrow): GamePadAction.PULL_LEFT.rawValue,
            MacKey(keyCode: kVK_RightArrow): GamePadAction.PULL_RIGHT.rawValue,
            MacKey(keyCode: kVK_UpArrow): GamePadAction.PULL_UP.rawValue,
            MacKey(keyCode: kVK_DownArrow): GamePadAction.PULL_DOWN.rawValue,
            MacKey(keyCode: kVK_Space): GamePadAction.PRESS_FIRE.rawValue
        ]

        let stdKeyMap2 = [

            MacKey(keyCode: kVK_ANSI_S): GamePadAction.PULL_LEFT.rawValue,
            MacKey(keyCode: kVK_ANSI_D): GamePadAction.PULL_RIGHT.rawValue,
            MacKey(keyCode: kVK_ANSI_E): GamePadAction.PULL_UP.rawValue,
            MacKey(keyCode: kVK_ANSI_X): GamePadAction.PULL_DOWN.rawValue,
            MacKey(keyCode: kVK_ANSI_C): GamePadAction.PRESS_FIRE.rawValue
        ]

        // Emulation keys
        register(Keys.Con.mouseKeyMap, encodable: emptyMap)
        register(Keys.Con.joyKeyMap1, encodable: stdKeyMap1)
        register(Keys.Con.joyKeyMap2, encodable: stdKeyMap2)
        register(Keys.Con.disconnectJoyKeys, true)

        // Joysticks
        register(Keys.Con.autofire, false)
        register(Keys.Con.autofireBullets, -3)
        register(Keys.Con.autofireFrequency, 2.5)

        // Mouse
        register(Keys.Con.retainMouseKeyComb, 0)
        register(Keys.Con.retainMouseWithKeys, true)
        register(Keys.Con.retainMouseByClick, true)
        register(Keys.Con.retainMouseByEntering, false)
        register(Keys.Con.releaseMouseKeyComb, 0)
        register(Keys.Con.releaseMouseWithKeys, true)
        register(Keys.Con.releaseMouseByShaking, true)
    }

    func removeControlsUserDefaults() {

        log(level: 2)

        let keys = [ Keys.Con.mouseKeyMap,
                     Keys.Con.joyKeyMap1,
                     Keys.Con.joyKeyMap2,
                     Keys.Con.disconnectJoyKeys,

                     Keys.Con.autofire,
                     Keys.Con.autofireBullets,
                     Keys.Con.autofireFrequency,

                     Keys.Con.retainMouseKeyComb,
                     Keys.Con.retainMouseWithKeys,
                     Keys.Con.retainMouseByClick,
                     Keys.Con.retainMouseByEntering,
                     Keys.Con.releaseMouseKeyComb,
                     Keys.Con.releaseMouseWithKeys,
                     Keys.Con.releaseMouseByShaking ]

        for key in keys { removeKey(key) }
    }
}

extension Preferences {

    func saveControlsUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!

        defaults.encode(Keys.Con.mouseKeyMap, keyMaps[0])
        defaults.encode(Keys.Con.joyKeyMap1, keyMaps[1])
        defaults.encode(Keys.Con.joyKeyMap2, keyMaps[2])
        defaults.set(Keys.Con.disconnectJoyKeys, disconnectJoyKeys)

        defaults.set(Keys.Con.autofire, autofire)
        defaults.set(Keys.Con.autofireBullets, autofireBullets)
        defaults.set(Keys.Con.autofireFrequency, autofireFrequency)

        defaults.set(Keys.Con.retainMouseKeyComb, retainMouseKeyComb)
        defaults.set(Keys.Con.retainMouseWithKeys, retainMouseWithKeys)
        defaults.set(Keys.Con.retainMouseByClick, retainMouseByClick)
        defaults.set(Keys.Con.retainMouseByEntering, retainMouseByEntering)
        defaults.set(Keys.Con.releaseMouseKeyComb, releaseMouseKeyComb)
        defaults.set(Keys.Con.releaseMouseWithKeys, releaseMouseWithKeys)
        defaults.set(Keys.Con.releaseMouseByShaking, releaseMouseByShaking)

        defaults.save()
    }

    func applyControlsUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!

        defaults.decode(Keys.Con.mouseKeyMap, &keyMaps[0])
        defaults.decode(Keys.Con.joyKeyMap1, &keyMaps[1])
        defaults.decode(Keys.Con.joyKeyMap2, &keyMaps[2])
        disconnectJoyKeys = defaults.bool(Keys.Con.disconnectJoyKeys)

        autofire = defaults.bool(Keys.Con.autofire)
        autofireBullets = defaults.int(Keys.Con.autofireBullets)
        autofireFrequency = defaults.double(Keys.Con.autofireFrequency)

        retainMouseKeyComb = defaults.int(Keys.Con.retainMouseKeyComb)
        retainMouseWithKeys = defaults.bool(Keys.Con.retainMouseWithKeys)
        retainMouseByClick = defaults.bool(Keys.Con.retainMouseByClick)
        retainMouseByEntering = defaults.bool(Keys.Con.retainMouseByEntering)
        releaseMouseKeyComb = defaults.int(Keys.Con.releaseMouseKeyComb)
        releaseMouseWithKeys = defaults.bool(Keys.Con.releaseMouseWithKeys)
        releaseMouseByShaking = defaults.bool(Keys.Con.releaseMouseByShaking)
    }
}

//
// User defaults (Devices)
//

extension Keys {
    
    struct Dev {

        static let schemes            = "Devices.Schemes"
    }
}

extension DefaultsProxy {

    func registerDevicesUserDefaults() {

    }

    func removeDevicesUserDefaults() {

    }
}

extension Preferences {

    func saveDevicesUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!

        defaults.save()
    }

    func applyDevicesUserDefaults() {

        log(level: 2)
    }
}

//
// User defaults (Keyboard)
//

extension Keys {
    
    struct Kbd {
        
        // Key map
        static let keyMap            = "Keyboard.KeyMap"
        static let mapKeysByPosition = "Keyboard.MapKeysByPosition"
    }
}

extension DefaultsProxy {

    func registerKeyboardUserDefaults() {

        log(level: 2)

        // Emulation keys
        register(Keys.Kbd.keyMap, encodable: KeyboardController.standardKeyMap)
        register(Keys.Kbd.mapKeysByPosition, false)
    }

    func removeKeyboardUserDefaults() {

        log(level: 2)

        let keys = [ Keys.Kbd.keyMap,
                     Keys.Kbd.mapKeysByPosition ]

        for key in keys { removeKey(key) }
    }
}

extension Preferences {

    func saveKeyboardUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!

        defaults.encode(Keys.Kbd.keyMap, keyMap)
        defaults.set(Keys.Kbd.mapKeysByPosition, mapKeysByPosition)

        defaults.save()
    }

    func applyKeyboardUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!

        defaults.decode(Keys.Kbd.keyMap, &keyMap)
        mapKeysByPosition = defaults.bool(Keys.Kbd.mapKeysByPosition)
    }
}

//
// User defaults (Hardware)
//

extension DefaultsProxy {

    func registerHardwareUserDefaults() {

        log(level: 2)
        // No GUI related items in this sections
    }

    func removeHardwareUserDefaults() {

        log(level: 2)

        remove(.VIC_REVISION)
        remove(.VIC_SPEED)
        remove(.VIC_POWER_SAVE)

        remove(.CIA_REVISION)
        remove(.TIMER_B_BUG)

        remove(.SID_REVISION)
        remove(.SID_FILTER)
        remove(.SID_ENABLE, [0, 1, 2, 3])
        remove(.SID_ADDRESS, [0, 1, 2, 3])

        remove(.GLUE_LOGIC)
        remove(.POWER_GRID)

        remove(.RAM_PATTERN)
    }
}

extension Configuration {

    func saveHardwareUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!

        c64.suspend()

        defaults.set(.VIC_REVISION, vicRevision)
        defaults.set(.VIC_SPEED, vicSpeed)
        defaults.set(.GRAY_DOT_BUG, vicGrayDotBug)

        defaults.set(.CIA_REVISION, ciaRevision)
        defaults.set(.TIMER_B_BUG, ciaTimerBBug)

        defaults.set(.SID_REVISION, sidRevision)
        defaults.set(.SID_FILTER, sidFilter)
        defaults.set(.SID_ENABLE, 1, sidEnable1)
        defaults.set(.SID_ENABLE, 2, sidEnable2)
        defaults.set(.SID_ENABLE, 3, sidEnable3)
        defaults.set(.SID_ADDRESS, 1, sidAddress1)
        defaults.set(.SID_ADDRESS, 2, sidAddress2)
        defaults.set(.SID_ADDRESS, 3, sidAddress3)

        defaults.set(.GLUE_LOGIC, glueLogic)
        defaults.set(.POWER_GRID, powerGrid)

        defaults.set(.RAM_PATTERN, ramPattern)

        defaults.save()

        c64.resume()
    }

    func applyHardwareUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!

        c64.suspend()

        vicRevision = defaults.get(.VIC_REVISION)
        vicSpeed = defaults.get(.VIC_SPEED)
        vicGrayDotBug = defaults.get(.GRAY_DOT_BUG) != 0

        ciaRevision = defaults.get(.CIA_REVISION)
        ciaTimerBBug = defaults.get(.TIMER_B_BUG) != 0

        sidRevision = defaults.get(.SID_REVISION)
        sidFilter = defaults.get(.SID_FILTER) != 0
        sidEnable1 = defaults.get(.SID_ENABLE, 1) != 0
        sidEnable2 = defaults.get(.SID_ENABLE, 2) != 0
        sidEnable3 = defaults.get(.SID_ENABLE, 3) != 0
        sidAddress1 = defaults.get(.SID_ADDRESS, 1)
        sidAddress2 = defaults.get(.SID_ADDRESS, 2)
        sidAddress3 = defaults.get(.SID_ADDRESS, 3)

        glueLogic = defaults.get(.GLUE_LOGIC)
        powerGrid = defaults.get(.POWER_GRID)

        ramPattern = defaults.get(.RAM_PATTERN)

        c64.resume()
    }
}

//
// User defaults (Peripherals)
//

extension Keys {
    
    struct Per {

        // Ports
        static let gameDevice1      = "Peripherals.ControlPort1"
        static let gameDevice2      = "Peripherals.ControlPort2"
    }
}

extension DefaultsProxy {

    func registerPeripheralsUserDefaults() {

        log(level: 2)

        // Port assignments
        register(Keys.Per.gameDevice1, -1)
        register(Keys.Per.gameDevice2, -1)
    }

    func removePeripheralsUserDefaults() {

        log(level: 2)

        let D8D9 = [DriveID.DRIVE8.rawValue, DriveID.DRIVE9.rawValue]

        remove(.DRV_CONNECT, D8D9)
        remove(.DRV_TYPE, D8D9)
        remove(.DRV_RAM, D8D9)
        remove(.DRV_PARCABLE, D8D9)
        remove(.DRV_AUTO_CONFIG, D8D9)
        removeKey(Keys.Per.gameDevice1)
        removeKey(Keys.Per.gameDevice2)
    }
}

extension Configuration {

    func savePeripheralsUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!
        let D8 = DriveID.DRIVE8.rawValue
        let D9 = DriveID.DRIVE9.rawValue

        c64.suspend()

        defaults.set(.DRV_CONNECT, D8, drive8Connected)
        defaults.set(.DRV_TYPE, D8, drive8Type)
        defaults.set(.DRV_RAM, D8, drive8Ram)
        defaults.set(.DRV_PARCABLE, D8, drive8ParCable)
        defaults.set(.DRV_AUTO_CONFIG, D8, drive8AutoConf)

        defaults.set(.DRV_CONNECT, D9, drive9Connected)
        defaults.set(.DRV_TYPE, D9, drive8Type)
        defaults.set(.DRV_RAM, D9, drive8Ram)
        defaults.set(.DRV_PARCABLE, D9, drive8ParCable)
        defaults.set(.DRV_AUTO_CONFIG, D9, drive8AutoConf)

        defaults.set(Keys.Per.gameDevice1, gameDevice1)
        defaults.set(Keys.Per.gameDevice2, gameDevice2)

        defaults.set(.MOUSE_MODEL, mouseModel)

        defaults.save()

        c64.resume()
    }

    func applyPeripheralsUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!
        let D8 = DriveID.DRIVE8.rawValue
        let D9 = DriveID.DRIVE9.rawValue

        c64.suspend()

        drive8Connected = defaults.get(.DRV_CONNECT, D8) != 0
        drive8Type = defaults.get(.DRV_TYPE, D8)
        drive8Ram = defaults.get(.DRV_RAM, D8)
        drive8ParCable = defaults.get(.DRV_PARCABLE, D8)
        drive8AutoConf = defaults.get(.DRV_AUTO_CONFIG, D8) != 0

        drive9Connected = defaults.get(.DRV_CONNECT, D9) != 0
        drive9Type = defaults.get(.DRV_TYPE, D9)
        drive9Ram = defaults.get(.DRV_RAM, D9)
        drive9ParCable = defaults.get(.DRV_PARCABLE, D9)
        drive9AutoConf = defaults.get(.DRV_AUTO_CONFIG, D9) != 0

        gameDevice1 = defaults.int(Keys.Per.gameDevice1)
        gameDevice2 = defaults.int(Keys.Per.gameDevice2)

        mouseModel = defaults.get(.MOUSE_MODEL)

        c64.resume()
    }
}

//
// User defaults (Compatibility)
//

extension DefaultsProxy {

    func registerCompatibilityUserDefaults() {

        log(level: 2)
        // No GUI related items in this sections
    }

    func removeCompatibilityUserDefaults() {

        log(level: 2)
        let D8 = DriveID.DRIVE8.rawValue
        let D9 = DriveID.DRIVE9.rawValue

        remove(.DRV_POWER_SAVE, D8)
        remove(.DRV_POWER_SAVE, D9)
        remove(.VIC_POWER_SAVE)
        remove(.SID_POWER_SAVE)
        remove(.SS_COLLISIONS)
        remove(.SB_COLLISIONS)
    }
}

extension Configuration {

    func saveCompatibilityUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!
        let D8 = DriveID.DRIVE8.rawValue
        let D9 = DriveID.DRIVE9.rawValue

        c64.suspend()

        defaults.set(.DRV_POWER_SAVE, D8, drive8PowerSave)
        defaults.set(.DRV_POWER_SAVE, D9, drive9PowerSave)
        defaults.set(.VIC_POWER_SAVE, viciiPowerSave)
        defaults.set(.SID_POWER_SAVE, sidPowerSave)
        defaults.set(.SS_COLLISIONS, ssCollisions)
        defaults.set(.SB_COLLISIONS, sbCollisions)
        defaults.save()

        c64.resume()
    }

    func applyCompatibilityUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!
        let D8 = DriveID.DRIVE8.rawValue
        let D9 = DriveID.DRIVE9.rawValue

        c64.suspend()

        drive8PowerSave = defaults.get(.DRV_POWER_SAVE, D8) != 0
        drive9PowerSave = defaults.get(.DRV_POWER_SAVE, D9) != 0
        viciiPowerSave = defaults.get(.VIC_POWER_SAVE) != 0
        sidPowerSave = defaults.get(.SID_POWER_SAVE) != 0
        ssCollisions = defaults.get(.SS_COLLISIONS) != 0
        sbCollisions = defaults.get(.SB_COLLISIONS) != 0

        c64.resume()
    }
}

//
// User defaults (Audio)
//

extension DefaultsProxy {

    func registerAudioUserDefaults() {

        log(level: 2)
        // No GUI related items in this sections
    }

    func removeAudioUserDefaults() {

        log(level: 2)

        let D8D9 = [DriveID.DRIVE8.rawValue, DriveID.DRIVE9.rawValue]

        remove(.SID_ENGINE)
        remove(.SID_SAMPLING)
        remove(.SID_FILTER)
        remove(.AUDVOL, [0, 1, 2, 3])
        remove(.AUDPAN, [0, 1, 2, 3])
        remove(.AUDVOLL)
        remove(.AUDVOLR)
        remove(.DRV_STEP_VOL, D8D9)
        remove(.DRV_INSERT_VOL, D8D9)
        remove(.DRV_EJECT_VOL, D8D9)
        remove(.DRV_PAN, D8D9)
    }
}

extension Configuration {

    func saveAudioUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!
        let D8 = DriveID.DRIVE8.rawValue
        let D9 = DriveID.DRIVE9.rawValue

        c64.suspend()

        defaults.set(.AUDVOL, 0, vol0)
        defaults.set(.AUDVOL, 1, vol1)
        defaults.set(.AUDVOL, 2, vol2)
        defaults.set(.AUDVOL, 3, vol3)
        defaults.set(.AUDPAN, 0, pan0)
        defaults.set(.AUDPAN, 1, pan1)
        defaults.set(.AUDPAN, 2, pan2)
        defaults.set(.AUDPAN, 3, pan3)
        defaults.set(.AUDVOLL, volL)
        defaults.set(.AUDVOLR, volR)
        defaults.set(.SID_SAMPLING, sidSampling)
        defaults.set(.DRV_PAN, D8, drive8Pan)
        defaults.set(.DRV_PAN, D9, drive9Pan)
        defaults.set(.DRV_STEP_VOL, [D8, D9], stepVolume)
        defaults.set(.DRV_INSERT_VOL, [D8, D9], insertVolume)
        defaults.set(.DRV_EJECT_VOL, [D8, D9], ejectVolume)
        defaults.set(.SID_FILTER, sidFilter)
        defaults.save()

        c64.resume()
    }

    func applyAudioUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!
        let D8 = DriveID.DRIVE8.rawValue
        let D9 = DriveID.DRIVE9.rawValue

        c64.suspend()

        vol0 = defaults.get(.AUDVOL, 0)
        vol1 = defaults.get(.AUDVOL, 1)
        vol2 = defaults.get(.AUDVOL, 2)
        vol3 = defaults.get(.AUDVOL, 3)

        pan0 = defaults.get(.AUDPAN, 0)
        pan1 = defaults.get(.AUDPAN, 1)
        pan2 = defaults.get(.AUDPAN, 2)
        pan3 = defaults.get(.AUDPAN, 3)

        drive8Pan = defaults.get(.DRV_PAN, D8)
        drive9Pan = defaults.get(.DRV_PAN, D9)

        volL = defaults.get(.AUDVOLL)
        volR = defaults.get(.AUDVOLR)
        sidSampling = defaults.get(.SID_SAMPLING)
        stepVolume = defaults.get(.DRV_STEP_VOL, D8)
        insertVolume = defaults.get(.DRV_INSERT_VOL, D8)
        ejectVolume = defaults.get(.DRV_EJECT_VOL, D8)
        sidFilter = defaults.get(.SID_FILTER) != 0

        c64.resume()
    }
}

//
// User defaults (Video)
//

extension Keys {
    
    struct Vid {

        // Geometry
        static let hCenter            = "Geometry.HCenter"
        static let vCenter            = "Geometry.VCenter"
        static let hZoom              = "Geometry.HZoom"
        static let vZoom              = "Geometry.VZoom"

        // Shaders
        static let upscaler           = "Shaders.Upscaler"
        static let blur               = "Shaders.Blur"
        static let blurRadius         = "Shaders.BlurRadius"
        static let bloom              = "Shaders.Bloom"
        static let bloomRadiusR       = "Shaders.BloonRadiusR"
        static let bloomRadiusG       = "Shaders.BloonRadiusG"
        static let bloomRadiusB       = "Shaders.BloonRadiusB"
        static let bloomBrightness    = "Shaders.BloomBrightness"
        static let bloomWeight        = "Shaders.BloomWeight"
        static let flicker            = "Shaders.Flicker"
        static let flickerWeight      = "Shaders.FlickerWeight"
        static let dotMask            = "Shaders.DotMask"
        static let dotMaskBrightness  = "Shaders.DotMaskBrightness"
        static let scanlines          = "Shaders.Scanlines"
        static let scanlineBrightness = "Shaders.ScanlineBrightness"
        static let scanlineWeight     = "Shaders.ScanlineWeight"
        static let disalignment       = "Shaders.Disalignment"
        static let disalignmentH      = "Shaders.DisalignmentH"
        static let disalignmentV      = "Shaders.DisalignmentV"
    }
}

extension DefaultsProxy {

    func registerVideoUserDefaults() {

        log(level: 2)

        registerColorUserDefaults()
        registerGeometryUserDefaults()
        registerShaderUserDefaults()
    }

    func registerColorUserDefaults() {

        log(level: 2)
        // No GUI related keys in this category
    }

    func registerGeometryUserDefaults() {

        log(level: 2)

        register(Keys.Vid.hCenter, 0)
        register(Keys.Vid.vCenter, 0)
        register(Keys.Vid.hZoom, 0)
        register(Keys.Vid.vZoom, 0.046)
    }

    func registerShaderUserDefaults() {

        log(level: 2)

        register(Keys.Vid.upscaler, 0)
        register(Keys.Vid.blur, 1)
        register(Keys.Vid.blurRadius, 0)
        register(Keys.Vid.bloom, 0)
        register(Keys.Vid.bloomRadiusR, 1.0)
        register(Keys.Vid.bloomRadiusG, 1.0)
        register(Keys.Vid.bloomRadiusB, 1.0)
        register(Keys.Vid.bloomBrightness, 0.4)
        register(Keys.Vid.bloomWeight, 1.21)
        register(Keys.Vid.dotMask, 0)
        register(Keys.Vid.dotMaskBrightness, 0.7)
        register(Keys.Vid.scanlines, 0)
        register(Keys.Vid.scanlineBrightness, 0.55)
        register(Keys.Vid.scanlineWeight, 0.11)
        register(Keys.Vid.disalignment, 0)
        register(Keys.Vid.disalignmentH, 0.001)
        register(Keys.Vid.disalignmentV, 0.001)
    }

    func removeVideoUserDefaults() {

        log(level: 2)

        removeColorUserDefaults()
        removeGeometryUserDefaults()
        removeShaderUserDefaults()
    }

    func removeColorUserDefaults() {

        log(level: 2)

        remove(.PALETTE)
        remove(.BRIGHTNESS)
        remove(.CONTRAST)
        remove(.SATURATION)
    }

    func removeGeometryUserDefaults() {

        log(level: 2)

        let keys = [ Keys.Vid.hCenter,
                     Keys.Vid.vCenter,
                     Keys.Vid.hZoom,
                     Keys.Vid.vZoom ]

        for key in keys { removeKey(key) }
    }

    func removeShaderUserDefaults() {

        log(level: 2)

        let keys = [ Keys.Vid.upscaler,
                     Keys.Vid.blur,
                     Keys.Vid.blurRadius,
                     Keys.Vid.bloom,
                     Keys.Vid.bloomRadiusR,
                     Keys.Vid.bloomRadiusG,
                     Keys.Vid.bloomRadiusB,
                     Keys.Vid.bloomBrightness,
                     Keys.Vid.bloomWeight,
                     Keys.Vid.dotMask,
                     Keys.Vid.dotMaskBrightness,
                     Keys.Vid.scanlines,
                     Keys.Vid.scanlineBrightness,
                     Keys.Vid.scanlineWeight,
                     Keys.Vid.disalignment,
                     Keys.Vid.disalignmentH,
                     Keys.Vid.disalignmentV ]

        for key in keys { removeKey(key) }
    }
}

extension Configuration {

    func saveVideoUserDefaults() {

        log(level: 2)

        saveColorUserDefaults()
        saveGeometryUserDefaults()
        saveShaderUserDefaults()
    }

    func saveColorUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!

        c64.suspend()

        defaults.set(.PALETTE, palette)
        defaults.set(.BRIGHTNESS, brightness)
        defaults.set(.CONTRAST, contrast)
        defaults.set(.SATURATION, saturation)

        defaults.save()

        c64.resume()
    }

    func saveGeometryUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!

        c64.suspend()

        defaults.set(Keys.Vid.hCenter, hCenter)
        defaults.set(Keys.Vid.vCenter, vCenter)
        defaults.set(Keys.Vid.hZoom, hZoom)
        defaults.set(Keys.Vid.vZoom, vZoom)

        defaults.save()

        c64.resume()
    }

    func saveShaderUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!

        c64.suspend()

        defaults.set(Keys.Vid.upscaler, upscaler)
        defaults.set(Keys.Vid.blur, blur)
        defaults.set(Keys.Vid.blurRadius, blurRadius)
        defaults.set(Keys.Vid.bloom, bloom)
        defaults.set(Keys.Vid.bloomRadiusR, bloomRadiusR)
        defaults.set(Keys.Vid.bloomRadiusG, bloomRadiusG)
        defaults.set(Keys.Vid.bloomRadiusB, bloomRadiusB)
        defaults.set(Keys.Vid.bloomBrightness, bloomBrightness)
        defaults.set(Keys.Vid.bloomWeight, bloomWeight)
        defaults.set(Keys.Vid.dotMask, dotMask)
        defaults.set(Keys.Vid.dotMaskBrightness, dotMaskBrightness)
        defaults.set(Keys.Vid.scanlines, scanlines)
        defaults.set(Keys.Vid.scanlineBrightness, scanlineBrightness)
        defaults.set(Keys.Vid.scanlineWeight, scanlineWeight)
        defaults.set(Keys.Vid.disalignment, disalignment)
        defaults.set(Keys.Vid.disalignmentH, disalignmentH)
        defaults.set(Keys.Vid.disalignmentV, disalignmentV)

        defaults.save()

        c64.resume()
    }

    func applyVideoUserDefaults() {

        log(level: 2)

        applyColorUserDefaults()
        applyGeometryUserDefaults()
        applyShaderUserDefaults()
    }

    func applyColorUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!

        c64.suspend()

        palette = defaults.get(.PALETTE)
        brightness = defaults.get(.BRIGHTNESS)
        contrast = defaults.get(.CONTRAST)
        saturation = defaults.get(.SATURATION)

        c64.resume()
    }

    func applyGeometryUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!

        c64.suspend()

        hCenter = defaults.float(Keys.Vid.hCenter)
        hCenter = defaults.float(Keys.Vid.vCenter)
        hZoom = defaults.float(Keys.Vid.hZoom)
        vZoom = defaults.float(Keys.Vid.vZoom)

        c64.resume()
    }

    func applyShaderUserDefaults() {

        log(level: 2)
        let defaults = C64Proxy.defaults!

        c64.suspend()

        upscaler = defaults.int(Keys.Vid.upscaler)
        blur = defaults.int(Keys.Vid.blur)
        blurRadius = defaults.float(Keys.Vid.blurRadius)
        bloom = defaults.int(Keys.Vid.bloom)
        bloomRadiusR = defaults.float(Keys.Vid.bloomRadiusR)
        bloomRadiusG = defaults.float(Keys.Vid.bloomRadiusG)
        bloomRadiusB = defaults.float(Keys.Vid.bloomRadiusB)
        bloomBrightness = defaults.float(Keys.Vid.bloomBrightness)
        bloomWeight = defaults.float(Keys.Vid.bloomWeight)
        dotMask = defaults.int(Keys.Vid.dotMask)
        dotMaskBrightness = defaults.float(Keys.Vid.dotMaskBrightness)
        scanlines = defaults.int(Keys.Vid.scanlines)
        scanlineBrightness = defaults.float(Keys.Vid.scanlineBrightness)
        scanlineWeight = defaults.float(Keys.Vid.scanlineWeight)
        disalignment = defaults.int(Keys.Vid.disalignment)
        disalignmentH = defaults.float(Keys.Vid.disalignmentH)
        disalignmentV = defaults.float(Keys.Vid.disalignmentV)

        c64.resume()
    }
}
