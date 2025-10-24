// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import AppKit

enum SFSymbol {

    case alarm
    case arrowClock
    case arrowDown
    case arrowkeys
    case arrowUp
    case bus
    case bulletlist
    case chip
    case console
    case engine
    case gamecontroller
    case gauge
    case gear
    case keyboard
    case magnifyingglass
    case magnifyingglass2
    case magnifyingglass3
    case mouse
    case nosign
    case pause
    case play
    case plus
    case power
    case reset
    case revert
    case trash

    // Custom images
    case stepInto
    case stepOver
    case stepCycle
    case stepLine
    case stepFrame
    
    var systemNames: [String] {

        switch self {

        case .alarm:            return [ "alarm" ]
        case .arrowClock:       return [ "clock.arrow.trianglehead.counterclockwise.rotate.90",
                                         "clock.arrow.circlepath" ]
        case .arrowDown:        return [ "arrow.down.circle" ]
        case .arrowkeys:        return [ "arrowkeys" ]
        case .arrowUp:          return [ "arrow.up.circle" ]
        case .bus:              return [ "waveform.path.ecg.magnifyingglass", "waveform.path.ecg" ]
        case .bulletlist:       return [ "list.bullet" ]
        case .chip:             return [ "memorychip" ]
        case .console:          return [ "text.rectangle", "apple.terminal", "text.alignleft" ]
        case .engine:           return [ "engine.combustion" ]
        case .gamecontroller:   return [ "gamecontroller" ]
        case .gauge:            return [ "gauge.chart.lefthalf.righthalf", "gauge.with.needle" ]
        case .gear:             return [ "gear" ]
        case .keyboard:         return [ "keyboard" ]
        case .magnifyingglass:  return [ "magnifyingglass" ]
        case .magnifyingglass2: return [ "waveform.badge.magnifyingglass", "magnifyingglass" ]
        case .magnifyingglass3: return [ "waveform.path.ecg.magnifyingglass", "magnifyingglass" ]
        case .mouse:            return [ "computermouse" ]
        case .nosign:           return [ "nosign" ]
        case .pause:            return [ "pause.circle" ]
        case .play:             return [ "play.circle" ]
        case .plus:             return [ "plus.app" ]
        case .power:            return [ "power" ]
        case .reset:            return [ "arrow.counterclockwise.circle" ]
        case .revert:           return [ "slider.horizontal.2.arrow.trianglehead.counterclockwise" ]
        case .trash:            return [ "trash" ]

        default:
            return []
        }
    }

    static func get(_ symbol: SFSymbol, size: CGFloat = 25, description: String? = nil) -> NSImage {

        let border = CGFloat(4)

        switch symbol {
        case stepInto:  return get(name: "stepIntoTemplate", size: size - border)!
        case stepOver:  return get(name: "stepOverTemplate", size: size - border)!
        case stepCycle: return get(name: "stepCycleTemplate", size: size - border)!
        case stepLine:  return get(name: "stepLineTemplate", size: size - border)!
        case stepFrame: return get(name: "stepFrameTemplate", size: size - border)!

        default:
            break
        }

        let config = NSImage.SymbolConfiguration(pointSize: size, weight: .light, scale: .small)

        for name in symbol.systemNames {

            if let img = NSImage(systemSymbolName: name, accessibilityDescription: description) {
                if let result = img.withSymbolConfiguration(config) {
                    return result
                }
            }
        }

        return NSImage(systemSymbolName: "questionmark.circle", accessibilityDescription: description)!
    }

    static func get(name: String, size: CGFloat = 25) -> NSImage? {

        let img = NSImage(named: name)!
        return img.resize(size: CGSize(width: size, height: size));
    }

}

