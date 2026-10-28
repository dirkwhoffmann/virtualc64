---
generator: doxide
---


# Emulator

**`class Emulator`**

 Gives access to the internal components

 
!!! warning
 These are advanced interfaces. They return the internal objects
     the APIs operate on, bypassing all synchronization the dedicated API
     functions provide. Use them only while the emulator is suspended or from
     within a function passed to call(). To make use of the objects, include
     the corresponding internal headers. What they offer is not covered by
     any compatibility promise.

 The getters that take a number throw a CoreError
 (CoreError::OPT_INV_ARG) if the number is out of range.


