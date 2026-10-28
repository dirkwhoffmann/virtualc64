---
generator: doxide
---


# KeyboardAPI

**`class KeyboardAPI : public API`**

Keyboard Public API


## Functions

| Name | Description |
| ---- | ----------- |
| [isPressed](#isPressed) |  Checks if a key is currently pressed. |
| [press](#press) |  Presses a key :material-location-enter: `key` :        The key to press. :material-location-enter: `delay` :      An optional delay in seconds. :material-location-enter: `duration` :    An optional time in seconds after which the key is released again. If 0, the key stays pressed until it is released explicitly. If no delay is specified, the function will immediately modify the C64's keyboard matrix. Otherwise, it will ask the event scheduler to modify the matrix with the specified delay. !!! note If you wish to press multiple keys, make sure to let some time pass between two key presses. You need to give the C64 time to scan the keyboard matrix before another key can be pressed.  |
| [toggle](#toggle) |  Toggles a key :material-location-enter: `key` :        The key to press or release. :material-location-enter: `delay` :      An optional delay in seconds. :material-location-enter: `duration` :    An optional time in seconds after which the key is toggled back. If 0, the key is toggled only once. If no delay is specified, the function will immediately modify the C64's keyboard matrix. Otherwise, it will ask the event scheduler to modify the matrix with the specified delay.  |
| [release](#release) |  Releases a key :material-location-enter: `key` :        The key to release. :material-location-enter: `delay` :      An optional delay in seconds. If no delay is specified, the function will immediately modify the C64's keyboard matrix. Otherwise, it will ask the event scheduler to modify the matrix with the specified delay.  |
| [releaseAll](#releaseAll) |  Releases all currently pressed keys  |
| [isLocked](#isLocked) |  Checks if a key is currently pressed. |
| [lock](#lock) |  Locks a key :material-location-enter: `key` :        The key to lock. When a key is marked as locked, it won't be released when release() or releaseAll() is called. :material-location-enter: `delay` :      An optional delay in seconds.  |
| [unlock](#unlock) |  Unlocks a key :material-location-enter: `key` :         The key to unlock. Removes the lock, putting the key back into it's normal state. :material-location-enter: `delay` :      An optional delay in seconds.  |
| [unlockAll](#unlockAll) |  Unlocks all keys Removes the lock for all keys, putting them back into their normal state. |
| [autoType](#autoType) |  Uses the auto-typing daemon to type a string. |
| [abortTyping](#abortTyping) |  Aborts any active auto-typing activity. |

## Function Details

### abortTyping<a name="abortTyping"></a>
!!! function "void abortTyping()"
    
     Aborts any active auto-typing activity.
    

### autoType<a name="autoType"></a>
!!! function "void autoType(const string &amp;text)"
    
     Uses the auto-typing daemon to type a string.
     
    :material-location-enter: `text`
    :       The text to type.
    

### isLocked<a name="isLocked"></a>
!!! function "bool isLocked(C64Key key) const"
    
     Checks if a key is currently pressed.
     
    :material-location-enter: `key`
    :        The key to check.
    

### isPressed<a name="isPressed"></a>
!!! function "bool isPressed(C64Key key) const"
    
     Checks if a key is currently pressed.
     
    :material-location-enter: `key`
    :        The key to check.
    

### lock<a name="lock"></a>
!!! function "void lock(C64Key key, double delay = 0.0)"
    
     Locks a key
    
     
    :material-location-enter: `key`
    :        The key to lock.
    
     When a key is marked as locked, it won't be released when release()
     or releaseAll() is called.
    
     
    :material-location-enter: `delay`
    :      An optional delay in seconds.
    

### press<a name="press"></a>
!!! function "void press(C64Key key, double delay = 0.0, double duration = 0.0)"
    
     Presses a key
     
    :material-location-enter: `key`
    :        The key to press.
     
    :material-location-enter: `delay`
    :      An optional delay in seconds.
     
    :material-location-enter: `duration`
    :    An optional time in seconds after which the key is
                     released again. If 0, the key stays pressed until it
                     is released explicitly.
    
     If no delay is specified, the function will immediately modify the
     C64's keyboard matrix. Otherwise, it will ask the event scheduler
     to modify the matrix with the specified delay.
    
     
    !!! note
     If you wish to press multiple keys, make sure to let some time
         pass between two key presses. You need to give the C64 time to scan the
         keyboard matrix before another key can be pressed.
    

### release<a name="release"></a>
!!! function "void release(C64Key key, double delay = 0.0)"
    
     Releases a key
     
    :material-location-enter: `key`
    :        The key to release.
     
    :material-location-enter: `delay`
    :      An optional delay in seconds.
    
     If no delay is specified, the function will immediately modify the
     C64's keyboard matrix. Otherwise, it will ask the event scheduler
     to modify the matrix with the specified delay.
    

### releaseAll<a name="releaseAll"></a>
!!! function "void releaseAll(double delay = 0.0)"
    
     Releases all currently pressed keys
    

### toggle<a name="toggle"></a>
!!! function "void toggle(C64Key key, double delay = 0.0, double duration = 0.0)"
    
     Toggles a key
     
    :material-location-enter: `key`
    :        The key to press or release.
     
    :material-location-enter: `delay`
    :      An optional delay in seconds.
     
    :material-location-enter: `duration`
    :    An optional time in seconds after which the key is
                     toggled back. If 0, the key is toggled only once.
    
     If no delay is specified, the function will immediately modify the
     C64's keyboard matrix. Otherwise, it will ask the event scheduler
     to modify the matrix with the specified delay.
    

### unlock<a name="unlock"></a>
!!! function "void unlock(C64Key key, double delay = 0.0)"
    
     Unlocks a key
    
     
    :material-location-enter: `key`
    :         The key to unlock.
    
     Removes the lock, putting the key back into it's normal state.
    
     
    :material-location-enter: `delay`
    :      An optional delay in seconds.
    

### unlockAll<a name="unlockAll"></a>
!!! function "void unlockAll(double delay = 0.0)"
    
     Unlocks all keys
    
     Removes the lock for all keys, putting them back into their normal state.
    
     
    :material-location-enter: `delay`
    :      An optional delay in seconds.
    

