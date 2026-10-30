# Building the Core {#guide_build}

The emulator core lives in the `VCCore` directory of the repository. It is a plain C++20 library with a CMake build. The macOS application uses the same sources through the Xcode project.

## Requirements

- A C++20 compiler: GCC, Clang, or Visual Studio (MSVC).
- CMake 3.20 or newer.
- A threading library (POSIX threads on Linux and macOS, found automatically by CMake).
- Optionally zlib. If CMake finds it, the core is built with support for compressed files (gzip). Without zlib, the build still succeeds and the compressed formats are unavailable.

The compiler options are strict: the sources are compiled with `-Wall -Werror` (GCC and Clang) or `/W4 /WX` (MSVC), so a warning fails the build.

## Targets

`VCCore/CMakeLists.txt` defines two targets and pulls in the libraries they depend on:

| Target | Description |
|--------|-------------|
| `VCCore` | The emulator core as a library. It links the utility library `utlib`, the retro library `rvlib` (disk images, file systems, ROMs), the SID emulation `resid`, and the threading library. |
| `VC64Headless` | A small command-line app that is used for smoke tests (see @ref guide_headless "Headless App"). |

If you do not set a build type, CMake builds the Release configuration.

## Building on Linux and macOS

Configure, build, and run the self tests from the root of the repository:

```
cmake -B build -S VCCore
cmake --build build
ctest --test-dir build --verbose
```

To build a Debug version, pass `-DCMAKE_BUILD_TYPE=Debug` to the first command. To choose a compiler, pass `-DCMAKE_CXX_COMPILER=clang++` (or `g++`) and the matching `-DCMAKE_C_COMPILER`.

## Building on Windows

With Visual Studio, use the compiler detected by CMake and select the configuration when building:

```
cmake -B build -S VCCore
cmake --build build --config Release
ctest --test-dir build --build-config Release --verbose
```

With MinGW (MSYS2), use the Ninja generator:

```
cmake -G Ninja -B build -S VCCore -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The headless app additionally links `ws2_32` on Windows. CMake takes care of that.

## Self tests

`ctest` runs the headless app three times:

| Test | Command | What it does |
|------|---------|--------------|
| SelfTest1 | `VC64Headless --verbose --footprint` | Reports the size of the core objects. |
| SelfTest2 | `VC64Headless --verbose --smoke` | Runs a RetroShell script that exercises the emulator. |
| SelfTest3 | `VC64Headless --verbose --diagnose` | Launches the emulator thread and runs the self-diagnosis script. |

The same three commands run on every push on GitHub, for Linux (GCC and Clang), Windows (MSVC), and MinGW, each in the Release and Debug configuration.

## Using the core in your project

The easiest way to use the core is to add it as a subdirectory of your own CMake project:

```
add_subdirectory(path/to/VCCore)
add_executable(myapp main.cpp)
target_link_libraries(myapp VCCore)
```

Linking `VCCore` brings in the other libraries and adds the `VCCore` directory to the include path, so the API header is found with:

```cpp
#include "VirtualC64.h"
```

The class [vc64::VirtualC64](@ref vc64::VirtualC64) is the entry point of the API. The following program creates an emulator instance, configures it, and runs it. The emulator computes frames in a thread of its own, so the main thread is free to do other work.

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

The chapter @ref guide_headless "Headless App" shows a complete program that is used for smoke tests. The [API overview](@ref api_overview) lists all sub-APIs, and the [Architecture](architecture.md) explains which thread may call what.

