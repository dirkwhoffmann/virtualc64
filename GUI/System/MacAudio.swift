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

    var parent: MyController!

    // Audio source
    var emu: EmulatorProxy? { return parent.emu }

    // Gateway to the host's audio unit
    var audiounit: AUAudioUnit!

    // Sample rate of the host's audio unit
    var sampleRate = 0.0

    // Indicates if sound samples should be handed over or not
    var muted = false

    // Cached audio players
    var audioPlayers: [String: [AVAudioPlayer]] = [:]
    
    convenience init?(with controller: MyController) {

        debug(.lifetime, "Initializing audio interface")

        self.init()
        parent = controller

        // Create AudioUnit
        let compDesc = AudioComponentDescription(
            componentType: kAudioUnitType_Output,
            componentSubType: kAudioUnitSubType_DefaultOutput,
            componentManufacturer: kAudioUnitManufacturer_Apple,
            componentFlags: 0,
            componentFlagsMask: 0)
        
        do { try audiounit = AUAudioUnit(componentDescription: compDesc) } catch {

            warn("Failed to create AUAudioUnit")
            return
        }
        
        // Query parameters
        let hardwareFormat = audiounit.outputBusses[0].format
        let channels = hardwareFormat.channelCount
        sampleRate = hardwareFormat.sampleRate
        let stereo = (channels > 1)
        
        // Pass some host parameters to the emulator
        emu?.set(.HOST_SAMPLE_RATE, value: Int(sampleRate))

        // Make the input bus compatible with the output bus
        let renderFormat = AVAudioFormat(standardFormatWithSampleRate: sampleRate,
                                         channels: (stereo ? 2 : 1))

        do { try audiounit.inputBusses[0].setFormat(renderFormat!) } catch {

            warn("Failed to set render format on input bus")
            return
        }

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
        
        // Allocate render resources and start the audio hardware
        do { try audiounit.allocateRenderResources() } catch {

            warn("Failed to allocate RenderResources")
            return
        }
        do { try audiounit.startHardware() } catch {
            
            warn("Failed to start audio hardware")
            return
        }
    }

    func shutDown() {
        
        audiounit.stopHardware()
        audiounit.outputProvider = nil
    }

    private func renderMono(inputDataList: UnsafeMutablePointer<AudioBufferList>,
                            frameCount: UInt32) {

        let bufferList = UnsafeMutableAudioBufferListPointer(inputDataList)
        assert(bufferList.count == 1)
        
        let ptr = bufferList[0].mData!.assumingMemoryBound(to: Float.self)

        if muted {
            memset(ptr, 0, 4 * Int(frameCount))
        } else {
            emu?.audioPort.copyMono(ptr, size: Int(frameCount))
        }
    }
    
    private func renderStereo(inputDataList: UnsafeMutablePointer<AudioBufferList>,
                              frameCount: UInt32) {

        let bufferList = UnsafeMutableAudioBufferListPointer(inputDataList)
        assert(bufferList.count > 1)
        
        let ptr1 = bufferList[0].mData!.assumingMemoryBound(to: Float.self)
        let ptr2 = bufferList[1].mData!.assumingMemoryBound(to: Float.self)

        if muted {
            memset(ptr1, 0, 4 * Int(frameCount))
            memset(ptr2, 0, 4 * Int(frameCount))
        } else {
            emu?.audioPort.copyStereo(ptr1, buffer2: ptr2, size: Int(frameCount))
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
