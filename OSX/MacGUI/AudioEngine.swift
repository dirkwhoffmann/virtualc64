//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation
import AVFoundation

public class AudioEngine: NSObject {

    var sid: SIDProxy!
    var audiounit : AUAudioUnit!

    override init()
    {
        super.init()
    }
    
    @objc convenience init?(withSID proxy: SIDProxy)
    {
        track()
    
        self.init()
        sid = proxy

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
        // let inputbusses = audiounit.inputBusses.count
        // let outputbusses = audiounit.outputBusses.count
        let hardwareFormat = audiounit.outputBusses[0].format
        let channels = hardwareFormat.channelCount
        let sampleRate = hardwareFormat.sampleRate
        let stereo = (channels > 1)
        // NSLog("  Number of input busses:      \(inputbusses)")
        // NSLog("  Number of output busses:     \(outputbusses)")
        // NSLog("  Number of channels of bus 0: \(channels)")
        // NSLog("  Sample rate:                 \(sampleRate)")
        // NSLog("  Stereo:                      \(stereo)")
        
        // Make input bus compatible with output bus
        let renderFormat = AVAudioFormat(standardFormatWithSampleRate: sampleRate,
                                         channels: (stereo ? 2 : 1))
        do { try audiounit.inputBusses[0].setFormat(renderFormat!) } catch {
            track("Failed to set render format on input bus")
            return
        }
        
        // Tell SID to use the correct sample rate
        sid.setSampleRate(UInt32(sampleRate))
        
        // Register render callback
        if (stereo) {
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
     }
    
    private func renderMono(inputDataList : UnsafeMutablePointer<AudioBufferList>,
                            frameCount : UInt32)
    {
        let bufferList = UnsafeMutableAudioBufferListPointer(inputDataList)
        assert(bufferList.count == 1)
        
        let ptr = bufferList[0].mData!.assumingMemoryBound(to: Float.self)
        sid.readMonoSamples(ptr, size: Int(frameCount))
    }
  
    private func renderStereo(inputDataList : UnsafeMutablePointer<AudioBufferList>,
                            frameCount : UInt32)
    {
        let bufferList = UnsafeMutableAudioBufferListPointer(inputDataList)
        assert(bufferList.count > 1)
        
        let ptr1 = bufferList[0].mData!.assumingMemoryBound(to: Float.self)
        let ptr2 = bufferList[1].mData!.assumingMemoryBound(to: Float.self)
        sid.readStereoSamples(ptr1, buffer2: ptr2, size: Int(frameCount))
    }
    
    //! @brief  Start playing sound
    @objc func startPlayback() -> Bool {

        do { try audiounit.startHardware() } catch {
            NSLog("Failed to start audio hardware")
            return false
        }
        
        return true
    }
    
    //! @brief  Stop playing sound
    @objc func stopPlayback() {
        
        audiounit.stopHardware()
    }
}
