![alt Logo](http://www.dirkwhoffmann.de/virtualC64/pics/banner.png)

## About

VirtualC64 emulates a Commodore 64 on the Mac. Originally, the emulator was intended as a demonstration of a virtual CPU environment that can be used in a one-year or two-year course on computer technology. Over the years, the emulator has grown steadily and its emulation accuracy has increased continuously. Although the emulator has not yet reached the impressive compatibility of VICE, VirtualC64 has evolved into a full-blown C64 emulator over the years.  
 
VirtualC64 has three goals. In addition to achieving a high level of compatibility, I want to create a user interface that is as easy to use as possible. Thirdly, I am trying to create a well-documented and understandable code base. If you can identify with these goals, I would be happy to actively participate in this project. Please note that I don't want to port the emulator to another operating system at the moment. 

VirtualC64 is open source and released under the GNU General Public License.

Dirk Hoffmann, 2018
www.dirkwhoffmann.de/virtualC64

## Source code organization

### Top-level directory structure

C64 : Contains the core emulator, written in C++. The code is meant to be architecture independent. 
OSX : Contains everything related to the OS X version. The GUI code is located in sub directory MacGUI

### Overall architecture

VirtualC64 consists of three major components:

1. The graphical user interface (written in Swift and ObjC)
2. The proxy (written in ObjC)
3. The core emulator (written in C++)

The GUI talks to VirtualC64 by calling proxy methods. VirtualC64 talks back via
a message queue that triggers a callback function whenever a message is written into the queue. 

    ------------------------------------------------------------------
    | callback()                                        putMessage() |
    v                                                                |
    ----------------------          ----------------------          ----------------------
    |                    |  func()  |                    |  func()  |                    |
    |        GUI         |--------->|      C64Proxy      |--------->|        C64         |
    |  (Swift and ObjC)  |          | Swift / C++ bridge |          |    (C++ world)     |
    ----------------------          ----------------------          ----------------------


### Initialization procedure

To get VirtualC64 up and running, you need to perform the following steps:

1. Create a C64 object
c64 = new C64()

2. Register a listener callback
c64->setListener(...)

3. Load Roms
c64->loadRom(...)

4. Configure
c64->set...() etc.

5. Start the execution thread
c64->run()


loadRom() is an important function because it not only installs the necessary ROMs to run the emulator, but also triggers an important side-effect. Once the last ROM has been installed, the function writes the READ_TO_RUN message to the message queue. The GUI reacts to this message by calling run(). This function starts the execution thread and brings the emulator to life. 

 
### Message queue

To communicate with the GUI, the emulator writes messages into a message queue. At launch, the GUI will register a callback function that is invoked whenever a new message is written into the queue. In the current implementation, a Swift closure is registered that processes all incoming messages. 

If you use the core emulator in a different environment and do not want register a callback, you can periodically query the queue with code similar to the following Objective-C example code: 

    while ((message = [c64 message]) != NULL) {
        switch (message->id) {
            
            case MSG_READY_TO_RUN:
                [c64 run];
                
                ...
        }
    }

You can register the listener any time, even to a running emulator. At the time the listener is registered, all pending messages are automatically sent to the registered callback. 

### The execution thread

The execution thread is organized as an infinite loop. In each iteration, control is
passed to the VIC, CIAs, CPU, VIAs, and the disk drive. The VIC chip draws the screen
contents into a simple byte array, the so called screen buffer. The asynchronously
running GUI copies the screen buffer contents onto a GPU texture which is then rendered
by the GPU.

The following methods control the execution thread:


run: Runs the emulation thread
 
The GUI invokes this method, e.g., when the user lauches the
emulator the first time or hits the continue button.

halt: Pauses the emulation thread
 
The GUI invokes this method, e.g., when the user hits the pause button.
VirtualC64 itself calls this method when, e.g., a breakpoint is reached.

suspend / result: Temporarily pauses the emulation thread
 
If multiple operations need to be executed atomically (such as
taking an emulator snapshot), the operations are embedded inside a
suspend() / resume() block. Both methods use halt() and run() internally.

powerUp:
 
This method restarts the emulation on a freshly initialized computer.
It has the same effect as switching a real C64 off and on again. Note that a
 few items are retained during a reset, e.g., an attached cartridge.




