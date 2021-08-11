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
#include "IO.h"

Drive::Drive(DriveID id, C64 &ref) : SubComponent(ref), deviceNr(id)
{
    assert(deviceNr == DRIVE8 || deviceNr == DRIVE9);
	
    subComponents = std::vector <C64Component *> {
        
        &mem,
        &cpu,
        &via1,
        &via2,
        &disk
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
    disk.clearDisk();
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
    DriveConfig defaults = getDefaultConfig();

    setConfigItem(OPT_DRV_AUTO_CONFIG, deviceNr, defaults.autoConfig);

    setConfigItem(OPT_DRV_TYPE, deviceNr, defaults.type);
    setConfigItem(OPT_DRV_RAM, deviceNr, defaults.ram);
    setConfigItem(OPT_DRV_PARCABLE, deviceNr, defaults.parCable);
    setConfigItem(OPT_DRV_CONNECT, deviceNr, deviceNr == DRIVE8);
    setConfigItem(OPT_DRV_POWER_SWITCH, deviceNr, defaults.switchedOn);
    setConfigItem(OPT_DRV_POWER_SAVE, deviceNr, defaults.powerSave);

    setConfigItem(OPT_DRV_EJECT_DELAY, deviceNr, defaults.ejectDelay);
    setConfigItem(OPT_DRV_SWAP_DELAY, deviceNr, defaults.swapDelay);
    setConfigItem(OPT_DRV_INSERT_DELAY, deviceNr, defaults.insertDelay);
    
    setConfigItem(OPT_DRV_PAN, deviceNr, defaults.pan);
    setConfigItem(OPT_DRV_POWER_VOL, deviceNr, defaults.powerVolume);
    setConfigItem(OPT_DRV_STEP_VOL, deviceNr, defaults.stepVolume);
    setConfigItem(OPT_DRV_INSERT_VOL, deviceNr, defaults.insertVolume);
    setConfigItem(OPT_DRV_EJECT_VOL, deviceNr, defaults.ejectVolume);
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
            assert(false);
            return 0;
    }
}

void
Drive::setConfigItem(Option option, i64 value)
{
    switch (option) {

        case OPT_DRV_AUTO_CONFIG:
        case OPT_DRV_POWER_SAVE:
        case OPT_DRV_EJECT_DELAY:
        case OPT_DRV_SWAP_DELAY:
        case OPT_DRV_INSERT_DELAY:
        case OPT_DRV_POWER_VOL:
        case OPT_DRV_STEP_VOL:
        case OPT_DRV_INSERT_VOL:
        case OPT_DRV_EJECT_VOL:
        case OPT_DRV_PAN:
            
            setConfigItem(option, DRIVE8, value);
            setConfigItem(option, DRIVE9, value);
            return;

        default:
            return;
    }
}

void
Drive::setConfigItem(Option option, long id, i64 value)
{
    if (id != deviceNr) return;
    
    switch (option) {

        case OPT_DRV_AUTO_CONFIG:
        {
            suspended {
                
                config.autoConfig = value;
                if (value) autoConfigure();
            }
            return;
        }
        case OPT_DRV_TYPE:
        {
            if (!DriveTypeEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, DriveTypeEnum::keyList());
            }
            
            config.type = (DriveType)value;
            return;
        }
        case OPT_DRV_RAM:
        {
            if (!DriveRamEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, DriveRamEnum::keyList());
            }
            
            suspended {
                
                config.ram = (DriveRam)value;
                mem.updateBankMap();
            }
            return;
        }
        case OPT_DRV_PARCABLE:
        {
            if (!ParCableTypeEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, ParCableTypeEnum::keyList());
            }
            
            suspended {

                config.parCable = (ParCableType)value;
                mem.updateBankMap();
            }
            return;
        }
        case OPT_DRV_CONNECT:
        {
            if (value && !c64.hasRom(ROM_TYPE_VC1541)) {
                warn("Can't connect drive (ROM missing).\n");
                return;
            }
            suspended {

                config.connected = value;
                reset(true);
            }
            msgQueue.put(value ? MSG_DRIVE_CONNECT : MSG_DRIVE_DISCONNECT, deviceNr);
            return;
        }
        case OPT_DRV_POWER_SWITCH:
        {
            if (value && !isPoweredOn()) {
                warn("Can't switch drive on (not connected).\n");
                // throw VCError(ERROR_DRV_NOT_CONNECTED);
                return;
            }
            suspended {
                
                config.switchedOn = value;
                reset(true);
            }
            msgQueue.put(value ? MSG_DRIVE_POWER_ON : MSG_DRIVE_POWER_OFF, deviceNr);
            return;
        }
        case OPT_DRV_POWER_SAVE:
        {
            config.powerSave = value;
            wakeUp();
            return;
        }
        case OPT_DRV_EJECT_DELAY:
        {
            config.ejectDelay = value;
            return;
        }
        case OPT_DRV_SWAP_DELAY:
        {
            config.swapDelay = value;
            return;
        }
        case OPT_DRV_INSERT_DELAY:
        {
            config.insertDelay = value;
            return;
        }
        case OPT_DRV_PAN:
        {
            config.pan = value;
            return;
        }
        case OPT_DRV_POWER_VOL:
        {
            value = std::clamp(value, 0LL, 100LL);

            config.powerVolume = value;
            return;
        }
        case OPT_DRV_STEP_VOL:
        {
            value = std::clamp(value, 0LL, 100LL);

            config.stepVolume = value;
            return;
        }
        case OPT_DRV_EJECT_VOL:
        {
            value = std::clamp(value, 0LL, 100LL);
            
            config.ejectVolume = value;
            return;
        }
        case OPT_DRV_INSERT_VOL:
        {
            value = std::clamp(value, 0LL, 100LL);
            
            config.insertVolume = value;
            return;
        }
        default:
            return;
    }
}

void
Drive::autoConfigure()
{
    switch (RomFile::identifier(mem.romFNV64())) {
            
        case VC1541C_01:
        case VC1541C_02:
            
            debug(CNF_DEBUG, "autoConfigure: VC1541C\n");
            setConfigItem(OPT_DRV_TYPE, deviceNr, DRIVE_VC1541C);
            setConfigItem(OPT_DRV_RAM, deviceNr, DRVRAM_NONE);
            setConfigItem(OPT_DRV_PARCABLE, deviceNr, PAR_CABLE_NONE);
            break;
            
        case VC1541_II_1987:
        case VC1541_II_NEWTRONIC:
        case VC1541_II_RELOC_PATCH:
        case VC1541_II_JIFFY:
        case VC1541_II_JIFFY_V600:
        case VC1541_64ER_V3:

            debug(CNF_DEBUG, "autoConfigure: VC1541 II\n");
            setConfigItem(OPT_DRV_TYPE, deviceNr, DRIVE_VC1541II);
            setConfigItem(OPT_DRV_RAM, deviceNr, DRVRAM_NONE);
            setConfigItem(OPT_DRV_PARCABLE, deviceNr, PAR_CABLE_NONE);
            break;

        case VC1541_SPEEDDOS_PLUS:
        case VC1541_SPEEDDOS_27:

            debug(CNF_DEBUG, "autoConfigure: VC1541 SpeedDOS\n");
            setConfigItem(OPT_DRV_TYPE, deviceNr, DRIVE_VC1541II);
            setConfigItem(OPT_DRV_RAM, deviceNr, DRVRAM_NONE);
            setConfigItem(OPT_DRV_PARCABLE, deviceNr, PAR_CABLE_STANDARD);
            break;

        case VC1541_DOLPHIN_20:
        case VC1541_DOLPHIN_20_SLVDR:

            debug(CNF_DEBUG, "autoConfig: Dolphin DOS\n");
            setConfigItem(OPT_DRV_TYPE, deviceNr, DRIVE_VC1541II);
            setConfigItem(OPT_DRV_RAM, deviceNr, DRVRAM_8000_9FFF);
            setConfigItem(OPT_DRV_PARCABLE, deviceNr, PAR_CABLE_STANDARD);
            break;

        case VC1541_DOLPHIN_30:
        case VC1541_DOLPHIN_30_SLVDR:

            debug(CNF_DEBUG, "autoConfig: Dolphin DOS 3\n");
            setConfigItem(OPT_DRV_TYPE, deviceNr, DRIVE_VC1541II);
            setConfigItem(OPT_DRV_RAM, deviceNr, DRVRAM_6000_7FFF);
            setConfigItem(OPT_DRV_PARCABLE, deviceNr, PAR_CABLE_DOLPHIN3);
            break;
            
        default:
            
            debug( CNF_DEBUG, "AutoConfig: Rom not recognized\n");
    }
}

void
Drive::_dump(dump::Category category, std::ostream& os) const
{
    using namespace util;
    
    if (category & dump::Config) {
    
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
        
        mem.C64Component::_dump(dump::BankMap, os);
    }
    
    if (category & dump::State) {
         
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
    
    if (category & dump::BankMap) {

        mem.dump(dump::BankMap, os);
    }
    
    if (category & dump::Disk) {
        
        if (hasDisk()) {
            disk.dump(dump::State, os);
        } else {
            os << "No disk";
        }
    }
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
            u64 cycle = ++cpu.cycle;
            cpu.executeOneCycle();
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
        if (readMode() && readBitFromHead()) {
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
            if (writeMode() && !getLightBarrier()) {
                writeBitToHead(writeShiftreg & 0x80);
                disk.setModified(true);
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
Drive::setZone(u8 value)
{
    assert(value < 4);
    
    if (value != zone) {
        trace(DRV_DEBUG, "Switching from disk zone %d to disk zone %d\n", zone, value);
        zone = value;
    }
}

void
Drive::setRedLED(bool b)
{
    if (!redLED && b) {
        redLED = true;
        wakeUp();
        msgQueue.put(MSG_DRIVE_LED_ON, deviceNr);
        
    } else if (redLED && !b) {
        redLED = false;
        wakeUp();
        msgQueue.put(MSG_DRIVE_LED_OFF, deviceNr);
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

        float position = (float)offset / (float)disk.lengthOfHalftrack(halftrack);
        halftrack++;
        offset = (HeadPos)(position * disk.lengthOfHalftrack(halftrack));
        
        trace(DRV_DEBUG, "Moving head up to halftrack %d (track %2.1f) (offset %d)\n",
              halftrack, (halftrack + 1) / 2.0, offset);
        trace(DRV_DEBUG, "Halftrack %d has %d bits.\n", halftrack, disk.lengthOfHalftrack(halftrack));
    }
   
    assert(disk.isValidHeadPos(halftrack, offset));
    
    msgQueue.put(MSG_DRIVE_STEP,
                   config.pan << 24 | config.stepVolume << 16 | halftrack << 8 | deviceNr);
}

void
Drive::moveHeadDown()
{
    if (halftrack > 1) {
        float position = (float)offset / (float)disk.lengthOfHalftrack(halftrack);
        halftrack--;
        offset = (HeadPos)(position * disk.lengthOfHalftrack(halftrack));
        
        trace(DRV_DEBUG, "Moving head down to halftrack %d (track %2.1f)\n",
              halftrack, (halftrack + 1) / 2.0);
        trace(DRV_DEBUG, "Halftrack %d has %d bits.\n", halftrack, disk.lengthOfHalftrack(halftrack));
    }
    
    assert(disk.isValidHeadPos(halftrack, offset));
    
    msgQueue.put(MSG_DRIVE_STEP,
                   config.pan << 24 | config.stepVolume << 16 | halftrack << 8 | deviceNr);
}

void
Drive::setModifiedDisk(bool value)
{
    disk.setModified(value);
    msgQueue.put(value ? MSG_DISK_UNSAVED : MSG_DISK_SAVED, deviceNr);
}

void
Drive::insertDisk(const string &path, bool wp)
{
    auto disk = Disk::make(c64, path);
    
    if (disk) {
        insertDisk(disk, wp);
    }
}

void
Drive::insertDisk(Disk *otherDisk, bool wp)
{
    debug(DSKCHG_DEBUG, "insertDisk(otherDisk %p)\n", otherDisk);

    suspended {
        
        if (!diskToInsert) {
            
            // Initiate the disk change procedure
            wakeUp();
            diskToInsert = otherDisk;
            diskToInsertWP = wp;
            diskChangeCounter = 1;
        }
    }
}

void
Drive::insertNewDisk(DOSType fsType)
{
    PETName<16> name = PETName<16>("NEW DISK");
    insertNewDisk(fsType, name);
}

void
Drive::insertNewDisk(DOSType fsType, PETName<16> name)
{
    Disk *newDisk = Disk::make(c64, fsType, name);
    insertDisk(newDisk, false);
}

void
Drive::insertFileSystem(FSDevice *device, bool wp)
{
    debug(DSKCHG_DEBUG, "insertFileSystem(%p)\n", device);
    insertDisk(Disk::makeWithFileSystem(c64, *device), wp);
}

void
Drive::insertG64(G64File *g64, bool wp)
{
    debug(DSKCHG_DEBUG, "insertG64(%p)\n", g64);
    insertDisk(Disk::makeWithG64(c64, g64), wp);
}

void
Drive::insertDisk(AnyCollection &collection, bool wp)
{
    debug(DSKCHG_DEBUG, "insertDisk(collection)\n");
    insertDisk(Disk::makeWithCollection(c64, collection), wp);
}

void 
Drive::ejectDisk()
{
    debug(DSKCHG_DEBUG, "ejectDisk()\n");

    suspended {
        
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
            disk.clearDisk();
            
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
            msgQueue.put(MSG_DISK_EJECT,
                           config.pan << 24 | config.ejectVolume << 16 | halftrack << 8 | deviceNr);
            
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

            // Copy the disk contents
            isize size = diskToInsert->size();
            u8 *buffer = new u8[size];
            diskToInsert->save(buffer);
            disk.load(buffer);
            delete[] buffer;
            diskToInsert = nullptr;
            
            // Enable or disable the write protection
            disk.setWriteProtection(diskToInsertWP);

            // Inform listeners
            msgQueue.put(MSG_DISK_INSERT,
                           config.pan << 24 | config.insertVolume << 16 | halftrack << 8 | deviceNr);
            return;
        }
        default:
            assert(false);
    }
}
