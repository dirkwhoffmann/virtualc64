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

#include "VC64Core/EmulatorTypes.h"

// Foundation
#include "VC64Core/Base/CmdQueueTypes.h"
#include "VC64Core/Base/CoreComponentTypes.h"
#include "VC64Core/Base/ErrorTypes.h"
#include "VC64Core/Base/MsgQueueTypes.h"
#include "VC64Core/Base/OptionTypes.h"
#include "VC64Core/Base/ThreadTypes.h"

// Components
#include "VC64Core/Components/C64Types.h"
#include "VC64Core/Components/CIA/CIATypes.h"
#include "VC64Core/Components/CPU/CPUTypes.h"
#include "VC64Core/Components/Memory/MemoryTypes.h"
#include "VC64Core/Components/SID/SIDTypes.h"
#include "VC64Core/Components/VICII/VICIITypes.h"

// Ports
#include "VC64Core/Ports/AudioPortTypes.h"
#include "VC64Core/Ports/ControlPortTypes.h"
#include "VC64Core/Ports/ExpansionPortTypes.h"
#include "VC64Core/Ports/PowerPortTypes.h"
#include "VC64Core/Ports/UserPortTypes.h"

// Peripherals
#include "VC64Core/Peripherals/Datasette/DatasetteTypes.h"
#include "VC64Core/Peripherals/Drive/DriveTypes.h"
#include "VC64Core/Peripherals/Drive/DiskTypes.h"
#include "VC64Core/Peripherals/Drive/DiskAnalyzerTypes.h"
#include "VC64Core/Peripherals/Joystick/JoystickTypes.h"
#include "VC64Core/Peripherals/Keyboard/KeyboardTypes.h"
#include "VC64Core/Peripherals/Monitor/MonitorTypes.h"
#include "VC64Core/Peripherals/Mouse/MouseTypes.h"
#include "VC64Core/Peripherals/Paddle/PaddleTypes.h"
#include "VC64Core/Peripherals/Drive/ParallelCableTypes.h"

// Media
#include "VC64Core/Media/MediaFileTypes.h"
#include "VC64Core/Media/Cartridges/CartridgeTypes.h"
#include "VC64Core/FileSystems/FSTypes.h"

// Miscellaneous
#include "VC64Core/Misc/Recorder/RecorderTypes.h"
#include "VC64Core/Misc/RemoteServers/RemoteManagerTypes.h"
#include "VC64Core/Misc/RemoteServers/RemoteServerTypes.h"
#include "VC64Core/Misc/RetroShell/RetroShellTypes.h"

#include "Constants.h"
