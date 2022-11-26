// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#define CHECK_WATCHPOINT \
if constexpr (ENABLE_WATCHPOINTS) { \
if ((flags & CPU_CHECK_WP) && debugger.watchpointMatches(addr)) { \
watchpointReached(addr); \
}}

template <CPURevision C> u8
Peddle::peek(u16 addr)
{
    CHECK_WATCHPOINT

    if (hasProcessorPort<C>()) {

        if (addr < 2) return addr ? readPort() : readPortDir();
    }
    return read8(addr & addrMask<C>());
}

template <CPURevision C> u8
Peddle::peekZP(u8 addr)
{
    CHECK_WATCHPOINT

    if (hasProcessorPort<C>()) {

        if (addr < 2) return addr ? readPort() : readPortDir();
    }
    return read8(addr & addrMask<C>());
}

template <CPURevision C> u8
Peddle::peekStack(u8 addr)
{
    CHECK_WATCHPOINT
    return read8(u16(addr) + 0x100);
}

template <CPURevision C> void
Peddle::peekIdle(u16 addr)
{
    if (EMULATE_IDLE_ACCESSES) {

        CHECK_WATCHPOINT

        if (hasProcessorPort<C>()) {

            if (addr < 2) return addr ? (void)readPort() : (void)readPortDir();
        }
        (void)read8(addr & addrMask<C>());
    }
}

template <CPURevision C> void
Peddle::peekZPIdle(u8 addr)
{
    if (EMULATE_IDLE_ACCESSES) {

        CHECK_WATCHPOINT

        if (hasProcessorPort<C>()) {

            if (addr < 2) return addr ? (void)readPort() : (void)readPortDir();
        }
        (void)read8(u16(addr));
    }
}

template <CPURevision C> void
Peddle::peekStackIdle(u8 addr)
{
    if (EMULATE_IDLE_ACCESSES) {

        CHECK_WATCHPOINT
        (void)read8(u16(addr) + 0x100);
    }
}

template <CPURevision C> void
Peddle::poke(u16 addr, u8 val)
{
    CHECK_WATCHPOINT

    if (hasProcessorPort<C>()) {

        if (addr < 2) { addr ? writePort(val) : writePortDir(val); return; }
    }
    write8(addr & addrMask<C>(), val);
}

template <CPURevision C> void
Peddle::pokeZP(u8 addr, u8 val)
{
    CHECK_WATCHPOINT

    if (hasProcessorPort<C>()) {

        if (addr < 2) { addr ? writePort(val) : writePortDir(val); return; }
    }
    write8(u16(addr), val);
}

template <CPURevision C> void
Peddle::pokeStack(u8 addr, u8 val)
{
    CHECK_WATCHPOINT
    write8(u16(addr) + 0x100, val);
}

u8
Peddle::readPort() const
{
    return (reg.pport.data & reg.pport.direction) | (externalPortBits() & ~reg.pport.direction);
}
