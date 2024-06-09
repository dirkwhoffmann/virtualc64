// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension Inspector {

    private func cacheEvents() {

        if let emu = emu {
            eventInfo = emu.paused ? emu.c64.info : emu.c64.cachedInfo
        }
    }

    func refreshEvents(count: Int = 0, full: Bool = false) {

        cacheEvents()

        evCpuProgress.stringValue = "\(eventInfo.cpuProgress) Cycles"
        evVicProgress.stringValue = "\(eventInfo.frame) Frames"
        evCia1Progress.stringValue = "\(eventInfo.cia1Progress) Cycles"
        evCia2Progress.stringValue = "\(eventInfo.cia2Progress) Cycles"

        evTableView.refresh(count: count, full: full)
    }
}
