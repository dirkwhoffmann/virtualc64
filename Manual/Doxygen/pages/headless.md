# Headless App {#headless}

The headless app is a small command-line program that performs smoke tests at the end of an automated build chain. It creates an emulator instance, launches the emulator thread, and runs RetroShell scripts through the scripting interface. The program is a good starting point to learn how to embed the emulator core into your project: it shows how to launch the emulator, how to configure it, how to hand it work, and how to process messages in a registered callback.

The interesting part is the function that runs a script. It does the following:

1. Creates the emulator and launches the emulator thread, registering `vc64::process` as message callback.
2. Installs the MEGA65 OpenROMs, so that no original ROMs are needed, and disconnects the floppy drives.
3. Starts the script with \ref vc64::RetroShellAPI::execScriptAsync "execScriptAsync()", which returns immediately.
4. Waits until the callback signals that the script has finished.

The callback runs on the emulator thread. It only records the outcome (`RSH_ERROR` or `RSH_DONE`) and wakes up the waiting main thread.

\snippet Headless.cpp runScript

The message callback consists of a plain function that forwards to a member function of the application object, which was passed as the `listener` argument to \ref vc64::VirtualC64::launch "launch()":

\snippet Headless.cpp process

<div class="section_buttons">

| Previous                                |
|:----------------------------------------|
| [Communication Model](communication.md) |

</div>
