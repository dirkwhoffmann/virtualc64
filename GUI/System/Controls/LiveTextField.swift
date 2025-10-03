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
class LiveTextField: NSTextField {

    override func textDidChange(_ notification: Notification) {

        super.textDidChange(notification)
        if let action = self.action {
            NSApp.sendAction(action, to: self.target, from: self)
        }
    }
}
