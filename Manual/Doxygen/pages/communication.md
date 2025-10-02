# Communication model

## Overview

The following diagram summarizes how the emulator core interacts with the front-end application. 
 
@image html communication.png

VirtualC64 utilizes the following communication mechanisms:

- **Command queue**

  The command queue is a central entry point for notifying the emulator about external events, such as the user pressing a mouse button. The command queue maintains a ring buffer queuing all incoming commands. In the update phase of the emulator core, all pending commands are processed and deleted from the ring buffer.
    
- **Custom APIs**

 Most components of the C64 (CPU, VICII, CIAs, etc.) provide custom APIs for performing special operations. For example, the VICII API provides a function returning a pointer to the latest stable emulator texture. As a rule of thumb, a custom API function exists for all tasks the command queue cannot accomplish. This comprises all functions returning a value or those that may throw an exception on failure. 
     
- **Message queue**
 
 The core emulator utilizes the message queue to inform the GUI about important events. At startup, the GUI registers a command receiver that processes all incoming messages. For instance, when the GUI requests a snapshot via the command queue, the core will send a message to the GUI once the snapshot is ready for pickup.  

<div class="section_buttons">

| Previous                      |                           Next |
|:------------------------------|-------------------------------:|
| [State Model](state.md)       |               vc64::VirtualC64 |
 
</div>
