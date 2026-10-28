---
generator: doxide
---


# API

**`class API`**

File system format of a disk (see retro::vault::cbm::FSFormat)
Base class of all APIs

 Provides the suspend / resume mechanism that every API class inherits.


## Functions

| Name | Description |
| ---- | ----------- |
| [suspend](#suspend) |  Suspends the emulator thread Blocks until the emulator thread has stopped at a safe point, i.e., it is neither computing a frame nor processing its command queue. The call can be nested: the thread continues once resume() has been called as often as suspend(). Use it to read or modify the state of a component that is not protected by the API itself. Only one thread can hold a suspension at a time. Another thread that calls suspend(), or a blocking API function that waits for the emulator thread, waits until the holder has resumed the emulator. The holder itself must not call such a function, because it would wait for itself: they throw a CoreError, and so does halt(). Do not call suspend() from the emulator thread.  |
| [resume](#resume) |  Resumes the emulator thread after a call to suspend()  |

## Function Details

### resume<a name="resume"></a>
!!! function "void resume() const"
    
     Resumes the emulator thread after a call to suspend()
    

### suspend<a name="suspend"></a>
!!! function "void suspend() const"
    
     Suspends the emulator thread
    
     Blocks until the emulator thread has stopped at a safe point, i.e.,
     it is neither computing a frame nor processing its command queue. The
     call can be nested: the thread continues once resume() has been called
     as often as suspend(). Use it to read or modify the state of a
     component that is not protected by the API itself.
    
     Only one thread can hold a suspension at a time. Another thread that
     calls suspend(), or a blocking API function that waits for the
     emulator thread, waits until the holder has resumed the emulator.
     The holder itself must not call such a function, because it would wait
     for itself: they throw a CoreError, and so does halt(). Do not call
     suspend() from the emulator thread.
    

