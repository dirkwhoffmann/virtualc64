//
//  MyController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 29.01.18.
//

import Foundation

extension MyController {
    
    //! @brief  GamePadManager delegation method
    func joystickEvent(slot: Int, event: JoystickEvent) {
        
        if (slot == gamepadSlotA) {
            print("Trigger on Port A")
            c64.joystickA.trigger(event)
        }

        if (slot == gamepadSlotB) {
            print("Trigger on Port B")
            c64.joystickB.trigger(event)
        }
    }    
}
