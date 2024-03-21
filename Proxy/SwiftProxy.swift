// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// swiftlint:disable nesting

class SwiftAPI {

    // References to the emulator core
    var core: vc64.VirtualC64!

    init(with core: vc64.VirtualC64) {

        self.core = core
    }
}

class SwiftProxy: SwiftAPI {

    // Message receiver
    private var delegate: ((vc64.Message) -> Void)!

    // Sub-proxies
    let c64: SwiftC64Proxy

    override init(with core: vc64.VirtualC64) {

        c64 = .init(with: core)
        super.init(with: core)
    }

    //
    // Static methods
    //

    static var version: String { return String(vc64.VirtualC64.version()) }
    static var build: String { return String(vc64.VirtualC64.build()) }

    //
    // Analyzing the emulator
    //

    var config: vc64.EmulatorConfig { return core.getConfig().pointee }
    var state: vc64.EmulatorInfo { return core.getState().pointee }
    var stats: vc64.EmulatorStats { return core.getStats().pointee }

    //
    // Querying the emulator state
    //

    var isPoweredOn: Bool { return core.isPoweredOn(); }
    var isPoweredOff: Bool { return core.isPoweredOff(); }
    var isPaused: Bool { return core.isPaused(); }
    var isRunning: Bool { return core.isRunning(); }
    var isSuspended: Bool { return core.isSuspended(); }
    var isHalted: Bool { return core.isHalted(); }
    var isWarping: Bool { return core.isWarping(); }
    var isTracking: Bool { return core.isTracking(); }

    //
    // Controlling the emulator state
    //

    func powerOn() { core.powerOn() }
    func powerOff() { core.powerOff() }
    func run() { core.run() }
    func pause() { core.pause() }
    func halt() { core.halt() }
    func stopAndGo() { core.stopAndGo() }
    func suspend() { core.suspendThread() }
    func resume() { core.resumeThread() }
    func warpOn(source: Int = 0) { core.warpOn(source) }
    func warpOff(source: Int = 0) { core.warpOff(source) }
    func trackOn(source: Int = 0) { core.trackOn(source) }
    func trackOff(source: Int = 0) { core.trackOff(source) }

    //
    // Single-stepping
    //

    func stepInto() { core.stepInto() }
    func stepOver() { core.stepOver() }

    //
    // Synchronizing the emulator thread
    //

    func wakeUp() { core.wakeUp() }

    //
    // Accessing video data
    //

    func getTexture() -> UnsafeMutablePointer<u32>? { return core.getTexture() }
    func getNoise() -> UnsafeMutablePointer<u32>? { return core.getNoise() }

    //
    // Configuring the emulator
    //

    func launch(delegate: @escaping (vc64.Message) -> Void) {

        self.delegate = delegate

        // Convert 'self' to a void pointer
        let myself = UnsafeRawPointer(Unmanaged.passUnretained(self).toOpaque())

        core.launch(myself) { (ptr, msg: vc64.Message) in

            // Convert void pointer back to 'self'
            let myself = Unmanaged<SwiftProxy>.fromOpaque(ptr!).takeUnretainedValue()

            // Process message in the main thread
            DispatchQueue.main.async { myself.delegate(msg) }
        }
    }

    func get(option: vc64.Option) -> Int64 { return core.get(option) }
    func get(option: vc64.Option, id: Int) -> Int64 { return core.get(option, id) }
    func set(model: vc64.C64Model) { core.set(model) }

    func set(option: vc64.Option, value: Int64) throws {

        var error = vc64.VC64Error()
        core.set(option, value, &error)
        if error.data != 0 { throw VC64Error(error) }
    }

    func set(option: vc64.Option, id: Int, value: Int64) throws {

        var error = vc64.VC64Error()
        core.set(option, id, value, &error)
        if error.data != 0 { throw VC64Error(error) }
    }

    func exportConfig(path: String) throws {

        var error = vc64.VC64Error()
        core.exportConfig(std.string(path), &error)
        if error.data != 0 { throw VC64Error(error) }
    }
}

//
// Constants
//

extension SwiftProxy {

    struct Constants {

        struct Texture {

            static var Width: Int { return vc64.Texture.width }
            static var Height: Int { return vc64.Texture.height }
        }
    }
}

//
// C64
//

class SwiftC64Proxy: SwiftAPI {

    func hardReset() {
        core.c64.hardReset()
    }

    func softReset() {
        core.c64.softReset()
    }
}

// swiftlint:enable nesting
