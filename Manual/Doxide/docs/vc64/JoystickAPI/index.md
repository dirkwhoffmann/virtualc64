---
generator: doxide
---


# JoystickAPI

**`class JoystickAPI : public API`**

Joystick Public API


## Functions

| Name | Description |
| ---- | ----------- |
| [getConfig](#getConfig) |  Returns the component's current configuration. |
| [getInfo](#getInfo) |  Returns the component's current state. |
| [getCachedInfo](#getCachedInfo) | Returns the most recent state snapshot recorded by the emulator thread (see getInfo()). |
| [trigger](#trigger) |  Triggers a joystick action. |
| [trigger](#trigger) |  Sets the complete joystick state. |

## Function Details

### getCachedInfo<a name="getCachedInfo"></a>
!!! function "const JoystickInfo &amp;getCachedInfo() const"
    
    Returns the most recent state snapshot recorded by the emulator thread (see getInfo()).
    

### getConfig<a name="getConfig"></a>
!!! function "const JoystickConfig &amp;getConfig() const"
    
     Returns the component's current configuration.
    

### getInfo<a name="getInfo"></a>
!!! function "const JoystickInfo &amp;getInfo() const"
    
     Returns the component's current state.
    
     getInfo() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedInfo()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

### trigger<a name="trigger"></a>
!!! function "void trigger(GamePadAction event)"
    
     Triggers a joystick action.
    

!!! function "void trigger(const std::array&lt;bool, 5&gt; &amp;state, const std::array&lt;bool, 5&gt; &amp;previous)"
    
     Sets the complete joystick state.
    
     The states are ordered up, down, left, right, fire. The directions are
     applied as given, which is harmless to repeat. The fire button is only
     forwarded if it differs in 'previous', because pressing it again would
     toggle autofire. Pass the state that was reported by the previous call.
    
     
    :material-location-enter: `state`
    :          The new states of up, down, left, right, and fire.
     
    :material-location-enter: `previous`
    :       The previous states.
    

