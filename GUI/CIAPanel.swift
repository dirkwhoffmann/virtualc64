// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension Inspector {
    
    private var selectedCia: Int { return ciaSelector.indexOfSelectedItem }
    private var ciaProxy: CIAProxy { return selectedCia == 0 ? c64.cia1 : c64.cia2 }

    private func cacheCIA() {

        ciaInfo = ciaProxy.getInfo()
    }

    func refreshCIA(count: Int = 0, full: Bool = false) {

        cacheCIA()

        if full {
            let cia1 = selectedCia == 0

            ciaPA7.title = "PA7: " + (cia1 ? "C0,JB0"    : "VA14")
            ciaPA6.title = "PA6: " + (cia1 ? "C1,JB1"    : "VA15")
            ciaPA5.title = "PA5: " + (cia1 ? "C2,JB2"    : "User M")
            ciaPA4.title = "PA4: " + (cia1 ? "C3,JB4"    : "ATN")
            ciaPA3.title = "PA3: " + (cia1 ? "C4,BTNB"   : "CLK")
            ciaPA2.title = "PA2: " + (cia1 ? "C5"        : "DATA")
            ciaPA1.title = "PA1: " + (cia1 ? "C6"        : "CLK")
            ciaPA0.title = "PA0: " + (cia1 ? "C"         : "DATA")

            ciaPB7.title = "PB7: " + (cia1 ? "R0, JOYA0" : "User C")
            ciaPB6.title = "PB6: " + (cia1 ? "R1, JOYA1" : "User D")
            ciaPB5.title = "PB5: " + (cia1 ? "R2, JOYA2" : "User E")
            ciaPB4.title = "PB4: " + (cia1 ? "R3, JOYA3" : "User F")
            ciaPB3.title = "PB3: " + (cia1 ? "R4, BTNA"  : "User H")
            ciaPB2.title = "PB2: " + (cia1 ? "R5"        : "User J")
            ciaPB1.title = "PB1: " + (cia1 ? "R6"        : "User K")
            ciaPB0.title = "PB0: " + (cia1 ? "R7"        : "User L")

            let elements = [ ciaPRAbinary: fmt8b,
                             ciaDDRAbinary: fmt8b,
                             ciaPRBbinary: fmt8b,
                             ciaDDRBbinary: fmt8b,
                             ciaICRbinary: fmt8b,
                             ciaIMRbinary: fmt8b,
                             ciaPRA: fmt8,
                             ciaDDRA: fmt8,
                             ciaPRB: fmt8,
                             ciaDDRB: fmt8,
                             ciaTodHours: fmt8,
                             ciaTodMinutes: fmt8,
                             ciaTodSeconds: fmt8,
                             ciaTodTenth: fmt8,
                             ciaAlarmHours: fmt8,
                             ciaAlarmMinutes: fmt8,
                             ciaAlarmSeconds: fmt8,
                             ciaAlarmTenth: fmt8,
                             ciaIMR: fmt8,
                             ciaICR: fmt8,
                             ciaSDR: fmt8,
                             ciaSSR: fmt8b,
                             ciaTA: fmt16,
                             ciaTAlatch: fmt16,
                             ciaTB: fmt16,
                             ciaTBlatch: fmt16 ]

            for (c, f) in elements { assignFormatter(f, c!) }
        }

        ciaTA.intValue = Int32(ciaInfo!.timerA.count)
        ciaTAlatch.intValue = Int32(ciaInfo!.timerA.latch)
        ciaTArunning.state = ciaInfo!.timerA.running ? .on : .off
        ciaTAtoggle.state = ciaInfo!.timerA.toggle ? .on : .off
        ciaTApbout.state = ciaInfo!.timerA.pbout ? .on : .off
        ciaTAoneShot.state = ciaInfo!.timerA.oneShot ? .on : .off

        ciaTB.intValue = Int32(ciaInfo!.timerB.count)
        ciaTBlatch.intValue = Int32(ciaInfo!.timerB.latch)
        ciaTBrunning.state = ciaInfo!.timerB.running ? .on : .off
        ciaTBtoggle.state = ciaInfo!.timerB.toggle ? .on : .off
        ciaTBpbout.state = ciaInfo!.timerB.pbout ? .on : .off
        ciaTBoneShot.state = ciaInfo!.timerB.oneShot ? .on : .off

        ciaPRA.intValue = Int32(ciaInfo!.portA.reg)
        ciaPRAbinary.intValue = Int32(ciaInfo!.portA.reg)
        ciaDDRA.intValue = Int32(ciaInfo!.portA.dir)
        ciaDDRAbinary.intValue = Int32(ciaInfo!.portA.dir)

        var bits = ciaInfo!.portA.port
        ciaPA7.state = (bits & 0b10000000) != 0 ? .on : .off
        ciaPA6.state = (bits & 0b01000000) != 0 ? .on : .off
        ciaPA5.state = (bits & 0b00100000) != 0 ? .on : .off
        ciaPA4.state = (bits & 0b00010000) != 0 ? .on : .off
        ciaPA3.state = (bits & 0b00001000) != 0 ? .on : .off
        ciaPA2.state = (bits & 0b00000100) != 0 ? .on : .off
        ciaPA1.state = (bits & 0b00000010) != 0 ? .on : .off
        ciaPA0.state = (bits & 0b00000001) != 0 ? .on : .off

        ciaPRB.intValue = Int32(ciaInfo!.portB.reg)
        ciaPRBbinary.intValue = Int32(ciaInfo!.portB.reg)
        ciaPRB.intValue = Int32(ciaInfo!.portB.reg)
        ciaDDRB.intValue = Int32(ciaInfo!.portB.dir)

        bits = ciaInfo!.portB.port
        ciaPB7.state = (bits & 0b10000000) != 0 ? .on : .off
        ciaPB6.state = (bits & 0b01000000) != 0 ? .on : .off
        ciaPB5.state = (bits & 0b00100000) != 0 ? .on : .off
        ciaPB4.state = (bits & 0b00010000) != 0 ? .on : .off
        ciaPB3.state = (bits & 0b00001000) != 0 ? .on : .off
        ciaPB2.state = (bits & 0b00000100) != 0 ? .on : .off
        ciaPB1.state = (bits & 0b00000010) != 0 ? .on : .off
        ciaPB0.state = (bits & 0b00000001) != 0 ? .on : .off

        ciaICR.intValue = Int32(ciaInfo!.icr)
        ciaICRbinary.intValue = Int32(ciaInfo!.icr)
        ciaIMR.intValue = Int32(ciaInfo!.imr)
        ciaIMRbinary.intValue = Int32(ciaInfo!.imr)
        ciaIntLineLow.state = ciaInfo!.intLine ? .off : .on

        ciaTodHours.intValue = Int32(ciaInfo!.tod.time.hours)
        ciaTodMinutes.intValue = Int32(ciaInfo!.tod.time.minutes)
        ciaTodSeconds.intValue = Int32(ciaInfo!.tod.time.seconds)
        ciaTodTenth.intValue = Int32(ciaInfo!.tod.time.tenth)
        ciaTodIntEnable.state = ciaInfo!.todIntEnable ? .on : .off
        ciaAlarmHours.intValue = Int32(ciaInfo!.tod.alarm.hours)
        ciaAlarmMinutes.intValue = Int32(ciaInfo!.tod.alarm.minutes)
        ciaAlarmSeconds.intValue = Int32(ciaInfo!.tod.alarm.seconds)
        ciaAlarmTenth.intValue = Int32(ciaInfo!.tod.alarm.tenth)

        ciaSDR.intValue = Int32(ciaInfo!.sdr)
        ciaSSR.intValue = Int32(ciaInfo!.ssr)

        let idlePercentage = Int(ciaInfo!.idlePercentage * 100)
        ciaIdleCycles.stringValue = "\(ciaInfo!.idleCycles) cycles"
        ciaIdleLevel.integerValue = idlePercentage
        ciaIdleLevelText.stringValue = "\(idlePercentage) %"
    }

    @IBAction func selectCIAAction(_ sender: Any!) {

        fullRefresh()
    }
}

/*
extension MyController {
    
    func refreshCIA() {
        
        var info: CIAInfo
        
        if selectedCia == 0 {
            info = c64.cia1.getInfo()
            ciaIntLineLow.title = "IRQ line active"
        } else {
            info = c64.cia2.getInfo()
            ciaIntLineLow.title = "NMI line active"
        }

        ciaPA.intValue = Int32(info.portA.port)
        ciaPAbinary.intValue = Int32(info.portA.port)
        ciaPRA.intValue = Int32(info.portA.reg)
        ciaDDRA.intValue = Int32(info.portA.dir)
        
        ciaPB.intValue = Int32(info.portB.port)
        ciaPBbinary.intValue = Int32(info.portB.port)
        ciaPRB.intValue = Int32(info.portB.reg)
        ciaDDRB.intValue = Int32(info.portB.dir)
        
        ciaTimerA.intValue = Int32(info.timerA.count)
        ciaLatchA.intValue = Int32(info.timerA.latch)
        ciaRunningA.state = info.timerA.running ? .on : .off
        ciaToggleA.state = info.timerA.toggle ? .on : .off
        ciaPBoutA.state = info.timerA.pbout ? .on : .off
        ciaOneShotA.state = info.timerA.oneShot ? .on : .off
        
        ciaTimerB.intValue = Int32(info.timerB.count)
        ciaLatchB.intValue = Int32(info.timerB.latch)
        ciaRunningB.state = info.timerB.running ? .on : .off
        ciaToggleB.state = info.timerB.toggle ? .on : .off
        ciaPBoutB.state = info.timerB.pbout ? .on : .off
        ciaOneShotB.state = info.timerB.oneShot ? .on : .off
        
        todHours.intValue = Int32(info.tod.time.hours)
        todMinutes.intValue = Int32(info.tod.time.minutes)
        todSeconds.intValue = Int32(info.tod.time.seconds)
        todTenth.intValue = Int32(info.tod.time.tenth)
        todIntEnable.state = info.todIntEnable ? .on : .off
        alarmHours.intValue = Int32(info.tod.alarm.hours)
        alarmMinutes.intValue = Int32(info.tod.alarm.minutes)
        alarmSeconds.intValue = Int32(info.tod.alarm.seconds)
        alarmTenth.intValue = Int32(info.tod.alarm.tenth)
        
        ciaIcr.intValue = Int32(info.icr)
        ciaIcrBinary.intValue = Int32(info.icr)
        ciaImr.intValue = Int32(info.imr)
        ciaImrBinary.intValue = Int32(info.imr)
        ciaIntLineLow.state = info.intLine ? .off : .on
    }

    private var selectedCia: Int { return ciaSelector.indexOfSelectedItem }
    
    @IBAction func selectCIAAction(_ sender: Any!) {
        
        refreshCIA()
    }
    
}
 */
