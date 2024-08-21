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
#include "Drive.h"
#include "Emulator.h"

namespace vc64 {

void
Drive::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::State) {

        os << tab("Idle");
        os << bol(isIdle()) << std::endl;
        os << tab("Motor");
        os << bol(isRotating(), "on", "off") << std::endl;
        os << tab("Has disk");
        os << bol(hasDisk()) << std::endl;
        os << tab("Bit ready timer");
        os << dec(bitReadyTimer) << std::endl;
        os << tab("Head position");
        os << dec(halftrack) << ":" << dec(offset) << std::endl;
        os << tab("SYNC");
        os << bol(sync) << std::endl;
        os << tab("Read mode");
        os << bol(readMode()) << std::endl;
    }

    if (category == Category::BankMap) {

        mem.dump(Category::BankMap, os);
    }

    if (category == Category::Disk || category == Category::Layout) {

        if (hasDisk()) {
            disk->dump(category, os);
        } else {
            os << "No disk";
        }
    }
}

void
Drive::operator << (SerChecker &worker)
{
    serialize(worker);

    if (hasDisk()) disk->serialize(worker);
    if (diskToInsert) diskToInsert->serialize(worker);
}

void
Drive::operator << (SerCounter &worker)
{
    serialize(worker);

    // Add the size of a boolean indicating whether a disk is inserted
    worker.count += sizeof(bool);

    // Add the disk size
    if (hasDisk()) disk->serialize(worker);

    // Add the ROM size
    if (config.saveRoms) worker << mem.rom;
}

void
Drive::operator << (SerReader &worker)
{
    serialize(worker);

    // Check if the snapshot includes a disk
    bool diskInSnapshot; worker << diskInSnapshot;

    // If yes, recreate the disk
    if (diskInSnapshot) {
        disk = std::make_unique<Disk>(worker);
    } else {
        disk = nullptr;
    }

    // Load the ROM if it is contained in the snapshot
    if (config.saveRoms) worker << mem.rom;
}

void
Drive::operator << (SerWriter &worker)
{
    serialize(worker);

    // Indicate whether this drive has a disk is inserted
    worker << hasDisk();

    // If yes, write the disk
    if (hasDisk()) disk->serialize(worker);

    // Save the ROM if applicable
    if (config.saveRoms) worker << mem.rom;
}

void 
Drive::cacheInfo(DriveInfo &result) const
{
    result.id = objid;

    result.hasDisk = hasDisk();
    result.hasUnprotectedDisk = hasUnprotectedDisk();
    result.hasProtectedDisk = hasProtectedDisk();
    result.hasUnmodifiedDisk = hasUnmodifiedDisk();
    result.hasModifiedDisk = hasModifiedDisk();

    result.greenLED = isPoweredOn();
    result.redLED = redLED;

    result.spinning = spinning;
    result.writing = writeMode();

    result.halftrack = halftrack;
    result.offset = offset;
}

void
Drive::_didReset(bool hard)
{
    cpu.reg.pc = 0xEAA0;
    halftrack = 41;

    needsEmulation = config.connected && config.switchedOn;
}

void
Drive::resetConfig()
{
    auto &defaults = emulator.defaults;

    for (auto &opt : getOptions()) {

        if (opt == OPT_DRV_CONNECT && !canConnect()) {

            setOption(opt, false);

        } else {

            setOption(opt, defaults.get(opt, objid));
        }
    }
}

i64
Drive::getOption(Option option) const
{
    switch (option) {

        case OPT_DRV_AUTO_CONFIG:   return (i64)config.autoConfig;
        case OPT_DRV_TYPE:          return (i64)config.type;
        case OPT_DRV_RAM:           return (i64)config.ram;
        case OPT_DRV_SAVE_ROMS:     return (i64)config.saveRoms;
        case OPT_DRV_PARCABLE:      return (i64)config.parCable;
        case OPT_DRV_CONNECT:       return (i64)config.connected;
        case OPT_DRV_POWER_SWITCH:  return (i64)config.switchedOn;
        case OPT_DRV_POWER_SAVE:    return (i64)config.powerSave;
        case OPT_DRV_EJECT_DELAY:   return (i64)config.ejectDelay;
        case OPT_DRV_SWAP_DELAY:    return (i64)config.swapDelay;
        case OPT_DRV_INSERT_DELAY:  return (i64)config.insertDelay;
        case OPT_DRV_PAN:           return (i64)config.pan;
        case OPT_DRV_POWER_VOL:     return (i64)config.powerVolume;
        case OPT_DRV_STEP_VOL:      return (i64)config.stepVolume;
        case OPT_DRV_INSERT_VOL:    return (i64)config.insertVolume;
        case OPT_DRV_EJECT_VOL:     return (i64)config.ejectVolume;

        default:
            fatalError;
    }
}

i64
Drive::getFallback(Option opt) const
{
    return emulator.defaults.getFallback(opt, objid);
}

void
Drive::checkOption(Option opt, i64 value)
{
    switch (opt) {

        case OPT_DRV_AUTO_CONFIG:

            return;

        case OPT_DRV_TYPE:

            if (!DriveTypeEnum::isValid(value)) {
                throw Error(VC64ERROR_OPT_INV_ARG, DriveTypeEnum::keyList());
            }
            return;

        case OPT_DRV_RAM:

            if (!DriveRamEnum::isValid(value)) {
                throw Error(VC64ERROR_OPT_INV_ARG, DriveRamEnum::keyList());
            }
            return;

        case OPT_DRV_SAVE_ROMS:

            config.saveRoms = bool(value);
            return;

        case OPT_DRV_PARCABLE:

            if (!ParCableTypeEnum::isValid(value)) {
                throw Error(VC64ERROR_OPT_INV_ARG, ParCableTypeEnum::keyList());
            }
            return;

        case OPT_DRV_CONNECT:

            if (value && !canConnect()) {
                throw Error(VC64ERROR_ROM_DRIVE_MISSING);
            }
            return;

        case OPT_DRV_POWER_SWITCH:
        case OPT_DRV_POWER_SAVE:
        case OPT_DRV_EJECT_DELAY:
        case OPT_DRV_SWAP_DELAY:
        case OPT_DRV_INSERT_DELAY:
        case OPT_DRV_PAN:
        case OPT_DRV_POWER_VOL:
        case OPT_DRV_STEP_VOL:
        case OPT_DRV_EJECT_VOL:
        case OPT_DRV_INSERT_VOL:

            return;

        default:
            throw Error(VC64ERROR_OPT_UNSUPPORTED);
    }
}

void
Drive::setOption(Option opt, i64 value)
{
    switch (opt) {

        case OPT_DRV_AUTO_CONFIG:

            config.autoConfig = bool(value);
            if (value) autoConfigure();
            return;

        case OPT_DRV_TYPE:

            config.type = DriveType(value);
            return;

        case OPT_DRV_RAM:

            config.ram = DriveRam(value);
            mem.updateBankMap();
            return;

        case OPT_DRV_SAVE_ROMS:

            config.saveRoms = bool(value);
            return;

        case OPT_DRV_PARCABLE:

            config.parCable = ParCableType(value);
            mem.updateBankMap();
            return;

        case OPT_DRV_CONNECT:

            config.connected = bool(value);
            hardReset();
            msgQueue.put(MSG_DRIVE_CONNECT, DriveMsg { i16(objid), i16(value), 0, 0 } );
            return;

        case OPT_DRV_POWER_SWITCH:

            config.switchedOn = bool(value);
            softReset();
            msgQueue.put(MSG_DRIVE_POWER, DriveMsg { .nr = i16(objid), .value = i16(value) } );
            return;

        case OPT_DRV_POWER_SAVE:

            config.powerSave = bool(value);
            wakeUp();
            return;

        case OPT_DRV_EJECT_DELAY:

            config.ejectDelay = isize(value);
            return;

        case OPT_DRV_SWAP_DELAY:

            config.swapDelay = isize(value);
            return;

        case OPT_DRV_INSERT_DELAY:

            config.insertDelay = isize(value);
            return;

        case OPT_DRV_PAN:

            config.pan = i16(value);
            return;

        case OPT_DRV_POWER_VOL:

            value = std::clamp(value, 0LL, 100LL);
            config.powerVolume = u8(value);
            return;

        case OPT_DRV_STEP_VOL:

            value = std::clamp(value, 0LL, 100LL);
            config.stepVolume = u8(value);
            return;

        case OPT_DRV_EJECT_VOL:

            value = std::clamp(value, 0LL, 100LL);
            config.ejectVolume = u8(value);
            return;

        case OPT_DRV_INSERT_VOL:

            value = std::clamp(value, 0LL, 100LL);
            config.insertVolume = u8(value);
            return;

        default:
            fatalError;
    }
}

void
Drive::autoConfigure()
{
    // switch (RomFile::identifier(mem.romFNV64())) {
    switch (mem.romFNV64()) {

        case 0x361A1EC48F04F5A4: // VC1541C_01
        case 0xB938E2DA07F4FE40: // VC1541C_02

            debug(CNF_DEBUG, "autoConfigure: VC1541C\n");
            setOption(OPT_DRV_TYPE, DRIVE_VC1541C);
            setOption(OPT_DRV_RAM, DRVRAM_NONE);
            setOption(OPT_DRV_PARCABLE, PAR_CABLE_NONE);
            break;

        case 0x44BBA0EAC5898597: // VC1541_II_1987
        case 0xA1D36980A17C8756: // VC1541_II_NEWTRONIC
        case 0x47CBA55F16FB3E09: // VC1541_II_RELOC_PATCH
        case 0x8B2A523E29BED889: // VC1541_II_JIFFY
        case 0xF7F4D931219DBB5D: // VC1541_II_JIFFY_V600
        case 0xB4027D6D9D61378A: // VC1541_64ER_V3

            debug(CNF_DEBUG, "autoConfigure: VC1541 II\n");
            setOption(OPT_DRV_TYPE, DRIVE_VC1541II);
            setOption(OPT_DRV_RAM, DRVRAM_NONE);
            setOption(OPT_DRV_PARCABLE, PAR_CABLE_NONE);
            break;

        case 0xC50EAFCBA50C4B63: // VC1541_SPEEDDOS_PLUS
        case 0x92ADEBA1BCCD8D31: // VC1541_SPEEDDOS_27

            debug(CNF_DEBUG, "autoConfigure: VC1541 SpeedDOS\n");
            setOption(OPT_DRV_TYPE, DRIVE_VC1541II);
            setOption(OPT_DRV_RAM, DRVRAM_NONE);
            setOption(OPT_DRV_PARCABLE, PAR_CABLE_STANDARD);
            break;

        case 0x28CD4E47A40C41CA: // VC1541_DOLPHIN_20
        case 0x1C1DDD64E02CAD32: // VC1541_DOLPHIN_20_SLVDR

            debug(CNF_DEBUG, "autoConfig: Dolphin DOS\n");
            setOption(OPT_DRV_TYPE, DRIVE_VC1541II);
            setOption(OPT_DRV_RAM, DRVRAM_8000_9FFF);
            setOption(OPT_DRV_PARCABLE, PAR_CABLE_STANDARD);
            break;

        case 0x09D8FBAB61E59FF0: // VC1541_DOLPHIN_30
        case 0xF684F72388EE5364: // VC1541_DOLPHIN_30_SLVDR

            debug(CNF_DEBUG, "autoConfig: Dolphin DOS 3\n");
            setOption(OPT_DRV_TYPE, DRIVE_VC1541II);
            setOption(OPT_DRV_RAM, DRVRAM_6000_7FFF);
            setOption(OPT_DRV_PARCABLE, PAR_CABLE_DOLPHIN3);
            break;

        default:

            debug( CNF_DEBUG, "AutoConfig: Rom not recognized\n");
    }
}

}
