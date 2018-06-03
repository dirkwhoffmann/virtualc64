/*
 * Author: Dirk W. Hoffmann. 2018, All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#import <Cocoa/Cocoa.h>
#import "C64_types.h"
#import "basic.h"

// Forward declarations of proxy classes
@class MyController;
@class C64Proxy;
@class SnapshotProxy;
@class D64ArchiveProxy; 
@class ArchiveProxy;
@class TAPProxy;
@class CRTProxy;

// Forward declarations of wrappers for C++ classes.
// We wrap classes into normal C structs to avoid any reference to C++.

struct C64Wrapper;
struct CpuWrapper;
struct MemoryWrapper;
struct VicWrapper;
struct CiaWrapper;
struct KeyboardWrapper;
struct ControlPortWrapper;
struct SidBridgeWrapper;
struct IecWrapper;
struct ExpansionPortWrapper;
struct Via6522Wrapper;
struct Disk525Wrapper;
struct Vc1541Wrapper;
struct DatasetteWrapper;
struct ContainerWrapper;

//
// CPU
//

@interface CPUProxy : NSObject {
        
	struct CpuWrapper *wrapper;
}

- (CPUInfo) getInfo;
- (void) dump;

- (BOOL) tracingEnabled;
- (void) setTraceMode:(BOOL)b;

- (uint16_t) pc;
- (void) setPC:(uint16_t)pc;
- (void) setSP:(uint8_t)sp;
- (void) setA:(uint8_t)a;
- (void) setX:(uint8_t)x;
- (void) setY:(uint8_t)y;
- (void) setNflag:(BOOL)b;
- (void) setZflag:(BOOL)b;
- (void) setCflag:(BOOL)b;
- (void) setIflag:(BOOL)b;
- (void) setBflag:(BOOL)b;
- (void) setDflag:(BOOL)b;
- (void) setVflag:(BOOL)b;

- (BOOL) breakpoint:(uint16_t)addr;
- (void) setBreakpoint:(uint16_t)addr;
- (void) deleteBreakpoint:(uint16_t)addr;
- (void) toggleBreakpoint:(uint16_t)addr;

- (DisassembledInstruction) disassemble:(uint16_t)addr hex:(BOOL)h;

@end

//
// Memory
//

@interface MemoryProxy : NSObject {
    
	struct MemoryWrapper *wrapper;
}

- (void) dump;

- (uint8_t) spy:(uint16_t)addr;
- (uint8_t) spy:(uint16_t)addr source:(MemoryType)source;

- (MemoryType) pokeTarget:(uint16_t)addr;
- (void) poke:(uint16_t)addr value:(uint8_t)value;
- (void) pokeTo:(uint16_t)addr value:(uint8_t)value target:(MemoryType)target;
- (void) pokeIO:(uint16_t)addr value:(uint8_t)value;
- (MemoryType) peekSource:(uint16_t)addr;

@end

// --------------------------------------------------------------------------
//                                    VIC
// --------------------------------------------------------------------------

@interface VICProxy : NSObject {
    
	struct VicWrapper *wrapper;
}

- (void *) screenBuffer;
- (NSColor *) color:(NSInteger)nr;
- (NSInteger) colorScheme;
- (void) setColorScheme:(NSInteger)scheme;

- (VICInfo) getInfo;
- (SpriteInfo) getSpriteInfo:(NSInteger)sprite;
- (void) dump;

- (void) setMemoryBankAddr:(uint16_t)addr;
- (void) setScreenMemoryAddr:(uint16_t)addr;
- (void) setCharacterMemoryAddr:(uint16_t)addr;

- (void) setDisplayMode:(DisplayMode)mode;
- (void) setScreenGeometry:(ScreenGeometry)mode;
- (void) setHorizontalRasterScroll:(NSInteger)offset;
- (void) setVerticalRasterScroll:(NSInteger)offset;

- (void) setSpriteEnabled:(NSInteger)nr value:(BOOL)flag;
- (void) toggleSpriteEnabled:(NSInteger)nr;
- (void) setSpriteX:(NSInteger)nr value:(int)x;
- (void) setSpriteY:(NSInteger)nr value:(int)y;
- (void) setSpriteStretchX:(NSInteger)nr value:(BOOL)flag;
- (void) toggleSpriteStretchX:(NSInteger)nr;
- (void) setSpriteStretchY:(NSInteger)nr value:(BOOL)flag;
- (void) toggleSpriteStretchY:(NSInteger)nr;
- (void) setSpriteColor:(NSInteger)nr value:(int)c;
- (void) setSpritePriority:(NSInteger)nr value:(BOOL)flag;
- (void) toggleSpritePriority:(NSInteger)nr;
- (void) setSpriteMulticolor:(NSInteger)nr value:(BOOL)flag;
- (void) toggleSpriteMulticolor:(NSInteger)nr;

- (void) setIrqOnSpriteSpriteCollision:(BOOL)value;
- (void) toggleIrqOnSpriteSpriteCollision;
- (void) setIrqOnSpriteBackgroundCollision:(BOOL)value;
- (void) toggleIrqOnSpriteBackgroundCollision;

- (uint16_t) rasterline;
- (void) setRasterline:(uint16_t)line;
- (uint16_t) rasterInterruptLine;
- (void) setRasterInterruptLine:(uint16_t)line;
- (BOOL) rasterInterruptFlag;
- (void) setRasterInterruptFlag:(BOOL)b;
- (void) toggleRasterInterruptFlag;

- (BOOL) hideSprites;
- (void) setHideSprites:(BOOL)b;
- (BOOL) showIrqLines;
- (void) setShowIrqLines:(BOOL)b;
- (BOOL) showDmaLines;
- (void) setShowDmaLines:(BOOL)b;

@end

// --------------------------------------------------------------------------
//                                     CIA
// --------------------------------------------------------------------------

@interface CIAProxy : NSObject {
    
	struct CiaWrapper *wrapper;
}

- (void) dump;
- (void) setTraceMode:(bool)b;
- (CIAInfo) getInfo;

@end 

// --------------------------------------------------------------------------
//                                  Keyboard
// --------------------------------------------------------------------------

@interface KeyboardProxy : NSObject {
    
    struct KeyboardWrapper *wrapper;
}

- (void) dump;

- (void) pressKeyAtRow:(NSInteger)row col:(NSInteger)col;
- (void) pressRestoreKey;

- (void) releaseKeyAtRow:(NSInteger)row col:(NSInteger)col;
- (void) releaseRestoreKey;
- (void) releaseAll;

- (BOOL) shiftLockIsPressed;
- (void) lockShift;
- (void) unlockShift;

@end 

// --------------------------------------------------------------------------
//                                 Joystick
// -------------------------------------------------------------------------

@interface ControlPortProxy : NSObject {
    
    struct ControlPortWrapper *wrapper;
}

- (void) trigger:(JoystickEvent)event;
- (void) dump;

@end

// --------------------------------------------------------------------------
//                                    SID
// --------------------------------------------------------------------------

@interface SIDProxy : NSObject {
    
	struct SidBridgeWrapper *wrapper;
}

- (void) dump;
- (SIDInfo) getInfo;
- (VoiceInfo) getVoiceInfo:(NSInteger)voice;
- (uint32_t) sampleRate;
- (void) setSampleRate:(uint32_t)rate;
- (void) readMonoSamples:(float *)target size:(NSInteger)n;
- (void) readStereoSamples:(float *)target1 buffer2:(float *)target2 size:(NSInteger)n;
- (void) readStereoSamplesInterleaved:(float *)target size:(NSInteger)n;
- (NSInteger) ringbufferSize;
- (NSInteger) bufferUnderflows;
- (NSInteger) bufferOverflows;
- (double) fillLevel;
- (float) snoop:(NSInteger)offset;
- (float) snoop:(NSInteger)offset range:(NSInteger)range;

@end

// --------------------------------------------------------------------------
//                                   IEC bus
// -------------------------------------------------------------------------

@interface IECProxy : NSObject {

    struct IecWrapper *wrapper;
}

- (void) dump;
- (bool) tracingEnabled;
- (void) setTraceMode:(bool)b;
- (bool) isDriveConnected;
- (void) connectDrive;
- (void) disconnectDrive;
- (BOOL) atnLine;
- (BOOL) clockLine;
- (BOOL) dataLine;

@end

// --------------------------------------------------------------------------
//                                 Expansion port
// -------------------------------------------------------------------------

@interface ExpansionPortProxy : NSObject {
    
    struct ExpansionPortWrapper *wrapper;
}

- (void) dump;
- (bool) cartridgeAttached; 
- (CartridgeType) cartridgeType;
- (void) pressFirstButton;
- (void) pressSecondButton;

@end

// --------------------------------------------------------------------------
//                                      VIA
// -------------------------------------------------------------------------

@interface VIAProxy : NSObject {
    
	struct Via6522Wrapper *wrapper;
}

- (void) dump;
- (bool) tracingEnabled;
- (void) setTraceMode:(bool)b;

@end

// --------------------------------------------------------------------------
//                                5,25" diskette
// -------------------------------------------------------------------------

@interface Disk525Proxy : NSObject {
    
    struct Disk525Wrapper *wrapper;
}

- (void) dump;
- (BOOL)isWriteProtected;
- (void)setWriteProtection:(BOOL)b;
- (BOOL)isModified;
- (void)setModified:(BOOL)b;
- (NSInteger)numTracks;

@end

// --------------------------------------------------------------------------
//                                    VC1541
// -------------------------------------------------------------------------

@interface VC1541Proxy : NSObject {
    
	struct Vc1541Wrapper *wrapper;
    
    // sub proxys
	CPUProxy *cpu;
	VIAProxy *via1;
	VIAProxy *via2;
    Disk525Proxy *disk;
}

@property (readonly) struct Vc1541Wrapper *wrapper;
@property (readonly) CPUProxy *cpu;
@property (readonly) VIAProxy *via1;
@property (readonly) VIAProxy *via2;
@property (readonly) Disk525Proxy *disk;

- (VIAProxy *) via:(int)num;

- (void) dump;
- (bool) tracingEnabled;
- (void) setTraceMode:(bool)b;
- (bool) hasRedLED;
- (bool) hasDisk;
- (bool) hasModifiedDisk;
- (void) ejectDisk;
- (bool) writeProtection;
- (void) setWriteProtection:(bool)b;
- (bool) DiskModified;
- (void) setDiskModified:(bool)b;
- (bool) soundMessagesEnabled;
- (void) setSendSoundMessages:(bool)b;

- (NSInteger) halftrack;
- (void) setHalftrack:(NSInteger)value;
- (NSInteger) numberOfBits;
- (NSInteger) bitOffset;
- (void) setBitOffset:(NSInteger)value;
- (NSInteger) readBitFromHead;
- (void) writeBitToHead:(NSInteger)value;

- (void) moveHeadUp;
- (void) moveHeadDown;
- (void) rotateDisk;
- (void) rotateBack;

- (const char *)dataAbs:(NSInteger)start;
- (const char *)dataAbs:(NSInteger)start length:(NSInteger)n;
- (const char *)dataRel:(NSInteger)start;
- (const char *)dataRel:(NSInteger)start length:(NSInteger)n;

- (bool) exportToD64:(NSString *)path;
- (void) playSound:(NSString *)name volume:(float)v;

@end

// --------------------------------------------------------------------------
//                                  Datasette
// --------------------------------------------------------------------------

@interface DatasetteProxy : NSObject {
    
    struct DatasetteWrapper *wrapper;
}

- (void) dump;

- (bool) hasTape;
- (void) pressPlay;
- (void) pressStop;
- (void) rewind;
- (void) ejectTape;
- (NSInteger) getType; 
- (long) durationInCycles;
- (int) durationInSeconds;
- (NSInteger) head;
- (NSInteger) headInCycles;
- (int) headInSeconds;
- (void) setHeadInCycles:(long)value;
- (BOOL) motor;
- (BOOL) playKey;

@end


// -------------------------------------------------------------------------
//                                    C64
// -------------------------------------------------------------------------

@interface C64Proxy : NSObject {
    
	struct C64Wrapper *wrapper;
    
	// Sub component proxys
	CPUProxy *cpu;
	MemoryProxy *mem;
	VICProxy *vic;
	CIAProxy *cia1;
	CIAProxy *cia2;
	SIDProxy *sid;
	KeyboardProxy *keyboard;
    ControlPortProxy *port1;
    ControlPortProxy *port2;
	IECProxy *iec;
    ExpansionPortProxy *expansionport;
	VC1541Proxy *vc1541;
    DatasetteProxy *datasette;

	//! Indicates that data is transmitted on the IEC bus
	BOOL iecBusIsBusy;

    //! Indicates that data is transmitted on the datasette data line
    BOOL tapeBusIsBusy;

    //! Currently used color scheme
    long colorScheme;
}

@property (readonly) CPUProxy *cpu;
@property (readonly) MemoryProxy *mem;
@property (readonly) VICProxy *vic;
@property (readonly) CIAProxy *cia1;
@property (readonly) CIAProxy *cia2;
@property (readonly) SIDProxy *sid;
@property (readonly) KeyboardProxy *keyboard;
@property (readonly) ControlPortProxy *port1;
@property (readonly) ControlPortProxy *port2;
@property (readonly) IECProxy *iec;
@property (readonly) ExpansionPortProxy *expansionport;
@property (readonly) VC1541Proxy *vc1541;
@property (readonly) DatasetteProxy *datasette;

@property BOOL iecBusIsBusy;
@property BOOL tapeBusIsBusy;

- (struct C64Wrapper *)wrapper;
- (void) kill;

// Hardware configuration
- (bool) reSID;
- (void) setReSID:(bool)b;
- (bool) audioFilter;
- (void) setAudioFilter:(bool)b;
- (NSInteger) samplingMethod;
- (void) setSamplingMethod:(NSInteger)value;
- (NSInteger) chipModel;
- (void) setChipModel:(NSInteger)value;
- (void) rampUp;
- (void) rampUpFromZero;
- (void) rampDown;

// Loadind and saving
- (void)_loadFromSnapshotWrapper:(struct ContainerWrapper *) snapshot;
- (void)loadFromSnapshot:(SnapshotProxy *) snapshot;
- (void)_saveToSnapshotWrapper:(struct ContainerWrapper *) snapshot;
- (void)saveToSnapshot:(SnapshotProxy *) snapshot;

- (CIAProxy *) cia:(int)num;

- (void) dump;
- (BOOL) developmentMode;

- (VC64Message)message;
- (void) putMessage:(VC64Message)msg;
- (void) setListener:(const void *)sender function:(void(*)(const void *, int))func;

- (void) powerUp;
- (void) ping;
- (void) halt;
- (void) step;
- (void) stepOver;
- (bool) isRunnable;
- (void) run;
- (void) suspend;
- (void) resume; 
- (bool) isHalted;
- (bool) isRunning;
- (bool) isPAL;
- (bool) isNTSC;
- (void) setPAL;
- (void) setNTSC;
- (void) setNTSC:(BOOL)b;

- (bool) isBasicRom:(NSURL *)url;
- (bool) loadBasicRom:(NSURL *)url;
- (bool) isBasicRomLoaded;
- (bool) isCharRom:(NSURL *)url;
- (bool) loadCharRom:(NSURL *)url;
- (bool) isCharRomLoaded;
- (bool) isKernalRom:(NSURL *)url;
- (bool) loadKernalRom:(NSURL *)url;
- (bool) isKernalRomLoaded;
- (bool) isVC1541Rom:(NSURL *)url;
- (bool) loadVC1541Rom:(NSURL *)url;
- (bool) isVC1541RomLoaded;
- (bool) isRom:(NSURL *)url;
- (bool) loadRom:(NSURL *)url;

- (bool) attachCartridgeAndReset:(CRTProxy *)c;
- (void) detachCartridgeAndReset;
- (bool) isCartridgeAttached;

- (bool) insertDisk:(ArchiveProxy *)a;
- (bool) flushArchive:(ArchiveProxy *)a item:(NSInteger)nr;

- (bool) insertTape:(TAPProxy *)a;

- (NSInteger) mouseModel;
- (void) setMouseModel:(NSInteger)model;
- (void) connectMouse:(NSInteger)toPort;
- (void) disconnectMouse;
- (void) setMouseXY:(NSPoint)position;
- (void) setMouseLeftButton:(BOOL)pressed;
- (void) setMouseRightButton:(BOOL)pressed;

- (bool) warp;
- (void) setWarp:(bool)b;
- (bool) alwaysWarp;
- (void) setAlwaysWarp:(bool)b;
- (bool) warpLoad;
- (void) setWarpLoad:(bool)b;
- (UInt64) cycles;
- (UInt64) frames;

// Snapshot storage
- (void) setAutoSaveSnapshots:(bool)b;

- (NSInteger) numAutoSnapshots;
- (NSData *) autoSnapshotData:(NSInteger)nr;
- (unsigned char *) autoSnapshotImageData:(NSInteger)nr;
- (NSInteger) autoSnapshotImageWidth:(NSInteger)nr;
- (NSInteger) autoSnapshotImageHeight:(NSInteger)nr;
- (time_t) autoSnapshotTimestamp:(NSInteger)nr;
- (bool) restoreAutoSnapshot:(NSInteger)nr;
- (bool) restoreLatestAutoSnapshot;

- (NSInteger) numUserSnapshots;
- (NSData *) userSnapshotData:(NSInteger)nr;
- (unsigned char *) userSnapshotImageData:(NSInteger)nr;
- (NSInteger) userSnapshotImageWidth:(NSInteger)nr;
- (NSInteger) userSnapshotImageHeight:(NSInteger)nr;
- (time_t) userSnapshotTimestamp:(NSInteger)nr;
- (bool) takeUserSnapshot;
- (bool) restoreUserSnapshot:(NSInteger)nr;
- (bool) restoreLatestUserSnapshot;
- (void) deleteUserSnapshot:(NSInteger)nr;

// Audio hardware
- (BOOL) enableAudio;
- (void) disableAudio;

@end


// --------------------------------------------------------------------------
//               C O N T A I N E R   P R O X Y   C L A S S E S
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
//                              ContainerProxy
// --------------------------------------------------------------------------

@interface ContainerProxy : NSObject {
    
    struct ContainerWrapper *wrapper;
}

- (struct ContainerWrapper *)wrapper;

- (ContainerType)type; 
- (NSInteger)sizeOnDisk;
- (void)readFromBuffer:(const void *)buffer length:(NSInteger)length;
- (NSInteger)writeToBuffer:(void *)buffer;
@end

// --------------------------------------------------------------------------
//                               SnapshotProxy
// --------------------------------------------------------------------------

@interface SnapshotProxy : ContainerProxy {
}

+ (BOOL)isSupportedSnapshot:(const void *)buffer length:(NSInteger)length;
+ (BOOL)isUnsupportedSnapshot:(const void *)buffer length:(NSInteger)length;
+ (BOOL)isSupportedSnapshotFile:(NSString *)path;
+ (BOOL)isUnsupportedSnapshotFile:(NSString *)path;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)path;
+ (instancetype)makeWithC64:(C64Proxy *)c64proxy;

- (NSInteger)imageWidth;
- (NSInteger)imageHeight;
- (unsigned char *)imageData;
@end

// --------------------------------------------------------------------------
//                                  CRTProxy
// --------------------------------------------------------------------------

@interface CRTProxy : ContainerProxy {
}

+ (BOOL)isCRTFile:(NSString *)path;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)path;

- (NSString *)cartridgeName;
- (CartridgeType)cartridgeType;
- (NSString *)cartridgeTypeName;
- (BOOL)isSupported;
- (NSInteger)exromLine;
- (NSInteger)gameLine;
- (NSInteger)chipCount;
- (NSInteger)typeOfChip:(NSInteger)nr;
- (NSInteger)loadAddrOfChip:(NSInteger)nr;
- (NSInteger)sizeOfChip:(NSInteger)nr;
@end

// --------------------------------------------------------------------------
//                                  TAPProxy
// --------------------------------------------------------------------------

@interface TAPProxy : ContainerProxy {
}

+ (BOOL)isTAPFile:(NSString *)path;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)path;

- (NSInteger)TAPversion;
@end

// --------------------------------------------------------------------------
//                                ArchiveProxy
// --------------------------------------------------------------------------

@interface ArchiveProxy : ContainerProxy {
}

+ (instancetype)make;
+ (instancetype)makeWithFile:(NSString *)path;

- (NSInteger)numberOfItems;
- (NSString *)nameOfItem:(NSInteger)item;
- (NSString *)unicodeNameOfItem:(NSInteger)item maxChars:(NSInteger)max;
- (NSInteger)sizeOfItem:(NSInteger)item;
- (NSInteger)sizeOfItemInBlocks:(NSInteger)item;
- (NSString *)typeOfItem:(NSInteger)item;

// Think about a better API for accessing tracks and sectors directly
- (NSString *)byteStream:(NSInteger)n offset:(NSInteger)offset num:(NSInteger)num;
@end

@interface T64Proxy : ArchiveProxy
{
}
+ (BOOL)isT64File:(NSString *)filename;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)filename;
+ (instancetype)makeWithAnyArchive:(ArchiveProxy *)otherArchive;
@end

@interface PRGProxy : ArchiveProxy
{
}
+ (BOOL)isPRGFile:(NSString *)filename;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)filename;
+ (instancetype)makeWithAnyArchive:(ArchiveProxy *)otherArchive;
@end

@interface P00Proxy : ArchiveProxy
{
}
+ (BOOL)isP00File:(NSString *)filename;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)filename;
+ (instancetype)makeWithAnyArchive:(ArchiveProxy *)otherArchive;
@end

@interface D64Proxy : ArchiveProxy
{
}
+ (BOOL)isD64File:(NSString *)filename;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)filename;
+ (instancetype)makeWithAnyArchive:(ArchiveProxy *)otherArchive;
+ (instancetype)makeWithVC1541:(VC1541Proxy *)vc1541;
@end

@interface G64Proxy : ArchiveProxy
{
}
+ (BOOL)isG64File:(NSString *)filename;
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype) makeWithFile:(NSString *)filename;
@end

@interface NIBProxy : ArchiveProxy
{
}
+ (BOOL) isNIBFile:(NSString *)filename;
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype) makeWithFile:(NSString *)filename;
@end

@interface FileProxy : ArchiveProxy
{
}
+ (instancetype) makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype) makeWithFile:(NSString *)filename;
@end

