Welcome to the official VirtualC64 documentation. Here you will find a detailed view of VirtualC64's public API. It summarizes the necesarry information for developers to incoorporate the emulation core in own projects. For more information about VirtualC64 plase visit the [official website](https://dirkwhoffmann.github.io/virtualc64/).

\image html ad-vc.png width=400px

![Watchers](https://img.shields.io/github/watchers/dirkwhoffmann/virtualC64?style=social)
![Stars](https://img.shields.io/github/stars/dirkwhoffmann/virtualC64?style=social)
![Forks](https://img.shields.io/github/forks/dirkwhoffmann/virtualC64?style=social)

![Build](https://github.com/dirkwhoffmann/virtualC64/workflows/CMake/badge.svg)
![Build](https://github.com/dirkwhoffmann/virtualC64/workflows/CMake-MinGW/badge.svg)
![MSVC](https://github.com/dirkwhoffmann/virtualC64/workflows/CMake-MSVC/badge.svg)

## Example

VirtualC64 is implemented in C++20 with high portability in mind. The following code snippet demonstrates the general usage model of the emulator core.

```cpp
#include "VirtualC64.h"

int main(int argc, char *argv[]) 
{

	// Create an emulator instance
	VirtualC64 c64;

	// Launch the emulator thread (with messageReceiver defined elsewhere)
	c64.launch(this, messageReceiver);

	// Configure the emulator
	c64.set(OPT_VICII_REVISION, VICII_PAL_6569_R3);

	// Start the emulator
	c64.run() 

	//
	// Interact with the emulator
	// 
}
```

## See also

- Class [VirtualC64](#vc64::VirtualC64) which provides the public API.

<div class="section_buttons">

|                        Next |
|----------------------------:|
| [Headless App](headless.md) |
 
</div>
