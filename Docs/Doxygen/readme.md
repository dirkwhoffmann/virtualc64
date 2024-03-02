# Welcome

Welcome to the official VirtualC64 documentation. Here you will find a detailed view of VirtualC64's public API. It summarizes the necesarry information for developers to incoorporate the emulation core in own projects. For more information about VirtualC64 plase visit the [official website](https://dirkwhoffmann.github.io/virtualc64/).

\image html ad-vc.png width=400px

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

For a runnable demo program, please see the Headless application in the main folder. It creates an emulator instance and utilizes it to perform some simple smoke tests.

## Where to go from here? 

Start with the VirtualC64 class which is provides the public API.