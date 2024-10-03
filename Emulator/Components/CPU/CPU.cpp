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

#include "config.h"
#include "PeddleConfig.h"
#include "CPU.h"
#include "Emulator.h"
#include "IOUtils.h"

namespace vc64 {

CPU::CPU(C64& ref) : Peddle(ref)
{

}

CPU::CPU(CPURevision cpuModel, C64& ref) : CPU(ref)
{
    setModel(cpuModel);
}

void
CPU::writePort(u8 val)
{
    Peddle::writePort(val);

    // Check the motor bit of the datasette
    if (reg.pport.direction & 0x20) {
        datasette.setMotor((val & 0x20) == 0);
    }

    // When writing to the port register, the last VICII byte appears
    mem.ram[0x0001] = vic.getDataBusPhi1();

    // Switch memory banks
    mem.updatePeekPokeLookupTables();
}

void
CPU::writePortDir(u8 val)
{
    auto port = reg.pport.data;
    auto direction = reg.pport.direction;

    // If bits 3, 6, or 7 are configured as outputs, they are not floating
    if (GET_BIT(val, 3)) dischargeCycleBit3 = 0;
    if (GET_BIT(val, 6)) dischargeCycleBit6 = 0;
    if (GET_BIT(val, 7)) dischargeCycleBit7 = 0;

    // If bits 3, 6, or 7 change from output to input, they become floating
    if (FALLING_EDGE_BIT(direction, val, 3) && GET_BIT(port, 3) != 0)
        dischargeCycleBit3 = UINT64_MAX;
    if (FALLING_EDGE_BIT(direction, val, 6) && GET_BIT(port, 6) != 0)
        dischargeCycleBit6 = cpu.clock + dischargeCyclesVICE;
    if (FALLING_EDGE_BIT(direction, val, 7) && GET_BIT(port, 7) != 0)
        dischargeCycleBit7 = cpu.clock + dischargeCyclesVICE;

    // Update the port register
    Peddle::writePortDir(val);

    // When writing to the direction register, the last VICII byte appears
    mem.ram[0x0000] = vic.getDataBusPhi1();

    // Switch memory banks
    mem.updatePeekPokeLookupTables();
}

u8
CPU::externalPortBits() const
{
    /* If the port bits are configured as inputs and no datasette is attached,
     * the following values are returned:
     *
     *     Bit 0:  1 (bit is driven by a pull-up resistor)
     *     Bit 1:  1 (bit is driven by a pull-up resistor)
     *     Bit 2:  1 (bit is driven by a pull-up resistor)
     * ??? Bit 3:  Eventually 0 (acts a a capacitor)
     * ??? Bit 3:  0 (bit is driven by a pull-down resistor)
     *     Bit 4:  1 (bit is driven by a pull-up resistor)
     *     Bit 5:  0 (bit is driven by a pull-down resistor)
     *     Bit 6:  Eventually 0 (acts a a capacitor)
     *     Bit 7:  Eventually 0 (acts a a capacitor)
     */
    u8 bit3 = (dischargeCycleBit3 > cpu.clock) ? 0x08 : 0x00;
    u8 bit6 = (dischargeCycleBit6 > cpu.clock) ? 0x40 : 0x00;
    u8 bit7 = (dischargeCycleBit7 > cpu.clock) ? 0x80 : 0x00;
    u8 bit4 = datasette.getPlayKey() ? 0x00 : 0x10;

    return bit7 | bit6 | bit4 | bit3 | 0x07;
}

void
CPU::cpuDidJam()
{
    c64.signalJammed();
}

void
CPU::nmiWillTrigger()
{
    if (isC64CPU()) {
        expansionPort.nmiWillTrigger();
    }
}

void
CPU::nmiDidTrigger()
{
    if (isC64CPU()) {
        expansionPort.nmiDidTrigger();
    }
}

void
CPU::breakpointReached(u16 addr) const
{
    c64.signalBreakpoint();
}

void
CPU::watchpointReached(u16 addr) const
{
    c64.signalWatchpoint();
}

void
CPU::instructionLogged() const
{

}

void
CPU::jumpedTo(u16 addr) const
{
    msgQueue.put(MSG_CPU_JUMPED, CpuMsg { .pc = addr } );
}

void
CPU::jump(u16 addr)
{
    {   SUSPENDED

        debugger.jump(addr);
    }
}

void 
CPU::processCommand(const Cmd &cmd)
{
    switch (cmd.type) {
            
        case CMD_CPU_BRK:
            
            cpu.next = BRK;
            cpu.reg.pc0 = cpu.reg.pc - 1;
            break;
            
        case CMD_CPU_NMI:
            
            if (cmd.value) {
                cpu.pullDownNmiLine(INTSRC_EXP);
            } else {
                cpu.releaseNmiLine(INTSRC_EXP);
            }
            break;
            
        case CMD_BP_SET_AT:         cpu.setBreakpoint(u32(cmd.value)); break;
        case CMD_BP_MOVE_TO:        cpu.moveBreakpoint(isize(cmd.value), u32(cmd.value2)); break;
        case CMD_BP_REMOVE_NR:      cpu.deleteBreakpoint(isize(cmd.value)); break;
        case CMD_BP_REMOVE_AT:      cpu.deleteBreakpointAt(u32(cmd.value)); break;
        case CMD_BP_REMOVE_ALL:     cpu.deleteAllBreakpoints(); break;
        case CMD_BP_ENABLE_NR:      cpu.enableBreakpoint(isize(cmd.value)); break;
        case CMD_BP_ENABLE_AT:      cpu.enableBreakpoint(u32(cmd.value)); break;
        case CMD_BP_ENABLE_ALL:     cpu.enableAllBreakpoints(); break;
        case CMD_BP_DISABLE_NR:     cpu.disableBreakpoint(isize(cmd.value)); break;
        case CMD_BP_DISABLE_AT:     cpu.disableBreakpointAt(u32(cmd.value)); break;
        case CMD_BP_DISABLE_ALL:    cpu.disableAllBreakpoints(); break;

        case CMD_WP_SET_AT:         cpu.setWatchpoint(u32(cmd.value)); break;
        case CMD_WP_MOVE_TO:        cpu.moveWatchpoint(isize(cmd.value), u32(cmd.value2)); break;
        case CMD_WP_REMOVE_NR:      cpu.deleteWatchpoint(isize(cmd.value)); break;
        case CMD_WP_REMOVE_AT:      cpu.deleteWatchpointAt(u32(cmd.value)); break;
        case CMD_WP_REMOVE_ALL:     cpu.deleteAllWatchpoints(); break;
        case CMD_WP_ENABLE_NR:      cpu.enableWatchpoint(isize(cmd.value)); break;
        case CMD_WP_ENABLE_AT:      cpu.enableWatchpoint(u32(cmd.value)); break;
        case CMD_WP_ENABLE_ALL:     cpu.enableAllWatchpoints(); break;
        case CMD_WP_DISABLE_NR:     cpu.disableWatchpoint(isize(cmd.value)); break;
        case CMD_WP_DISABLE_AT:     cpu.disableWatchpoint(u32(cmd.value)); break;
        case CMD_WP_DISABLE_ALL:    cpu.disableAllWatchpoints(); break;

        default:
            fatalError;
    }
}

void
CPU::setBreakpoint(u32 addr, isize ignores)
{
    if (debugger.breakpoints.isSetAt(addr)) throw Error(VC64ERROR_BP_ALREADY_SET, addr);

    debugger.breakpoints.setAt(addr, ignores);
    msgQueue.put(MSG_BREAKPOINT_UPDATED);
}

void 
CPU::moveBreakpoint(isize nr, u32 newAddr)
{
    if (!debugger.breakpoints.guardNr(nr)) throw Error(VC64ERROR_BP_NOT_FOUND, nr);

    debugger.breakpoints.moveTo(nr, newAddr);
    msgQueue.put(MSG_BREAKPOINT_UPDATED);
}

void
CPU::deleteBreakpoint(isize nr)
{
    if (!debugger.breakpoints.guardNr(nr)) throw Error(VC64ERROR_BP_NOT_FOUND, nr);

    debugger.breakpoints.remove(nr);
    msgQueue.put(MSG_BREAKPOINT_UPDATED);
}

void
CPU::deleteBreakpointAt(u32 addr)
{
    if (!debugger.breakpoints.guardAt(addr)) throw Error(VC64ERROR_BP_NOT_FOUND, addr);

    debugger.breakpoints.removeAt(addr);
    msgQueue.put(MSG_BREAKPOINT_UPDATED);
}

void
CPU::deleteAllBreakpoints()
{
    debugger.breakpoints.removeAll();
    msgQueue.put(MSG_BREAKPOINT_UPDATED);
}

void
CPU::toggleBreakpoint(isize nr)
{
    debugger.breakpoints.isEnabled(nr) ? disableBreakpoint(nr) : enableBreakpoint(nr);
}

void 
CPU::setEnableBreakpoint(isize nr, bool value)
{
    if (!debugger.breakpoints.guardNr(nr)) throw Error(VC64ERROR_BP_NOT_FOUND, nr);

    debugger.breakpoints.setEnable(nr, value);
    msgQueue.put(MSG_BREAKPOINT_UPDATED);
}

void 
CPU::setEnableBreakpointAt(u32 addr, bool value)
{
    if (!debugger.breakpoints.guardAt(addr)) throw Error(VC64ERROR_BP_NOT_FOUND, addr);

    debugger.breakpoints.setEnableAt(addr, value);
    msgQueue.put(MSG_BREAKPOINT_UPDATED);
}

void
CPU::setEnableAllBreakpoints(bool value)
{
    debugger.breakpoints.setEnableAll(value);
    msgQueue.put(MSG_BREAKPOINT_UPDATED);
}

void
CPU::setWatchpoint(u32 addr, isize ignores)
{
    if (debugger.watchpoints.isSetAt(addr)) throw Error(VC64ERROR_WP_ALREADY_SET, addr);

    debugger.watchpoints.setAt(addr, ignores);
    msgQueue.put(MSG_WATCHPOINT_UPDATED);
}

void
CPU::moveWatchpoint(isize nr, u32 newAddr)
{
    if (!debugger.watchpoints.guardNr(nr)) throw Error(VC64ERROR_WP_NOT_FOUND, nr);

    debugger.watchpoints.moveTo(nr, newAddr);
    msgQueue.put(MSG_WATCHPOINT_UPDATED);
}

void
CPU::deleteWatchpoint(isize nr)
{
    if (!debugger.watchpoints.guardNr(nr)) throw Error(VC64ERROR_WP_NOT_FOUND, nr);

    debugger.watchpoints.remove(nr);
    msgQueue.put(MSG_WATCHPOINT_UPDATED);
}

void
CPU::deleteWatchpointAt(u32 addr)
{
    if (!debugger.watchpoints.guardAt(addr)) throw Error(VC64ERROR_WP_NOT_FOUND, addr);

    debugger.watchpoints.removeAt(addr);
    msgQueue.put(MSG_WATCHPOINT_UPDATED);
}

void
CPU::deleteAllWatchpoints()
{
    debugger.watchpoints.removeAll();
    msgQueue.put(MSG_WATCHPOINT_UPDATED);
}

void
CPU::toggleWatchpoint(isize nr)
{
    debugger.watchpoints.isEnabled(nr) ? disableWatchpoint(nr) : enableWatchpoint(nr);
}

void
CPU::setEnableWatchpoint(isize nr, bool value)
{
    if (!debugger.watchpoints.guardNr(nr)) throw Error(VC64ERROR_WP_NOT_FOUND, nr);

    debugger.watchpoints.setEnable(nr, value);
    msgQueue.put(MSG_WATCHPOINT_UPDATED);
}

void
CPU::setEnableWatchpointAt(u32 addr, bool value)
{
    if (!debugger.watchpoints.guardAt(addr)) throw Error(VC64ERROR_WP_NOT_FOUND, addr);

    debugger.watchpoints.setEnableAt(addr, value);
    msgQueue.put(MSG_WATCHPOINT_UPDATED);
}

void
CPU::setEnableAllWatchpoints(bool value)
{
    debugger.watchpoints.setEnableAll(value);
    msgQueue.put(MSG_WATCHPOINT_UPDATED);
}


//
// Memory API
//

u8
CPU::read(u16 addr)
{
    switch (id) {

        case 0: return mem.peek(addr);
        case 1: return drive8.mem.peek(addr);
        case 2: return drive9.mem.peek(addr);

        default:
            fatalError;
    }
}

void
CPU::write(u16 addr, u8 val)
{
    switch (id) {

        case 0: mem.poke(addr, val); break;
        case 1: drive8.mem.poke(addr, val); break;
        case 2: drive9.mem.poke(addr, val); break;

        default:
            fatalError;
    }
}

u8
CPU::readDasm(u16 addr) const
{
    switch (id) {

        case 0: return mem.spypeek(addr);
        case 1: return drive8.mem.spypeek(addr);
        case 2: return drive9.mem.spypeek(addr);

        default:
            fatalError;
    }
}

u16
CPU::readResetVector()
{
    return mem.resetVector();
}

#if PEDDLE_SIMPLE_MEMORY_API == false

namespace vc64::peddle {

#define CHECK_WATCHPOINT \
if constexpr (PEDDLE_ENABLE_WATCHPOINTS) { \
if ((flags & CPU_CHECK_WP) && debugger.watchpointMatches(addr)) { \
watchpointReached(addr); \
}}

template <CPURevision C> u8
Peddle::read(u16 addr)
{
    CHECK_WATCHPOINT

    if (hasProcessorPort<C>()) {

        if (addr < 2) return addr ? readPort() : readPortDir();
    }
    return read(addr & addrMask<C>());
}
template u8 Peddle::read<MOS_6502>(u16 addr);
template u8 Peddle::read<MOS_6507>(u16 addr);
template u8 Peddle::read<MOS_6510>(u16 addr);
template u8 Peddle::read<MOS_8502>(u16 addr);

template <CPURevision C> u8
Peddle::readZeroPage(u8 addr)
{
    CHECK_WATCHPOINT

    switch (C) {

        case MOS_6510: return mem.peekZP(addr);
        case MOS_6502: return id == 1 ? drive8.mem.peekZP(addr) : drive9.mem.peekZP(addr);

        default:
            fatalError;
    }
}
template u8 Peddle::readZeroPage<MOS_6502>(u8 addr);
template u8 Peddle::readZeroPage<MOS_6507>(u8 addr);
template u8 Peddle::readZeroPage<MOS_6510>(u8 addr);
template u8 Peddle::readZeroPage<MOS_8502>(u8 addr);

template <CPURevision C> u8
Peddle::readStack(u8 addr)
{
    CHECK_WATCHPOINT

    switch (C) {

        case MOS_6510: return mem.peekStack(addr);
        case MOS_6502: return id == 1 ? drive8.mem.peekStack(addr) : drive9.mem.peekStack(addr);

        default:
            fatalError;
    }
}
template u8 Peddle::readStack<MOS_6502>(u8 addr);
template u8 Peddle::readStack<MOS_6507>(u8 addr);
template u8 Peddle::readStack<MOS_6510>(u8 addr);
template u8 Peddle::readStack<MOS_8502>(u8 addr);

template <CPURevision C> void
Peddle::readIdle(u16 addr)
{
    if (PEDDLE_EMULATE_IDLE_ACCESSES) {

        CHECK_WATCHPOINT

        switch (C) {

            case MOS_6510: return mem.peekIdle(addr);
            case MOS_6502: return id == 1 ? drive8.mem.peekIdle(addr) : drive9.mem.peekIdle(addr);

            default:
                fatalError;
        }
    }
}
template void Peddle::readIdle<MOS_6502>(u16 addr);
template void Peddle::readIdle<MOS_6507>(u16 addr);
template void Peddle::readIdle<MOS_6510>(u16 addr);
template void Peddle::readIdle<MOS_8502>(u16 addr);

template <CPURevision C> void
Peddle::readZeroPageIdle(u8 addr)
{
    if (PEDDLE_EMULATE_IDLE_ACCESSES) {

        CHECK_WATCHPOINT

        switch (C) {

            case MOS_6510: return mem.peekZPIdle(addr);
            case MOS_6502: return id == 1 ? drive8.mem.peekZPIdle(addr) : drive9.mem.peekZPIdle(addr);

            default:
                fatalError;
        }
    }
}
template void Peddle::readZeroPageIdle<MOS_6502>(u8 addr);
template void Peddle::readZeroPageIdle<MOS_6507>(u8 addr);
template void Peddle::readZeroPageIdle<MOS_6510>(u8 addr);
template void Peddle::readZeroPageIdle<MOS_8502>(u8 addr);

template <CPURevision C> void
Peddle::readStackIdle(u8 addr)
{
    if (PEDDLE_EMULATE_IDLE_ACCESSES) {

        CHECK_WATCHPOINT

        switch (C) {

            case MOS_6510: return mem.peekStackIdle(addr);
            case MOS_6502: return id == 1 ? drive8.mem.peekStackIdle(addr) : drive9.mem.peekStackIdle(addr);

            default:
                fatalError;
        }
    }
}
template void Peddle::readStackIdle<MOS_6502>(u8 addr);
template void Peddle::readStackIdle<MOS_6507>(u8 addr);
template void Peddle::readStackIdle<MOS_6510>(u8 addr);
template void Peddle::readStackIdle<MOS_8502>(u8 addr);


template <CPURevision C> void
Peddle::write(u16 addr, u8 val)
{
    CHECK_WATCHPOINT

    switch (C) {

        case MOS_6510: return mem.poke(addr, val);
        case MOS_6502: return id == 1 ? drive8.mem.poke(addr, val) : drive9.mem.poke(addr, val);

        default:
            fatalError;
    }
}
template void Peddle::write<MOS_6502>(u16 addr, u8 val);
template void Peddle::write<MOS_6507>(u16 addr, u8 val);
template void Peddle::write<MOS_6510>(u16 addr, u8 val);
template void Peddle::write<MOS_8502>(u16 addr, u8 val);

template <CPURevision C> void
Peddle::writeZeroPage(u8 addr, u8 val)
{
    CHECK_WATCHPOINT

    switch (C) {

        case MOS_6510: return mem.pokeZP(addr, val);
        case MOS_6502: return id == 1 ? drive8.mem.pokeZP(addr, val) : drive9.mem.pokeZP(addr, val);

        default:
            fatalError;
    }
}
template void Peddle::writeZeroPage<MOS_6502>(u8 addr, u8 val);
template void Peddle::writeZeroPage<MOS_6507>(u8 addr, u8 val);
template void Peddle::writeZeroPage<MOS_6510>(u8 addr, u8 val);
template void Peddle::writeZeroPage<MOS_8502>(u8 addr, u8 val);

template <CPURevision C> void
Peddle::writeStack(u8 addr, u8 val)
{
    CHECK_WATCHPOINT

    switch (C) {

        case MOS_6510: return mem.pokeStack(addr, val);
        case MOS_6502: return id == 1 ? drive8.mem.pokeStack(addr, val) : drive9.mem.pokeStack(addr, val);

        default:
            fatalError;
    }
}
template void Peddle::writeStack<MOS_6502>(u8 addr, u8 val);
template void Peddle::writeStack<MOS_6507>(u8 addr, u8 val);
template void Peddle::writeStack<MOS_6510>(u8 addr, u8 val);
template void Peddle::writeStack<MOS_8502>(u8 addr, u8 val);

template <CPURevision C> u16
Peddle::readResetVector()
{
    u16 addr = 0xFFFC & addrMask<C>();
    return u16(read<C>(addr) | read<C>(addr + 1) << 8);
}
template u16 Peddle::readResetVector<MOS_6502>();
template u16 Peddle::readResetVector<MOS_6507>();
template u16 Peddle::readResetVector<MOS_6510>();
template u16 Peddle::readResetVector<MOS_8502>();

}

#endif

}
