---
generator: doxide
---


# MouseAPI

**`class MouseAPI : public API`**

Mouse Public API


## Functions

| Name | Description |
| ---- | ----------- |
| [getConfig](#getConfig) |  Returns the component's current configuration. |
| [setXY](#setXY) | Moves the mouse :material-location-enter: `x` :       New absolute horizontal coordinate :material-location-enter: `y` :       New absolute vertical coordinate  |
| [setDxDy](#setDxDy) | Moves the mouse :material-location-enter: `dx` :          Relative horizontal mouse movement :material-location-enter: `dy` :          Relative vertical mouse movement  |
| [trigger](#trigger) | Triggers a mouse button event :material-location-enter: `action` :      The triggered event  |
| [detectShakeXY](#detectShakeXY) | Feeds a coordinate into the shake detector. |
| [detectShakeDxDy](#detectShakeDxDy) | Feeds a coordinate into the shake detector. |

## Function Details

### detectShakeDxDy<a name="detectShakeDxDy"></a>
!!! function "bool detectShakeDxDy(double dx, double dy)"
    
    Feeds a coordinate into the shake detector.
    
     The shake detector keeps track of the transmitted coordinates and
     scans for rapid movements caused by shaking the mouse.
    
     
    :material-location-enter: `dx`
    :      Current horizontal mouse position, relative to the
                 previous position.
     
    :material-location-enter: `dy`
    :      Current vertical mouse position, relative to the
                 previous position.
     
    :material-keyboard-return: **Return**
    :        true iff a shaking mouse has been detected.
    

### detectShakeXY<a name="detectShakeXY"></a>
!!! function "bool detectShakeXY(double x, double y)"
    
    Feeds a coordinate into the shake detector.
    
     The shake detector keeps track of the transmitted coordinates and
     scans for rapid movements caused by shaking the mouse.
    
     
    :material-location-enter: `x`
    :       Current horizontal mouse position.
     
    :material-location-enter: `y`
    :       Current vertical mouse position.
     
    :material-keyboard-return: **Return**
    :        true iff a shaking mouse has been detected.
    

### getConfig<a name="getConfig"></a>
!!! function "const MouseConfig &amp;getConfig() const"
    
     Returns the component's current configuration.
    

### setDxDy<a name="setDxDy"></a>
!!! function "void setDxDy(double dx, double dy)"
    
    Moves the mouse
     
    :material-location-enter: `dx`
    :          Relative horizontal mouse movement
     
    :material-location-enter: `dy`
    :          Relative vertical mouse movement
    

### setXY<a name="setXY"></a>
!!! function "void setXY(double x, double y)"
    
    Moves the mouse
     
    :material-location-enter: `x`
    :       New absolute horizontal coordinate
     
    :material-location-enter: `y`
    :       New absolute vertical coordinate
    

### trigger<a name="trigger"></a>
!!! function "void trigger(GamePadAction action)"
    
    Triggers a mouse button event
     
    :material-location-enter: `action`
    :      The triggered event
    

