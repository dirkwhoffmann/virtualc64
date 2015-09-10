## About

VirtualC64 emulates a Commodore 64 personal computer on your Macintosh. I started the project in 2006 to create a virtual CPU environment that can be used as a demonstrator program in a first year or second year course on computer engineering. Over the years, I have added more and more features and emulation accuracy has increased steadily. Although the emulator still does not reach the awesome compatibility of VICE, VirtualC64 can call itself a full-grown Commodore 64 emulator by now without hesitation. 

The VirtualC64 project is based on three major goals. First of all, I am trying to achieve high compatibility by emulating all components cycle accurately. Secondly, I want to create a fun-to-use emulator that presents itself as user-friendly as it can get. Last but not least I am trying to maintain a well-documented and easy-to-read codebase that makes it easy to add new features. If you agree with VirtualC64's philosophy, you are welcome to join the project. The core emulator is written in C++ and the graphical user interface is a native Cocoa application written in Objective-C. Please note that I don't want to port the emulator to any other operating system yet. VirtualC64 is open source software published under the GNU general public license.

Dirk Hoffmann, 2015

## Source code organization

### Top-level directory structure

C64 : Contains the core emulator, written in C++. The code is meant to be architecture independent. 
OSX : Contains everything related to the OS X version
      The GUI code is located at OSX/MacGUI
    
### Starting points

To understand how the core emulator works, start looking at function run() in class C64.
To understand how the OS X GUI code works, start looking a method processMessage(...) in class MyController. It contains the message parsing code that periodically gets updated from the core emulator which runs in a separate thread. The emulator lifts off when message MSG_ROM_COMPLETE arrives.

