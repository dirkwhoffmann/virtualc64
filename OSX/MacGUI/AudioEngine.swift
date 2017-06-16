/**
Audio interface that connects the macOS GUI with the core emulator
 
 - Remark:
    Replaces old Objective-C interface AudioDevice
 
 - Author: Dirk W. Hoffmann
 - Copyright: Dirk W. Hoffmann
*/

// TODO
// Optimize renderStereo (pass pointer to sid and copy there)
// Add readStereoSamplesInterleaved
//     readStereoSamples(float *left, float *right)
// Add catch code to try blocks
// Print if mono or stereo, pass sample rate to sid
// Make inline callback func a real func
// Make sure emulator does not crash if audio object cannot be initalized

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
        print("  number of output busses:    \(busses)")
        print("  number of channels per bus: \(channels)")
        print("  sample rate:                \(sampleRate)")
        
        // Make input busses compatible with the output busses
        let renderFormat = AVAudioFormat(standardFormatWithSampleRate: sampleRate,
                                         channels: channels)
        try! audiounit.inputBusses[0].setFormat(renderFormat)
        
        // Tell SID to use the correct sample rate
        sid.setSampleRate(UInt32(sampleRate))
        
        // Callback code
        /*
        let callback : AURenderPullInputBlock = {
            (actionflags: UnsafeMutablePointer<AudioUnitRenderActionFlags>,
            ts: UnsafePointer<AudioTimeStamp>,
            fc: AUAudioFrameCount,
            bus: Int,
            rawBuff: UnsafeMutablePointer<AudioBufferList>
            ) -> AUAudioUnitStatus
            in
            let bufferList = UnsafeMutableAudioBufferListPointer(rawBuff)
        
            if bufferList.count > 1 {
                self.renderStereo(buffer1: bufferList[0],
                                  buffer2: bufferList[1])
            } else if bufferList.count > 0 {
                self.renderMono(buffer: bufferList[0])
            }
            return noErr
        }
         */
        
        // Register render callback
        audiounit.outputProvider = { ( // AURenderPullInputBlock
            actionFlags,
            timestamp,
            frameCount,
            inputBusNumber,
            inputDataList ) -> AUAudioUnitStatus in
            
            self.render(inputDataList: inputDataList, frameCount: frameCount)
            return(0)
        }

        // Try to allocate render resources
        do { try audiounit.allocateRenderResources() } catch {
            NSLog("Failed to allocate RenderResources")
            return nil
        }
        
        NSLog("AudioEngine initialized successfully")
     }
    

    /*
    func callbackfunc(flags: UnsafeMutablePointer<AudioUnitRenderActionFlags>,
                  ts: UnsafePointer<AudioTimeStamp>,
                  fc: AUAudioFrameCount,
                  bus: Int,
                  rawBuff: UnsafeMutablePointer<AudioBufferList>) -> AUAudioUnitStatus {

        let bufferList = UnsafeMutableAudioBufferListPointer(rawBuff)
        
        if bufferList.count > 1 {
            self.renderStereo(buffer1: bufferList[0],
                              buffer2: bufferList[1])
        } else if bufferList.count > 0 {
            self.renderMono(buffer: bufferList[0])
        }
        return noErr
    }
*/
    
    private func render(inputDataList : UnsafeMutablePointer<AudioBufferList>,
                        frameCount : UInt32) {
        
        let bufferList = UnsafeMutableAudioBufferListPointer(inputDataList)
        if bufferList.count > 1 {
            self.renderStereo(buffer1: bufferList[0],
                              buffer2: bufferList[1], frameCount: frameCount)
        } else if bufferList.count > 0 {
            self.renderMono(buffer: bufferList[0], frameCount: frameCount)
        }
    }

        
        
    func renderMono(buffer: AudioBuffer, frameCount: UInt32) {
        let sizeOfFloat = MemoryLayout<Float>.size
        let frames = Int(buffer.mDataByteSize) / sizeOfFloat
        let ptr = buffer.mData!.assumingMemoryBound(to: Float.self)
        sid.readMonoSamples(ptr, size: frames)
    }

    func renderStereo(buffer1: AudioBuffer, buffer2 : AudioBuffer, frameCount: UInt32) {
        let sizeOfFloat = UInt32(MemoryLayout<Float>.size)
        let frames = buffer1.mDataByteSize / sizeOfFloat
        if (frames != frameCount) {
            print("FRAME COUNT MISMATCH")
        }
        let ptr1 = buffer1.mData!.assumingMemoryBound(to: Float.self)
        let ptr2 = buffer2.mData!.assumingMemoryBound(to: Float.self)
        sid.readStereoSamples(ptr1, buffer2: ptr2, size: Int(frames))
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
