// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension DispatchTime {

    static func diffNano(_ t: DispatchTime) -> UInt64 {
        return DispatchTime.now().uptimeNanoseconds - t.uptimeNanoseconds
    }

    static func diffMicroSec(_ t: DispatchTime) -> UInt64 { return diffNano(t) / 1_000 }
    static func diffMilliSec(_ t: DispatchTime) -> UInt64 { return diffNano(t) / 1_000_000 }
    static func diffSec(_ t: DispatchTime) -> UInt64 { return diffNano(t) / 1_000_000_000 }
}
