// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

/* Configuration
 *
 * This class stores all items that are specific to an individual emulator
 * instance. Each instance keeps its own object of this class inside the
 * document controller.
 *
 * See class "Preferences" for shared settings.
 */

class Configuration {
    
    var parent: MyController!
    var c64: C64Proxy { return parent.c64 }
    var renderer: Renderer { return parent.renderer }
    var gamePadManager: GamePadManager { return parent.gamePadManager }
    
    //
    // Roms
    //
    
    //
    // Hardware
    //
    
    var vicRevision: Int {
        get { return c64.getConfig(OPT_VIC_REVISION) }
        set { c64.configure(OPT_VIC_REVISION, value: newValue) }
    }
    
    var vicGrayDotBug: Bool {
        get { return c64.getConfig(OPT_GRAY_DOT_BUG) != 0}
        set { c64.configure(OPT_GRAY_DOT_BUG, enable: newValue) }
    }

    var ciaRevision: Int {
        get { return c64.getConfig(OPT_CIA_REVISION) }
        set { c64.configure(OPT_CIA_REVISION, value: newValue) }
    }

    var ciaTimerBBug: Bool {
        get { return c64.getConfig(OPT_TIMER_B_BUG) != 0}
        set { c64.configure(OPT_TIMER_B_BUG, enable: newValue) }
    }

    var sidRevision: Int {
        get { return c64.getConfig(OPT_SID_REVISION) }
        set { c64.configure(OPT_SID_REVISION, value: newValue) }
    }
    
    var sidFilter: Bool {
        get { return c64.getConfig(OPT_SID_FILTER) != 0 }
        set { c64.configure(OPT_SID_FILTER, enable: newValue) }
    }
    
    var sidEngine: Int {
        get { return c64.getConfig(OPT_SID_ENGINE) }
        set { c64.configure(OPT_SID_ENGINE, value: newValue) }
    }
    
    var sidSampling: Int {
        get { return c64.getConfig(OPT_SID_SAMPLING) }
        set { c64.configure(OPT_SID_SAMPLING, value: newValue) }
    }

    var glueLogic: Int {
        get { return c64.getConfig(OPT_GLUE_LOGIC) }
        set { c64.configure(OPT_GLUE_LOGIC, value: newValue) }
    }
    
    var ramPattern: Int {
        get { return c64.getConfig(OPT_RAM_PATTERN) }
        set { c64.configure(OPT_RAM_PATTERN, value: newValue) }
    }

    var drive8Connected: Bool {
        get { return c64.getConfig(OPT_DRIVE_CONNECT, drive: DRIVE8) != 0 }
        set { c64.configure(OPT_DRIVE_CONNECT, drive: DRIVE8, enable: newValue )}
    }
    
    var drive8Type: Int {
        get { return c64.getConfig(OPT_DRIVE_TYPE, drive: DRIVE8) }
        set { c64.configure(OPT_DRIVE_TYPE, drive: DRIVE8, value: newValue )}
    }
    
    var drive9Connected: Bool {
        get { return c64.getConfig(OPT_DRIVE_CONNECT, drive: DRIVE9) != 0 }
        set { c64.configure(OPT_DRIVE_CONNECT, drive: DRIVE9, enable: newValue )}
    }
    
    var drive9Type: Int {
        get { return c64.getConfig(OPT_DRIVE_TYPE, drive: DRIVE9) }
        set { c64.configure(OPT_DRIVE_TYPE, drive: DRIVE9, value: newValue )}
    }
    
    // Ports
    var gameDevice1 = -1
    var gameDevice2 = -1
    /*
    var gameDevice1 = HardwareDefaults.A500.gameDevice1 {
        didSet {
            
            // Try to connect the device
            gamePadManager.connect(slot: gameDevice1, port: 1)
            gamePadManager.listDevices()
            
            // Read back the real connection status
            let device1 = gamePadManager.getSlot(port: 1)
            let device2 = gamePadManager.getSlot(port: 2)
            if gameDevice1 != device1 { gameDevice1 = device1 }
            if gameDevice2 != device2 { gameDevice2 = device2 }
            
            parent.toolbar.validateVisibleItems()
        }
    }
    var gameDevice2 = HardwareDefaults.A500.gameDevice2 {
        didSet {
            
            // Try to connect the device
            gamePadManager.connect(slot: gameDevice2, port: 2)
            gamePadManager.listDevices()
            
            // Read back the real connection status
            let device1 = gamePadManager.getSlot(port: 1)
            let device2 = gamePadManager.getSlot(port: 2)
            if gameDevice1 != device1 { gameDevice1 = device1 }
            if gameDevice2 != device2 { gameDevice2 = device2 }
            
            parent.toolbar.validateVisibleItems()
        }
    }
    */
    
    //
    // Video settings
    //
    
    var palette: Int {
        get { return c64.vic.videoPalette() }
        set { c64.vic.setVideoPalette(newValue) }
    }
    var brightness: Double {
        get { return c64.vic.brightness() }
        set { c64.vic.setBrightness(newValue) }
    }
    var contrast: Double {
        get { return c64.vic.contrast() }
        set { c64.vic.setContrast(newValue) }
    }
    var saturation: Double {
        get { c64.vic.saturation() }
        set { c64.vic.setSaturation(newValue) }
    }
    var hCenter = VideoDefaults.tft.hCenter {
        didSet { renderer.updateTextureRect() }
    }
    var vCenter = VideoDefaults.tft.vCenter {
        didSet { renderer.updateTextureRect() }
    }
    var hZoom = VideoDefaults.tft.hZoom {
        didSet { renderer.updateTextureRect() }
    }
    var vZoom = VideoDefaults.tft.vZoom {
        didSet { renderer.updateTextureRect() }
    }
    var upscaler = VideoDefaults.tft.upscaler {
        didSet { if !renderer.selectUpscaler(upscaler) { upscaler = oldValue } }
    }
    var blur = VideoDefaults.tft.blur {
        didSet { renderer.shaderOptions.blur = blur }
    }
    var blurRadius = VideoDefaults.tft.blurRadius {
        didSet { renderer.shaderOptions.blurRadius = blurRadius }
    }
    var bloom = VideoDefaults.tft.bloom {
        didSet { renderer.shaderOptions.bloom = bloom }
    }
    var bloomRadiusR = VideoDefaults.tft.bloomRadiusR {
        didSet { renderer.shaderOptions.bloomRadiusR = bloomRadiusR }
    }
    var bloomRadiusG = VideoDefaults.tft.bloomRadiusG {
        didSet { renderer.shaderOptions.bloomRadiusG = bloomRadiusG }
    }
    var bloomRadiusB = VideoDefaults.tft.bloomRadiusB {
        didSet { renderer.shaderOptions.bloomRadiusB = bloomRadiusB }
    }
    var bloomBrightness = VideoDefaults.tft.bloomBrightness {
        didSet { renderer.shaderOptions.bloomBrightness = bloomBrightness }
    }
    var bloomWeight = VideoDefaults.tft.bloomWeight {
        didSet { renderer.shaderOptions.bloomWeight = bloomWeight }
    }
    var dotMask = VideoDefaults.tft.dotMask {
        didSet {
            renderer.shaderOptions.dotMask = dotMask
            renderer.buildDotMasks()
        }
    }
    var dotMaskBrightness = VideoDefaults.tft.dotMaskBrightness {
        didSet {
            renderer.shaderOptions.dotMaskBrightness = dotMaskBrightness
            renderer.buildDotMasks()
        }
    }
    var scanlines = VideoDefaults.tft.scanlines {
        didSet { renderer.shaderOptions.scanlines = scanlines }
    }
    var scanlineBrightness = VideoDefaults.tft.scanlineBrightness {
        didSet { renderer.shaderOptions.scanlineBrightness = scanlineBrightness }
    }
    var scanlineWeight = VideoDefaults.tft.scanlineWeight {
        didSet { renderer.shaderOptions.scanlineWeight = scanlineWeight }
    }
    var disalignment = VideoDefaults.tft.disalignment {
        didSet { renderer.shaderOptions.disalignment = disalignment }
    }
    var disalignmentH = VideoDefaults.tft.disalignmentH {
        didSet { renderer.shaderOptions.disalignmentH = disalignmentH }
    }
    var disalignmentV = VideoDefaults.tft.disalignmentV {
        didSet { renderer.shaderOptions.disalignmentV = disalignmentV }
    }
    
    init(with controller: MyController) { parent = controller }

    //
    // Roms
    //
    
    func loadRomUserDefaults() {
        
        c64.suspend()
        
        if let url = UserDefaults.basicRomUrl {
            track("Seeking Basic Rom")
            c64.loadBasicRom(fromFile: url)
        }
        if let url = UserDefaults.kernalRomUrl {
            track("Seeking Kernal Rom")
            c64.loadKernalRom(fromFile: url)
        }
        if let url = UserDefaults.charRomUrl {
            track("Seeking Character Rom")
            c64.loadCharRom(fromFile: url)
        }
        if let url = UserDefaults.vc1541RomUrl {
            track("Seeking VC1541 Rom")
            c64.loadVC1541Rom(fromFile: url)
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
        vicGrayDotBug = defaults.vicGrayDotBug

        ciaRevision = defaults.ciaRevision.rawValue
        ciaTimerBBug = defaults.ciaTimerBBug

        sidRevision = defaults.sidRevision.rawValue
        sidFilter = defaults.sidFilter

        sidEngine = defaults.sidEngine.rawValue
        sidSampling = defaults.sampling.rawValue
        
        glueLogic = defaults.glueLogic.rawValue
        ramPattern = defaults.ramPattern.rawValue
        
        drive8Connected = defaults.driveConnect[0]
        drive8Type = defaults.driveType[0].rawValue
        drive9Connected = defaults.driveConnect[1]
        drive9Type = defaults.driveType[1].rawValue
        
        gameDevice1 = defaults.gameDevice1
        gameDevice2 = defaults.gameDevice2

        c64.resume()
    }
    
    func loadHardwareUserDefaults() {
        
        let defaults = UserDefaults.standard
        
        c64.suspend()
        
        vicRevision = defaults.integer(forKey: Keys.vicRevision)
        vicGrayDotBug = defaults.bool(forKey: Keys.vicGrayDotBug)

        ciaRevision = defaults.integer(forKey: Keys.ciaRevision)
        ciaTimerBBug = defaults.bool(forKey: Keys.ciaTimerBBug)

        sidRevision = defaults.integer(forKey: Keys.sidRevision)
        sidFilter = defaults.bool(forKey: Keys.sidFilter)
        
        sidEngine = defaults.integer(forKey: Keys.sidEngine)
        sidSampling = defaults.integer(forKey: Keys.sidSampling)
        
        glueLogic = defaults.integer(forKey: Keys.glueLogic)
        ramPattern = defaults.integer(forKey: Keys.ramPattern)
        
        drive8Connected = defaults.bool(forKey: Keys.drive8Connect)
        drive8Type = defaults.integer(forKey: Keys.drive8Type)
        drive9Connected = defaults.bool(forKey: Keys.drive9Connect)
        drive9Type = defaults.integer(forKey: Keys.drive9Type)
        
        gameDevice1 = defaults.integer(forKey: Keys.gameDevice1)
        gameDevice2 = defaults.integer(forKey: Keys.gameDevice2)

        c64.resume()
    }
    
    func saveHardwareUserDefaults() {
        
        track()
        
        let defaults = UserDefaults.standard

        defaults.set(vicRevision, forKey: Keys.vicRevision)
        defaults.set(vicGrayDotBug, forKey: Keys.vicGrayDotBug)

        defaults.set(ciaRevision, forKey: Keys.ciaRevision)
        defaults.set(ciaTimerBBug, forKey: Keys.ciaTimerBBug)

        defaults.set(sidRevision, forKey: Keys.sidRevision)
        defaults.set(sidFilter, forKey: Keys.sidFilter)
        
        defaults.set(sidEngine, forKey: Keys.sidEngine)
        defaults.set(sidSampling, forKey: Keys.sidSampling)
        
        defaults.set(glueLogic, forKey: Keys.glueLogic)
        defaults.set(ramPattern, forKey: Keys.ramPattern)

        defaults.set(drive8Connected, forKey: Keys.drive8Connect)
        defaults.set(drive8Type, forKey: Keys.drive8Type)
        defaults.set(drive9Connected, forKey: Keys.drive9Connect)
        defaults.set(drive9Type, forKey: Keys.drive9Type)

        defaults.set(gameDevice1, forKey: Keys.gameDevice1)
        defaults.set(gameDevice2, forKey: Keys.gameDevice2)
    }
    
    //
    // Video
    //
    
    func loadColorDefaults(_ defaults: VideoDefaults) {
        
        c64.suspend()
        
        palette = defaults.palette.rawValue
        brightness = defaults.brightness
        contrast = defaults.contrast
        saturation = defaults.saturation
        
        c64.resume()
    }
    
    func loadGeometryDefaults(_ defaults: VideoDefaults) {
        
        hCenter = defaults.hCenter
        vCenter = defaults.vCenter
        hZoom = defaults.hZoom
        vZoom = defaults.vZoom
        
        renderer.updateTextureRect()
    }
    
    func loadShaderDefaults(_ defaults: VideoDefaults) {
        
        upscaler = defaults.upscaler
        
        blur = defaults.blur
        blurRadius = defaults.blurRadius
        
        bloom = defaults.bloom
        bloomRadiusR = defaults.bloomRadiusR
        bloomRadiusG = defaults.bloomRadiusG
        bloomRadiusB = defaults.bloomRadiusB
        bloomBrightness = defaults.bloomBrightness
        bloomWeight = defaults.bloomWeight
        dotMask = defaults.dotMask
        dotMaskBrightness = defaults.dotMaskBrightness
        scanlines = defaults.scanlines
        scanlineBrightness = defaults.scanlineBrightness
        scanlineWeight = defaults.scanlineWeight
        disalignment = defaults.disalignment
        disalignmentH = defaults.disalignmentH
        disalignment = defaults.disalignment
        
        renderer.buildDotMasks()
    }
    
    func loadVideoDefaults(_ defaults: VideoDefaults) {
         
         loadColorDefaults(defaults)
         loadGeometryDefaults(defaults)
         loadColorDefaults(defaults)
     }

     func loadVideoUserDefaults() {
         
         let defaults = UserDefaults.standard
         
         c64.suspend()
         
         palette = defaults.integer(forKey: Keys.palette)
         brightness = defaults.double(forKey: Keys.brightness)
         contrast = defaults.double(forKey: Keys.contrast)
         saturation = defaults.double(forKey: Keys.saturation)

         hCenter = defaults.float(forKey: Keys.hCenter)
         vCenter = defaults.float(forKey: Keys.vCenter)
         hZoom = defaults.float(forKey: Keys.hZoom)
         vZoom = defaults.float(forKey: Keys.vZoom)

         upscaler = defaults.integer(forKey: Keys.upscaler)
         
         bloom = Int32(defaults.integer(forKey: Keys.bloom))
         bloomRadiusR = defaults.float(forKey: Keys.bloomRadiusR)
         bloomRadiusG = defaults.float(forKey: Keys.bloomRadiusG)
         bloomRadiusB = defaults.float(forKey: Keys.bloomRadiusB)
         bloomBrightness = defaults.float(forKey: Keys.bloomBrightness)
         bloomWeight = defaults.float(forKey: Keys.bloomWeight)
         dotMask = Int32(defaults.integer(forKey: Keys.dotMask))
         dotMaskBrightness = defaults.float(forKey: Keys.dotMaskBrightness)
         scanlines = Int32(defaults.integer(forKey: Keys.scanlines))
         scanlineBrightness = defaults.float(forKey: Keys.scanlineBrightness)
         scanlineWeight = defaults.float(forKey: Keys.scanlineWeight)
         disalignment = Int32(defaults.integer(forKey: Keys.disalignment))
         disalignmentH = defaults.float(forKey: Keys.disalignmentH)
         disalignmentV = defaults.float(forKey: Keys.disalignmentV)
         
         renderer.updateTextureRect()
         renderer.buildDotMasks()
         
         c64.resume()
     }
     
     func saveVideoUserDefaults() {
         
         track()
         
         let defaults = UserDefaults.standard
         
         defaults.set(palette, forKey: Keys.palette)
         defaults.set(brightness, forKey: Keys.brightness)
         defaults.set(contrast, forKey: Keys.contrast)
         defaults.set(saturation, forKey: Keys.saturation)

         defaults.set(hCenter, forKey: Keys.hCenter)
         defaults.set(vCenter, forKey: Keys.vCenter)
         defaults.set(hZoom, forKey: Keys.hZoom)
         defaults.set(vZoom, forKey: Keys.vZoom)

         defaults.set(upscaler, forKey: Keys.upscaler)
         
         defaults.set(bloom, forKey: Keys.bloom)
         defaults.set(bloomRadiusR, forKey: Keys.bloomRadiusR)
         defaults.set(bloomRadiusG, forKey: Keys.bloomRadiusG)
         defaults.set(bloomRadiusB, forKey: Keys.bloomRadiusB)
         defaults.set(bloomBrightness, forKey: Keys.bloomBrightness)
         defaults.set(bloomWeight, forKey: Keys.bloomWeight)
         defaults.set(dotMask, forKey: Keys.dotMask)
         defaults.set(dotMaskBrightness, forKey: Keys.dotMaskBrightness)
         defaults.set(scanlines, forKey: Keys.scanlines)
         defaults.set(scanlineBrightness, forKey: Keys.scanlineBrightness)
         defaults.set(scanlineWeight, forKey: Keys.scanlineWeight)
         defaults.set(disalignment, forKey: Keys.disalignment)
         defaults.set(disalignmentH, forKey: Keys.disalignmentH)
         defaults.set(disalignmentV, forKey: Keys.disalignmentV)
     }
}
