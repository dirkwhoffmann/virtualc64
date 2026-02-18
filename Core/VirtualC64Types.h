// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------
/// @file

#pragma once

// Infrastructure
#include "Infrastructure/EmulatorTypes.h"
#include "Infrastructure/CmdQueueTypes.h"
#include "Infrastructure/CoreObjectTypes.h"
#include "Infrastructure/CoreComponentTypes.h"
#include "Infrastructure/ErrorTypes.h"
#include "Infrastructure/MsgQueueTypes.h"
#include "Infrastructure/OptionTypes.h"
#include "Infrastructure/ThreadTypes.h"

// Components
#include "Components/C64Types.h"
#include "Components/CIA/CIATypes.h"
#include "Components/CPU/CPUTypes.h"
#include "Components/Memory/MemoryTypes.h"
#include "Components/SID/SIDTypes.h"
#include "Components/VICII/VICIITypes.h"

// Ports
#include "Ports/AudioPortTypes.h"
#include "Ports/ControlPortTypes.h"
#include "Ports/ExpansionPortTypes.h"
#include "Ports/PowerPortTypes.h"
#include "Ports/UserPortTypes.h"

// Peripherals
#include "Peripherals/Datasette/DatasetteTypes.h"
#include "Peripherals/Drive/DriveTypes.h"
#include "Peripherals/Drive/DiskTypes.h"
#include "Peripherals/Drive/DiskAnalyzerTypes.h"
#include "Peripherals/Joystick/JoystickTypes.h"
#include "Peripherals/Keyboard/KeyboardTypes.h"
#include "Peripherals/Monitor/MonitorTypes.h"
#include "Peripherals/Mouse/MouseTypes.h"
#include "Peripherals/Paddle/PaddleTypes.h"
#include "Peripherals/Drive/ParallelCableTypes.h"

// Media
#include "Media/MediaFileTypes.h"
#include "Media/Cartridges/CartridgeTypes.h"
// #include "FileSystems/FSTypes.h"

// Miscellaneous
#include "Misc/RemoteServers/RemoteManagerTypes.h"
#include "Misc/RemoteServers/RemoteServerTypes.h"
#include "Misc/RetroShell/RetroShellTypes.h"

#include "Constants.h"
