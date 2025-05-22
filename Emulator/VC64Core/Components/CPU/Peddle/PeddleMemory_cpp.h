// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#define CHECK_WATCHPOINT \
if constexpr (PEDDLE_ENABLE_WATCHPOINTS) { \
if ((flags & CPU_CHECK_WP) && debugger.watchpointMatches(addr)) { \
watchpointReached(addr); \
}}

#if PEDDLE_SIMPLE_MEMORY_API == true

template <CPURevision C> u8
Peddle::read(u16 addr)
{
    CHECK_WATCHPOINT

    if (hasProcessorPort<C>()) {

        if (addr < 2) return addr ? readPort() : readPortDir();
    }
    return read(addr & addrMask<C>());
}

template <CPURevision C> u8
Peddle::readZeroPage(u8 addr)
{
    CHECK_WATCHPOINT

    if (hasProcessorPort<C>()) {

        if (addr < 2) return addr ? readPort() : readPortDir();
    }
    return read(addr & addrMask<C>());
}

template <CPURevision C> u8
Peddle::readStack(u8 addr)
{
    CHECK_WATCHPOINT
    return read(u16(addr) + 0x100);
}

template <CPURevision C> void
Peddle::readIdle(u16 addr)
{
    if (PEDDLE_EMULATE_IDLE_ACCESSES) {

        CHECK_WATCHPOINT

        if (hasProcessorPort<C>()) {

            if (addr < 2) return addr ? (void)readPort() : (void)readPortDir();
        }
        (void)read(addr & addrMask<C>());
    }
}

template <CPURevision C> void
Peddle::readZeroPageIdle(u8 addr)
{
    if (PEDDLE_EMULATE_IDLE_ACCESSES) {

        CHECK_WATCHPOINT

        if (hasProcessorPort<C>()) {

            if (addr < 2) return addr ? (void)readPort() : (void)readPortDir();
        }
        (void)read(u16(addr));
    }
}

template <CPURevision C> void
Peddle::readStackIdle(u8 addr)
{
    if (PEDDLE_EMULATE_IDLE_ACCESSES) {

        CHECK_WATCHPOINT
        (void)read(u16(addr) + 0x100);
    }
}

template <CPURevision C> void
Peddle::write(u16 addr, u8 val)
{
    CHECK_WATCHPOINT

    if (hasProcessorPort<C>()) {

        if (addr < 2) { addr ? writePort(val) : writePortDir(val); return; }
    }
    write(addr & addrMask<C>(), val);
}

template <CPURevision C> void
Peddle::writeZeroPage(u8 addr, u8 val)
{
    CHECK_WATCHPOINT

    if (hasProcessorPort<C>()) {

        if (addr < 2) { addr ? writePort(val) : writePortDir(val); return; }
    }
    write(u16(addr), val);
}

template <CPURevision C> void
Peddle::writeStack(u8 addr, u8 val)
{
    CHECK_WATCHPOINT
    write(u16(addr) + 0x100, val);
}

/*
template <CPURevision C> u16
Peddle::readResetVector()
{
    u16 addr = 0xFFFC & addrMask<C>();
    return u16(read<C>(addr) | read<C>(addr + 1) << 8);
}
*/

#endif

u16
Peddle::readResetVector()
{
    u16 addr = 0xFFFC & addrMask();
    return u16(read(addr) | read(addr + 1) << 8);
}

template <CPURevision C> u16
Peddle::readDasm(u16 addr) const
{
    return readDasm(addr & addrMask<C>());
}

u8
Peddle::readPort() const
{
    return (reg.pport.data & reg.pport.direction) | (externalPortBits() & ~reg.pport.direction);
}

