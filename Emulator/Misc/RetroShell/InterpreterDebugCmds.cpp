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
             [this](Arguments& argv, long value) {

        c64.pause();
    });

    root.add({"continue"},
             "Continues emulation",
             [this](Arguments& argv, long value) {

        c64.run();
    });

    root.add({"step"},
             "Steps into the next instruction",
             [this](Arguments& argv, long value) {

        c64.stepInto();
    });

    root.add({"next"},
             "Steps over the next instruction",
             [this](Arguments& argv, long value) {

        c64.stepOver();
    });

    root.add({"goto"}, { Arg::address },
             "Redirects the program counter",
             [this](Arguments& argv, long value) {

        retroShell << "TODO\n";
        // cpu.jump((u32)parseNum(argv));
    });

    root.add({"disassemble"}, { }, { Arg::address },
             "Runs disassembler",
             [this](Arguments& argv, long value) {

        std::stringstream ss;

        auto addr = argv.empty() ? cpu.getPC0() : u16(parseNum(argv));
        cpu.disassembler.disassembleRange(ss, addr, 16);

        retroShell << '\n' << ss << '\n';
    });


    root.newGroup("Debugging components");

    root.add({"c64"},           "The virtual Commodore 64");
    root.add({"cpu"},           "MOS 6810 CPU");
    root.add({"memory"},        "Ram and Rom");
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

    root.newGroup("");

    root.add({"c64"},
             "Displays the component state");

    root.add({"c64", ""},
             "Inspects the internal state",
             [this](Arguments& argv, long value) {

        retroShell.dump(c64, Category::Inspection);
    });

    root.add({"c64", "progress"},
             "Displays clocks and the frame counter",
             [this](Arguments& argv, long value) {

        retroShell.dump(c64, Category::Progress);
    });

    root.add({"c64", "host"},
             "Displays information about the host machine",
             [this](Arguments& argv, long value) {

        retroShell.dump(host, Category::Inspection);
    });

    root.add({"c64", "debug"},
             "Displays additional debug information",
             [this](Arguments& argv, long value) {

        retroShell.dump(c64, Category::Debug);
    });


    //
    // Memory
    //

    root.add({"memory", ""},
             "Inspects the internal state",
             [this](Arguments& argv, long value) {

        retroShell.dump(mem, Category::Inspection);
    });

    root.add({"memory", "dump"}, { Arg::address },
             "Generates a memory hexdump",
             [this](Arguments& argv, long value) {

        std::stringstream ss;
        mem.memDump(ss, u16(parseNum(argv)));
        retroShell << '\n' << ss << '\n';
    });


    //
    // Drive
    //

    for (isize i = 0; i < 2; i++) {

        string drive = (i == 0) ? "drive8" : "drive9";

        root.add({drive, ""},
                 "Inspects the internal state",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            retroShell.dump(drive, Category::Inspection);
        });

        root.add({drive, "bankmap"},
                 "Displays the memory layout",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            retroShell.dump(drive, Category::BankMap);
        });

        root.add({drive, "disk"},
                 "Inspects the current disk",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            retroShell.dump(drive, Category::Disk);
        });

        root.add({drive, "layout"},
                 "Displays the disk layout",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            retroShell.dump(drive, Category::Layout);
        });

        root.add({drive, "debug"},
                 "Displays additional debug information",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            retroShell.dump(drive, Category::Debug);
        });
    }


    //
    // Datasette
    //


    root.add({"datasette", ""},
             "Inspects the internal state",
             [this](Arguments& argv, long value) {

        retroShell.dump(datasette, Category::State);
    });


    //
    // CPU
    //

    root.add({"cpu", ""},
             "Inspects the internal state",
             [this](Arguments& argv, long value) {

        retroShell.dump(cpu, Category::Inspection);
    });

    root.add({"cpu", "debug"},
             "Displays additional debug information",
             [this](Arguments& argv, long value) {

        retroShell.dump(cpu, Category::Debug);
    });


    //
    // CIA
    //

    for (isize i = 0; i < 2; i++) {

        string cia = (i == 0) ? "cia1" : "cia1";

        root.add({cia, ""},
                 "Displays the component state",
                 [this](Arguments& argv, long value) {

            if (value == 0) {
                retroShell.dump(cia1, Category::Inspection);
            } else {
                retroShell.dump(cia2, Category::Inspection);
            }

        }, i);

        root.add({cia, "debug"},
                 "Displays the current state",
                 [this](Arguments& argv, long value) {

            if (value == 0) {
                retroShell.dump(cia1, Category::Debug);
            } else {
                retroShell.dump(cia2, Category::Debug);
            }

        }, i);

        root.add({cia, "registers"},
                 "Displays the current register values",
                 [this](Arguments& argv, long value) {

            if (value == 0) {
                retroShell.dump(cia1, Category::Registers);
            } else {
                retroShell.dump(cia2, Category::Registers);
            }

        }, i);

        root.add({cia, "tod"},
                 "Displays the state of the TOD clock",
                 [this](Arguments& argv, long value) {

            if (value == 0) {
                retroShell.dump(cia1.tod, Category::State);
            } else {
                retroShell.dump(cia2.tod, Category::State);
            }

        }, i);
    }


    //
    // VICII
    //

    root.add({"vicii", ""},
             "Inspects the internal state",
             [this](Arguments& argv, long value) {

        retroShell.dump(vic, Category::Inspection);
    });

    root.add({"vicii", "registers"},
             "Dumps all VICII registers",
             [this](Arguments& argv, long value) {

        retroShell.dump(vic, Category::Registers);
    });

    root.add({"vicii", "debug"},
             "Displays additional debug information",
             [this](Arguments& argv, long value) {

        retroShell.dump(vic, Category::Debug);
    });


    //
    // SID
    //

    root.add({"sid", ""},
             "Inspects the internal state",
             [this](Arguments& argv, long value) {

        retroShell.dump(muxer, Category::Inspection);
    });

    root.add({"sid", "debug"},
             "Displays additional debug information",
             [this](Arguments& argv, long value) {

        retroShell.dump(muxer, Category::Debug);
    });


    //
    // Control port
    //

    for (isize i = 0; i < 2; i++) {

        string port = (i == 0) ? "controlport1" : "controlport2";

        root.add({port, ""},
                 "Inspects the internal state",
                 [this](Arguments& argv, long value) {

            retroShell.dump(value == 0 ? port1 : port2, Category::Inspection);
        });
    }


    //
    // Expansion port
    //

    root.add({"expansion", ""},
             "Inspects the internal state",
             [this](Arguments& argv, long value) {

        retroShell.dump(expansionport, Category::Inspection);
    });

    root.add({"expansion", "debug"},
             "Displays additional debug information",
             [this](Arguments& argv, long value) {

        retroShell.dump(expansionport, Category::Debug);
    });

    
    //
    // Keyboard
    //

    root.add({"keyboard", ""},
             "Inspects the internal state",
             [this](Arguments& argv, long value) {

        retroShell.dump(keyboard, Category::Inspection);
    });


    //
    // Joystick
    //

    root.add({"joystick", ""},
             "Inspects the internal state",
             [this](Arguments& argv, long value) {

        retroShell.dump(port1.joystick, Category::State);
        retroShell << '\n';
        retroShell.dump(port2.joystick, Category::State);
    });


    //
    // Mouse
    //

    root.add({"mouse", ""},
             "Inspects the internal state",
             [this](Arguments& argv, long value) {

        retroShell.dump(port1.mouse, Category::State);
        retroShell << '\n';
        retroShell.dump(port2.mouse, Category::State);
    });


    //
    // Parallel cable
    //

    root.add({"parcable"},
             "Inspects the internal state",
             [this](Arguments& argv, long value) {

        retroShell.dump(parCable, Category::State);
    });
}


}
