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
    var emu: EmulatorProxy? { return parent.emu }
    var renderer: Renderer { return parent.renderer }
    var gamePadManager: GamePadManager { return parent.gamePadManager }
    var ressourceManager: RessourceManager { return renderer.ressourceManager }
    
    //
    // Hardware
    //
    
    var vicRevision: Int {
        get { return emu?.get(.VICII_REVISION) ?? 0 }
        set { emu?.set(.VICII_REVISION, value: newValue) }
    }

    var vicGrayDotBug: Bool {
        get { return emu?.get(.VICII_GRAY_DOT_BUG) != 0 }
        set { emu?.set(.VICII_GRAY_DOT_BUG, enable: newValue) }
    }
    
    var vicHideSprites: Bool {
        get { return emu?.get(.VICII_HIDE_SPRITES) != 0 }
        set { emu?.set(.VICII_HIDE_SPRITES, enable: newValue) }
    }
        
    var vicCutLayers: Int {
        get { return emu?.get(.VICII_CUT_LAYERS) ?? 0 }
        set { emu?.set(.VICII_CUT_LAYERS, value: newValue) }
    }
    
    var vicCutOpacity: Int {
        get { return emu?.get(.VICII_CUT_OPACITY) ?? 0 }
        set { emu?.set(.VICII_CUT_OPACITY, value: newValue) }
    }
        
    var ciaRevision: Int {
        get { return emu?.get(.CIA_REVISION) ?? 0 }
        set { emu?.set(.CIA_REVISION, value: newValue) }
    }
    
    var ciaTimerBBug: Bool {
        get { return emu?.get(.CIA_TIMER_B_BUG) != 0}
        set { emu?.set(.CIA_TIMER_B_BUG, enable: newValue) }
    }
    
    var sidRevision: Int {
        get { return emu?.get(.SID_REVISION, id: 0) ?? 0 }
        set { emu?.set(.SID_REVISION, value: newValue) }
    }
    
    var sidFilter: Bool {
        get { return emu?.get(.SID_FILTER, id: 0) != 0 }
        set { emu?.set(.SID_FILTER, enable: newValue) }
    }
    
    var glueLogic: Int {
        get { return emu?.get(.GLUE_LOGIC) ?? 0 }
        set { emu?.set(.GLUE_LOGIC, value: newValue) }
    }

    var powerGrid: Int {
        get { return emu?.get(.POWER_GRID) ?? 0 }
        set { emu?.set(.POWER_GRID, value: newValue) }
    }

    var ramPattern: Int {
        get { return emu?.get(.MEM_INIT_PATTERN) ?? 0 }
        set { emu?.set(.MEM_INIT_PATTERN, value: newValue) }
    }
    
    //
    // Peripherals
    //
    
    var drive8Connected: Bool {
        get { return emu?.get(.DRV_CONNECT, drive: DRIVE8) != 0 }
        set { emu?.set(.DRV_CONNECT, drive: DRIVE8, enable: newValue ) }
    }
    
    var drive9Connected: Bool {
        get { return emu?.get(.DRV_CONNECT, drive: DRIVE9) != 0 }
        set { emu?.set(.DRV_CONNECT, drive: DRIVE9, enable: newValue ) }
    }

    var drive8AutoConf: Bool {
        get { return emu?.get(.DRV_AUTO_CONFIG, drive: DRIVE8) != 0 }
        set { emu?.set(.DRV_AUTO_CONFIG, drive: DRIVE8, enable: newValue ) }
    }
    
    var drive9AutoConf: Bool {
        get { return emu?.get(.DRV_AUTO_CONFIG, drive: DRIVE9) != 0}
        set { emu?.set(.DRV_AUTO_CONFIG, drive: DRIVE9, enable: newValue ) }
    }
    
    var drive8Type: Int {
        get { return emu?.get(.DRV_TYPE, drive: DRIVE8) ?? 0 }
        set { emu?.set(.DRV_TYPE, drive: DRIVE8, value: newValue )}
    }
    
    var drive9Type: Int {
        get { return emu?.get(.DRV_TYPE, drive: DRIVE9) ?? 0 }
        set { emu?.set(.DRV_TYPE, drive: DRIVE9, value: newValue )}
    }

    var drive8Ram: Int {
        get { return emu?.get(.DRV_RAM, drive: DRIVE8) ?? 0 }
        set { emu?.set(.DRV_RAM, drive: DRIVE8, value: newValue )}
    }

    var drive9Ram: Int {
        get { return emu?.get(.DRV_RAM, drive: DRIVE9) ?? 0 }
        set { emu?.set(.DRV_RAM, drive: DRIVE9, value: newValue )}
    }

    var drive8ParCable: Int {
        get { return emu?.get(.DRV_PARCABLE, drive: DRIVE8) ?? 0 }
        set { emu?.set(.DRV_PARCABLE, drive: DRIVE8, value: newValue ) }
    }

    var drive9ParCable: Int {
        get { return emu?.get(.DRV_PARCABLE, drive: DRIVE9) ?? 0 }
        set { emu?.set(.DRV_PARCABLE, drive: DRIVE9, value: newValue ) }
    }
    
    var drive8PowerSwitch: Bool {
        get { return emu?.get(.DRV_POWER_SWITCH, drive: DRIVE8) != 0 }
        set { emu?.set(.DRV_POWER_SWITCH, drive: DRIVE8, enable: newValue )}
    }
    
    var drive9PowerSwitch: Bool {
        get { return emu?.get(.DRV_POWER_SWITCH, drive: DRIVE9) != 0 }
        set { emu?.set(.DRV_POWER_SWITCH, drive: DRIVE9, enable: newValue )}
    }

    var datasetteConnected: Bool {
        get { return emu?.get(.DAT_CONNECT) != 0 }
        set { emu?.set(.DAT_CONNECT, enable: newValue ) }
    }

    var datasetteModel: Int {
        get { return emu?.get(.DAT_MODEL) ?? 0 }
        set { emu?.set(.DAT_MODEL, value: newValue )}
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
        get { return emu?.get(.MOUSE_MODEL, id: 1) ?? 0 }
        set { emu?.set(.MOUSE_MODEL, value: newValue) }
    }

    var paddleOrientation: Int {
        get { return emu?.get(.PADDLE_ORIENTATION, id: 1) ?? 0 }
        set { emu?.set(.PADDLE_ORIENTATION, value: newValue) }
    }

    var autofire: Bool {
        get { return emu?.get(.AUTOFIRE, id: 1) != 0 }
        set { emu?.set(.AUTOFIRE, enable: newValue) }
    }

    var autofireBursts: Bool {
        get { return emu?.get(.AUTOFIRE_BURSTS, id: 1) != 0 }
        set { emu?.set(.AUTOFIRE_BURSTS, enable: newValue) }
    }

    var autofireBullets: Int {
        get { return emu?.get(.AUTOFIRE_BULLETS, id: 1) ?? 0 }
        set { emu?.set(.AUTOFIRE_BULLETS, value: newValue) }
    }

    var autofireFrequency: Int {
        get { return emu?.get(.AUTOFIRE_DELAY, id: 1) ?? 0 }
        set { emu?.set(.AUTOFIRE_DELAY, value: newValue) }
    }

    //
    // Performance
    //

    var drive8PowerSave: Bool {
        get { return emu?.get(.DRV_POWER_SAVE, drive: DRIVE8) != 0 }
        set { emu?.set(.DRV_POWER_SAVE, drive: DRIVE8, enable: newValue) }
    }

    var drive9PowerSave: Bool {
        get { return emu?.get(.DRV_POWER_SAVE, drive: DRIVE9) != 0 }
        set { emu?.set(.DRV_POWER_SAVE, drive: DRIVE9, enable: newValue) }
    }

    var viciiPowerSave: Bool {
        get { return emu?.get(.VICII_POWER_SAVE) != 0 }
        set { emu?.set(.VICII_POWER_SAVE, enable: newValue) }
    }

    var sidPowerSave: Bool {
        get { return emu?.get(.SID_POWER_SAVE, id: 0) != 0 }
        set { emu?.set(.SID_POWER_SAVE, enable: newValue) }
    }

    var ssCollisions: Bool {
        get { return emu?.get(.VICII_SS_COLLISIONS) != 0 }
        set { emu?.set(.VICII_SS_COLLISIONS, enable: newValue) }
    }

    var sbCollisions: Bool {
        get { return emu?.get(.VICII_SB_COLLISIONS) != 0 }
        set { emu?.set(.VICII_SB_COLLISIONS, enable: newValue) }
    }

    var warpMode: Int {
        get { return emu?.get(.C64_WARP_MODE) ?? 0 }
        set { emu?.set(.C64_WARP_MODE, value: newValue) }
    }

    var warpBoot: Int {
        get { return emu?.get(.C64_WARP_BOOT) ?? 0 }
        set { emu?.set(.C64_WARP_BOOT, value: newValue) }
    }

    var vsync: Bool {
        get { return emu?.get(.C64_VSYNC) != 0 }
        set { emu?.set(.C64_VSYNC, enable: newValue) }
    }
    var speedBoost: Int {
        get { return emu?.get(.C64_SPEED_BOOST) ?? 0 }
        set { emu?.set(.C64_SPEED_BOOST, value: newValue) }
    }

    var runAhead: Int {
        get { return emu?.get(.C64_RUN_AHEAD) ?? 0 }
        set { emu?.set(.C64_RUN_AHEAD, value: newValue) }
    }

    //
    // Audio
    //
    
    var sidEnable1: Bool {
        get { return emu?.get(.SID_ENABLE, id: 1) != 0 }
        set { emu?.set(.SID_ENABLE, id: 1, enable: newValue) }
    }
    var sidEnable2: Bool {
        get { return emu?.get(.SID_ENABLE, id: 2) != 0 }
        set { emu?.set(.SID_ENABLE, id: 2, enable: newValue) }
    }
    var sidEnable3: Bool {
        get { return emu?.get(.SID_ENABLE, id: 3) != 0 }
        set { emu?.set(.SID_ENABLE, id: 3, enable: newValue) }
    }
    var sidAddress1: Int {
        get { return emu?.get(.SID_ADDRESS, id: 1) ?? 0 }
        set { emu?.set(.SID_ADDRESS, id: 1, value: newValue) }
    }
    var sidAddress2: Int {
        get { return emu?.get(.SID_ADDRESS, id: 2) ?? 0 }
        set { emu?.set(.SID_ADDRESS, id: 2, value: newValue) }
    }
    var sidAddress3: Int {
        get { return emu?.get(.SID_ADDRESS, id: 3) ?? 0 }
        set { emu?.set(.SID_ADDRESS, id: 3, value: newValue) }
    }
    var sidEngine: Int {
        get { return emu?.get(.SID_ENGINE, id: 0) ?? 0 }
        set { emu?.set(.SID_ENGINE, value: newValue) }
    }
    var sidSampling: Int {
        get { return emu?.get(.SID_SAMPLING, id: 0) ?? 0 }
        set { emu?.set(.SID_SAMPLING, value: newValue) }
    }
    var vol0: Int {
        get { return emu?.get(.AUD_VOL0) ?? 0 }
        set { emu?.set(.AUD_VOL0, value: newValue) }
    }
    var vol1: Int {
        get { return emu?.get(.AUD_VOL1) ?? 0 }
        set { emu?.set(.AUD_VOL1, value: newValue) }
    }
    var vol2: Int {
        get { return emu?.get(.AUD_VOL2) ?? 0 }
        set { emu?.set(.AUD_VOL2, value: newValue) }
    }
    var vol3: Int {
        get { return emu?.get(.AUD_VOL3) ?? 0 }
        set { emu?.set(.AUD_VOL3, value: newValue) }
    }
    var pan0: Int {
        get { return emu?.get(.AUD_PAN0) ?? 0 }
        set { emu?.set(.AUD_PAN0, value: newValue) }
    }
    var pan1: Int {
        get { return emu?.get(.AUD_PAN1) ?? 0 }
        set { emu?.set(.AUD_PAN1, value: newValue) }
    }
    var pan2: Int {
        get { return emu?.get(.AUD_PAN2) ?? 0 }
        set { emu?.set(.AUD_PAN2, value: newValue) }
    }
    var pan3: Int {
        get { return emu?.get(.AUD_PAN3) ?? 0 }
        set { emu?.set(.AUD_PAN3, value: newValue) }
    }
    var volL: Int {
        get { return emu?.get(.AUD_VOL_L) ?? 0 }
        set { emu?.set(.AUD_VOL_L, value: newValue) }
    }
    var volR: Int {
        get { return emu?.get(.AUD_VOL_R) ?? 0 }
        set { emu?.set(.AUD_VOL_R, value: newValue) }
    }
    var stepVolume: Int {
        get { return emu?.get(.DRV_STEP_VOL, drive: DRIVE8) ?? 0 }
        set { emu?.set(.DRV_STEP_VOL, value: newValue) }
    }
    var insertVolume: Int {
        get { return emu?.get(.DRV_INSERT_VOL, drive: DRIVE8) ?? 0 }
        set { emu?.set(.DRV_INSERT_VOL, value: newValue) }
    }
    var ejectVolume: Int {
        get { return emu?.get(.DRV_EJECT_VOL, drive: DRIVE8) ?? 0 }
        set { emu?.set(.DRV_EJECT_VOL, value: newValue) }
    }
    var drive8Pan: Int {
        get { return emu?.get(.DRV_PAN, drive: DRIVE8) ?? 0 }
        set { emu?.set(.DRV_PAN, drive: DRIVE8, value: newValue) }
    }
    var drive9Pan: Int {
        get { return emu?.get(.DRV_PAN, drive: DRIVE9) ?? 0 }
        set { emu?.set(.DRV_PAN, drive: DRIVE9, value: newValue) }
    }

    //
    // Video
    //

    var palette: Int {
        get { return emu?.get(.MON_PALETTE) ?? 0 }
        set { emu?.set(.MON_PALETTE, value: newValue) }
    }

    var brightness: Int {
        get { return emu?.get(.MON_BRIGHTNESS) ?? 0 }
        set { emu?.set(.MON_BRIGHTNESS, value: newValue) }
    }

    var contrast: Int {
        get { return emu?.get(.MON_CONTRAST) ?? 0 }
        set { emu?.set(.MON_CONTRAST, value: newValue) }
    }

    var saturation: Int {
        get { return emu?.get(.MON_SATURATION) ?? 0 }
        set { emu?.set(.MON_SATURATION, value: newValue) }
    }

    var hCenter: Int {
        get { return emu?.get(.MON_HCENTER) ?? 0 }
        set {
            emu?.set(.MON_HCENTER, value: newValue)
            renderer.canvas.updateTextureRect()
        }
    }
    var vCenter: Int {
        get { return emu?.get(.MON_VCENTER) ?? 0 }
        set {
            emu?.set(.MON_VCENTER, value: newValue)
            renderer.canvas.updateTextureRect()
        }
    }

    var hZoom: Int {
        get { return emu?.get(.MON_HZOOM) ?? 0 }
        set {
            emu?.set(.MON_HZOOM, value: newValue)
            renderer.canvas.updateTextureRect()
        }
    }

    var vZoom: Int {
        get { return emu?.get(.MON_VZOOM) ?? 0 }
        set {
            emu?.set(.MON_VZOOM, value: newValue)
            renderer.canvas.updateTextureRect()
        }
    }

    var upscaler: Int {
        get { return emu?.get(.MON_UPSCALER) ?? 0 }
        set {
            if ressourceManager.selectUpscaler(newValue) {
                emu?.set(.MON_UPSCALER, value: newValue)
            }
        }
    }

    var blur: Int {
        get { return emu?.get(.MON_BLUR) ?? 0 }
        set {
            emu?.set(.MON_BLUR, value: newValue)
            renderer.shaderOptions.blur = Int32(newValue)
        }
    }

    var blurRadius: Int {
        get { return emu?.get(.MON_BLUR_RADIUS) ?? 0 }
        set {
            emu?.set(.MON_BLUR_RADIUS, value: newValue)
            renderer.shaderOptions.blurRadius = Float(newValue) / 1000.0
        }
    }

    var bloom: Int {
        get { return emu?.get(.MON_BLOOM) ?? 0 }
        set {
            if ressourceManager.selectBloomFilter(newValue) {
                emu?.set(.MON_BLOOM, value: newValue)
                renderer.shaderOptions.bloom = Int32(bloom)
            }
        }
    }

    var bloomRadius: Int {
        get { return emu?.get(.MON_BLOOM_RADIUS) ?? 0 }
        set {
            emu?.set(.MON_BLOOM_RADIUS, value: newValue)
            renderer.shaderOptions.bloomRadius = Float(newValue) / 1000.0
        }
    }

    var bloomBrightness: Int {
        get { return emu?.get(.MON_BLOOM_BRIGHTNESS) ?? 0 }
        set {
            emu?.set(.MON_BLOOM_BRIGHTNESS, value: newValue)
            renderer.shaderOptions.bloomBrightness = Float(newValue) / 1000.0
        }
    }

    var bloomWeight: Int {
        get { return emu?.get(.MON_BLOOM_WEIGHT) ?? 0 }
        set {
            emu?.set(.MON_BLOOM_WEIGHT, value: newValue)
            renderer.shaderOptions.bloomWeight = Float(newValue) / 1000.0
        }
    }

    var dotMask: Int {
        get { return emu?.get(.MON_DOTMASK) ?? 0 }
        set {
            renderer.shaderOptions.dotMask = Int32(newValue)
            ressourceManager.buildDotMasks()
            if ressourceManager.selectDotMask(newValue) {
                emu?.set(.MON_DOTMASK, value: newValue)
            }
        }
    }

    var dotMaskBrightness: Int {
        get { return emu?.get(.MON_DOTMASK_BRIGHTNESS) ?? 0 }
        set {
            emu?.set(.MON_DOTMASK_BRIGHTNESS, value: newValue)
            renderer.shaderOptions.dotMaskBrightness = Float(newValue) / 1000.0
            ressourceManager.buildDotMasks()
            ressourceManager.selectDotMask(dotMask)
        }
    }

    var scanlines: Int {
        get { return emu?.get(.MON_SCANLINES) ?? 0 }
        set {
            renderer.shaderOptions.scanlines = Int32(newValue)
            if ressourceManager.selectScanlineFilter(newValue) {
                emu?.set(.MON_SCANLINES, value: newValue)
            }
        }
    }

    var scanlineBrightness: Int {
        get { return emu?.get(.MON_SCANLINE_BRIGHTNESS) ?? 0 }
        set {
            emu?.set(.MON_SCANLINE_BRIGHTNESS, value: newValue)
            renderer.shaderOptions.scanlineBrightness = Float(newValue) / 1000.0
        }
    }

    var scanlineWeight: Int {
        get { return emu?.get(.MON_SCANLINE_WEIGHT) ?? 0 }
        set {
            emu?.set(.MON_SCANLINE_WEIGHT, value: newValue)
            renderer.shaderOptions.scanlineWeight = Float(newValue) / 1000.0
        }
    }

    var disalignment: Int {
        get { return emu?.get(.MON_DISALIGNMENT) ?? 0 }
        set {
            emu?.set(.MON_DISALIGNMENT, value: newValue)
            renderer.shaderOptions.disalignment = Int32(newValue)
        }
    }

    var disalignmentH: Int {
        get { return emu?.get(.MON_DISALIGNMENT_H) ?? 0 }
        set {
            emu?.set(.MON_DISALIGNMENT_H, value: newValue)
            renderer.shaderOptions.disalignmentH = Float(newValue) / 1000000.0
        }
    }
    
    var disalignmentV: Int {
        get { return emu?.get(.MON_DISALIGNMENT_V) ?? 0 }
        set {
            emu?.set(.MON_DISALIGNMENT_V, value: newValue)
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
                if let file = try? MediaFileProxy.make(with: url!) {
                    if file.type == type { emu?.loadRom(file) }
                }
            }
        }

        debug(.defaults)
        
        if let emu = emu {

            emu.suspend()
            load(UserDefaults.basicRomUrl, type: .BASIC_ROM)
            load(UserDefaults.charRomUrl, type: .CHAR_ROM)
            load(UserDefaults.kernalRomUrl, type: .KERNAL_ROM)
            load(UserDefaults.vc1541RomUrl, type: .VC1541_ROM)
            emu.resume()
        }
    }

    func saveRomUserDefaults() throws {

        debug(.defaults)

        var url: URL?

        func save(_ type: vc64.RomType) throws {

            if url == nil { throw VC64Error(vc64.ErrorCode.FILE_CANT_WRITE) }
            try? FileManager.default.removeItem(at: url!)
            try emu?.saveRom(type, url: url!)
        }

        if let emu = emu {

            emu.suspend()

            do {
                url = UserDefaults.basicRomUrl;  try save(.BASIC)
                url = UserDefaults.charRomUrl;   try save(.CHAR)
                url = UserDefaults.kernalRomUrl; try save(.KERNAL)
                url = UserDefaults.vc1541RomUrl; try save(.VC1541)

            } catch {

                emu.resume()
                throw error
            }

            emu.resume()
        }
    }
}
