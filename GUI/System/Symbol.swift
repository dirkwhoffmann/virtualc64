// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import AppKit

@MainActor
enum Symbol {

    // Actions
    case pause
    case play
    case plus
    case power
    case reset
    case revert
    case trash
    case stepInto
    case stepOver
    case stepCycle
    case stepLine
    case stepFrame

    // Status
    case serverListening
    case serverConnected

    // Pictograms
    case alarm
    case bulletlist
    case chip
    case engine
    case gear
    case magnifyingglass
    case magnifyingglass2
    case magnifyingglass3

    // Devices
    case bus
    case console
    case gamecontroller
    case keyboard
    case mouse
    case nosign

    // Indicators
    case arrowClock
    case arrowUp
    case arrowDown
    case arrowkeys
    case gauge

    var systemNames: [String] {

        switch self {

            // Actions
        case .pause:            return [ "pause.circle" ]
        case .play:             return [ "play.circle" ]
        case .plus:             return [ "plus.app" ]
        case .power:            return [ "power" ]
        case .reset:            return [ "arrow.counterclockwise.circle" ]
        case .revert:           return [ "slider.horizontal.2.arrow.trianglehead.counterclockwise" ]
        case .trash:            return [ "trash" ]
        case .stepInto:         return [ "stepIntoTemplate" ]
        case .stepOver:         return [ "stepOverTemplate" ]
        case .stepCycle:        return [ "stepCycleTemplate" ]
        case .stepLine:         return [ "stepLineTemplate" ]
        case .stepFrame:        return [ "stepFrameTemplate" ]

            // Status
        case .serverListening:  return [ "point.3.connected.trianglepath.dotted" ]
        case .serverConnected:  return [ "point.3.filled.connected.trianglepath.dotted" ]

            // Pictograms
        case .alarm:            return [ "alarm" ]
        case .bulletlist:       return [ "list.bullet" ]
        case .chip:             return [ "memorychip" ]
        case .engine:           return [ "engine.combustion" ]
        case .gear:             return [ "gear" ]
        case .magnifyingglass:  return [ "magnifyingglass" ]
        case .magnifyingglass2: return [ "waveform.badge.magnifyingglass", "magnifyingglass" ]
        case .magnifyingglass3: return [ "waveform.path.ecg.magnifyingglass", "magnifyingglass" ]

            // Devices
        case .bus:              return [ "waveform.path.ecg.magnifyingglass", "waveform.path.ecg" ]
        case .console:          return [ "fossil.shell", "text.justify.left", "text.rectangle", "apple.terminal", "text.alignleft" ]
        case .gamecontroller:   return [ "gamecontroller" ]
        case .keyboard:         return [ "keyboard" ]
        case .mouse:            return [ "computermouse" ]
        case .nosign:           return [ "nosign" ]

            // Indicators
        case .arrowClock:       return [ "clock.arrow.trianglehead.counterclockwise.rotate.90",
                                         "clock.arrow.circlepath" ]
        case .arrowUp:          return [ "arrow.up.circle" ]
        case .arrowDown:        return [ "arrow.down.circle" ]
        case .arrowkeys:        return [ "arrowkeys" ]
        case .gauge:            return [ "gauge.with.needle", "gauge.chart.lefthalf.righthalf" ]
        }
    }

    private struct CacheKey: Hashable {

        let symbol: Symbol
        let size: CGFloat
    }

    private static var imageCache = [CacheKey: NSImage]()

    static func get(_ symbol: Symbol, size: CGFloat = 25, description: String? = nil) -> NSImage {

        // Return cached image if available
        if let cached = imageCache[CacheKey(symbol: symbol, size: size)] { return cached }

        let config = NSImage.SymbolConfiguration(pointSize: size, weight: .light, scale: .small)
        let border = CGFloat(4)

        for name in symbol.systemNames {

            // Look up the image in the assets folder (custom images)
            if let img = get(name: name, size: size - border) {
                img.isTemplate = true
                return img
            }

            // Look up the image in the system SF library
            if let img = NSImage(systemSymbolName: name, accessibilityDescription: description) {
                if let result = img.withSymbolConfiguration(config) {
                    return result
                }
            }
        }

        // No image found
        return NSImage(systemSymbolName: "questionmark.circle", accessibilityDescription: description)!
    }

    static func get(name: String, size: CGFloat = 25) -> NSImage? {

        guard let img = NSImage(named: name) else { return nil }
        return img.resize(size: CGSize(width: size, height: size))
    }
}
