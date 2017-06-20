/*
 * (C) 2017 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

import Foundation

//!@ brief Keyboard event handler
extension MyController
{
    
    override open func keyDown(with event: NSEvent)
    {
        // Exit fullscreen mode if ESC is pressed
        if (event.keyCode == MAC_ESC && metalScreen.fullscreen) {
            window!.toggleFullScreen(nil)
        }
        
        // Pass all keyboard events to the metal view
        metalScreen.keyDown(with: event)
    }
    
    override open func keyUp(with event: NSEvent)
    {
        metalScreen.keyUp(with: event)
    }

    override open func flagsChanged(with event: NSEvent) {
        metalScreen.flagsChanged(with: event)
    }
}
