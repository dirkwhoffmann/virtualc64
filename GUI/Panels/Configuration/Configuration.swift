// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

/* This class stores all items that are specific to an individual emulator
 * instance. Each instance keeps its own object of this class inside the
 * document controller.
 *
 * See class "Preferences" for shared settings.
 */

class Configuration {
    
    var parent: MyController!
    var c64: EmulatorProxy { return parent.c64 }
    var v64: SwiftProxy { return parent.v64 }
    var renderer: Renderer { return parent.renderer }
    var gamePadManager: GamePadManager { return parent.gamePadManager }
    var ressourceManager: RessourceManager { return renderer.ressourceManager }
    
    //
    // Hardware
    //
    
    var vicRevision: Int {
        get { return v64.getConfig(.VICII_REVISION) }
        set { v64.configure(.VICII_REVISION, value: newValue) }
    }

    var vicGrayDotBug: Bool {
        get { return v64.getConfig(.VICII_GRAY_DOT_BUG) != 0 }
        set { v64.configure(.VICII_GRAY_DOT_BUG, value: newValue) }
    }
    
    var vicHideSprites: Bool {
        get { return v64.getConfig(.VICII_HIDE_SPRITES) != 0 }
        set { v64.configure(.VICII_HIDE_SPRITES, value: newValue) }
    }
        
    var vicCutLayers: Int {
        get { return v64.getConfig(.VICII_CUT_LAYERS) }
        set { v64.configure(.VICII_CUT_LAYERS, value: newValue) }
    }
    
    var vicCutOpacity: Int {
        get { return v64.getConfig(.VICII_CUT_OPACITY) }
        set { v64.configure(.VICII_CUT_OPACITY, value: newValue) }
    }
        
    var ciaRevision: Int {
        get { return v64.getConfig(.CIA_REVISION) }
        set { v64.configure(.CIA_REVISION, value: newValue) }
    }
    
    var ciaTimerBBug: Bool {
        get { return v64.getConfig(.CIA_TIMER_B_BUG) != 0}
        set { v64.configure(.CIA_TIMER_B_BUG, value: newValue) }
    }
    
    var sidRevision: Int {
        get { return v64.getConfig(.SID_REVISION) }
        set { v64.configure(.SID_REVISION, value: newValue) }
    }
    
    var sidFilter: Bool {
        get { return v64.getConfig(.SID_FILTER) != 0 }
        set { v64.configure(.SID_FILTER, value: newValue) }
    }
    
    var glueLogic: Int {
        get { return v64.getConfig(.GLUE_LOGIC) }
        set { v64.configure(.GLUE_LOGIC, value: newValue) }
    }

    var powerGrid: Int {
        get { return v64.getConfig(.POWER_GRID) }
        set { v64.configure(.POWER_GRID, value: newValue) }
    }

    var ramPattern: Int {
        get { return v64.getConfig(.RAM_PATTERN) }
        set { v64.configure(.RAM_PATTERN, value: newValue) }
    }
    
    //
    // Peripherals
    //
    
    var drive8Connected: Bool {
        get { return v64.getConfig(.DRV_CONNECT, drive: DRIVE8) != 0 }
        set { v64.configure(.DRV_CONNECT, drive: DRIVE8, enable: newValue ) }
    }
    
    var drive9Connected: Bool {
        get { return v64.getConfig(.DRV_CONNECT, drive: DRIVE9) != 0 }
        set { v64.configure(.DRV_CONNECT, drive: DRIVE9, enable: newValue ) }
    }

    var drive8AutoConf: Bool {
        get { return v64.getConfig(.DRV_AUTO_CONFIG, drive: DRIVE8) != 0 }
        set { v64.configure(.DRV_AUTO_CONFIG, drive: DRIVE8, enable: newValue ) }
    }
    
    var drive9AutoConf: Bool {
        get { return v64.getConfig(.DRV_AUTO_CONFIG, drive: DRIVE9) != 0}
        set { v64.configure(.DRV_AUTO_CONFIG, drive: DRIVE9, enable: newValue ) }
    }
    
    var drive8Type: Int {
        get { return v64.getConfig(.DRV_TYPE, drive: DRIVE8) }
        set { v64.configure(.DRV_TYPE, drive: DRIVE8, value: newValue )}
    }
    
    var drive9Type: Int {
        get { return v64.getConfig(.DRV_TYPE, drive: DRIVE9) }
        set { v64.configure(.DRV_TYPE, drive: DRIVE9, value: newValue )}
    }

    var drive8Ram: Int {
        get { return v64.getConfig(.DRV_RAM, drive: DRIVE8) }
        set { v64.configure(.DRV_RAM, drive: DRIVE8, value: newValue )}
    }

    var drive9Ram: Int {
        get { return v64.getConfig(.DRV_RAM, drive: DRIVE9) }
        set { v64.configure(.DRV_RAM, drive: DRIVE9, value: newValue )}
    }

    var drive8ParCable: Int {
        get { return v64.getConfig(.DRV_PARCABLE, drive: DRIVE8) }
        set { v64.configure(.DRV_PARCABLE, drive: DRIVE8, value: newValue ) }
    }

    var drive9ParCable: Int {
        get { return v64.getConfig(.DRV_PARCABLE, drive: DRIVE9) }
        set { v64.configure(.DRV_PARCABLE, drive: DRIVE9, value: newValue ) }
    }
    
    var drive8PowerSwitch: Bool {
        get { return v64.getConfig(.DRV_POWER_SWITCH, drive: DRIVE8) != 0 }
        set { v64.configure(.DRV_POWER_SWITCH, drive: DRIVE8, enable: newValue )}
    }
    
    var drive9PowerSwitch: Bool {
        get { return v64.getConfig(.DRV_POWER_SWITCH, drive: DRIVE9) != 0 }
        set { v64.configure(.DRV_POWER_SWITCH, drive: DRIVE9, enable: newValue )}
    }

    var datasetteConnected: Bool {
        get { return v64.getConfig(.DAT_CONNECT) != 0 }
        set { v64.configure(.DAT_CONNECT, value: newValue ) }
    }

    var datasetteModel: Int {
        get { return v64.getConfig(.DAT_MODEL) }
        set { v64.configure(.DAT_MODEL, value: newValue )}
    }

    var gameDevice1 = -1 {
        didSet {
            
            // Try to connect the device (may disconnect the other device)
            gamePadManager.connect(slot: gameDevice1, port: 0)
            gamePadManager.listDevices()
            
            // Avoid double mappings
            if gameDevice1 != -1 && gameDevice1 == gameDevice2 {
                gameDevice2 = -1
            }
            
            parent.toolbar.validateVisibleItems()
        }
    }
    
    var gameDevice2 = -2 {
        didSet {
            
            // Try to connect the device (may disconnect the other device)
            gamePadManager.connect(slot: gameDevice2, port: 1)
            gamePadManager.listDevices()
            
            // Avoid double mappings
            if gameDevice2 != -1 && gameDevice2 == gameDevice1 {
                gameDevice1 = -1
            }
            
            parent.toolbar.validateVisibleItems()
        }
    }
    
    var mouseModel: Int {
        get { return v64.getConfig(.MOUSE_MODEL, id: 1) }
        set { v64.configure(.MOUSE_MODEL, value: newValue) }
    }

    var autofire: Bool {
        get { return v64.getConfig(.AUTOFIRE, id: 1) != 0 }
        set { v64.configure(.AUTOFIRE, value: newValue) }
    }

    var autofireBursts: Bool {
        get { return v64.getConfig(.AUTOFIRE_BURSTS, id: 1) != 0 }
        set { v64.configure(.AUTOFIRE_BURSTS, value: newValue) }
    }

    var autofireBullets: Int {
        get { return v64.getConfig(.AUTOFIRE_BULLETS, id: 1) }
        set { v64.configure(.AUTOFIRE_BULLETS, value: newValue) }
    }

    var autofireFrequency: Int {
        get { return v64.getConfig(.AUTOFIRE_DELAY, id: 1) }
        set { v64.configure(.AUTOFIRE_DELAY, value: newValue) }
    }

    //
    // Performance
    //

    var drive8PowerSave: Bool {
        get { return v64.getConfig(.DRV_POWER_SAVE, drive: DRIVE8) != 0 }
        set { v64.configure(.DRV_POWER_SAVE, drive: DRIVE8, enable: newValue) }
    }

    var drive9PowerSave: Bool {
        get { return v64.getConfig(.DRV_POWER_SAVE, drive: DRIVE9) != 0 }
        set { v64.configure(.DRV_POWER_SAVE, drive: DRIVE9, enable: newValue) }
    }

    var viciiPowerSave: Bool {
        get { return v64.getConfig(.VICII_POWER_SAVE) != 0 }
        set { v64.configure(.VICII_POWER_SAVE, value: newValue) }
    }

    var sidPowerSave: Bool {
        get { return v64.getConfig(.SID_POWER_SAVE) != 0 }
        set { v64.configure(.SID_POWER_SAVE, value: newValue) }
    }

    var ssCollisions: Bool {
        get { return v64.getConfig(.VICII_SS_COLLISIONS) != 0 }
        set { v64.configure(.VICII_SS_COLLISIONS, value: newValue) }
    }

    var sbCollisions: Bool {
        get { return v64.getConfig(.VICII_SB_COLLISIONS) != 0 }
        set { v64.configure(.VICII_SB_COLLISIONS, value: newValue) }
    }

    var warpMode: Int {
        get { return v64.getConfig(.EMU_WARP_MODE) }
        set { v64.configure(.EMU_WARP_MODE, value: newValue) }
    }

    var warpBoot: Int {
        get { return v64.getConfig(.EMU_WARP_BOOT) }
        set { v64.configure(.EMU_WARP_BOOT, value: newValue) }
    }

    var vsync: Bool {
        get { return v64.getConfig(.EMU_VSYNC) != 0 }
        set { v64.configure(.EMU_VSYNC, value: newValue) }
    }
    var timeLapse: Int {
        get { return v64.getConfig(.EMU_TIME_LAPSE) }
        set { v64.configure(.EMU_TIME_LAPSE, value: newValue) }
    }

    var runAhead: Int {
        get { return v64.getConfig(.EMU_RUN_AHEAD) }
        set { v64.configure(.EMU_RUN_AHEAD, value: newValue) }
    }

    //
    // Audio
    //
    
    var sidEnable1: Bool {
        get { return v64.getConfig(.SID_ENABLE, id: 1) != 0 }
        set { v64.configure(.SID_ENABLE, id: 1, value: newValue) }
    }
    var sidEnable2: Bool {
        get { return v64.getConfig(.SID_ENABLE, id: 2) != 0 }
        set { v64.configure(.SID_ENABLE, id: 2, value: newValue) }
    }
    var sidEnable3: Bool {
        get { return v64.getConfig(.SID_ENABLE, id: 3) != 0 }
        set { v64.configure(.SID_ENABLE, id: 3, value: newValue) }
    }
    var sidAddress1: Int {
        get { return v64.getConfig(.SID_ADDRESS, id: 1) }
        set { v64.configure(.SID_ADDRESS, id: 1, value: newValue) }
    }
    var sidAddress2: Int {
        get { return v64.getConfig(.SID_ADDRESS, id: 2) }
        set { v64.configure(.SID_ADDRESS, id: 2, value: newValue) }
    }
    var sidAddress3: Int {
        get { return v64.getConfig(.SID_ADDRESS, id: 3) }
        set { v64.configure(.SID_ADDRESS, id: 3, value: newValue) }
    }
    var sidEngine: Int {
        get { return v64.getConfig(.SID_ENGINE) }
        set { v64.configure(.SID_ENGINE, value: newValue) }
    }
    var sidSampling: Int {
        get { return v64.getConfig(.SID_SAMPLING) }
        set { v64.configure(.SID_SAMPLING, value: newValue) }
    }
    var vol0: Int {
        get { return v64.getConfig(.AUD_VOL, id: 0) }
        set { v64.configure(.AUD_VOL, id: 0, value: newValue) }
    }
    var vol1: Int {
        get { return v64.getConfig(.AUD_VOL, id: 1) }
        set { v64.configure(.AUD_VOL, id: 1, value: newValue) }
    }
    var vol2: Int {
        get { return v64.getConfig(.AUD_VOL, id: 2) }
        set { v64.configure(.AUD_VOL, id: 2, value: newValue) }
    }
    var vol3: Int {
        get { return v64.getConfig(.AUD_VOL, id: 3) }
        set { v64.configure(.AUD_VOL, id: 3, value: newValue) }
    }
    var pan0: Int {
        get { return v64.getConfig(.AUD_PAN, id: 0) }
        set { v64.configure(.AUD_PAN, id: 0, value: newValue) }
    }
    var pan1: Int {
        get { return v64.getConfig(.AUD_PAN, id: 1) }
        set { v64.configure(.AUD_PAN, id: 1, value: newValue) }
    }
    var pan2: Int {
        get { return v64.getConfig(.AUD_PAN, id: 2) }
        set { v64.configure(.AUD_PAN, id: 2, value: newValue) }
    }
    var pan3: Int {
        get { return v64.getConfig(.AUD_PAN, id: 3) }
        set { v64.configure(.AUD_PAN, id: 3, value: newValue) }
    }
    var volL: Int {
        get { return v64.getConfig(.AUD_VOL_L) }
        set { v64.configure(.AUD_VOL_L, value: newValue) }
    }
    var volR: Int {
        get { return v64.getConfig(.AUD_VOL_R) }
        set { v64.configure(.AUD_VOL_R, value: newValue) }
    }
    var stepVolume: Int {
        get { return v64.getConfig(.DRV_STEP_VOL, drive: DRIVE8) }
        set { v64.configure(.DRV_STEP_VOL, value: newValue) }
    }
    var insertVolume: Int {
        get { return v64.getConfig(.DRV_INSERT_VOL, drive: DRIVE8) }
        set { v64.configure(.DRV_INSERT_VOL, value: newValue) }
    }
    var ejectVolume: Int {
        get { return v64.getConfig(.DRV_EJECT_VOL, drive: DRIVE8) }
        set { v64.configure(.DRV_EJECT_VOL, value: newValue) }
    }
    var drive8Pan: Int {
        get { return v64.getConfig(.DRV_PAN, drive: DRIVE8) }
        set { v64.configure(.DRV_PAN, drive: DRIVE8, value: newValue) }
    }
    var drive9Pan: Int {
        get { return v64.getConfig(.DRV_PAN, drive: DRIVE9) }
        set { v64.configure(.DRV_PAN, drive: DRIVE9, value: newValue) }
    }

    //
    // Video
    //

    var palette: Int {
        get { return v64.getConfig(.MON_PALETTE) }
        set { v64.configure(.MON_PALETTE, value: newValue) }
    }
    var brightness: Int {
        get { return v64.getConfig(.MON_BRIGHTNESS) }
        set { v64.configure(.MON_BRIGHTNESS, value: newValue) }
    }
    var contrast: Int {
        get { return v64.getConfig(.MON_CONTRAST) }
        set { v64.configure(.MON_CONTRAST, value: newValue) }
    }
    var saturation: Int {
        get { return v64.getConfig(.MON_SATURATION) }
        set { v64.configure(.MON_SATURATION, value: newValue) }
    }
    var hCenter: Int {
        get { return v64.getConfig(.MON_HCENTER) }
        set {
            v64.configure(.MON_HCENTER, value: newValue)
            renderer.canvas.updateTextureRect()
        }
    }
    var vCenter: Int {
        get { return v64.getConfig(.MON_VCENTER) }
        set {
            v64.configure(.MON_VCENTER, value: newValue)
            renderer.canvas.updateTextureRect()
        }
    }
    var hZoom: Int {
        get { return v64.getConfig(.MON_HZOOM) }
        set {
            v64.configure(.MON_HZOOM, value: newValue)
            renderer.canvas.updateTextureRect()
        }
    }
    var vZoom: Int {
        get { return v64.getConfig(.MON_VZOOM) }
        set {
            v64.configure(.MON_VZOOM, value: newValue)
            renderer.canvas.updateTextureRect()
        }
    }
    var upscaler: Int {
        get { return v64.getConfig(.MON_UPSCALER) }
        set {
            if ressourceManager.selectUpscaler(newValue) {
                v64.configure(.MON_UPSCALER, value: newValue)
            }
        }
    }
    var blur: Int {
        get { return v64.getConfig(.MON_BLUR) }
        set {
            v64.configure(.MON_BLUR, value: newValue)
            renderer.shaderOptions.blur = Int32(newValue)
        }
    }
    var blurRadius: Int {
        get { return v64.getConfig(.MON_BLUR_RADIUS) }
        set {
            v64.configure(.MON_BLUR_RADIUS, value: newValue)
            renderer.shaderOptions.blurRadius = Float(newValue) / 1000.0
        }
    }
    var bloom: Int {
        get { return v64.getConfig(.MON_BLOOM) }
        set {
            if ressourceManager.selectBloomFilter(newValue) {
                v64.configure(.MON_BLOOM, value: newValue)
                renderer.shaderOptions.bloom = Int32(bloom)
            }
        }
    }
    var bloomRadius: Int {
        get { return v64.getConfig(.MON_BLOOM_RADIUS) }
        set {
            v64.configure(.MON_BLOOM_RADIUS, value: newValue)
            renderer.shaderOptions.bloomRadius = Float(newValue) / 1000.0
        }
    }
    var bloomBrightness: Int {
        get { return v64.getConfig(.MON_BLOOM_BRIGHTNESS) }
        set {
            v64.configure(.MON_BLOOM_BRIGHTNESS, value: newValue)
            renderer.shaderOptions.bloomBrightness = Float(newValue) / 1000.0
        }
    }
    var bloomWeight: Int {
        get { return v64.getConfig(.MON_BLOOM_WEIGHT) }
        set {
            v64.configure(.MON_BLOOM_WEIGHT, value: newValue)
            renderer.shaderOptions.bloomWeight = Float(newValue) / 1000.0
        }
    }
    var dotMask: Int {
        get { return v64.getConfig(.MON_DOTMASK) }
        set {
            renderer.shaderOptions.dotMask = Int32(newValue)
            ressourceManager.buildDotMasks()
            if ressourceManager.selectDotMask(newValue) {
                v64.configure(.MON_DOTMASK, value: newValue)
            }
        }
    }
    var dotMaskBrightness: Int {
        get { return v64.getConfig(.MON_DOTMASK_BRIGHTNESS) }
        set {
            v64.configure(.MON_DOTMASK_BRIGHTNESS, value: newValue)
            renderer.shaderOptions.dotMaskBrightness = Float(newValue) / 1000.0
            ressourceManager.buildDotMasks()
            ressourceManager.selectDotMask(dotMask)
        }
    }
    var scanlines: Int {
        get { return v64.getConfig(.MON_SCANLINES) }
        set {
            renderer.shaderOptions.scanlines = Int32(newValue)
            if !ressourceManager.selectScanlineFilter(newValue) {
                v64.configure(.MON_SCANLINES, value: newValue)
            }
        }
    }
    var scanlineBrightness: Int {
        get { return v64.getConfig(.MON_SCANLINE_BRIGHTNESS) }
        set {
            v64.configure(.MON_SCANLINE_BRIGHTNESS, value: newValue)
            renderer.shaderOptions.scanlineBrightness = Float(newValue) / 1000.0
        }
    }
    var scanlineWeight: Int {
        get { return v64.getConfig(.MON_SCANLINE_WEIGHT) }
        set {
            v64.configure(.MON_SCANLINE_WEIGHT, value: newValue)
            renderer.shaderOptions.scanlineWeight = Float(newValue) / 1000.0
        }
    }
    var disalignment: Int {
        get { return v64.getConfig(.MON_DISALIGNMENT) }
        set {
            v64.configure(.MON_DISALIGNMENT, value: newValue)
            renderer.shaderOptions.disalignment = Int32(newValue)
        }
    }
    var disalignmentH: Int {
        get { return v64.getConfig(.MON_DISALIGNMENT_H) }
        set {
            v64.configure(.MON_DISALIGNMENT_H, value: newValue)
            renderer.shaderOptions.disalignmentH = Float(newValue) / 1000000.0
        }
    }
    var disalignmentV: Int {
        get { return v64.getConfig(.MON_DISALIGNMENT_V) }
        set {
            v64.configure(.MON_DISALIGNMENT_V, value: newValue)
            renderer.shaderOptions.disalignmentV = Float(newValue) / 1000000.0
        }
    }
    init(with controller: MyController) { parent = controller }

    //
    // Roms
    //

    func loadRomUserDefaults() {

        func load(_ url: URL?, type: vc64.FileType) {

            if url != nil {
                if let file = try? RomFileProxy.make(with: url!) {
                    if file.type == type { c64.loadRom(file) }
                }
            }
        }

        debug(.defaults)
        
        v64.suspend()
        load(UserDefaults.basicRomUrl, type: .BASIC_ROM)
        load(UserDefaults.charRomUrl, type: .CHAR_ROM)
        load(UserDefaults.kernalRomUrl, type: .KERNAL_ROM)
        load(UserDefaults.vc1541RomUrl, type: .VC1541_ROM)
        v64.resume()
    }

    func saveRomUserDefaults() throws {

        debug(.defaults)

        var url: URL?

        func save(_ type: vc64.RomType) throws {

            if url == nil { throw VC64Error(vc64.ErrorCode.FILE_CANT_WRITE) }
            try? FileManager.default.removeItem(at: url!)
            try c64.saveRom(type, url: url!)
        }

        v64.suspend()

        do {
            url = UserDefaults.basicRomUrl;  try save(.BASIC)
            url = UserDefaults.charRomUrl;   try save(.CHAR)
            url = UserDefaults.kernalRomUrl; try save(.KERNAL)
            url = UserDefaults.vc1541RomUrl; try save(.VC1541)

        } catch {

            v64.resume()
            throw error
        }

        v64.resume()
    }
}
