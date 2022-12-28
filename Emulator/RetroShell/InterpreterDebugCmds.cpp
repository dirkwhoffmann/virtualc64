// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "RetroShell.h"
#include "C64.h"
#include "IOUtils.h"
#include "Parser.h"

#include <fstream>
#include <sstream>

namespace vc64 {

void
Interpreter::initDebugShell(Command &root)
{
    initCommons(root);

    //
    // Top-level commands
    //

    root.newGroup("Controlling the instruction stream");

    root.add({"pause"},
             "Pauses emulation",
             &RetroShell::exec <Token::pause>);

    root.add({"continue"},
             "Continues emulation",
             &RetroShell::exec <Token::cont>);

    root.add({"step"},
             "Steps into the next instruction",
             &RetroShell::exec <Token::step>);

    root.add({"next"},
             "Steps over the next instruction",
             &RetroShell::exec <Token::next>);

    root.add({"goto"}, { Arg::address },
             "Redirects the program counter",
             &RetroShell::exec <Token::jump>);

    root.add({"disassemble"}, { Arg::address },
             "Runs disassembler",
             &RetroShell::exec <Token::disassemble>);


    root.newGroup("Debugging components");

    root.add({"c64"},           "The virtual Commodore 64");
    root.add({"memory"},        "Ram and Rom");
    root.add({"cpu"},           "MOS 6510 CPU");
    root.add({"cia1"},          "Complex Interface Adapter 1");
    root.add({"cia2"},          "Complex Interface Adapter 2");
    root.add({"vicii"},         "Video Interface Controller");
    root.add({"sid"},           "Sound Interface Device");

    root.newGroup("Debugging ports");

    root.add({"controlport1"},  "Control port 1");
    root.add({"controlport2"},  "Control port 2");
    root.add({"expansion"},     "Expansion port");

    root.newGroup("Debugging peripherals");

    root.add({"keyboard"},      "Keyboard");
    root.add({"mouse"},         "mouse");
    root.add({"joystick"},      "Joystick");
    root.add({"datasette"},     "Commodore tape drive");
    root.add({"drive8"},        "Floppy drive 8");
    root.add({"drive9"},        "Floppy drive 9");
    root.add({"parcable"},      "Parallel drive cable");


    //
    // C64
    //

    root.add({"c64"},
             "Displays the component state");

    root.add({"c64", ""},
             "Inspects the internal state",
             &RetroShell::exec <Token::c64, Token::inspect>);

    root.add({"c64", ""},
             "Displays additional debug information",
             &RetroShell::exec <Token::c64, Token::debug>);


    //
    // Memory
    //

    root.add({"memory", "inspect"},
             "Displays the component state",
             &RetroShell::exec <Token::memory, Token::inspect>);


    //
    // Drive
    //

    for (isize i = 0; i < 2; i++) {

        string drive = (i == 0) ? "drive8" : "drive9";

        root.add({drive, "inspect"},
                 "Displays the component state");

        root.add({drive, "inspect", "state"},
                 "Displays the drive state",
                 &RetroShell::exec <Token::drive, Token::inspect, Token::state>);

        root.add({drive, "inspect", "bankmap"},
                 "Displays the memory layout",
                 &RetroShell::exec <Token::drive, Token::inspect, Token::bankmap>);

        root.add({drive, "inspect", "disk"},
                 "Displays the disk state",
                 &RetroShell::exec <Token::drive, Token::inspect, Token::disk>);
    }


    //
    // Datasette
    //


    root.add({"datasette", "inspect"},
             "Displays the component state",
             &RetroShell::exec <Token::datasette, Token::inspect>);


    //
    // CPU
    //

    root.add({"cpu", "inspect"},
             "Displays the component state");

    root.add({"cpu", "inspect", "state"},
             "Displays the current state",
             &RetroShell::exec <Token::cpu, Token::inspect, Token::state>);

    root.add({"cpu", "inspect", "registers"},
             "Displays the current register values",
             &RetroShell::exec <Token::cpu, Token::inspect, Token::registers>);


    //
    // CIA
    //

    for (isize i = 0; i < 2; i++) {

        string cia = (i == 0) ? "cia1" : "cia1";

        root.add({cia, "inspect"},
                 "Displays the component state");

        root.add({cia, "inspect", "state"},
                 "Displays the current state",
                 &RetroShell::exec <Token::cia, Token::inspect, Token::state>);

        root.add({cia, "inspect", "registers"},
                 "Displays the current register values",
                 &RetroShell::exec <Token::cia, Token::inspect, Token::registers>);

        root.add({cia, "inspect", "tod"},
                 "Displays the state of the TOD clock",
                 &RetroShell::exec <Token::cia, Token::inspect, Token::tod>);
    }


    //
    // VICII
    //

    root.add({"vicii", "inspect"},
             "Displays the internal state");

    root.add({"vicii", "inspect", "registers"},
             "Displays the register contents",
             &RetroShell::exec <Token::vicii, Token::inspect, Token::registers>);

    root.add({"vicii", "inspect", "state"},
             "Displays the current state",
             &RetroShell::exec <Token::vicii, Token::inspect, Token::state>);


    //
    // SID
    //

    root.add({"sid", "inspect"},
             "Displays the internal state");

    root.add({"sid", "inspect", "state"}, { Arg::value },
             "Displays the current state of a single SID instance",
             &RetroShell::exec <Token::sid, Token::inspect, Token::state>);

    root.add({"sid", "inspect", "registers"},
             "Displays the registers of a single SID instance",
             &RetroShell::exec <Token::sid, Token::inspect, Token::registers>);


    //
    // Control port
    //

    for (isize i = 0; i < 2; i++) {

        string port = (i == 0) ? "controlport1" : "controlport2";

        root.add({port, "inspect"},
                 "Displays the internal state",
                 &RetroShell::exec <Token::controlport, Token::inspect>);
    }


    //
    // Expansion port
    //

    root.add({"expansion", "inspect"},
             "Displays the internal state",
             &RetroShell::exec <Token::expansion, Token::inspect>);


    //
    // Keyboard
    //

    root.add({"keyboard", "inspect"},
             "Displays the internal state",
             &RetroShell::exec <Token::keyboard, Token::inspect>);


    //
    // Joystick
    //

    root.add({"joystick", "inspect"},
             "Displays the internal state",
             &RetroShell::exec <Token::joystick, Token::inspect>);


    //
    // Mouse
    //

    root.add({"mouse", "inspect"},
             "Displays the internal state",
             &RetroShell::exec <Token::mouse, Token::inspect>);


    //
    // Parallel cable
    //

    root.add({"parcable", "inspect"},
             "Displays the internal state",
             &RetroShell::exec <Token::parcable, Token::inspect>);
}


}
