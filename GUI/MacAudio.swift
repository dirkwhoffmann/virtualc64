// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import AVFoundation

public class MacAudio: NSObject {

    var parent: MyController!
    var audiounit: AUAudioUnit!
    var sid: SIDProxy!

    var prefs: Preferences { return parent.pref }
    
    // Indicates if the this emulator instance owns the audio unit
    var isRunning = false
    
    // Cached audio players
    var audioPlayers: [String: [AVAudioPlayer]] = [:]
    
    override init() {
        
        super.init()
    }
    
    convenience init?(with controller: MyController) {
        
        track()
        
        self.init()
        parent = controller
        sid = controller.c64.sid
        
        // Setup component description for AudioUnit
        let compDesc = AudioComponentDescription(
            componentType: kAudioUnitType_Output,
            componentSubType: kAudioUnitSubType_DefaultOutput,
            componentManufacturer: kAudioUnitManufacturer_Apple,
            componentFlags: 0,
            componentFlagsMask: 0)
        
        // Create AudioUnit
        do { try audiounit = AUAudioUnit(componentDescription: compDesc) } catch {
            track("Failed to create AUAudioUnit")
            return
        }
        
        // Query AudioUnit
        let hardwareFormat = audiounit.outputBusses[0].format
        let channels = hardwareFormat.channelCount
        let sampleRate = hardwareFormat.sampleRate
        let stereo = (channels > 1)
        
        // Make input bus compatible with output bus
        let renderFormat = AVAudioFormat(standardFormatWithSampleRate: sampleRate,
                                         channels: (stereo ? 2 : 1))
        do { try audiounit.inputBusses[0].setFormat(renderFormat!) } catch {
            track("Failed to set render format on input bus")
            return
        }
        
        // Inform SID about the sample rate
        sid.setSampleRate(sampleRate)
        
        // Register render callback
        if stereo {
            audiounit.outputProvider = { ( // AURenderPullInputBlock
                actionFlags,
                timestamp,
                frameCount,
                inputBusNumber,
                inputDataList ) -> AUAudioUnitStatus in
                
                self.renderStereo(inputDataList: inputDataList, frameCount: frameCount)
                return 0
            }
        } else {
            audiounit.outputProvider = { ( // AURenderPullInputBlock
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
            track("Failed to allocate RenderResources")
            return nil
        }
        
        track("Success")
    }
    
    func shutDown() {
        
        track()
        stopPlayback()
        sid = nil
    }
    
    private func renderMono(inputDataList: UnsafeMutablePointer<AudioBufferList>,
                            frameCount: UInt32) {
        
        let bufferList = UnsafeMutableAudioBufferListPointer(inputDataList)
        assert(bufferList.count == 1)
        
        let ptr = bufferList[0].mData!.assumingMemoryBound(to: Float.self)
        sid.copyMono(ptr, size: Int(frameCount))
    }
    
    private func renderStereo(inputDataList: UnsafeMutablePointer<AudioBufferList>,
                              frameCount: UInt32) {
        
        let bufferList = UnsafeMutableAudioBufferListPointer(inputDataList)
        assert(bufferList.count > 1)
        
        let ptr1 = bufferList[0].mData!.assumingMemoryBound(to: Float.self)
        let ptr2 = bufferList[1].mData!.assumingMemoryBound(to: Float.self)
        sid.copyStereo(ptr1, buffer2: ptr2, size: Int(frameCount))
    }
    
    // Connects SID to the audio backend
    @discardableResult
    func startPlayback() -> Bool {
        
        if !isRunning {
            do { try audiounit.startHardware() } catch {
                track("Failed to start audio hardware")
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
    
    // Plays a sound file
    func playSound(name: String, volume: Float = 1.0) {
        
        // Check for cached players for this sound file
        if audioPlayers[name] == nil {
            
            // Lookup sound file in bundle
            guard let url = Bundle.main.url(forResource: name, withExtension: "aiff") else {
                track("Cannot open sound file \(name)")
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
            player.pan = Float(prefs.driveSoundPan)
            player.play()
            return
        }
    }
}
