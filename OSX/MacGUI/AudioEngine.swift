//
//  AudioEngine.swift
//  V64
//
//  Created by Dirk Hoffmann on 15.06.17.
//
//

import Foundation
import AVFoundation

@objc public class AudioEngine: NSObject {

    var sid: SIDProxy!
    var iounit : AUAudioUnit!

    let twopi:Float = 2.0 * 3.14159
    var freq:Float = 440.00
    var sampleRate:Float = 44100.00
    override init()
    {
        print("\(#function)")
        super.init()
    }
    
    convenience init(withSID proxy: SIDProxy)
    {
        print("")
        print("\(#function)")
        print("")
    
        self.init()
        sid = proxy

        // Create AudioUnit
        let subType = kAudioUnitSubType_HALOutput
        let ioUnitDesc = AudioComponentDescription(
            componentType: kAudioUnitType_Output,
            componentSubType: subType,
            componentManufacturer: kAudioUnitManufacturer_Apple,
            componentFlags: 0,
            componentFlagsMask: 0)
        iounit = try! AUAudioUnit(componentDescription: ioUnitDesc)
        
        // Configure
        let hardwareFormat = iounit.outputBusses[0].format
        let renderFormat = AVAudioFormat(
            standardFormatWithSampleRate: hardwareFormat.sampleRate,
            channels: hardwareFormat.channelCount)
        try! iounit.inputBusses[0].setFormat(renderFormat)

        print("")
        print("sample rate: \(renderFormat.sampleRate)")
        print("")
        
        // Callback code
        let callback : AURenderPullInputBlock = {
            (flags: UnsafeMutablePointer<AudioUnitRenderActionFlags>,
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
        
        iounit.outputProvider = callback
        try! iounit.allocateRenderResources()
    }
    
    func renderMono(buffer: AudioBuffer) {
        let nframes = Int(buffer.mDataByteSize) / MemoryLayout<Float>.size
        let mdata = buffer.mData
        let ptr = mdata!.assumingMemoryBound(to: Float.self)
        //var ptr = UnsafeMutablePointer<Float>(buffer.mData)!
        sid.readMonoSamples(ptr, size: nframes)
    }

    func renderStereo(buffer1: AudioBuffer, buffer2 : AudioBuffer) {
        let nframes = Int(buffer1.mDataByteSize) / (MemoryLayout<Float>.size)
        let mdata1 = buffer1.mData
        let mdata2 = buffer2.mData
        var ptr1 = mdata1!.assumingMemoryBound(to: Float.self)
        var ptr2 = mdata2!.assumingMemoryBound(to: Float.self)

        for _ in 0 ..< nframes {
            let sample = sid.getSample()
            ptr1.pointee = sample
            ptr1 = ptr1.successor()
            ptr2.pointee = sample
            ptr2 = ptr2.successor()
        }
    }


    /*! @brief  Zeroes out loopBuffer
     */
    func clearBuffer() {
        
        print("")
        print("\(#function)")
        print("")
    }
    
    /*! @brief  Start playing sound
     */
    func startPlayback() {

        print("")
        print("\(#function)")
        print("")
        
        try! iounit.startHardware() // Let's get loud
    }
    
    /*! @brief  Stop playing sound
     */
    func stopPlayback() {
        
        print("\(#function)")
        iounit.stopHardware()
    }
}
