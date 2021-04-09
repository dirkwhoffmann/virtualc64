// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Interpreter.h"
#include "RetroShell.h"

void
Interpreter::registerInstructions()
{
    //
    // Commands
    //
    
    root.add({"clear"},
             "command", "Clears the console window",
             &RetroShell::exec <Token::clear>);
    root.seek("clear")->hidden = true;

    root.add({"close"},
             "command", "Hides the debug console",
             &RetroShell::exec <Token::close>);
    root.seek("close")->hidden = true;
    
    root.add({"joshua"},
             "command", "",
             &RetroShell::exec <Token::easteregg>);
    root.seek("joshua")->hidden = true;

    root.add({"source"},
             "command", "Processes a command script",
             &RetroShell::exec <Token::source>, 1);
    
    
    //
    // C64
    //
    
    root.add({"c64"},
             "component", "The virtual Commodore 64");
        
    root.add({"c64", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::c64, Token::config>);

    root.add({"c64", "power"},
             "command", "Switches the C64 on or off");
    
    root.add({"c64", "power", "on"},
             "state", "Switches the C64 on",
             &RetroShell::exec <Token::c64, Token::power, Token::on>);

    root.add({"c64", "power", "off"},
             "state", "Switches the C64 off",
             &RetroShell::exec <Token::c64, Token::power, Token::off>);

    root.add({"c64", "run"},
             "command", "Starts the emulator thread",
             &RetroShell::exec <Token::c64, Token::run>);

    root.add({"c64", "pause"},
             "command", "Halts the emulator thread",
             &RetroShell::exec <Token::c64, Token::pause>);
    
    root.add({"c64", "reset"},
             "command", "Performs a hard reset",
             &RetroShell::exec <Token::c64, Token::reset>);
    
    root.add({"c64", "inspect"},
             "command", "Displays the component state",
             &RetroShell::exec <Token::c64, Token::inspect>);

    
    //
    // Memory
    //
    
    root.add({"memory"},
             "component", "Ram and Rom");
    
    root.add({"memory", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::memory, Token::config>);

    root.add({"memory", "set"},
             "command", "Configures the component");
        
    root.add({"memory", "set", "raminit"},
             "key", "Determines how Ram is initialized on startup",
             &RetroShell::exec <Token::memory, Token::set, Token::raminitpattern>, 1);
    
    root.add({"memory", "load"},
             "command", "Installs a Rom image",
             &RetroShell::exec <Token::memory, Token::load>, 1);

    root.add({"memory", "inspect"},
             "command", "Displays the component state",
             &RetroShell::exec <Token::memory, Token::inspect>);

    
    //
    // CPU
    //
    
    root.add({"cpu"},
             "component", "Motorola 68k CPU");
    
    root.add({"cpu", "inspect"},
             "command", "Displays the component state");

    root.add({"cpu", "inspect", "state"},
             "command", "Displays the current state",
             &RetroShell::exec <Token::cpu, Token::inspect, Token::state>);

    root.add({"cpu", "inspect", "registers"},
             "command", "Displays the current register values",
             &RetroShell::exec <Token::cpu, Token::inspect, Token::registers>);

    
    //
    // CIA
    //
    
    root.add({"cia1"},
             "component", "Complex Interface Adapter 1");

    root.add({"cia2"},
             "component", "Complex Interface Adapter 2");

    root.add({"cia1","cia2"}, {"", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::cia, Token::config>);

    root.add({"cia1","cia2"}, {"", "set"},
             "command", "Configures the component");
        
    root.add({"cia1","cia2"}, {"", "set", "revision"},
             "key", "Selects the emulated chip model",
             &RetroShell::exec <Token::cia, Token::set, Token::revision>, 1);

    root.add({"cia1","cia2"}, {"", "set", "timerbbug"},
             "key", "Enables or disables the timer B hardware bug",
             &RetroShell::exec <Token::cia, Token::set, Token::timerbbug>, 1);

    root.add({"cia1","cia2"}, {"", "inspect"},
             "command", "Displays the component state");

    root.add({"cia1","cia2"}, {"", "inspect", "state"},
             "category", "Displays the current state",
             &RetroShell::exec <Token::cia, Token::inspect, Token::state>);

    root.add({"cia1","cia2"}, {"", "inspect", "registers"},
             "category", "Displays the current register values",
             &RetroShell::exec <Token::cia, Token::inspect, Token::registers>);

    root.add({"cia1","cia2"}, {"", "inspect", "tod"},
             "category", "Displays the state of the TOD clock",
             &RetroShell::exec <Token::cia, Token::inspect, Token::tod>);

    
    //
    // VICII
    //
    
    root.add({"vicii"},
             "component", "Video Interface Controller");
    
    root.add({"vicii", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::vicii, Token::config>);

    root.add({"vicii", "set"},
             "command", "Configures the component");

    root.add({"vicii", "set", "revision"},
             "key", "Selects the emulated chip model",
             &RetroShell::exec <Token::vicii, Token::set, Token::revision>, 1);
    
    root.add({"vicii", "set", "graydotbug"},
             "key", "Enables or disables the gray dot bug",
             &RetroShell::exec <Token::vicii, Token::set, Token::graydotbug>, 1);

    root.add({"vicii", "set", "sscollisions"},
             "key", "Enables or disables sprite-sprite collision detection",
             &RetroShell::exec <Token::vicii, Token::set, Token::sscollisions>, 1);

    root.add({"vicii", "set", "sbcollisions"},
             "key", "Enables or disables sprite-background collision detection",
             &RetroShell::exec <Token::vicii, Token::set, Token::sbcollisions>, 1);
    
    root.add({"vicii", "inspect"},
             "command", "Displays the internal state");

    root.add({"vicii", "inspect", "state"},
             "category", "Displays the current state",
             &RetroShell::exec <Token::vicii, Token::inspect, Token::state>);

    
    //
    // Monitor
    //

    root.add({"monitor"},
             "component", "C64 monitor");

    root.add({"monitor", "set"},
             "command", "Configures the component");

    root.add({"monitor", "set", "palette"},
             "key", "Selects the color palette",
             &RetroShell::exec <Token::monitor, Token::set, Token::palette>, 1);

    root.add({"monitor", "set", "brightness"},
             "key", "Adjusts the monitor brightness",
             &RetroShell::exec <Token::monitor, Token::set, Token::brightness>, 1);

    root.add({"monitor", "set", "contrast"},
             "key", "Adjusts the monitor contrast",
             &RetroShell::exec <Token::monitor, Token::set, Token::contrast>, 1);

    root.add({"monitor", "set", "saturation"},
             "key", "Adjusts the color saturation",
             &RetroShell::exec <Token::monitor, Token::set, Token::saturation>, 1);

    
    //
    // SID
    //

    root.add({"sid"},
             "component", "Sound Interface Device");

    root.add({"sid", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::sid, Token::config>);

    root.add({"sid", "set"},
             "command", "Configures the component");

    root.add({"sid", "set", "engine"},
             "key", "Selects the SID backend",
             &RetroShell::exec <Token::sid, Token::set, Token::engine>, 1);

    root.add({"sid", "set", "revision"},
             "key", "Selects the emulated chip model",
             &RetroShell::exec <Token::sid, Token::set, Token::revision>, 1);

    root.add({"sid", "set", "sampling"},
             "key", "Selects the sampling method",
             &RetroShell::exec <Token::sid, Token::set, Token::sampling>, 1);

    root.add({"sid", "set", "filter"},
             "key", "Configures the audio filter",
             &RetroShell::exec <Token::sid, Token::set, Token::filter>, 1);

    root.add({"sid", "set", "volume"},
             "command", "Sets the volume");

    root.add({"sid", "set", "volume", "channel0"},
             "key", "Sets the volume for the first SID",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 1, 0);

    root.add({"sid", "set", "volume", "channel1"},
             "key", "Sets the volume for the second SID",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 1, 1);
    
    root.add({"sid", "set", "volume", "channel2"},
             "key", "Sets the volume for the third SID",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 1, 2);
    
    root.add({"sid", "set", "volume", "channel3"},
             "key", "Sets the volume for the fourth SID",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 1, 3);

    root.add({"sid", "set", "volume", "left"},
             "key", "Sets the master volume for the left speaker",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 1, 4);
    
    root.add({"sid", "set", "volume", "right"},
             "key", "Sets the master volume for the right speaker",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 1, 5);

    root.add({"sid", "set", "pan"},
             "key", "Sets the pan for one of the four SIDs");
    
    root.add({"sid", "set", "pan", "channel0"},
             "key", "Sets the pan for the first SID",
             &RetroShell::exec <Token::sid, Token::set, Token::pan>, 1, 0);
    
    root.add({"sid", "set", "pan", "channel1"},
             "key", "Sets the pan for the second SID",
             &RetroShell::exec <Token::sid, Token::set, Token::pan>, 1, 1);
    
    root.add({"sid", "set", "pan", "channel2"},
             "key", "Sets the pan for the third SID",
             &RetroShell::exec <Token::sid, Token::set, Token::pan>, 1, 2);
    
    root.add({"sid", "set", "pan", "channel3"},
             "key", "Sets the pan for the fourth SID",
             &RetroShell::exec <Token::sid, Token::set, Token::pan>, 1, 3);
    
    root.add({"sid", "inspect"},
             "command", "Displays the internal state");

    root.add({"sid", "inspect", "state"},
             "category", "Displays the current state of a single SID instance",
             &RetroShell::exec <Token::sid, Token::inspect, Token::state>, 1);

    root.add({"sid", "inspect", "registers"},
             "category", "Displays the registers of a single SID instance",
             &RetroShell::exec <Token::sid, Token::inspect, Token::registers>, 1);
    
    
    //
    // Control port
    //
    
    root.add({"controlport1"},
             "component", "Control port 1");
    
    root.add({"controlport2"},
             "component", "Control port 2");
    
    root.add({"controlport1", "controlport2"}, {"", "inspect"},
             "command", "Displays the internal state",
             &RetroShell::exec <Token::controlport, Token::inspect>);
    

    //
    // Keyboard
    //

    root.add({"keyboard"},
             "component", "Keyboard");

    root.add({"keyboard", "inspect"},
             "command", "Displays the internal state",
             &RetroShell::exec <Token::keyboard, Token::inspect>);

    
    //
    // Mouse
    //

    /*
    root.add({"mouse"},
             "component", "Mouse");

    root.add({"mouse", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::mouse, Token::config>);
    
    root.add({"mouse", "set"},
             "command", "Configures the component");
        
    root.add({"mouse", "set", "pullup"},
             "key", "Enables or disables the emulation of pull-up resistors",
             &RetroShell::exec <Token::mouse, Token::set, Token::pullup>, 1);

    root.add({"mouse", "set", "shakedetector"},
             "key", "Enables or disables the shake detector",
             &RetroShell::exec <Token::mouse, Token::set, Token::shakedetector>, 1);

    root.add({"mouse", "set", "velocity"},
             "key", "Sets the horizontal and vertical mouse velocity",
             &RetroShell::exec <Token::mouse, Token::set, Token::velocity>, 1);

    root.add({"mouse", "inspect"},
             "command", "Displays the internal state",
             &RetroShell::exec <Token::mouse, Token::inspect>);
    
    
    //
    // Serial port
    //
    
    root.add({"serial"},
             "component", "Serial port");

    root.add({"serial", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::serial, Token::config>);

    root.add({"serial", "set"},
             "command", "Configures the component");
        
    root.add({"serial", "set", "device"},
             "key", "",
             &RetroShell::exec <Token::serial, Token::set, Token::device>, 1);

    root.add({"serial", "inspect"},
             "command", "Displays the internal state",
             &RetroShell::exec <Token::serial, Token::inspect>);

    
    //
    // Disk controller
    //
    
    root.add({"diskcontroller"},
             "component", "Disk Controller");

    root.add({"diskcontroller", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::dc, Token::config>);

    root.add({"diskcontroller", "set"},
             "command", "Configures the component");
        
    root.add({"diskcontroller", "set", "speed"},
             "key", "Configures the drive speed",
             &RetroShell::exec <Token::dc, Token::speed>, 1);

    root.add({"diskcontroller", "dsksync"},
             "command", "Secures the DSKSYNC register");

    root.add({"diskcontroller", "dsksync", "auto"},
             "key", "Always receive a SYNC signal",
             &RetroShell::exec <Token::dc, Token::dsksync, Token::autosync>, 1);

    root.add({"diskcontroller", "dsksync", "lock"},
             "key", "Prevents writes to DSKSYNC",
             &RetroShell::exec <Token::dc, Token::dsksync, Token::lock>, 1);
        
    root.add({"diskcontroller", "inspect"},
             "command", "Displays the internal state",
             &RetroShell::exec <Token::dc, Token::inspect>);


    //
    // Df0, Df1, Df2, Df3
    //
    
    root.add({"df0"},
             "component", "Floppy drive 0");

    root.add({"df1"},
             "component", "Floppy drive 1");

    root.add({"df2"},
             "component", "Floppy drive 2");

    root.add({"df3"},
             "component", "Floppy drive 3");

    root.add({"dfn"},
             "component", "All connected drives");

    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::dfn, Token::config>);

    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "connect"},
             "command", "Connects the drive",
             &RetroShell::exec <Token::dfn, Token::connect>);
    root.seek("df0")->remove("connect");

    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "disconnect"},
             "command", "Disconnects the drive",
             &RetroShell::exec <Token::dfn, Token::disconnect>);
    root.seek("df0")->remove("disconnect");

    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "eject"},
             "command", "Ejects a floppy disk",
             &RetroShell::exec <Token::dfn, Token::eject>);

    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "insert"},
             "command", "Inserts a floppy disk",
             &RetroShell::exec <Token::dfn, Token::insert>, 1);

    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "set"},
             "command", "Configures the component");
    
    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "set", "model"},
             "key", "Selects the drive model",
             &RetroShell::exec <Token::dfn, Token::set, Token::model>, 1);
    
    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "set", "mechanics"},
             "key", "Enables or disables the emulation of mechanical delays",
             &RetroShell::exec <Token::dfn, Token::set, Token::mechanics>, 1);
    
    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "set", "searchpath"},
             "key", "Sets the search path for media files",
             &RetroShell::exec <Token::dfn, Token::set, Token::searchpath>, 1);
    
    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "set", "defaultfs"},
             "key", "Determines the default file system type for blank disks",
             &RetroShell::exec <Token::dfn, Token::set, Token::defaultfs>, 1);
    
    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "set", "defaultbb"},
             "key", "Determines the default boot block type for blank disks",
             &RetroShell::exec <Token::dfn, Token::set, Token::defaultbb>, 1);
    
    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "set", "pan"},
             "key", "Sets the pan for drive sounds",
             &RetroShell::exec <Token::dfn, Token::set, Token::pan>, 1);
    
    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "audiate"},
             "command", "Sets the volume of drive sounds",
             &RetroShell::exec <Token::dfn, Token::set, Token::mechanics>);
    
    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "audiate", "insert"},
             "command", "Makes disk insertions audible",
             &RetroShell::exec <Token::dfn, Token::audiate, Token::insert>, 1);
    
    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "audiate", "eject"},
             "command", "Makes disk ejections audible",
             &RetroShell::exec <Token::dfn, Token::audiate, Token::eject>, 1);
    
    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "audiate", "step"},
             "command", "Makes disk ejections audible",
             &RetroShell::exec <Token::dfn, Token::audiate, Token::step>, 1);
    
    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "audiate", "poll"},
             "command", "Makes polling clicks audible",
             &RetroShell::exec <Token::dfn, Token::audiate, Token::poll>, 1);
    
    root.add({"df0", "df1", "df2", "df3", "dfn"}, {"", "inspect"},
             "command", "Displays the internal state",
             &RetroShell::exec <Token::dfn, Token::inspect>);
     */
}
