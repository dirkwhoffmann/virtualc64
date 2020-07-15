// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class Configuration {
    
    var parent: MyController!
    var c64: C64Proxy { return parent.c64 }
    var renderer: Renderer { return parent.renderer }
    var gamePadManager: GamePadManager { return parent.gamePadManager }
    
    //
    // Rom settings
    //
    
    var basicRomURL: URL = URL(fileURLWithPath: "/")
    var charRomURL: URL = URL(fileURLWithPath: "/")
    var kernalRomURL: URL = URL(fileURLWithPath: "/")
    var vc1541RomURL: URL = URL(fileURLWithPath: "/")

    //
    // Hardware settings
    //
    
    var vicRevision: Int {
        get { return c64.getConfig(OPT_VIC_REVISION) }
        set { c64.configure(OPT_VIC_REVISION, value: newValue) }
    }
    
    var grayDotBug: Bool {
        get { return c64.getConfig(OPT_GRAY_DOT_BUG) != 0}
        set { c64.configure(OPT_GRAY_DOT_BUG, enable: newValue) }
    }

    var glueLogic: Int {
        get { return c64.getConfig(OPT_GLUE_LOGIC) }
        set { c64.configure(OPT_GLUE_LOGIC, value: newValue) }
    }

    var ciaRevision: Int {
        get { return c64.getConfig(OPT_CIA_REVISION) }
        set { c64.configure(OPT_VIC_REVISION, value: newValue) }
    }

    var timerBBug: Bool {
        get { return c64.getConfig(OPT_TIMER_B_BUG) != 0}
        set { c64.configure(OPT_TIMER_B_BUG, enable: newValue) }
    }

    var sidRevision: Int {
        get { return c64.getConfig(OPT_SID_REVISION) }
        set { c64.configure(OPT_SID_REVISION, value: newValue) }
    }

    init(with controller: MyController) { parent = controller }
    
    //
    // Roms
    //
    
    func loadRomUserDefaults() {
        
        c64.suspend()
        
        if let url = UserDefaults.basicRomUrl {
            track("Seeking Basic Rom")
            c64.loadRom(url)
        }
        if let url = UserDefaults.kernalRomUrl {
            track("Seeking Kernal Rom")
            c64.loadRom(url)
        }
        if let url = UserDefaults.charRomUrl {
            track("Seeking Character Rom")
            c64.loadRom(url)
        }
        if let url = UserDefaults.vc1541RomUrl {
            track("Seeking VC1541 Rom")
            c64.loadRom(url)
        }
        
        c64.resume()
    }
    
    func saveRomUserDefaults() {
        
        let fm = FileManager.default
        
        c64.suspend()
        
        if let url = UserDefaults.basicRomUrl {
            track("Saving Basic Rom")
            try? fm.removeItem(at: url)
            c64.saveBasicRom(url)
        }
        if let url = UserDefaults.charRomUrl {
            track("Saving Character Rom")
            try? fm.removeItem(at: url)
            c64.saveCharRom(url)
        }
        if let url = UserDefaults.kernalRomUrl {
            track("Saving Kernal Rom")
            try? fm.removeItem(at: url)
            c64.saveKernalRom(url)
        }
        if let url = UserDefaults.vc1541RomUrl {
            track("Saving VC1541 Rom")
            try? fm.removeItem(at: url)
            c64.saveVC1541Rom(url)
        }

        c64.resume()
    }
    
    //
    // Hardware
    //
    
    func loadHardwareDefaults(_ defaults: HardwareDefaults) {
        
        c64.suspend()
        
        vicRevision = defaults.vicRevision.rawValue
        grayDotBug = defaults.grayDotBug
        glueLogic = defaults.glueLogic.rawValue
        ciaRevision = defaults.ciaRevision.rawValue
        timerBBug = defaults.timerBBug
        sidRevision = defaults.sidRevision.rawValue
    
        c64.resume()
    }
    
    func loadHardwareUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        c64.suspend()
        
        vicRevision = defaults.integer(forKey: Keys.vicRevision)
        grayDotBug = defaults.bool(forKey: Keys.grayDotBug)
        glueLogic = defaults.integer(forKey: Keys.glueLogic)
        ciaRevision = defaults.integer(forKey: Keys.ciaRevision)
        timerBBug = defaults.bool(forKey: Keys.timerBBug)
        sidRevision = defaults.integer(forKey: Keys.sidRevision)

        c64.resume()
    }
    
    func saveHardwareUserDefaults() {
        
        track()
        
        let defaults = UserDefaults.standard

        defaults.set(vicRevision, forKey: Keys.vicRevision)
        defaults.set(grayDotBug, forKey: Keys.grayDotBug)
        defaults.set(glueLogic, forKey: Keys.glueLogic)
        defaults.set(ciaRevision, forKey: Keys.ciaRevision)
        defaults.set(timerBBug, forKey: Keys.timerBBug)
        defaults.set(sidRevision, forKey: Keys.sidRevision)
    }
}
