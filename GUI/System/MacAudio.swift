// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import AVFoundation

public class MacAudio: NSObject {

    // Current audio source
    private var emu: EmulatorProxy?

    // New audio source
    private var newEmu: EmulatorProxy?

    // Switch delay
    private var switchDelay = 0

    // Gateway to the host's audio unit
    var audiounit: AUAudioUnit!

    // Sample rate of the host's audio unit
    var sampleRate = 0.0

    // Indicates if the this emulator instance owns the audio unit
    var isRunning = false
    
    // Cached audio players
    var audioPlayers: [String: [AVAudioPlayer]] = [:]
    
    override init() {
        
        super.init()

        debug(.lifetime, "Initializing audio interface")
        
        // Setup component description for AudioUnit
        let compDesc = AudioComponentDescription(
            componentType: kAudioUnitType_Output,
            componentSubType: kAudioUnitSubType_DefaultOutput,
            componentManufacturer: kAudioUnitManufacturer_Apple,
            componentFlags: 0,
            componentFlagsMask: 0)
        
        // Create AudioUnit
        do { try audiounit = AUAudioUnit(componentDescription: compDesc) } catch {

            warn("Failed to create AUAudioUnit")
            return
        }
        
        // Query AudioUnit
        let hardwareFormat = audiounit.outputBusses[0].format
        let channels = hardwareFormat.channelCount
        sampleRate = hardwareFormat.sampleRate
        let stereo = (channels > 1)
        
        // Make input bus compatible with output bus
        let renderFormat = AVAudioFormat(standardFormatWithSampleRate: sampleRate,
                                         channels: (stereo ? 2 : 1))
        do { try audiounit.inputBusses[0].setFormat(renderFormat!) } catch {

            warn("Failed to set render format on input bus")
            return
        }

        // Inform the emulator about the sample rate
        emu?.set(.HOST_SAMPLE_RATE, value: Int(sampleRate))

        // Register render callback
        if stereo {
            audiounit.outputProvider = { (
                actionFlags,
                timestamp,
                frameCount,
                inputBusNumber,
                inputDataList ) -> AUAudioUnitStatus in
                
                self.renderStereo(inputDataList: inputDataList, frameCount: frameCount)
                return 0
            }
        } else {
            audiounit.outputProvider = { (
                actionFlags,
                timestamp,
                frameCount,
                inputBusNumber,
                inputDataList ) -> AUAudioUnitStatus in
                
                self.renderMono(inputDataList: inputDataList, frameCount: frameCount)
                return 0
            }
        }
        
        // Allocate render resources
        do { try audiounit.allocateRenderResources() } catch {

            warn("Failed to allocate RenderResources")
            return
        }

        startPlayback()
    }
    
    func shutDown() {
        
        debug(.shutdown)

        stopPlayback()
        emu = nil
        newEmu = nil
    }
    
    func switchSource(_ newSource: EmulatorProxy) {

        if emu != newSource {

            // Fade out volume
            emu?.sid.rampDown()

            // Remember the new audio source
            newEmu = newSource

            // Schedule the assignment of the new audio source
            switchDelay = 20
        }
    }

    private func renderMono(inputDataList: UnsafeMutablePointer<AudioBufferList>,
                            frameCount: UInt32) {
        
        updateSource()

        let bufferList = UnsafeMutableAudioBufferListPointer(inputDataList)
        assert(bufferList.count == 1)
        
        let ptr = bufferList[0].mData!.assumingMemoryBound(to: Float.self)

        if emu == nil {
            memset(ptr, 0, Int(frameCount))
        } else {
            emu!.sid.copyMono(ptr, size: Int(frameCount))
        }
    }
    
    private func renderStereo(inputDataList: UnsafeMutablePointer<AudioBufferList>,
                              frameCount: UInt32) {
        
        updateSource()

        let bufferList = UnsafeMutableAudioBufferListPointer(inputDataList)
        assert(bufferList.count > 1)
        
        let ptr1 = bufferList[0].mData!.assumingMemoryBound(to: Float.self)
        let ptr2 = bufferList[1].mData!.assumingMemoryBound(to: Float.self)

        if emu == nil {
            memset(ptr1, 0, Int(frameCount))
            memset(ptr2, 0, Int(frameCount))
        } else {
            emu!.sid.copyStereo(ptr1, buffer2: ptr2, size: Int(frameCount))
        }
    }
    
    private func updateSource() {

        if emu != newEmu {

            if switchDelay > 0 {

                debug(.audio, "Switch countdown \(switchDelay)")
                switchDelay -= 1

            } else {

                debug(.audio, "Switching source")
                emu = newEmu
                emu?.sid.rampUp()
            }
        }
    }

    // Connects SID to the audio backend
    @discardableResult
    func startPlayback() -> Bool {
        
        if !isRunning {
            do { try audiounit.startHardware() } catch {
                warn("Failed to start audio hardware")
                return false
            }
        }
        
        isRunning = true
        return true
    }
    
    // Disconnects SID from the audio backend
    func stopPlayback() {
        
        if isRunning {
            audiounit.stopHardware()
            isRunning = false
        }
    }
    
    //
    // Playing sound files
    //
    
    func playPowerSound(volume: Int, pan: Int) {
        
        playSound(name: "1541_power_on_0", volume: volume, pan: pan)
    }

    func playStepSound(volume: Int, pan: Int) {
                
        playSound(name: "1541_track_change_2", volume: volume, pan: pan)
    }

    func playInsertSound(volume: Int, pan: Int) {
        
        playSound(name: "1541_door_closed_2", volume: volume, pan: pan)
    }
 
    func playEjectSound(volume: Int, pan: Int) {
        
        playSound(name: "1541_door_open_1", volume: volume, pan: pan)
    }
    
    func playSound(name: String, volume: Int, pan: Int) {

        let p = pan <= 50 ? pan : pan <= 150 ? 100 - pan : -200 + pan
        
        let scaledVolume = Float(volume) / 100.0
        let scaledPan = Float(p) / 50.0
                
        playSound(name: name, volume: scaledVolume, pan: scaledPan)
    }
    
    func playSound(name: String, volume: Float, pan: Float) {
                
        // Check for cached players for this sound file
        if audioPlayers[name] == nil {
            
            // Lookup sound file in bundle
            guard let url = Bundle.main.url(forResource: name, withExtension: "aiff") else {
                warn("Cannot open sound file \(name)")
                return
            }
            
            // Create a couple of player instances for this sound file
            do {
                audioPlayers[name] = []
                try audioPlayers[name]!.append(AVAudioPlayer(contentsOf: url))
                try audioPlayers[name]!.append(AVAudioPlayer(contentsOf: url))
                try audioPlayers[name]!.append(AVAudioPlayer(contentsOf: url))
            } catch let error {
                print(error.localizedDescription)
            }
        }
        
        // Play sound if a free is available
        for player in audioPlayers[name]! where !player.isPlaying {
            
            player.volume = volume
            player.pan = pan
            player.play()
            return
        }
    }
}
