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

        eventInfo = c64.eventInfo
    }

    func refreshEvents(count: Int = 0, full: Bool = false) {

        cacheEvents()

        evCpuProgress.integerValue = Int(eventInfo.cpuProgress)
        evVicProgress.integerValue = Int(eventInfo.frame)
        evCia1Progress.integerValue = Int(eventInfo.cia1Progress)
        evCia2Progress.integerValue = Int(eventInfo.cia2Progress)

        evTableView.refresh(count: count, full: full)
    }
}
