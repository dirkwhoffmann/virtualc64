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
    
    //
    // Keyboard events
    //
    
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
    
    //
    // Joystick simulation
    //

    /*! @brief  Computes unique fingerprint for a certain key combination pressed
     *          on the pyhsical Mac keyboard
     */
    func fingerprint(forKey keycode: Int32, withModifierFlags flags: UInt) -> MacKeyFingerprint
    {
        
        return 0
    }
    
    //! @brief  Returns fingerprint from keymap
    func joyFingerprint(forKeymap nr: Int32, direction d: JoystickDirection) -> MacKeyFingerprint
    {
 
        return 0
    }
    
    //! @brief  Stores fingerprint in keymap
    func setJoyFingerprint(_ fingerprint: MacKeyFingerprint, forKeymap map: Int32, direction d: JoystickDirection)
    {
        
    }
    
    //! @brief  Returns keymap entry as a printable charakter
    func joyChar(forKeymap nr: Int32, direction d: JoystickDirection) -> Int8
    {
        
        return 0
    }
    
    //! @brief  Stores a printable charakter in keymap
    func setJoyChar(_ c: Int8, forKeymap map: Int32, direction d: JoystickDirection)
    {
        
    }
    
    /*! @brief  Pulls joystick if key matches some value stored in keymap
     */
    func pullJoystick(device d: Int32, ifKeyMatches key: MacKeyFingerprint, inKeymap keymap: Int32) -> Bool
    {
        return true
    }
    
    /*! @brief  Releases joystick if key matches some value stored in keymap
     */
    func releaseJoystick(device d: Int32, ifKeyMatches key: MacKeyFingerprint, inKeymap keymap: Int32) -> Bool
    {
        
        return true
    }
    
    /*! @brief  Translates a pressed key on the Mac keyboard to a C64 key fingerprint
     *  @note   The returned value can be used as argument for the emulators pressKey() function
     */
    func translateKey(_ key: Int8, plainkey: Int8, keycode: Int16, flags: UInt) -> C64KeyFingerprint
    {
        return 0
    }

}
