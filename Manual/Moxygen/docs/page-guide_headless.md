---
title: "Headless App"
kind: page
---
# Headless App {#headlessapp}

The headless app is a small command-line program that performs smoke tests at the end of an automated build chain. It creates an emulator instance, launches the emulator thread, and runs RetroShell scripts through the scripting interface. The program is a good starting point to learn how to embed the emulator core into your project: it shows how to launch the emulator, how to configure it, how to hand it work, and how to process messages in a registered callback.

The interesting part is the function that runs a script. It does the following:

1. Creates the emulator and launches the emulator thread, registering `vc64::process` as message callback.
1. Installs the MEGA65 OpenROMs, so that no original ROMs are needed, and disconnects the floppy drives.
1. Starts the script with [execScriptAsync()](vc64-RetroShellAPI.md#execscriptasync), which returns immediately.
1. Waits until the callback signals that the script has finished.

The callback runs on the emulator thread. It only records the outcome (`RSH_ERROR` or `RSH_DONE`) and wakes up the waiting main thread.

```cpp
void
Headless::runScript(const fs::path &path)
{
    // Read the input script
    // Script script(path);

    // Create an emulator instance
    VirtualC64 c64;

    // Launch the emulator thread
    c64.launch(this, vc64::process);

    // Plug in the three MEGA65 OpenROMs
    c64.c64.installOpenRoms();
    c64.c64.deleteRom(RomType::C64_VC1541);
    c64.setAsync(Opt::DRV_CONNECT, false, 0);
    c64.setAsync(Opt::DRV_CONNECT, false, 1);

    // Redirect shell output to the console in verbose mode
    if (keys.find("verbose") != keys.end()) c64.retroShell.setStream(std::cout);

    // Execute script
    const auto timeout = utl::Time::seconds(500.0);
    c64.retroShell.execScriptAsync(path);
    waitForWakeUp(timeout);
}
```

The message callback consists of a plain function that forwards to a member function of the application object, which was passed as the `listener` argument to [launch()](vc64-VirtualC64.md#launch):

```cpp
void
process(const void *listener, Message msg, std::exception_ptr)
{
    ((Headless *)listener)->process(msg);
}

void
Headless::process(Message msg)
{
    static bool messages = keys.find("messages") != keys.end();
    
    if (messages) {
        
        std::cout << MsgEnum::key(msg.type);
        std::cout << "(" << msg.value << ")";
        std::cout << std::endl;
    }

    switch (msg.type) {

        case Msg::RSH_ERROR:

            returnCode = 1;
            wakeUp();
            break;

        case Msg::RSH_DONE:

            // The script runs in the main shell (id 0)
            if (msg.value == 0) wakeUp();
            break;

        case Msg::ABORT:

            wakeUp();
            break;

        default:
            break;
    }
}
```

