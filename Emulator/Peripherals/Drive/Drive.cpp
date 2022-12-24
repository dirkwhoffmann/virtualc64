// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Drive.h"
#include "C64.h"
#include "IOUtils.h"

namespace vc64 {

Drive::Drive(isize nr, C64 &ref) : SubComponent(ref), deviceNr(nr)
{
    assert(deviceNr == DRIVE8 || deviceNr == DRIVE9);

    disk = std::make_unique<Disk>();
    
    subComponents = std::vector <C64Component *> {
        
        &mem,
        &cpu,
        &via1,
        &via2
    };
}

const char *
Drive::getDescription() const
{
    return deviceNr == DRIVE8 ? "Drive8" : "Drive9";
}

void
Drive::_initialize()
{
    C64Component::_initialize();
    
    insertionStatus = DISK_FULLY_EJECTED;
    disk->clearDisk();
}

void
Drive::_reset(bool hard)
{    
    RESET_SNAPSHOT_ITEMS(hard)

    cpu.reg.pc = 0xEAA0;
    halftrack = 41;
    
    needsEmulation = config.connected && config.switchedOn;
}

DriveConfig
Drive::getDefaultConfig()
{
    DriveConfig defaults;
    
    defaults.autoConfig = true;
    defaults.type = DRIVE_VC1541II;
    defaults.ram = DRVRAM_NONE;
    defaults.parCable = PAR_CABLE_NONE;
    defaults.powerSave = true;
    defaults.connected = false;
    defaults.switchedOn = true;
    defaults.ejectDelay = 30;
    defaults.swapDelay = 30;
    defaults.insertDelay = 30;
    defaults.pan = 0;
    defaults.powerVolume = 50;
    defaults.stepVolume = 50;
    defaults.insertVolume = 50;
    defaults.ejectVolume = 50;

    return defaults;
}

void
Drive::resetConfig()
{
    assert(isPoweredOff());
    auto &defaults = c64.defaults;

    std::vector <Option> options = {

        OPT_DRV_AUTO_CONFIG,
        OPT_DRV_TYPE,
        OPT_DRV_RAM,
        OPT_DRV_PARCABLE,
        OPT_DRV_CONNECT,
        OPT_DRV_POWER_SWITCH,
        OPT_DRV_POWER_SAVE,
        OPT_DRV_EJECT_DELAY,
        OPT_DRV_SWAP_DELAY,
        OPT_DRV_INSERT_DELAY,
        OPT_DRV_PAN,
        OPT_DRV_POWER_VOL,
        OPT_DRV_STEP_VOL,
        OPT_DRV_INSERT_VOL,
        OPT_DRV_EJECT_VOL
    };

    for (auto &option : options) {

        if (option == OPT_DRV_CONNECT && !canConnect()) {

            setConfigItem(option, false);
            setConfigItem(option, false);

        } else {

            setConfigItem(option, defaults.get(option, deviceNr));
            setConfigItem(option, defaults.get(option, deviceNr));
        }
    }
}

i64
Drive::getConfigItem(Option option) const
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

void
Drive::setConfigItem(Option option, i64 value)
{
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
                reset(true);
            }
            msgQueue.put(value ? MSG_DRIVE_CONNECT : MSG_DRIVE_DISCONNECT, deviceNr);
            return;
        }
        case OPT_DRV_POWER_SWITCH:
        {
            {   SUSPENDED
                
                config.switchedOn = bool(value);
                reset(true);
            }
            msgQueue.put(value ? MSG_DRIVE_POWER_ON : MSG_DRIVE_POWER_OFF, deviceNr);
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
    switch (RomFile::identifier(mem.romFNV64())) {
            
        case VC1541C_01:
        case VC1541C_02:
            
            debug(CNF_DEBUG, "autoConfigure: VC1541C\n");
            setConfigItem(OPT_DRV_TYPE, DRIVE_VC1541C);
            setConfigItem(OPT_DRV_RAM, DRVRAM_NONE);
            setConfigItem(OPT_DRV_PARCABLE, PAR_CABLE_NONE);
            break;
            
        case VC1541_II_1987:
        case VC1541_II_NEWTRONIC:
        case VC1541_II_RELOC_PATCH:
        case VC1541_II_JIFFY:
        case VC1541_II_JIFFY_V600:
        case VC1541_64ER_V3:

            debug(CNF_DEBUG, "autoConfigure: VC1541 II\n");
            setConfigItem(OPT_DRV_TYPE, DRIVE_VC1541II);
            setConfigItem(OPT_DRV_RAM, DRVRAM_NONE);
            setConfigItem(OPT_DRV_PARCABLE, PAR_CABLE_NONE);
            break;

        case VC1541_SPEEDDOS_PLUS:
        case VC1541_SPEEDDOS_27:

            debug(CNF_DEBUG, "autoConfigure: VC1541 SpeedDOS\n");
            setConfigItem(OPT_DRV_TYPE, DRIVE_VC1541II);
            setConfigItem(OPT_DRV_RAM, DRVRAM_NONE);
            setConfigItem(OPT_DRV_PARCABLE, PAR_CABLE_STANDARD);
            break;

        case VC1541_DOLPHIN_20:
        case VC1541_DOLPHIN_20_SLVDR:

            debug(CNF_DEBUG, "autoConfig: Dolphin DOS\n");
            setConfigItem(OPT_DRV_TYPE, DRIVE_VC1541II);
            setConfigItem(OPT_DRV_RAM, DRVRAM_8000_9FFF);
            setConfigItem(OPT_DRV_PARCABLE, PAR_CABLE_STANDARD);
            break;

        case VC1541_DOLPHIN_30:
        case VC1541_DOLPHIN_30_SLVDR:

            debug(CNF_DEBUG, "autoConfig: Dolphin DOS 3\n");
            setConfigItem(OPT_DRV_TYPE, DRIVE_VC1541II);
            setConfigItem(OPT_DRV_RAM, DRVRAM_6000_7FFF);
            setConfigItem(OPT_DRV_PARCABLE, PAR_CABLE_DOLPHIN3);
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
        
        mem.C64Component::_dump(Category::BankMap, os);
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
    
    if (category == Category::Disk) {
        
        if (hasDisk()) {
            disk->dump(Category::State, os);
        } else {
            os << "No disk";
        }
    }
}

isize
Drive::_size()
{
    util::SerCounter counter;

    applyToPersistentItems(counter);
    applyToResetItems(counter);

    // Add the size of the boolean indicating whether a disk is inserted
    counter.count += sizeof(bool);

    if (hasDisk()) {

        // Add the disk size
        disk->applyToPersistentItems(counter);
    }

    return counter.count;
}

u64
Drive::_checksum()
{
    util::SerChecker checker;

    applyToPersistentItems(checker);
    applyToResetItems(checker);

    // TODO: Get checksum from disk
    
    return checker.hash;
}

isize
Drive::_load(const u8 *buffer)
{
    util::SerReader reader(buffer);
    isize result;
    
    // Read own state
    applyToPersistentItems(reader);
    applyToResetItems(reader);

    // Check if the snapshot includes a disk
    bool diskInSnapshot; reader << diskInSnapshot;

    if (diskInSnapshot) {
        disk = std::make_unique<Disk>(reader);
    } else {
        disk = nullptr;
    }

    // Compute the number of read bytes and return
    result = isize(reader.ptr - buffer);
    trace(SNP_DEBUG, "Recreated from %ld bytes\n", result);
    return result;
}

isize
Drive::_save(u8 *buffer)
{
    util::SerWriter writer(buffer);
    isize result;
    
    // Write own state
    applyToPersistentItems(writer);
    applyToResetItems(writer);

    // Indicate whether this drive has a disk is inserted
    writer << hasDisk();

    // If yes, write the disk
    if (hasDisk()) disk->applyToPersistentItems(writer);
    
    // Compute the number of written bytes and return
    result = isize(writer.ptr - buffer);
    trace(SNP_DEBUG, "Serialized to %ld bytes\n", result);
    return result;
}

void
Drive::_run()
{

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
            if (iec.isDirtyDriveSide) iec.updateIecLinesDriveSide();

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
            byteReadyCounter = sync ? (byteReadyCounter + 1) % 8 : 0;
            
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
    if (!redLED && b) {
        
        redLED = true;
        wakeUp();
        msgQueue.put(MSG_DRIVE_LED_ON, deviceNr);
        return;
    }
    if (redLED && !b) {
        
        redLED = false;
        wakeUp();
        msgQueue.put(MSG_DRIVE_LED_OFF, deviceNr);
        return;
    }
}

void
Drive::setRotating(bool b)
{
    if (spinning == b) return;
    
    spinning = b;
    msgQueue.put(b ? MSG_DRIVE_MOTOR_ON : MSG_DRIVE_MOTOR_OFF, deviceNr);
    iec.updateTransferStatus();
}

void
Drive::wakeUp()
{
    if (isIdle()) {
        
        trace(DRV_DEBUG, "Exiting power-safe mode\n");
        msgQueue.put(MSG_DRIVE_POWER_SAVE_OFF, deviceNr);
        idleCounter = 0;
        needsEmulation = true;
    }
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

    msgQueue.put(MSG_DRIVE_STEP, deviceNr, halftrack, config.stepVolume, config.pan);
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

    msgQueue.put(MSG_DRIVE_STEP, deviceNr, halftrack, config.stepVolume, config.pan);
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
    if (hasDisk()) disk->setModified(value);
    msgQueue.put(value ? MSG_DISK_UNSAVED : MSG_DISK_SAVED, deviceNr);
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
            
            // Initiate the disk change procedure
            wakeUp();
            diskToInsert = std::move(disk);
            diskChangeCounter = 1;
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
            wakeUp();
            diskChangeCounter = 1;
        }
    }
}

void
Drive::vsyncHandler()
{
    // Only proceed if the drive is connected and switched on
    if (!config.connected || !config.switchedOn) return;

    // Emulate an ongoing disk state transition
    if (diskChangeCounter) {
        
        wakeUp();
        
        if (--diskChangeCounter == 0) {
            executeStateTransition();
        }
        return;
    }

    // Check if we sould enter power-safe mode
    if (!spinning && config.powerSave) {
        if (++idleCounter == powerSafeThreshold) {

            trace(DRV_DEBUG, "Entering power-save mode\n");
            needsEmulation = false;
            msgQueue.put(MSG_DRIVE_POWER_SAVE_ON, deviceNr);
        }
    }
}

void
Drive::executeStateTransition()
{
    switch (insertionStatus) {
            
        case DISK_FULLY_INSERTED:
        {
            trace(DSKCHG_DEBUG, "FULLY_INSERTED -> PARTIALLY_EJECTED\n");

            // Pull the disk half out (blocks the light barrier)
            insertionStatus = DISK_PARTIALLY_EJECTED;
            
            // Make sure the drive can no longer read from this disk
            disk->clearDisk();
            
            // Schedule the next transition
            diskChangeCounter = config.ejectDelay;
            return;
        }
        case DISK_PARTIALLY_EJECTED:
        {
            trace(DSKCHG_DEBUG, "PARTIALLY_EJECTED -> FULLY_EJECTED\n");

            // Take the disk out (unblocks the light barrier)
            insertionStatus = DISK_FULLY_EJECTED;
            
            // Inform listeners
            msgQueue.put(MSG_DISK_EJECT, deviceNr, halftrack, config.stepVolume, config.pan);
            
            // Schedule the next transition
            diskChangeCounter = config.swapDelay;
            return;
        }
        case DISK_FULLY_EJECTED:
        {
            trace(DSKCHG_DEBUG, "FULLY_EJECTED -> PARTIALLY_INSERTED\n");

            // Only proceed if a new disk is waiting for insertion
            if (!diskToInsert) return;
            
            // Push the new disk half in (blocks the light barrier)
            insertionStatus = DISK_PARTIALLY_INSERTED;
            
            // Schedule the next transition
            diskChangeCounter = config.insertDelay;
            return;
        }
        case DISK_PARTIALLY_INSERTED:
        {
            trace(DSKCHG_DEBUG, "PARTIALLY_INSERTED -> FULLY_INSERTED\n");

            // Fully insert the disk (unblocks the light barrier)
            insertionStatus = DISK_FULLY_INSERTED;
            disk = std::move(diskToInsert);
            
            // Inform listeners
            msgQueue.put(MSG_DISK_INSERT, deviceNr, halftrack, config.stepVolume, config.pan);
            return;
        }
        default:
            fatalError;
    }
}

}
