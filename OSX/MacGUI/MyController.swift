//
//  MyController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 29.01.18.
//

import Foundation

extension MyController {
    
    //! @brief  GamePadManager delegation method
    /*! @returns true, iff a joystick event has been triggered on port A or port B
     */
    @discardableResult
    func joystickEvent(slot: Int, event: JoystickEvent) -> Bool {
        
        if (slot == gamepadSlotA) {
            c64.joystickA.trigger(event)
            return true
        }

        if (slot == gamepadSlotB) {
            c64.joystickB.trigger(event)
            return true
        }
        
        return false
    }    
}
