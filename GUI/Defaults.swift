// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

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

        debug(.defaults, "Loading user defaults")

        do {
            let folder = try URL.appSupportFolder()
            let path = folder.appendingPathComponent("virtualc64.ini")

            do {
                try load(url: path)
                debug(.defaults, "Successfully loaded user defaults from file \(path)")
            } catch {
                warn("Failed to load user defaults from file \(path)")
            }

        } catch {
            warn("Failed to access application support folder")
        }
    }

    func save(url: URL) throws {

        let exception = ExceptionWrapper()
        save(url, exception: exception)
        if exception.errorCode != .OK { throw VC64Error(exception) }
    }

    func save() {

        debug(.defaults, "Saving user defaults")

        do {
            let folder = try URL.appSupportFolder()
            let path = folder.appendingPathComponent("virtualc64.ini")

            do {
                try save(url: path)
                debug(.defaults, "Successfully saved user defaults to file \(path)")
            } catch {
                warn("Failed to save user defaults file \(path)")
            }

        } catch {
            warn("Failed to access application support folder")
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
    func remove(_ option: vc64.Option, _ nr: Int) {
        remove(option, nr: nr)
    }
    func remove(_ option: vc64.Option, _ nr: [Int]) {
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
    func set(_ option: vc64.Option, _ val: Int) {
        setOpt(option, value: val)
    }
    func set(_ option: vc64.Option, _ val: Bool) {
        setOpt(option, value: val ? 1 : 0)
    }
    func set(_ option: vc64.Option, _ nr: Int, _ val: Int) {
        setOpt(option, nr: nr, value: val)
    }
    func set(_ option: vc64.Option, _ nr: Int, _ val: Bool) {
        setOpt(option, nr: nr, value: val ? 1 : 0)
    }
    func set(_ option: vc64.Option, _ nr: [Int], _ val: Int) {
        for n in nr { setOpt(option, nr: n, value: val) }
    }
    func set(_ option: vc64.Option, _ nr: [Int], _ val: Bool) {
        for n in nr { setOpt(option, nr: n, value: val ? 1 : 0) }
    }
    func get(_ option: vc64.Option) -> Int {
        return getOpt(option)
    }
    func get(_ option: vc64.Option, _ nr: Int) -> Int {
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

            if let data = Data(base64Encoded: jsonString) {

                if let decoded = try? JSONDecoder().decode(T.self, from: data) {
                    item = decoded
                } else {
                    warn("Failed to decode \(jsonString)")
                }
                return
            }
        }
        warn("Failed to decode jsonString")
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

    static func romUrl(fingerprint: Int) -> URL? {

        return romUrl(name: String(format: "%08x", fingerprint) + ".rom")
    }

    static func mediaUrl(name: String) -> URL? {

        let folder = try? URL.appSupportFolder("Media")
        return folder?.appendingPathComponent(name)
    }

    static var basicRomUrl: URL? { return romUrl(name: "basic.bin") }
    static var charRomUrl: URL? { return romUrl(name: "char.bin") }
    static var kernalRomUrl: URL? { return romUrl(name: "kernal.bin") }
    static var vc1541RomUrl: URL? { return romUrl(name: "vc1541.bin") }
}

//
// User defaults (all)
//

extension DefaultsProxy {

    func registerUserDefaults() {

        debug(.defaults, "Registering user defaults")

        registerGeneralUserDefaults()
        registerControlsUserDefaults()
        registerDevicesUserDefaults()
        registerKeyboardUserDefaults()

        registerHardwareUserDefaults()
        registerPeripheralsUserDefaults()
        registerPerformanceUserDefaults()
        registerAudioUserDefaults()
        registerVideoUserDefaults()
    }
}

extension Preferences {

    func applyUserDefaults() {

        debug(.defaults, "Applying user defaults")

        applyGeneralUserDefaults()
        applyControlsUserDefaults()
        applyDevicesUserDefaults()
        applyKeyboardUserDefaults()
    }
}

extension Configuration {

    func applyUserDefaults() {

        debug(.defaults)

        applyHardwareUserDefaults()
        applyPeripheralsUserDefaults()
        applyPerformanceUserDefaults()
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

        // Fullscreen
        static let keepAspectRatio        = "General.FullscreenKeepAspectRatio"
        static let exitOnEsc              = "General.FullscreenExitOnEsc"

        // Miscellaneous
        static let ejectWithoutAsking     = "General.EjectWithoutAsking"
        static let closeWithoutAsking     = "General.CloseWithoutAsking"
        static let pauseInBackground      = "General.PauseInBackground"
    }
}

extension DefaultsProxy {

    func registerGeneralUserDefaults() {

        debug(.defaults)

        // Snapshots
        register(Keys.Gen.autoSnapshots, false)
        register(Keys.Gen.autoSnapshotInterval, 20)

        // Screenshots
        register(Keys.Gen.screenshotSource, 0)
        register(Keys.Gen.screenshotTarget, NSBitmapImageRep.FileType.png.rawValue)

        // Captures
        register(Keys.Gen.ffmpegPath, "")
        register(Keys.Gen.captureSource, 0)

        // Fullscreen
        register(Keys.Gen.keepAspectRatio, false)
        register(Keys.Gen.exitOnEsc, true)

        // Misc
        register(Keys.Gen.ejectWithoutAsking, false)
        register(Keys.Gen.closeWithoutAsking, false)
        register(Keys.Gen.pauseInBackground, false)
    }

    func removeGeneralUserDefaults() {

        debug(.defaults)

        let keys = [ Keys.Gen.autoSnapshots,
                     Keys.Gen.autoSnapshotInterval,

                     Keys.Gen.screenshotSource,
                     Keys.Gen.screenshotTarget,

                     Keys.Gen.ffmpegPath,
                     Keys.Gen.captureSource,

                     Keys.Gen.keepAspectRatio,
                     Keys.Gen.exitOnEsc,

                     Keys.Gen.ejectWithoutAsking,
                     Keys.Gen.closeWithoutAsking,
                     Keys.Gen.pauseInBackground
        ]

        for key in keys { removeKey(key) }
    }
}

extension Preferences {

    func saveGeneralUserDefaults() {

        debug(.defaults)
        let defaults = EmulatorProxy.defaults!

        defaults.set(Keys.Gen.autoSnapshots, autoSnapshots)
        defaults.set(Keys.Gen.autoSnapshotInterval, snapshotInterval)

        defaults.set(Keys.Gen.screenshotSource, screenshotSource)
        defaults.set(Keys.Gen.screenshotTarget, screenshotTargetIntValue)

        defaults.set(Keys.Gen.ffmpegPath, ffmpegPath)
        defaults.set(Keys.Gen.captureSource, captureSource)

        defaults.set(Keys.Gen.keepAspectRatio, keepAspectRatio)
        defaults.set(Keys.Gen.exitOnEsc, exitOnEsc)

        defaults.set(Keys.Gen.ejectWithoutAsking, ejectWithoutAsking)
        defaults.set(Keys.Gen.closeWithoutAsking, closeWithoutAsking)
        defaults.set(Keys.Gen.pauseInBackground, pauseInBackground)

        defaults.save()
    }

    func applyGeneralUserDefaults() {

        debug(.defaults)
        let defaults = EmulatorProxy.defaults!

        autoSnapshots = defaults.bool(Keys.Gen.autoSnapshots)
        snapshotInterval = defaults.int(Keys.Gen.autoSnapshotInterval)

        screenshotSource = defaults.int(Keys.Gen.screenshotSource)
        screenshotTargetIntValue = defaults.int(Keys.Gen.screenshotTarget)

        ffmpegPath = defaults.string(Keys.Gen.ffmpegPath)
        captureSource = defaults.int(Keys.Gen.captureSource)

        keepAspectRatio = defaults.bool(Keys.Gen.keepAspectRatio)
        exitOnEsc = defaults.bool(Keys.Gen.exitOnEsc)

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

        debug(.defaults)

        let emptyMap: [MacKey: Int] = [:]

        let stdKeyMap1: [MacKey: Int] = [

            MacKey(keyCode: kVK_LeftArrow): vc64.GamePadAction.PULL_LEFT.rawValue,
            MacKey(keyCode: kVK_RightArrow): vc64.GamePadAction.PULL_RIGHT.rawValue,
            MacKey(keyCode: kVK_UpArrow): vc64.GamePadAction.PULL_UP.rawValue,
            MacKey(keyCode: kVK_DownArrow): vc64.GamePadAction.PULL_DOWN.rawValue,
            MacKey(keyCode: kVK_Space): vc64.GamePadAction.PRESS_FIRE.rawValue
        ]

        let stdKeyMap2 = [

            MacKey(keyCode: kVK_ANSI_S): vc64.GamePadAction.PULL_LEFT.rawValue,
            MacKey(keyCode: kVK_ANSI_D): vc64.GamePadAction.PULL_RIGHT.rawValue,
            MacKey(keyCode: kVK_ANSI_E): vc64.GamePadAction.PULL_UP.rawValue,
            MacKey(keyCode: kVK_ANSI_X): vc64.GamePadAction.PULL_DOWN.rawValue,
            MacKey(keyCode: kVK_ANSI_C): vc64.GamePadAction.PRESS_FIRE.rawValue
        ]

        // Emulation keys
        register(Keys.Con.mouseKeyMap, encodable: emptyMap)
        register(Keys.Con.joyKeyMap1, encodable: stdKeyMap1)
        register(Keys.Con.joyKeyMap2, encodable: stdKeyMap2)
        register(Keys.Con.disconnectJoyKeys, true)

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

        debug(.defaults)

        let keys = [ Keys.Con.mouseKeyMap,
                     Keys.Con.joyKeyMap1,
                     Keys.Con.joyKeyMap2,
                     Keys.Con.disconnectJoyKeys,

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

        debug(.defaults)
        let defaults = EmulatorProxy.defaults!

        defaults.encode(Keys.Con.mouseKeyMap, keyMaps[0])
        defaults.encode(Keys.Con.joyKeyMap1, keyMaps[1])
        defaults.encode(Keys.Con.joyKeyMap2, keyMaps[2])
        defaults.set(Keys.Con.disconnectJoyKeys, disconnectJoyKeys)

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

        debug(.defaults)
        let defaults = EmulatorProxy.defaults!

        defaults.decode(Keys.Con.mouseKeyMap, &keyMaps[0])
        defaults.decode(Keys.Con.joyKeyMap1, &keyMaps[1])
        defaults.decode(Keys.Con.joyKeyMap2, &keyMaps[2])
        disconnectJoyKeys = defaults.bool(Keys.Con.disconnectJoyKeys)

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

        debug(.defaults)
        let defaults = EmulatorProxy.defaults!

        defaults.save()
    }

    func applyDevicesUserDefaults() {

        debug(.defaults)
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

        debug(.defaults)

        // Emulation keys
        register(Keys.Kbd.keyMap, encodable: KeyboardController.standardKeyMap)
        register(Keys.Kbd.mapKeysByPosition, false)
    }

    func removeKeyboardUserDefaults() {

        debug(.defaults)

        let keys = [ Keys.Kbd.keyMap,
                     Keys.Kbd.mapKeysByPosition ]

        for key in keys { removeKey(key) }
    }
}

extension Preferences {

    func saveKeyboardUserDefaults() {

        debug(.defaults)
        let defaults = EmulatorProxy.defaults!

        defaults.encode(Keys.Kbd.keyMap, keyMap)
        defaults.set(Keys.Kbd.mapKeysByPosition, mapKeysByPosition)

        defaults.save()
    }

    func applyKeyboardUserDefaults() {

        debug(.defaults)
        let defaults = EmulatorProxy.defaults!

        defaults.decode(Keys.Kbd.keyMap, &keyMap)
        mapKeysByPosition = defaults.bool(Keys.Kbd.mapKeysByPosition)
    }
}

//
// Roms
//

extension Configuration {

}

//
// User defaults (Hardware)
//

extension DefaultsProxy {

    func registerHardwareUserDefaults() {

        debug(.defaults)
        // No GUI related items in this sections
    }

    func removeHardwareUserDefaults() {

        debug(.defaults)

        remove(.VICII_REVISION)
        remove(.VICII_POWER_SAVE)

        remove(.CIA_REVISION)
        remove(.CIA_TIMER_B_BUG)

        remove(.SID_REVISION)
        remove(.SID_FILTER)
        remove(.SID_ENABLE, [0, 1, 2, 3])
        remove(.SID_ADDRESS, [0, 1, 2, 3])

        remove(.GLUE_LOGIC)
        remove(.POWER_GRID)

        remove(.MEM_INIT_PATTERN)
    }
}

extension Configuration {

    func saveHardwareUserDefaults() {

        debug(.defaults)

        if let emu = emu {

            emu.suspend()

            let defaults = EmulatorProxy.defaults!

            defaults.set(.VICII_REVISION, vicRevision)
            defaults.set(.VICII_GRAY_DOT_BUG, vicGrayDotBug)

            defaults.set(.CIA_REVISION, ciaRevision)
            defaults.set(.CIA_TIMER_B_BUG, ciaTimerBBug)

            defaults.set(.SID_REVISION, [0, 1, 2, 3], sidRevision)
            defaults.set(.SID_FILTER, [0, 1, 2, 3], sidFilter)
            defaults.set(.SID_ENABLE, 1, sidEnable1)
            defaults.set(.SID_ENABLE, 2, sidEnable2)
            defaults.set(.SID_ENABLE, 3, sidEnable3)
            defaults.set(.SID_ADDRESS, 1, sidAddress1)
            defaults.set(.SID_ADDRESS, 2, sidAddress2)
            defaults.set(.SID_ADDRESS, 3, sidAddress3)

            defaults.set(.GLUE_LOGIC, glueLogic)
            defaults.set(.POWER_GRID, powerGrid)

            defaults.set(.MEM_INIT_PATTERN, ramPattern)

            defaults.save()

            emu.resume()
        }
    }

    func applyHardwareUserDefaults() {

        debug(.defaults)

        if let emu = emu {

            emu.suspend()

            let defaults = EmulatorProxy.defaults!

            vicRevision = defaults.get(.VICII_REVISION)
            vicGrayDotBug = defaults.get(.VICII_GRAY_DOT_BUG) != 0

            ciaRevision = defaults.get(.CIA_REVISION)
            ciaTimerBBug = defaults.get(.CIA_TIMER_B_BUG) != 0

            sidRevision = defaults.get(.SID_REVISION, 0)
            sidFilter = defaults.get(.SID_FILTER, 0) != 0
            sidEnable1 = defaults.get(.SID_ENABLE, 1) != 0
            sidEnable2 = defaults.get(.SID_ENABLE, 2) != 0
            sidEnable3 = defaults.get(.SID_ENABLE, 3) != 0
            sidAddress1 = defaults.get(.SID_ADDRESS, 1)
            sidAddress2 = defaults.get(.SID_ADDRESS, 2)
            sidAddress3 = defaults.get(.SID_ADDRESS, 3)

            glueLogic = defaults.get(.GLUE_LOGIC)
            powerGrid = defaults.get(.POWER_GRID)

            ramPattern = defaults.get(.MEM_INIT_PATTERN)

            emu.resume()
        }
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

        debug(.defaults)

        // Port assignments
        register(Keys.Per.gameDevice1, -1)
        register(Keys.Per.gameDevice2, -1)
    }

    func removePeripheralsUserDefaults() {

        debug(.defaults)

        remove(.DRV_CONNECT, [0, 1])
        remove(.DRV_TYPE, [0, 1])
        remove(.DRV_RAM, [0, 1])
        remove(.DRV_PARCABLE, [0, 1])
        remove(.DRV_AUTO_CONFIG, [0, 1])
        removeKey(Keys.Per.gameDevice1)
        removeKey(Keys.Per.gameDevice2)
    }
}

extension Configuration {

    func savePeripheralsUserDefaults() {

        debug(.defaults)

        if let emu = emu {

            emu.suspend()

            let defaults = EmulatorProxy.defaults!

            defaults.set(.DRV_CONNECT, 0, drive8Connected)
            defaults.set(.DRV_TYPE, 0, drive8Type)
            defaults.set(.DRV_RAM, 0, drive8Ram)
            defaults.set(.DRV_PARCABLE, 0, drive8ParCable)
            defaults.set(.DRV_AUTO_CONFIG, 0, drive8AutoConf)

            defaults.set(.DRV_CONNECT, 1, drive9Connected)
            defaults.set(.DRV_TYPE, 1, drive8Type)
            defaults.set(.DRV_RAM, 1, drive8Ram)
            defaults.set(.DRV_PARCABLE, 1, drive8ParCable)
            defaults.set(.DRV_AUTO_CONFIG, 1, drive8AutoConf)

            defaults.set(Keys.Per.gameDevice1, gameDevice1)
            defaults.set(Keys.Per.gameDevice2, gameDevice2)

            defaults.set(.MOUSE_MODEL, mouseModel)
            defaults.set(.PADDLE_ORIENTATION, paddleOrientation)

            defaults.save()

            emu.resume()
        }
    }

    func applyPeripheralsUserDefaults() {

        debug(.defaults)

        if let emu = emu {

            emu.suspend()

            let defaults = EmulatorProxy.defaults!

            drive8Connected = defaults.get(.DRV_CONNECT, 0) != 0
            drive8Type = defaults.get(.DRV_TYPE, 0)
            drive8Ram = defaults.get(.DRV_RAM, 0)
            drive8ParCable = defaults.get(.DRV_PARCABLE, 0)
            drive8AutoConf = defaults.get(.DRV_AUTO_CONFIG, 0) != 0

            drive9Connected = defaults.get(.DRV_CONNECT, 1) != 0
            drive9Type = defaults.get(.DRV_TYPE, 1)
            drive9Ram = defaults.get(.DRV_RAM, 1)
            drive9ParCable = defaults.get(.DRV_PARCABLE, 1)
            drive9AutoConf = defaults.get(.DRV_AUTO_CONFIG, 1) != 0

            gameDevice1 = defaults.int(Keys.Per.gameDevice1)
            gameDevice2 = defaults.int(Keys.Per.gameDevice2)

            mouseModel = defaults.get(.MOUSE_MODEL)
            paddleOrientation = defaults.get(.PADDLE_ORIENTATION)

            emu.resume()
        }
    }
}

//
// User defaults (Performance)
//

extension DefaultsProxy {

    func registerPerformanceUserDefaults() {

        debug(.defaults)
        // No GUI related items in this sections
    }

    func removePerformanceUserDefaults() {

        debug(.defaults)

        remove(.DRV_POWER_SAVE, [0, 1])
        remove(.VICII_POWER_SAVE)
        remove(.VICII_SS_COLLISIONS)
        remove(.VICII_SB_COLLISIONS)
        remove(.SID_POWER_SAVE, [0, 1, 2, 3])
        remove(.C64_WARP_MODE)
        remove(.C64_WARP_BOOT)
        remove(.C64_SPEED_BOOST)
        remove(.C64_VSYNC)
        remove(.C64_RUN_AHEAD)
    }
}

extension Configuration {

    func savePerformanceUserDefaults() {

        debug(.defaults)

        if let emu = emu {

            emu.suspend()

            let defaults = EmulatorProxy.defaults!

            defaults.set(.DRV_POWER_SAVE, 0, drive8PowerSave)
            defaults.set(.DRV_POWER_SAVE, 1, drive9PowerSave)
            defaults.set(.SID_POWER_SAVE, [0, 1, 2, 3], sidPowerSave)
            defaults.set(.VICII_POWER_SAVE, viciiPowerSave)
            defaults.set(.VICII_SS_COLLISIONS, ssCollisions)
            defaults.set(.VICII_SB_COLLISIONS, sbCollisions)
            defaults.set(.C64_WARP_MODE, warpMode)
            defaults.set(.C64_WARP_BOOT, warpBoot)
            defaults.set(.C64_SPEED_BOOST, speedBoost)
            defaults.set(.C64_VSYNC, vsync)
            defaults.set(.C64_RUN_AHEAD, runAhead)

            defaults.save()

            emu.resume()
        }
    }

    func applyPerformanceUserDefaults() {

        debug(.defaults)

        if let emu = emu {

            emu.suspend()

            let defaults = EmulatorProxy.defaults!

            drive8PowerSave = defaults.get(.DRV_POWER_SAVE, 0) != 0
            drive9PowerSave = defaults.get(.DRV_POWER_SAVE, 1) != 0
            sidPowerSave = defaults.get(.SID_POWER_SAVE, 0) != 0
            viciiPowerSave = defaults.get(.VICII_POWER_SAVE) != 0
            ssCollisions = defaults.get(.VICII_SS_COLLISIONS) != 0
            sbCollisions = defaults.get(.VICII_SB_COLLISIONS) != 0
            warpMode = defaults.get(.C64_WARP_MODE)
            warpBoot = defaults.get(.C64_WARP_BOOT)
            speedBoost = defaults.get(.C64_SPEED_BOOST)
            vsync = defaults.get(.C64_VSYNC) != 0
            runAhead = defaults.get(.C64_RUN_AHEAD)

            emu.resume()
        }
    }
}

//
// User defaults (Audio)
//

extension DefaultsProxy {

    func registerAudioUserDefaults() {

        debug(.defaults)
        // No GUI related items in this sections
    }

    func removeAudioUserDefaults() {

        debug(.defaults)

        remove(.SID_ENGINE, [0, 1, 2, 3])
        remove(.SID_SAMPLING, [0, 1, 2, 3])
        remove(.SID_FILTER, [0, 1, 2, 3])
        remove(.AUD_VOL0)
        remove(.AUD_VOL1)
        remove(.AUD_VOL2)
        remove(.AUD_VOL3)
        remove(.AUD_PAN0)
        remove(.AUD_PAN1)
        remove(.AUD_PAN2)
        remove(.AUD_PAN3)
        remove(.AUD_VOL_L)
        remove(.AUD_VOL_R)
        remove(.DRV_STEP_VOL, [0, 1])
        remove(.DRV_INSERT_VOL, [0, 1])
        remove(.DRV_EJECT_VOL, [0, 1])
        remove(.DRV_PAN, [0, 1])
    }
}

extension Configuration {

    func saveAudioUserDefaults() {

        debug(.defaults)

        if let emu = emu {

            emu.suspend()

            let defaults = EmulatorProxy.defaults!

            defaults.set(.AUD_VOL0, vol0)
            defaults.set(.AUD_VOL1, vol1)
            defaults.set(.AUD_VOL2, vol2)
            defaults.set(.AUD_VOL3, vol3)
            defaults.set(.AUD_PAN0, pan0)
            defaults.set(.AUD_PAN1, pan1)
            defaults.set(.AUD_PAN2, pan2)
            defaults.set(.AUD_PAN3, pan3)
            defaults.set(.AUD_VOL_L, volL)
            defaults.set(.AUD_VOL_R, volR)
            defaults.set(.SID_SAMPLING, [0, 1, 2, 3], sidSampling)
            defaults.set(.DRV_PAN, 0, drive8Pan)
            defaults.set(.DRV_PAN, 1, drive9Pan)
            defaults.set(.DRV_STEP_VOL, [0, 1], stepVolume)
            defaults.set(.DRV_INSERT_VOL, [0, 1], insertVolume)
            defaults.set(.DRV_EJECT_VOL, [0, 1], ejectVolume)
            defaults.set(.SID_FILTER, [0, 1, 2, 3], sidFilter)
            defaults.save()

            emu.resume()
        }
    }

    func applyAudioUserDefaults() {

        debug(.defaults)

        if let emu = emu {

            emu.suspend()

            let defaults = EmulatorProxy.defaults!

            vol0 = defaults.get(.AUD_VOL0)
            vol1 = defaults.get(.AUD_VOL1)
            vol2 = defaults.get(.AUD_VOL2)
            vol3 = defaults.get(.AUD_VOL3)

            pan0 = defaults.get(.AUD_PAN0)
            pan1 = defaults.get(.AUD_PAN1)
            pan2 = defaults.get(.AUD_PAN2)
            pan3 = defaults.get(.AUD_PAN3)

            drive8Pan = defaults.get(.DRV_PAN, 0)
            drive9Pan = defaults.get(.DRV_PAN, 1)

            volL = defaults.get(.AUD_VOL_L)
            volR = defaults.get(.AUD_VOL_R)
            sidSampling = defaults.get(.SID_SAMPLING, 0)
            stepVolume = defaults.get(.DRV_STEP_VOL, 0)
            insertVolume = defaults.get(.DRV_INSERT_VOL, 0)
            ejectVolume = defaults.get(.DRV_EJECT_VOL, 0)
            sidFilter = defaults.get(.SID_FILTER, 0) != 0

            emu.resume()
        }
    }
}

//
// User defaults (Video)
//

extension DefaultsProxy {

    func registerVideoUserDefaults() {

        debug(.defaults)
        // No GUI related keys in this category
    }

    func removeVideoUserDefaults() {

        debug(.defaults)

        removeColorUserDefaults()
        removeGeometryUserDefaults()
        removeShaderUserDefaults()
    }

    func removeColorUserDefaults() {

        debug(.defaults)

        remove(.MON_PALETTE)
        remove(.MON_BRIGHTNESS)
        remove(.MON_CONTRAST)
        remove(.MON_SATURATION)
    }

    func removeGeometryUserDefaults() {

        debug(.defaults)

        remove(.MON_HCENTER)
        remove(.MON_VCENTER)
        remove(.MON_HZOOM)
        remove(.MON_VZOOM)
    }

    func removeShaderUserDefaults() {

        debug(.defaults)

        remove(.MON_UPSCALER)
        remove(.MON_BLUR)
        remove(.MON_BLUR_RADIUS)
        remove(.MON_BLOOM)
        remove(.MON_BLOOM_RADIUS)
        remove(.MON_BLOOM_BRIGHTNESS)
        remove(.MON_BLOOM_WEIGHT)
        remove(.MON_DOTMASK)
        remove(.MON_DOTMASK_BRIGHTNESS)
        remove(.MON_SCANLINES)
        remove(.MON_SCANLINE_BRIGHTNESS)
        remove(.MON_SCANLINE_WEIGHT)
        remove(.MON_DISALIGNMENT)
        remove(.MON_DISALIGNMENT_H)
        remove(.MON_DISALIGNMENT_V)
    }
}

extension Configuration {

    func saveVideoUserDefaults() {

        debug(.defaults)

        saveColorUserDefaults()
        saveGeometryUserDefaults()
        saveShaderUserDefaults()
    }

    func saveColorUserDefaults() {

        debug(.defaults)

        if let emu = emu {

            emu.suspend()

            let defaults = EmulatorProxy.defaults!
            defaults.set(.MON_PALETTE, palette)
            defaults.set(.MON_BRIGHTNESS, brightness)
            defaults.set(.MON_CONTRAST, contrast)
            defaults.set(.MON_SATURATION, saturation)
            defaults.save()

            emu.resume()
        }
    }

    func saveGeometryUserDefaults() {

        debug(.defaults)

        if let emu = emu {

            emu.suspend()

            let defaults = EmulatorProxy.defaults!
            defaults.set(.MON_HCENTER, hCenter)
            defaults.set(.MON_VCENTER, vCenter)
            defaults.set(.MON_HZOOM, hZoom)
            defaults.set(.MON_VZOOM, vZoom)
            defaults.save()

            emu.resume()
        }
    }

    func saveShaderUserDefaults() {

        debug(.defaults)

        if let emu = emu {

            emu.suspend()

            let defaults = EmulatorProxy.defaults!
            defaults.set(.MON_UPSCALER, upscaler)
            defaults.set(.MON_BLUR, blur)
            defaults.set(.MON_BLUR_RADIUS, blurRadius)
            defaults.set(.MON_BLOOM, bloom)
            defaults.set(.MON_BLOOM_RADIUS, bloomRadius)
            defaults.set(.MON_BLOOM_BRIGHTNESS, bloomBrightness)
            defaults.set(.MON_BLOOM_WEIGHT, bloomWeight)
            defaults.set(.MON_DOTMASK, dotMask)
            defaults.set(.MON_DOTMASK_BRIGHTNESS, dotMaskBrightness)
            defaults.set(.MON_SCANLINES, scanlines)
            defaults.set(.MON_SCANLINE_BRIGHTNESS, scanlineBrightness)
            defaults.set(.MON_SCANLINE_WEIGHT, scanlineWeight)
            defaults.set(.MON_DISALIGNMENT, disalignment)
            defaults.set(.MON_DISALIGNMENT_H, disalignmentH)
            defaults.set(.MON_DISALIGNMENT_V, disalignmentV)
            defaults.save()

            emu.resume()
        }
    }

    func applyVideoUserDefaults() {

        debug(.defaults)

        applyColorUserDefaults()
        applyGeometryUserDefaults()
        applyShaderUserDefaults()
    }

    func applyColorUserDefaults() {

        debug(.defaults)

        if let emu = emu {

            emu.suspend()

            let defaults = EmulatorProxy.defaults!
            palette = defaults.get(.MON_PALETTE)
            brightness = defaults.get(.MON_BRIGHTNESS)
            contrast = defaults.get(.MON_CONTRAST)
            saturation = defaults.get(.MON_SATURATION)

            emu.resume()
        }
    }

    func applyGeometryUserDefaults() {

        debug(.defaults)

        if let emu = emu {

            emu.suspend()

            let defaults = EmulatorProxy.defaults!

            hCenter = defaults.get(.MON_HCENTER)
            vCenter = defaults.get(.MON_VCENTER)
            hZoom = defaults.get(.MON_HZOOM)
            vZoom = defaults.get(.MON_VZOOM)

            emu.resume()
        }
    }

    func applyShaderUserDefaults() {

        debug(.defaults)

        if let emu = emu {

            emu.suspend()

            let defaults = EmulatorProxy.defaults!

            upscaler = defaults.get(.MON_UPSCALER)
            blur = defaults.get(.MON_BLUR)
            blurRadius = defaults.get(.MON_BLUR_RADIUS)
            bloom = defaults.get(.MON_BLOOM)
            bloomRadius = defaults.get(.MON_BLOOM_RADIUS)
            bloomBrightness = defaults.get(.MON_BLOOM_BRIGHTNESS)
            bloomWeight = defaults.get(.MON_BLOOM_WEIGHT)
            dotMask = defaults.get(.MON_DOTMASK)
            dotMaskBrightness = defaults.get(.MON_DOTMASK_BRIGHTNESS)
            scanlines = defaults.get(.MON_SCANLINES)
            scanlineBrightness = defaults.get(.MON_SCANLINE_BRIGHTNESS)
            scanlineWeight = defaults.get(.MON_SCANLINE_WEIGHT)
            disalignment = defaults.get(.MON_DISALIGNMENT)
            disalignmentH = defaults.get(.MON_DISALIGNMENT_H)
            disalignmentV = defaults.get(.MON_DISALIGNMENT_V)

            emu.resume()
        }
    }
}
