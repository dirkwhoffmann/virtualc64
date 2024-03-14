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
#include "IOUtils.h"

namespace vc64 {

Drive::Drive(isize nr, C64 &ref) : SubComponent(ref), deviceNr(nr)
{
    assert(deviceNr == DRIVE8 || deviceNr == DRIVE9);

    disk = std::make_unique<Disk>();
    
    subComponents = std::vector <CoreComponent *> {
        
        &mem,
        &cpu,
        &via1,
        &via2
    };

    // Assign a unique ID to the CPU
    cpu.setID(nr == DRIVE8 ? 1 : 2);
}

const char *
Drive::getDescription() const
{
    return deviceNr == DRIVE8 ? "drive8" : "drive9";
}

void
Drive::_initialize()
{
    CoreComponent::_initialize();
    
    insertionStatus = DISK_FULLY_EJECTED;
    disk->clearDisk();
}

void
Drive::_reset(bool hard)
{    
    cpu.reg.pc = 0xEAA0;
    halftrack = 41;
    
    needsEmulation = config.connected && config.switchedOn;
}

DriveInfo
Drive::getInfo() const
{
    DriveInfo result;

    result.id = deviceNr;
    
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
    
    return result;
}

void
Drive::resetConfig()
{
    auto &defaults = emulator.defaults;

    for (auto &opt : getOptions()) {

        if (opt == OPT_DRV_CONNECT && !canConnect()) {

            setOption(opt, false);

        } else {

            setOption(opt, defaults.get(opt, deviceNr));
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
    return emulator.defaults.getFallback(opt, deviceNr);
}

void
Drive::setOption(Option option, i64 value)
{
    debug(CNF_DEBUG, "%s, %lld\n", OptionEnum::key(option), value);

    switch (option) {

        case OPT_DRV_AUTO_CONFIG:
        {
            {   SUSPENDED
                
                config.autoConfig = bool(value);
                if (value) autoConfigure();
            }
            return;
        }
        case OPT_DRV_TYPE:

            if (!DriveTypeEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, DriveTypeEnum::keyList());
            }
            
            config.type = DriveType(value);
            return;

        case OPT_DRV_RAM:
        {
            if (!DriveRamEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, DriveRamEnum::keyList());
            }
            
            {   SUSPENDED
                
                config.ram = DriveRam(value);
                mem.updateBankMap();
            }
            return;
        }
        case OPT_DRV_PARCABLE:
        {
            if (!ParCableTypeEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, ParCableTypeEnum::keyList());
            }
            
            {   SUSPENDED

                config.parCable = ParCableType(value);
                mem.updateBankMap();
            }
            return;
        }
        case OPT_DRV_CONNECT:
        {
            if (value && !canConnect()) {
                throw VC64Error(ERROR_ROM_DRIVE_MISSING);
            }
            
            {   SUSPENDED

                config.connected = bool(value);
                hardReset();
            }
            msgQueue.put(MSG_DRIVE_CONNECT, DriveMsg { i16(deviceNr), i16(value), 0, 0 } );
            return;
        }
        case OPT_DRV_POWER_SWITCH:
        {
            {   SUSPENDED
                
                config.switchedOn = bool(value);
                hardReset();
            }
            msgQueue.put(MSG_DRIVE_POWER, DriveMsg { .nr = i16(deviceNr), .value = i16(value) } );
            return;
        }
        case OPT_DRV_POWER_SAVE:
        {
            {   SUSPENDED
                
                config.powerSave = bool(value);
                wakeUp();
            }
            return;
        }
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

bool
Drive::canConnect()
{
    return c64.hasRom(ROM_TYPE_VC1541);
}

void
Drive::_dump(Category category, std::ostream& os) const
{
    using namespace util;
    
    if (category == Category::Config) {

        dumpConfig(os);

        /*
        os << tab("Auto config");
        os << bol(config.autoConfig) << std::endl;
        os << tab("Drive type");
        os << DriveTypeEnum::key(config.type) << std::endl;
        os << tab("Ram");
        os << DriveRamEnum::key(config.ram) << std::endl;
        os << tab("Parallel cable");
        os << ParCableTypeEnum::key(config.parCable) << std::endl;
        os << tab("Power save mode");
        os << bol(config.powerSave, "when idle", "never") << std::endl;
        os << tab("Connected");
        os << bol(config.connected) << std::endl;
        os << tab("Power switch");
        os << bol(config.switchedOn, "on", "off") << std::endl;
        os << tab("Pan");
        os << config.pan << std::endl;
        os << tab("Power volume");
        os << dec(config.powerVolume) << std::endl;
        os << tab("Step volume");
        os << dec(config.stepVolume) << std::endl;
        os << tab("Insert volume");
        os << dec(config.insertVolume) << std::endl;
        os << tab("Eject volume");
        os << dec(config.ejectVolume) << std::endl;
        */
        os << std::endl;
        mem.dump(Category::BankMap, os);
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
        os << dec(halftrack) << "::" << dec(offset) << std::endl;
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
    if (disk) disk->serialize(worker);
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
}

void
Drive::operator << (SerReader &worker)
{
    // Read own state
    serialize(worker);

    // Check if the snapshot includes a disk
    bool diskInSnapshot; worker << diskInSnapshot;

    // If yes, recreate the disk
    if (diskInSnapshot) {
        disk = std::make_unique<Disk>(worker);
    } else {
        disk = nullptr;
    }
}

void
Drive::operator << (SerWriter &worker)
{
    // Write own state
    serialize(worker);

    // Indicate whether this drive has a disk is inserted
    worker << hasDisk();

    // If yes, write the disk
    if (hasDisk()) disk->serialize(worker);
}

void
Drive::execute(u64 duration)
{
    elapsedTime += duration;
    
    while (nextClock < (i64)elapsedTime || nextCarry < (i64)elapsedTime) {

        if (nextClock <= nextCarry) {
            
            // Execute CPU and VIAs
            i64 cycle = ++cpu.clock;
            cpu.execute<MOS_6502>();
            if (cycle >= via1.wakeUpCycle) via1.execute(); else via1.idleCounter++;
            if (cycle >= via2.wakeUpCycle) via2.execute(); else via2.idleCounter++;
            updateByteReady();
            nextClock += 10000;

        } else {
            
            // Execute read/write logic
            if (spinning) executeUF4();
            nextCarry += delayBetweenTwoCarryPulses[zone];
        }
    }
    assert(nextClock >= (i64)elapsedTime && nextCarry >= (i64)elapsedTime);
}

void
Drive::executeUF4()
{
    // Increase counter
    counterUF4++;
    carryCounter++;
    
    // We assume that a new bit comes in every fourth cycle.
    // Later, we can decouple timing here to emulate asynchronicity.
    if (carryCounter % 4 == 0) {
        
        // When a bit comes in and ...
        //   ... it's value equals 0, nothing happens.
        //   ... it's value equals 1, counter UF4 is reset.
        if (readMode() && hasDisk() && readBitFromHead()) {
            counterUF4 = 0;
        }
        rotateDisk();
    }

    // Update SYNC signal
    sync = (readShiftreg & 0x3FF) != 0x3FF || writeMode();
    if (!sync) byteReadyCounter = 0;
    
    // The lower two bits of counter UF4 are used to clock the logic board:
    //
    //                        (6) Load the write shift register
    //                         |      if the byte ready counter equals 7.
    //                         v
    //         ---- ----           ---- ----
    // QBQA:  | 00   01 | 10   11 | 00   01 | 10   11 |
    //                   ---- ----           ---- ----
    //                   ^          ^    ^    ^    ^
    //                   |          |    |    |    |
    //                   |          |    |   (2) Byte ready is always 1 here.
    //                   |         (1)  (1) Byte ready may be 0 here.
    //                   |
    //                  (3) Execute UE3 (the byte ready counter)
    //                  (4) Execute write shift register
    //                  (5) Execute read shift register
    //
    
    switch (counterUF4 & 0x03) {
            
        case 0x00:
        case 0x01:
            
            // Computation of the Byte Ready and the Load signal
            //
            //           74LS191                             ---
            //           -------               VIA2::CA2 ---|   |
            //  SYNC --o| Load  |                UF4::QB --o| & |o-- Byte Ready
            //    QB ---| Clk   |                        ---|   |
            //          |    QD |   ---                  |   ---
            //          |    QC |--|   |    ---          |   ---
            //          |    QB |--| & |o--| 1 |o-----------|   |
            //          |    QA |--|   |    ---   UF4::QB --| & |o-- load UD3
            //           -------    ---           UF4::QA --|   |
            //             UE3                               ---
            
            // (1) Update value on Byte Ready line
            updateByteReady();
            break;
            
        case 0x02:
            
            // (2)
            raiseByteReady();
            
            // (3) Execute byte ready counter
            byteReadyCounter = sync ? (byteReadyCounter + 1) & 7 : 0;
            
            // (4) Execute the write shift register
            if (writeMode() && hasDisk() && !getLightBarrier()) {
                writeBitToHead(writeShiftreg & 0x80);
                disk->setModified(true);
            }
            writeShiftreg <<= 1;
            
            // (5) Execute read shift register
            readShiftreg <<= 1;
            readShiftreg |= ((counterUF4 & 0x0C) == 0);
            break;
            
        case 0x03:
            
            // (6)
            if (byteReadyCounter == 7) {
                writeShiftreg = via2.getPA();
            }
            break;
    }
}

void
Drive::updateByteReady()
{
    //
    //           74LS191                             ---
    //           -------               VIA2::CA2 ---|   |
    //  SYNC --o| Load  |                UF4::QB --o| & |o-- Byte Ready
    //    QB ---| Clk   |                        ---|   |
    //          |    QD |   ---                  |   ---
    //          |    QC |--|   |    ---          |
    //          |    QB |--| & |o--| 1 |o---------
    //          |    QA |--|   |    ---
    //           -------    ---
    //             UE3
    
    bool ca2 = via2.getCA2();
    bool qb = counterUF4 & 0x02;
    bool ue3 = (byteReadyCounter == 7);
    bool newByteReady = !(ca2 && !qb && ue3);
    
    if (byteReady != newByteReady) {
        byteReady = newByteReady;
        via2.CA1action(byteReady);
    }
}

void
Drive::raiseByteReady()
{
    if (!byteReady) {
        byteReady = true;
        via2.CA1action(true);
    }
}

void
Drive::setZone(isize value)
{
    assert(value < 4);
    
    if (value != zone) {
        trace(DRV_DEBUG, "Switching zone: %ld --> %ld\n", zone, value);
        zone = value;
    }
}

u8
Drive::readBitFromHead() const
{
    assert(hasDisk());
    return disk->readBitFromHalftrack(halftrack, offset);
}

void
Drive::writeBitToHead(u8 bit)
{
    assert(hasDisk());
    disk->writeBitToHalftrack(halftrack, offset, bit);
}

void
Drive::rotateDisk()
{
    if (hasDisk()) {
        if (++offset >= disk->lengthOfHalftrack(halftrack)) offset = 0;
    }
}

void
Drive::setRedLED(bool b)
{
    if (redLED != b) {

        redLED = b;
        wakeUp();
        msgQueue.put(MSG_DRIVE_LED, DriveMsg { .nr = i16(deviceNr), .value = b } );
        return;
    }
}

void
Drive::setRotating(bool b)
{
    if (spinning != b) {

        spinning = b;
        msgQueue.put(MSG_DRIVE_MOTOR, DriveMsg { .nr = i16(deviceNr), .value = b } );
        iec.updateTransferStatus();
    }
}

void
Drive::wakeUp(isize awakeness)
{
    if (isIdle()) {
        
        trace(DRV_DEBUG, "Exiting power-safe mode\n");
        msgQueue.put(MSG_DRIVE_POWER_SAVE, DriveMsg { .nr = i16(deviceNr), .value = 0 } );
        needsEmulation = true;
    }

    watchdog = awakeness;
}

void
Drive::moveHeadUp()
{
    if (halftrack < 84) {

        if (hasDisk()) {

            assert(disk->lengthOfHalftrack(halftrack) != 0);
            float pos = (float)offset / (float)disk->lengthOfHalftrack(halftrack);
            halftrack++;
            offset = (HeadPos)(pos * disk->lengthOfHalftrack(halftrack));
            assert(disk->isValidHeadPos(halftrack, offset));
            
        } else {
            
            halftrack++;
            offset = 0;
        }
        
        trace(DRV_DEBUG, "Moving head up to halftrack %ld (track %2.1f) (offset %ld)\n",
              halftrack, (halftrack + 1) / 2.0, offset);
    }

    msgQueue.put(MSG_DRIVE_STEP, DriveMsg {
        i16(deviceNr), i16(halftrack), config.stepVolume, config.pan
    });
}

void
Drive::moveHeadDown()
{
    if (halftrack > 1) {
        
        if (hasDisk()) {

            assert(disk->lengthOfHalftrack(halftrack) != 0);
            float pos = (float)offset / (float)disk->lengthOfHalftrack(halftrack);
            halftrack--;
            offset = (HeadPos)(pos * disk->lengthOfHalftrack(halftrack));
            assert(disk->isValidHeadPos(halftrack, offset));

        } else {
            
            halftrack--;
            offset = 0;
        }
        
        trace(DRV_DEBUG, "Moving head down to halftrack %ld (track %2.1f)\n",
              halftrack, (halftrack + 1) / 2.0);
    }

    msgQueue.put(MSG_DRIVE_STEP, DriveMsg {
        i16(deviceNr), i16(halftrack), config.stepVolume, config.pan
    });
}

bool
Drive::hasDisk() const
{
    return insertionStatus == DISK_FULLY_INSERTED;
}

bool
Drive::hasPartiallyRemovedDisk() const
{
    return
    insertionStatus == DISK_PARTIALLY_INSERTED ||
    insertionStatus == DISK_PARTIALLY_EJECTED;
}

void
Drive::setModificationFlag(bool value)
{
    if (hasDisk() && value != disk->isModified()) {

        disk->setModified(value);
        msgQueue.put(MSG_DISK_MODIFIED);
    }
}

void 
Drive::setProtection(bool value)
{
    if (hasDisk() && value != disk->isWriteProtected()) {

        disk->setWriteProtection(value);
        msgQueue.put(MSG_DISK_PROTECTED);
    }
}

void 
Drive::toggleProtection()
{
    if (hasDisk()) setProtection(!disk->isWriteProtected());
}

void
Drive::insertDisk(const string &path, bool wp)
{
    insertDisk(std::make_unique<Disk>(path, wp));
}

void
Drive::insertDisk(std::unique_ptr<Disk> disk)
{
    debug(DSKCHG_DEBUG, "insertDisk\n");

    {   SUSPENDED
        
        if (!diskToInsert) {

            diskToInsert = std::move(disk);

            // Initiate the disk change procedure
            scheduleFirstDiskChangeEvent(DCH_INSERT);
        }
    }
}

void
Drive::insertNewDisk(DOSType fsType, PETName<16> name)
{
    insertDisk(std::make_unique<Disk>(fsType, name));
}

void
Drive::insertFileSystem(const FileSystem &device, bool wp)
{
    insertDisk(std::make_unique<Disk>(device, wp));
}

void
Drive::insertD64(const D64File &d64, bool wp)
{
    insertDisk(std::make_unique<Disk>(d64, wp));
}

void
Drive::insertG64(const G64File &g64, bool wp)
{
    insertDisk(std::make_unique<Disk>(g64, wp));
}

void
Drive::insertCollection(AnyCollection &collection, bool wp)
{
    insertDisk(std::make_unique<Disk>(collection, wp));
}

void 
Drive::ejectDisk()
{
    debug(DSKCHG_DEBUG, "ejectDisk()\n");

    {   SUSPENDED
        
        if (insertionStatus == DISK_FULLY_INSERTED && !diskToInsert) {

            // Initiate the disk change procedure
            scheduleFirstDiskChangeEvent(DCH_EJECT);
        }
    }
}

void
Drive::vsyncHandler()
{
    // Only proceed if the drive is connected and switched on
    if (!config.connected || !config.switchedOn) return;

    // Check if we should enter power-safe mode
    if (!spinning && config.powerSave) {

        if (--watchdog == 0) {

            trace(DRV_DEBUG, "Entering power-save mode\n");
            needsEmulation = false;
            msgQueue.put(MSG_DRIVE_POWER_SAVE, DriveMsg { .nr = i16(deviceNr), .value = 1 } );
        }
    }
}

void
Drive::processCommand(const Cmd &cmd)
{
    switch (cmd.type) {

        case CMD_DSK_TOGGLE_WP:     toggleProtection(); break;
        case CMD_DSK_MODIFIED:      markDiskAsModified(); break;
        case CMD_DSK_UNMODIFIED:    markDiskAsUnmodified(); break;

        default:
            fatalError;
    }
}

void
Drive::scheduleFirstDiskChangeEvent(EventID id)
{
    // Exit power-safe mode and make sure the drive stays awake for a while
    wakeUp(200); // 200 frames

    // Schedule the first event
    if (isDrive8()) c64.scheduleImm<SLOT_DC8>(id);
    if (isDrive9()) c64.scheduleImm<SLOT_DC9>(id);
}

void
Drive::processDiskChangeEvent(EventID id)
{
    auto reschedule = [&](isize delay) {

        Cycle cycles = vic.getCyclesPerFrame() * delay;

        if (isDrive8()) c64.rescheduleInc<SLOT_DC8>(cycles);
        if (isDrive9()) c64.rescheduleInc<SLOT_DC9>(cycles);
    };

    auto terminate = [&]() {

        if (isDrive8()) c64.cancel<SLOT_DC8>();
        if (isDrive9()) c64.cancel<SLOT_DC9>();
    };

    switch (insertionStatus) {

        case DISK_FULLY_INSERTED:

            trace(DSKCHG_DEBUG, "FULLY_INSERTED -> PARTIALLY_EJECTED\n");

            // Pull the disk half out (blocks the light barrier)
            insertionStatus = DISK_PARTIALLY_EJECTED;

            // Make sure the drive can no longer read from this disk
            disk->clearDisk();

            // Schedule the next transition
            reschedule(config.ejectDelay);
            return;

        case DISK_PARTIALLY_EJECTED:

            trace(DSKCHG_DEBUG, "PARTIALLY_EJECTED -> FULLY_EJECTED\n");

            // Take the disk out (unblocks the light barrier)
            insertionStatus = DISK_FULLY_EJECTED;

            // Inform the GUI
            msgQueue.put(MSG_DISK_EJECT, DriveMsg {
                i16(deviceNr), i16(halftrack), config.stepVolume, config.pan
            });

            // Schedule the next transition
            reschedule(config.swapDelay);
            return;

        case DISK_FULLY_EJECTED:

            trace(DSKCHG_DEBUG, "FULLY_EJECTED -> PARTIALLY_INSERTED\n");

            // Only proceed if a new disk is waiting for insertion
            if (!diskToInsert) break;

            // Push the new disk half in (blocks the light barrier)
            insertionStatus = DISK_PARTIALLY_INSERTED;

            // Schedule the next transition
            reschedule(config.insertDelay);
            return;

        case DISK_PARTIALLY_INSERTED:

            trace(DSKCHG_DEBUG, "PARTIALLY_INSERTED -> FULLY_INSERTED\n");

            // Fully insert the disk (unblocks the light barrier)
            insertionStatus = DISK_FULLY_INSERTED;
            disk = std::move(diskToInsert);

            // Inform the GUI
            msgQueue.put(MSG_DISK_INSERT, DriveMsg {
                i16(deviceNr), i16(halftrack), config.stepVolume, config.pan
            });
            break;

        default:
            fatalError;
    }

    // Terminate the disk change procedure
    terminate();
}

}
