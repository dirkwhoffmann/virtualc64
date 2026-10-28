---
generator: doxide
---


# GuardsAPI

**`class GuardsAPI : public API`**

Guards API

 Gives access to one list of guards (the breakpoints or the watchpoints of the CPU). The functions that modify the
 list come in two flavors, following the usual scheme of the API:

 - The plain functions (e.g., setAt()) return when the emulator thread has
   carried out the request. A failure throws the corresponding exception
   (e.g., if a guard to remove does not exist). They throw
   CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot carry
   out the request, because it has not been launched yet, is halted or
   suspended, or if they are called from the emulator thread itself.
 - The functions with the Async suffix (e.g., setAtAsync()) queue the
   request and return at once. A failure is not reported to the caller.
   The change shows up in the query functions only after the emulator
   thread has carried it out (see sync()).


## Functions

| Name | Description |
| ---- | ----------- |
| [elements](#elements) |  Returns the number of guards in the guard list. |
| [guardNr](#guardNr) |  Returns information about a guard. |
| [guardAt](#guardAt) |  Returns information about a guard. |
| [setAt](#setAt) |  Sets a guard. |
| [setAtAsync](#setAtAsync) | Asynchronous version of setAt(): queues the request and returns at once.  |
| [moveTo](#moveTo) |  Relocates a guard. |
| [moveToAsync](#moveToAsync) | Asynchronous version of moveTo(): queues the request and returns at once.  |
| [remove](#remove) |  Deletes a guard. |
| [removeAsync](#removeAsync) | Asynchronous version of remove(): queues the request and returns at once.  |
| [removeAt](#removeAt) |  Deletes a guard. |
| [removeAtAsync](#removeAtAsync) | Asynchronous version of removeAt(): queues the request and returns at once.  |
| [removeAll](#removeAll) |  Deletes all guards. |
| [removeAllAsync](#removeAllAsync) | Asynchronous version of removeAll(): queues the request and returns at once.  |
| [enable](#enable) |  Enables a guard. |
| [enableAsync](#enableAsync) | Asynchronous version of enable(): queues the request and returns at once.  |
| [enableAt](#enableAt) |  Enables a guard. |
| [enableAtAsync](#enableAtAsync) | Asynchronous version of enableAt(): queues the request and returns at once.  |
| [enableAll](#enableAll) |  Enables all guards. |
| [enableAllAsync](#enableAllAsync) | Asynchronous version of enableAll(): queues the request and returns at once.  |
| [disable](#disable) |  Disables a guard. |
| [disableAsync](#disableAsync) | Asynchronous version of disable(): queues the request and returns at once.  |
| [disableAt](#disableAt) |  Disables a guard. |
| [disableAtAsync](#disableAtAsync) | Asynchronous version of disableAt(): queues the request and returns at once.  |
| [disableAll](#disableAll) |  Disables all guards. |
| [disableAllAsync](#disableAllAsync) | Asynchronous version of disableAll(): queues the request and returns at once.  |
| [toggle](#toggle) |  Enables a disabled guard and disables an enabled one. |
| [toggleAsync](#toggleAsync) | Asynchronous version of toggle(): queues the request and returns at once.  |

## Function Details

### disable<a name="disable"></a>
!!! function "void disable(isize nr)"
    
     Disables a guard.
     
    :material-location-enter: `nr`
    :         Number of the guard in the guard list
    

### disableAll<a name="disableAll"></a>
!!! function "void disableAll()"
    
     Disables all guards.
    

### disableAllAsync<a name="disableAllAsync"></a>
!!! function "void disableAllAsync()"
    
    Asynchronous version of disableAll(): queues the request and returns at once.
    

### disableAsync<a name="disableAsync"></a>
!!! function "void disableAsync(isize nr)"
    
    Asynchronous version of disable(): queues the request and returns at once.
    

### disableAt<a name="disableAt"></a>
!!! function "void disableAt(u32 target)"
    
     Disables a guard.
     
    :material-location-enter: `target`
    :     The target of the guard to be disabled
    

### disableAtAsync<a name="disableAtAsync"></a>
!!! function "void disableAtAsync(u32 target)"
    
    Asynchronous version of disableAt(): queues the request and returns at once.
    

### elements<a name="elements"></a>
!!! function "isize elements() const"
    
     Returns the number of guards in the guard list.
    

### enable<a name="enable"></a>
!!! function "void enable(isize nr)"
    
     Enables a guard.
     
    :material-location-enter: `nr`
    :         Number of the guard in the guard list
    

### enableAll<a name="enableAll"></a>
!!! function "void enableAll()"
    
     Enables all guards.
    

### enableAllAsync<a name="enableAllAsync"></a>
!!! function "void enableAllAsync()"
    
    Asynchronous version of enableAll(): queues the request and returns at once.
    

### enableAsync<a name="enableAsync"></a>
!!! function "void enableAsync(isize nr)"
    
    Asynchronous version of enable(): queues the request and returns at once.
    

### enableAt<a name="enableAt"></a>
!!! function "void enableAt(u32 target)"
    
     Enables a guard.
     
    :material-location-enter: `target`
    :     The target of the guard to be enabled
    

### enableAtAsync<a name="enableAtAsync"></a>
!!! function "void enableAtAsync(u32 target)"
    
    Asynchronous version of enableAt(): queues the request and returns at once.
    

### guardAt<a name="guardAt"></a>
!!! function "std::optional&lt;GuardInfo&gt; guardAt(u32 target) const"
    
     Returns information about a guard.
     
    :material-location-enter: `target`
    :     The target of the guard to query
     
    :material-keyboard-return: **Return**
    :    A copy of the guard's state, or nothing if the target is not guarded.
    

### guardNr<a name="guardNr"></a>
!!! function "std::optional&lt;GuardInfo&gt; guardNr(isize nr) const"
    
     Returns information about a guard.
     
    :material-location-enter: `nr`
    :         Number of the guard in the guard list
     
    :material-keyboard-return: **Return**
    :    A copy of the guard's state, or nothing if it does not exist.
    

### moveTo<a name="moveTo"></a>
!!! function "void moveTo(isize nr, u32 target)"
    
     Relocates a guard.
     
    :material-location-enter: `nr`
    :         Number of the guard in the guard list
     
    :material-location-enter: `target`
    :     New target
    

### moveToAsync<a name="moveToAsync"></a>
!!! function "void moveToAsync(isize nr, u32 target)"
    
    Asynchronous version of moveTo(): queues the request and returns at once.
    

### remove<a name="remove"></a>
!!! function "void remove(isize nr)"
    
     Deletes a guard.
     
    :material-location-enter: `nr`
    :         Number of the guard in the guard list
    

### removeAll<a name="removeAll"></a>
!!! function "void removeAll()"
    
     Deletes all guards.
    

### removeAllAsync<a name="removeAllAsync"></a>
!!! function "void removeAllAsync()"
    
    Asynchronous version of removeAll(): queues the request and returns at once.
    

### removeAsync<a name="removeAsync"></a>
!!! function "void removeAsync(isize nr)"
    
    Asynchronous version of remove(): queues the request and returns at once.
    

### removeAt<a name="removeAt"></a>
!!! function "void removeAt(u32 target)"
    
     Deletes a guard.
     
    :material-location-enter: `target`
    :     The target of the guard to be deleted.
    

### removeAtAsync<a name="removeAtAsync"></a>
!!! function "void removeAtAsync(u32 target)"
    
    Asynchronous version of removeAt(): queues the request and returns at once.
    

### setAt<a name="setAt"></a>
!!! function "void setAt(u32 target, isize ignores = 0)"
    
     Sets a guard.
     
    :material-location-enter: `target`
    :     The observed memory address (the target of the guard).
     
    :material-location-enter: `ignores`
    :    If a value greater zero is given, the guard has to be hit the specified number of times until program execution is paused.
    

### setAtAsync<a name="setAtAsync"></a>
!!! function "void setAtAsync(u32 target, isize ignores = 0)"
    
    Asynchronous version of setAt(): queues the request and returns at once.
    

### toggle<a name="toggle"></a>
!!! function "void toggle(isize nr)"
    
     Enables a disabled guard and disables an enabled one.
     
    :material-location-enter: `nr`
    :         Number of the guard in the guard list
    

### toggleAsync<a name="toggleAsync"></a>
!!! function "void toggleAsync(isize nr)"
    
    Asynchronous version of toggle(): queues the request and returns at once.
    

