// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// swiftlint:disable nesting

class SwiftProxy {

    // References to the emulator core
    var core: vc64.VirtualC64!

    // Message receiver
    private var delegate: ((vc64.Message) -> Void)!

    init(with core: vc64.VirtualC64) {

        debug(.lifetime, "Creating Swift Proxy")
        self.core = core
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
}

// swiftlint:enable nesting
