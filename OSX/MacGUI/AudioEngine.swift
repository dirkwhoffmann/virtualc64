/**
Audio interface that connects the macOS GUI with the core emulator
 
 - Remark:
    Replaces old Objective-C interface AudioDevice
 
 - Author: Dirk W. Hoffmann
 - Copyright: Dirk W. Hoffmann
*/

import Foundation
import AVFoundation

@objc public class AudioEngine: NSObject {

    var sid: SIDProxy!
    var audiounit : AUAudioUnit!

    override init()
    {
        print("\(#function)")
        super.init()
    }
    
    convenience init?(withSID proxy: SIDProxy)
    {
        print("")
        print("\(#function)")
    
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
            NSLog("Failed to intantiate AudioUnit")
            return nil
        }
        
        // Query AudioUnit
        let busses = audiounit.outputBusses.count
        let hardwareFormat = audiounit.outputBusses[0].format
        let channels = hardwareFormat.channelCount
        let sampleRate = hardwareFormat.sampleRate
        let stereo = (channels > 1)
        print("  number of output busses:    \(busses)")
        print("  number of channels per bus: \(channels)")
        print("  sample rate:                \(sampleRate)")
        print("  Stereo:                     \(stereo)")
        
        // Make input busses compatible with the output busses
        let renderFormat = AVAudioFormat(standardFormatWithSampleRate: sampleRate,
                                         channels: channels)
        try! audiounit.inputBusses[0].setFormat(renderFormat)
        
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
                return(0)
            }
        } else {
            audiounit.outputProvider = { ( // AURenderPullInputBlock
                actionFlags,
                timestamp,
                frameCount,
                inputBusNumber,
                inputDataList ) -> AUAudioUnitStatus in
                
                self.renderMono(inputDataList: inputDataList, frameCount: frameCount)
                return(0)
            }
        }

        // Allocate render resources
        do { try audiounit.allocateRenderResources() } catch {
            NSLog("Failed to allocate RenderResources")
            return nil
        }
        
        NSLog("AudioEngine initialized successfully")
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
    
    /*! @brief  Start playing sound
     */
    func startPlayback() -> Bool {

        print("\(#function)")
        do { try audiounit.startHardware() } catch {
            NSLog("Failed to start audio hardware")
            return false
        }
        
        return true
    }
    
    /*! @brief  Stop playing sound
     */
    func stopPlayback() {
        
        print("\(#function)")
        audiounit.stopHardware()
    }
}
