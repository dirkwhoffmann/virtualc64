---
title: "VirtualC64 API Documentation"
kind: page
---
# VirtualC64 API Documentation {#virtualc64apidocumentation}

This manual describes the public API of the VirtualC64 emulation core, i.e., everything a front-end needs to embed the emulator into an application. For general information about VirtualC64, visit the [official website](https://dirkwhoffmann.github.io/virtualc64/).

![VirtualC64 (light mode)](vc-light.png)![VirtualC64 (dark mode)](vc-dark.png)

![](https://img.shields.io/github/watchers/dirkwhoffmann/virtualC64?style=social)![](https://img.shields.io/github/stars/dirkwhoffmann/virtualC64?style=social)![](https://img.shields.io/github/forks/dirkwhoffmann/virtualC64?style=social)

![](https://img.shields.io/github/languages/code-size/dirkwhoffmann/virtualc64)![](https://img.shields.io/github/repo-size/dirkwhoffmann/virtualc64)

![](https://github.com/dirkwhoffmann/virtualc64/actions/workflows/cmake-multi-platform.yml/badge.svg)![](https://github.com/dirkwhoffmann/virtualc64/actions/workflows/cmake-mingw.yml/badge.svg)

## Overview
The emulator core is written in C++20 and designed to be portable. All of its functionality is reached through a single class, [vc64::VirtualC64](/reference/vc64-VirtualC64.md#virtualc64). The class contains one sub-API per hardware component, for example `cpu`, `vicii`, `sid`, `keyboard`, or `drive8`. All sub-APIs share the same conventions for threading and error handling, which are described in the chapters of this manual.

## Example
The following program creates an emulator instance, configures it, and runs it. The emulator computes frames in a thread of its own, so the main thread is free to do other work.

```cpp
#include "VirtualC64.h"

using namespace vc64;

// Receives all messages sent by the emulator. Note that the function is
// called on the emulator thread.
static void process(const void *listener, Message msg, std::exception_ptr error)
{
    // Forward the message to the user thread (not shown)
}

int main()
{
    // Create an emulator instance
    VirtualC64 c64;

    // Launch the emulator thread
    c64.launch(nullptr, process);

    // Install the ROMs. Here, we use the MEGA65 OpenROMs.
    c64.c64.installOpenRoms();

    // Configure the emulator
    c64.set(Opt::VICII_REVISION, long(VICIIRev::PAL_6569_R3));

    // Start the emulator
    c64.run();

    // Interact with the emulator, e.g., by typing a string
    c64.keyboard.autoType("LOAD \"*\",8,1\n");

    // ...
}
```

## Where to go from here

* [API Overview](/guide/api-overview.md) shows how the API is organized and links to all API classes. Start here to look up a function.
* [Threading and Errors](/guide/threading.md) explains which thread may call what, how failures are reported, and how the blocking and asynchronous flavors of the API functions differ.
* [State Model](/guide/state.md) describes the life cycle of the emulator thread.
* [Communication Model](/guide/communication.md) gives an overview of the command queue, the message queue, and the way data is read back from the emulator.
* [Headless App](/guide/headless.md) walks through a small command-line application that uses the API.
* Class [vc64::VirtualC64](/reference/vc64-VirtualC64.md#virtualc64) is the entry point of the API reference.

