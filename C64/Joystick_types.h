//
//  Joystick_types.h
//  V64
//
//  Created by Dirk Hoffmann on 15.04.18.
//

#ifndef JOYSTICK_TYPES_H
#define JOYSTICK_TYPES_H

/*! @brief    Joystick directions
 */
typedef enum {
    
    JOYSTICK_UP,
    JOYSTICK_DOWN,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT,
    JOYSTICK_FIRE,
    
} JoystickDirection;

/*! @brief    Joystick events
 */
typedef enum {
    
    PULL_UP,
    PULL_DOWN,
    PULL_LEFT,
    PULL_RIGHT,
    PRESS_FIRE,
    RELEASE_X,
    RELEASE_Y,
    RELEASE_XY,
    RELEASE_FIRE
    
} JoystickEvent;

#endif /* Joystick_types_h */
