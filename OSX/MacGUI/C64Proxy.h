/*
 * Author: Dirk W. Hoffmann. All rights reserved.
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
#import "VIC_globals.h"
#import "basic.h"

// Forward declarations
@class MyController;
@class C64Proxy;
@class SnapshotProxy;
@class D64ArchiveProxy; 
@class ArchiveProxy;
@class TAPProxy;
@class CRTProxy;

// Forward declarations of wrappers for C++ classes.
// We wrap classes into normal C structs to avoid any reference to C++ here.

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

// --------------------------------------------------------------------------
//                                    CPU
// --------------------------------------------------------------------------

@interface CPUProxy : NSObject {
        
	struct CpuWrapper *wrapper;
}

- (void) dump;
- (bool) tracingEnabled;
- (void) setTraceMode:(bool)b;
- (uint16_t) PC;
- (void) setPC:(uint16_t)pc;
- (uint8_t) SP;
- (void) setSP:(uint8_t)sp;
- (uint8_t) A;
- (void) setA:(uint8_t)a;
- (uint8_t) X;
- (void) setX:(uint8_t)x;
- (uint8_t) Y;
- (void) setY:(uint8_t)y;
- (bool) Nflag;
- (void) setNflag:(bool)b;
- (bool) Zflag;
- (void) setZflag:(bool)b;
- (bool) Cflag;
- (void) setCflag:(bool)b;
- (bool) Iflag;
- (void) setIflag:(bool)b;
- (bool) Bflag;
- (void) setBflag:(bool)b;
- (bool) Dflag;
- (void) setDflag:(bool)b;
- (bool) Vflag;
- (void) setVflag:(bool)b;

- (uint16_t) readPC;
- (uint16_t) addressOfNextInstruction;
- (DisassembledInstruction) disassemble:(uint16_t)addr hex:(BOOL)h;
// - (int) topOfCallStack;
// - (int) breakpoint:(uint16_t)addr;
// - (void) setBreakpoint:(uint16_t)addr tag:(uint8_t)t;
- (BOOL) hardBreakpoint:(uint16_t)addr;
- (void) setHardBreakpoint:(uint16_t)addr;
- (void) deleteHardBreakpoint:(uint16_t)addr;
- (void) toggleHardBreakpoint:(uint16_t)addr;
- (BOOL) softBreakpoint:(uint16_t)addr;
- (void) setSoftBreakpoint:(uint16_t)addr;
- (void) deleteSoftBreakpoint:(uint16_t)addr;
- (void) toggleSoftBreakpoint:(uint16_t)addr;

@end

// --------------------------------------------------------------------------
//                                  Memory
// --------------------------------------------------------------------------

@interface MemoryProxy : NSObject {
    
	struct MemoryWrapper *wrapper;
}

- (void) dump;

- (uint8_t) read:(uint16_t)addr;
- (uint16_t) readWord:(uint16_t)addr;
- (uint8_t) readFrom:(uint16_t)addr memtype:(MemoryType)source;
- (void) poke:(uint16_t)addr value:(uint8_t)val;
- (void) pokeTo:(uint16_t)addr value:(uint8_t)val memtype:(MemoryType)source;
- (bool) isValidAddr:(uint16_t)addr memtype:(MemoryType)source;

@end

// --------------------------------------------------------------------------
//                                    VIC
// --------------------------------------------------------------------------

@interface VICProxy : NSObject {
    
	struct VicWrapper *wrapper;
}

- (void) dump;

- (void *) screenBuffer;

- (NSColor *) color:(NSInteger)nr;
- (NSInteger) colorScheme;
- (void) setColorScheme:(NSInteger)scheme;

- (uint16_t) memoryBankAddr;
- (void) setMemoryBankAddr:(uint16_t)addr;
- (uint16_t) screenMemoryAddr;
- (void) setScreenMemoryAddr:(uint16_t)addr;
- (uint16_t) characterMemoryAddr;
- (void) setCharacterMemoryAddr:(uint16_t)addr;

- (int) displayMode;
- (void) setDisplayMode:(long)mode;
- (int) screenGeometry;
- (void) setScreenGeometry:(long)mode;
- (int) horizontalRasterScroll;
- (void) setHorizontalRasterScroll:(int)offset;
- (int) verticalRasterScroll;
- (void) setVerticalRasterScroll:(int)offset;

- (bool) spriteVisibilityFlag:(NSInteger)nr;
- (void) setSpriteVisibilityFlag:(NSInteger)nr value:(bool)flag;
- (void) toggleSpriteVisibilityFlag:(NSInteger)nr;

- (int) spriteX:(NSInteger)nr;
- (void) setSpriteX:(NSInteger)nr value:(int)x;
- (int) spriteY:(NSInteger)nr;
- (void) setSpriteY:(NSInteger)nr value:(int)y;

- (int) spriteColor:(NSInteger)nr;
- (void) setSpriteColor:(NSInteger)nr value:(int)c;
- (bool) spriteMulticolorFlag:(NSInteger)nr;
- (void) setSpriteMulticolorFlag:(NSInteger)nr value:(bool)flag;
- (void) toggleSpriteMulticolorFlag:(NSInteger)nr;

- (bool) spriteStretchXFlag:(NSInteger)nr;
- (void) setSpriteStretchXFlag:(NSInteger)nr value:(bool)flag;
- (void) toggleSpriteStretchXFlag:(NSInteger)nr;
- (bool) spriteStretchYFlag:(NSInteger)nr;
- (void) setSpriteStretchYFlag:(NSInteger)nr value:(bool)flag;
- (void) toggleSpriteStretchYFlag:(NSInteger)nr;

- (bool) spriteSpriteCollisionFlag;
- (void) setSpriteSpriteCollisionFlag:(bool)flag;
- (void) toggleSpriteSpriteCollisionFlag;

- (bool) spriteBackgroundCollisionFlag;
- (void) setSpriteBackgroundCollisionFlag:(bool)flag;
- (void) toggleSpriteBackgroundCollisionFlag;

- (uint16_t) rasterline;
- (void) setRasterline:(uint16_t)line;
- (uint16_t) rasterInterruptLine;
- (void) setRasterInterruptLine:(uint16_t)line;
- (bool) rasterInterruptFlag;
- (void) setRasterInterruptFlag:(bool)b;
- (void) toggleRasterInterruptFlag;

- (bool) hideSprites;
- (void) setHideSprites:(bool)b;
- (bool) showIrqLines;
- (void) setShowIrqLines:(bool)b;
- (bool) showDmaLines;
- (void) setShowDmaLines:(bool)b;

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

/*
- (BOOL) shiftKeyIsPressed;
- (BOOL) commodoreKeyIsPressed;
- (BOOL) ctrlKeyIsPressed;
- (BOOL) runstopKeyIsPressed;

- (void) toggleShiftKey;
- (void) toggleCommodoreKey;
- (void) toggleCtrlKey;
- (void) toggleRunstopKey;
*/

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
- (uint32_t) sampleRate;
- (void) setSampleRate:(uint32_t)rate;
- (void) readMonoSamples:(float *)target size:(NSInteger)n;
- (void) readStereoSamples:(float *)target1 buffer2:(float *)target2 size:(NSInteger)n;
- (void) readStereoSamplesInterleaved:(float *)target size:(NSInteger)n;

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
	MemoryProxy *mem;
	VIAProxy *via1;
	VIAProxy *via2;
    Disk525Proxy *disk;
}

@property (readonly) struct Vc1541Wrapper *wrapper;
@property (readonly) CPUProxy *cpu;
@property (readonly) MemoryProxy *mem;
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

