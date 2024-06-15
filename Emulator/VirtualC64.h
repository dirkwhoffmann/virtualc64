// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------
/// @file

#pragma once

#include "VirtualC64Types.h"
#include "Error.h"
#include "MediaFile.h"
#include <filesystem>

namespace vc64 {

using peddle::Guard;

//
// Public API
//

class API {

public:

    class Emulator *emu = nullptr;

    API() { }
    API(Emulator *emu) : emu(emu) { }

    void suspend();
    void resume();

    bool isUserThread() const;
};

/** Memory API
 */
struct MemoryAPI : API {

    class C64Memory *mem = nullptr;

    /** @brief  Returns the component's current configuration.
     */
    const MemConfig &getConfig() const;

    /** @brief  Returns the component's current state.
     */
    const MemInfo &getInfo() const;
    const MemInfo &getCachedInfo() const;

    /** @brief  Returns a string representations for a portion of memory.
     */
    string memdump(u16 addr, isize num, bool hex, isize pads, MemoryType src) const;
    string txtdump(u16 addr, isize num, MemoryType src) const;

    /** @brief Experimental
     */
    void drawHeatmap(u32 *buffer, isize width, isize height) const;

};


/** CPU API
 */
struct CPUAPI : API {

    class CPU *cpu = nullptr;

    /** @brief  Returns the component's current state.
     */
    const CPUInfo &getInfo() const;
    const CPUInfo &getCachedInfo() const;

    /** @brief  Returns the number of instructions in the record buffer.
     *  @note   The record buffer is only filled in track mode. To save
     *          computation time, the GUI enables track mode when the CPU
     *          inspector is opened and disables track mode when it is
     *          closed.
     */
    isize loggedInstructions() const;

    /** @brief  Empties the record buffer.
     */
    void clearLog();

    /** Determines how the disassembler displays numbers.
     *  @param  instrFormat Format for numbers inside instructions
     *  @param  dataFormat  Format for printed data values
     */
    void setNumberFormat(peddle::DasmNumberFormat instrFormat, peddle::DasmNumberFormat dataFormat);

    /** Disassembles an instruction.
     *  @param  dst     Destination buffer
     *  @param  fmt     String definining the output format
     *  @param  addr    Address of the instruction to disassemble
     *  @return Length of the disassembled instruction in bytes.
     */
    isize disassemble(char *dst, const char *fmt, u16 addr) const;

    /** Disassembles an instruction from the record buffer.
     *  @param  dst     Destination buffer
     *  @param  fmt     String definining the output format
     *  @param  nr      Index of the instruction in the record buffer
     *  @return Length of the disassembled instruction in bytes.
     */
    isize disassembleRecorded(char *dst, const char *fmt, isize nr) const;

    /** @brief  Returns a breakpoint from the breakpoint list.
     *  @param  nr      Number of the breakpoint.
     *  @return A pointer to the breakpoint or nullptr if it does not exist.
     */
    peddle::Guard *breakpointNr(long nr) const;

    /** @brief  Returns the breakpoint set on a specific address.
     *  @param  addr    Memory address.
     *  @return A pointer to the breakpoint or nullptr if it is not guarded.
     */
    Guard *breakpointAt(u32 addr) const;

    /** @brief  Returns a breakpoint from the breakpoint list.
     *  @param  nr      Number of the breakpoint.
     *  @return A pointer to the breakpoint or nullptr if it does not exist.
     */
    Guard *watchpointNr(long nr) const;

    /** @brief  Returns the breakpoint set on a specific address.
     *  @param  addr    Memory address.
     *  @return A pointer to the breakpoint or nullptr if it is not guarded.
     */
    Guard *watchpointAt(u32 addr) const;

};


/** CIA API
 */
struct CIAAPI : API {

    class CIA *cia = nullptr;

    /** @brief  Returns the component's current configuration.
     */
    const CIAConfig &getConfig() const;

    /** @brief  Returns the component's current state.
     */
    const CIAInfo &getInfo() const;
    const CIAInfo &getCachedInfo() const;

    /** @brief  Returns statistical information about the components.
     */
    CIAStats getStats() const;

};


/** VICII API
 */
struct VICIIAPI : API {

    class VICII *vicii = nullptr;

    /** @brief  Provides details about the currently selected VICII revision.
     */
    const VICIITraits &getTraits() const;

    /** @brief  Returns the component's current configuration.
     */
    const VICIIConfig &getConfig() const;

    /** @brief  Returns the component's current state.
     */
    const VICIIInfo &getInfo() const;
    const VICIIInfo &getCachedInfo() const;

    /** @brief  Returns information about a sprite.
     *  @param  nr   Number of the sprite (0 .. 7)
     */
    SpriteInfo getSpriteInfo(isize nr) const;

    /** @brief  Returns the RGBA value of a color
     */
    u32 getColor(isize nr) const;

    /** @brief  Returns the RGBA value of a color from a color palette.
     */
    u32 getColor(isize nr, Palette palette) const;

};


/** SID Public API
 */
struct SIDAPI : API {

    class SIDBridge *sidBridge = nullptr;

    /** @brief  Returns the current state of a specific SID.
     *  @param  nr      SID number (0 - 3). 0 is the primary SID.
     */
    SIDInfo getInfo(isize nr) const;
    SIDInfo getCachedInfo(isize nr) const;

    /// @}
    /// @name Visualizing waveforms
    /// @{

    /** @brief  Draws a visual representation of the waveform.
     *  The Mac app uses this function to visualize the contents of the
     *  audio buffer in one of it's inspector panels. */
    float draw(u32 *buffer, isize width, isize height,
               float maxAmp, u32 color, isize sid = -1) const;
    /// @}

};


/** Audio Port Public API
 */
struct AudioPortAPI : API {

    class AudioPort *audioPort = nullptr;

    /** @brief  Returns statistical information about the components.
     */
    AudioPortStats getStats() const;


    /// @}
    /// @name Retrieving audio data
    /// @{

    /** @brief  Extracts a number of mono samples from the audio buffer
     *  Internally, the audio port maintains a ringbuffer storing stereo
     *  audio samples. When this function is used, both internal stream are
     *  added together and written to to the destination buffer.
     *  @param  buffer  Pointer to the destination buffer
     *  @param  n       Number of sound samples to copy.
     *  @return         Number of actually copied sound sound samples.
     */
    isize copyMono(float *buffer, isize n);

    /** @brief  Extracts a number of stereo samples from the audio buffer.
     *  @param  left    Pointer to the left channel's destination buffer.
     *  @param  right   Pointer to the right channel's destination buffer.
     *  @param  n       Number of sound samples to copy.
     *  @return         Number of actually copied sound sound samples.
     */
    isize copyStereo(float *left, float *right, isize n);

    /** @brief  Extracts a number of stereo samples from the audio buffer.
     *  This function has to be used if a stereo stream is managed in a
     *  single destination buffer. The samples of both channels will be
     *  interleaved, that is, a sample for the left channel will be
     *  followed by a sample of the right channel and vice versa.
     *  @param  buffer  Pointer to the destinationleft buffer.
     *  @param  n       Number of sound samples to copy.
     *  @return         Number of actually copied sound sound samples.
     */
    isize copyInterleaved(float *buffer, isize n);

    /// @}
    /// @name Visualizing waveforms
    /// @{

    /** @brief  Draws a visual representation of the waveform.
     *  The Mac app uses this function to visualize the contents of the
     *  audio buffer in one of it's inspector panels. */
    float draw(u32 *buffer, isize width, isize height,
               float maxAmp, u32 color, isize sid = -1) const;
    /// @}

};


/** Video Port Public API
 */
struct VideoPortAPI : API {

    class VideoPort *videoPort = nullptr;

    /// @}
    /// @name Retrieving video data
    /// @{

    /** @brief  Returns a pointer to the most recent stable texture
     *
     * The texture dimensions are given by constants vc64::Texture::width
     * and vc64::Texture::height texels. Each texel is represented by a
     * 32 bit color value.
     */
    u32 *getTexture() const;
    u32 *getDmaTexture() const;

};


/** DMA Debugger Public API
 */
struct DmaDebuggerAPI : API {

    class DmaDebugger *dmaDebugger = nullptr;

    /** @brief  Returns the component's current configuration
     */
    const DmaDebuggerConfig &getConfig() const;

};


/** Keyboard Public API
 */
struct KeyboardAPI : API {

    class Keyboard *keyboard = nullptr;

    /** @brief  Checks if a key is currently pressed.
     *  @param  key     The key to check.
     */
    bool isPressed(C64Key key) const;

    /** @brief  Uses the auto-typing daemon to type a string.
     *  @param  text    The text to type.
     */
    void autoType(const string &text);

    /** @brief  Aborts any active auto-typing activity.
     */
    void abortAutoTyping();

};


/** Mouse Public API
 */
struct MouseAPI : API {

    class Mouse *mouse = nullptr;

    /** Feeds a coordinate into the shake detector.
     *
     *  The shake detector keeps track of the transmitted coordinates and
     *  scans for rapid movements caused by shaking the mouse.
     *
     *  @param x    Current horizontal mouse position.
     *  @param y    Current vertical mouse position.
     *  @return     true iff a shaking mouse has been detected.
     */
    bool detectShakeXY(double x, double y);

    /** Feeds a coordinate into the shake detector.
     *
     *  The shake detector keeps track of the transmitted coordinates and
     *  scans for rapid movements caused by shaking the mouse.
     *
     *  @param dx   Current horizontal mouse position, relative to the
     *              previous position.
     *  @param dy   Current vertical mouse position, relative to the
     *              previous position.
     *  @return     true iff a shaking mouse has been detected.
     */
    bool detectShakeDxDy(double dx, double dy);
};


/** Joystick Public API
 */
struct JoystickAPI : API {

    class Joystick *joystick = nullptr;

    /** @brief  Returns the component's current state.
     */
    const JoystickInfo &getInfo() const;
    const JoystickInfo &getCachedInfo() const;
};


/** Paddle Public API
 */
struct PaddleAPI : API {

    class Paddle *paddle = nullptr;

    /** @brief  Returns the component's current state.
     */
    const PaddleInfo &getInfo() const;
    const PaddleInfo &getCachedInfo() const;
};


/** Datasette Public API
 */
struct DatasetteAPI : API {

    class Datasette *datasette = nullptr;

    /** @brief  Returns the component's current state.
     */
    const DatasetteInfo &getInfo() const;
    const DatasetteInfo &getCachedInfo() const;

    /** @brief  Inserts a tape.
     *  @param  file    The tape to insert.
     */
    void insertTape(MediaFile &file);

    /** @brief  Ejects a tape.
     *  This function has no effect if no tape is inserted.
     */
    void ejectTape();

};


/** Control Port API
 */
struct ControlPortAPI : API {

    class ControlPort *controlPort = nullptr;

    /** @brief  Custom API of the joystick connected to this port
     */
    JoystickAPI joystick;

    /** @brief  Custom API of the mouse connected to this port
     */
    MouseAPI mouse;

    /** @brief  Custom API of the paddle connected to this port
     */
    PaddleAPI paddle;
};


/** Screen Recorder Public API
 */
struct RecorderAPI : API {

    class Recorder *recorder = nullptr;

    /** @brief  Returns the component's configuration.
     */
    const RecorderConfig &getConfig() const;

    /** @brief  Returns the component's current state.
     */
    const RecorderInfo &getInfo() const;
    const RecorderInfo &getCachedInfo() const;

    /** @brief  Returns the path to the FFmpeg executable.
     */
    const fs::path getExecPath() const;

    /** @brief  Sets the path to the FFmpeg executable.
     */
    void setExecPath(const std::filesystem::path &path);

    /** @brief  Starts the recorder.
     *  @param  x1      Horizontal start coordinate of the recorded area
     *  @param  y1      Vertical start coordinate of the recorded area
     *  @param  x2      Horizontal end coordinate of the recorded area
     *  @param  y2      Vertical stop coordinate of the recorded area
     */
    void startRecording(isize x1, isize y1, isize x2, isize y2);

    /** @brief  Interrupts a recording in progress.
     */
    void stopRecording();

    /** @brief  Exports the recorded video to a file.
     *  @param  path    The export destination.
     *  @return true on success.
     */
    bool exportAs(const std::filesystem::path &path);

};


/** Expansion Port Public API
 */
struct ExpansionPortAPI : API {

    class ExpansionPort *expansionPort = nullptr;

    /// @{
    /// @name Analyzing cartridges.

    /** @brief  Returns the traits of the current cartridge.
     */
    CartridgeTraits getCartridgeTraits() const;

    /** @brief  Returns the state of the current cartridge.
     */
    const CartridgeInfo &getInfo() const;
    const CartridgeInfo &getCachedInfo() const;

    /** @brief  Returns the state of one of the cartridge ROM packets.
     *  @param  nr      Number of the ROM packet.
     */
    CartridgeRomInfo getRomInfo(isize nr) const;

    /// @}
    /// @name Attaching and detaching cartridges.
    /// @{

    /** @brief  Attaches a cartridge to the expansion port.
     */
    void attachCartridge(const std::filesystem::path &path, bool reset = true);

    /** @brief  Attaches a cartridge to the expansion port.
     */
    void attachCartridge(const MediaFile &c, bool reset = true);

    /** @brief  Attaches a cartridge to the expansion port.
     */
    // void attachCartridge(Cartridge *c);

    /** @brief  Attaches a RAM Expansion Unit to the expansion port.
     */
    void attachReu(isize capacity);

    /** @brief  Attaches a GeoRAM module to the expansion port.
     */
    void attachGeoRam(isize capacity);

    /** @brief  Attaches an Isepic module to the expansion port.
     */
    void attachIsepicCartridge();

    /** @brief  Detaches the currently plugged in cartridge
     */
    void detachCartridge();

    /// @}

};


//
// Serial port (IEC bus)
//

struct SerialPortAPI : API {

    class SerialPort *serialPort = nullptr;

};


//
// Disk
//

/** Disk Public API
 */
struct DiskAPI : API {

    class Drive *drive = nullptr;

    /** @brief  Getter for the raw disk object
     *  @return A pointer to the disk object or nullptr if no disk is present.
     */
    class Disk *get();
};


//
// Drive
//

/** Drive Public API
 */
struct DriveAPI : API {

    class Drive *drive = nullptr;

    DiskAPI disk;

    /** @brief  Returns the component's current configuration.
     */
    const DriveConfig &getConfig() const;

    /** @brief  Returns the component's current state.
     */
    const DriveInfo &getInfo() const;
    const DriveInfo &getCachedInfo() const;

    /** @brief  Inserts a new disk.
     *  @param  fstype  The file system the disk should be formatted with.
     *  @param  name    A PET string with the name of the new disk.
     */
    void insertBlankDisk(DOSType fstype, string name);

    /** @brief  Inserts a disk created from a media file.
     *  @param  file    A media file wrapper object.
     *  @param  wp      Write-protection status of the disk.
     */
    void insertMedia(MediaFile &file, bool wp);

    /** @brief  Inserts a disk created from a file system.
     *  @param  fs      A file system wrapper object.
     *  @param  wp      Write-protection status of the disk.
     */
    void insertFileSystem(const class FileSystem &fs, bool wp);

    /** @brief  Ejects the current disk.
     */
    void ejectDisk();
};


/** RetroShell Public API
 */
struct RetroShellAPI : API {

    class RetroShell *retroShell = nullptr;

    /// @name Querying the console
    /// @{
    ///
    /** @brief  Returns a pointer to the text buffer.
     *  The text buffer contains the complete contents of the console. It
     *  will be expanded when new output is generated. When the buffer
     *  grows too large, old contents is cropped.
     */
    const char *text();

    /** @brief  Returns the relative cursor position.
     *  The returned value is relative to the end of the input line. A value
     *  of 0 indicates that the cursor is at the rightmost position, that
     *  is, one character after the last character of the input line. If the
     *  cursor is at the front of the input line, the value matches the
     *  length of the input line.
     */
    isize cursorRel();

    /// @}
    /// @name Typing characters and strings
    /// @{

    /** @brief  Informs RetroShell that a key has been typed.
     *  @param  key     The pressed key
     *  @param  shift   Status of the shift key
     */
    void press(RetroShellKey key, bool shift = false);

    /** @brief  Informs RetroShell that a key has been typed.
     *  @param  c       The pressed key
     */
    void press(char c);

    /** @brief  Informs RetroShell that multiple keys have been typed.
     *  @param  s       The typed text
     */
    void press(const string &s);

    /// @}
    /// @name Controlling the output stream
    /// @{

    /** @brief  Assign an additional output stream.
     *  In addition to writing the RetroShell output into the text buffer,
     *  RetroShell will write the output into the provides stream.
     */
    void setStream(std::ostream &os);

    /// @}
    /// @name Executing scripts
    /// @{

    /** @brief  Executes a script.
     *  The script is executes asynchroneously. However, RetroShell will
     *  send messages back to the GUI thread to inform about the execution
     *  state. After the last script command has been executed,
     *  MSG\_SCRIPT\_DONE is sent. If shell execution has been aborted due
     *  to an error, MSG\_SCRIPT\_ABORT is sent.
     */
    void execScript(std::stringstream &ss);
    void execScript(const std::ifstream &fs);
    void execScript(const string &contents);
    void execScript(const MediaFile &file);

    /// @}
};

/** The user's defaults storage
 *
 *  The defaults storage manages all configuration settings that persist across
 *  multiple application launches. It provides the following functionality:
 *
 *  - **Loading and saving the storage data**
 *
 *    You can persist the user's defaults storage in a file, a stream, or a
 *    string stream.
 *
 *  - **Reading and writing key-value pairs**
 *
 *    The return value is read from the user's defaults storage for registered
 *    keys. For unknown keys, an exception is thrown.
 *
 *  - **Registerung fallback values**
 *
 *    The fallback value is used for registered keys with no custom value set.
 *
 *    @note Setting a fallback value for an unknown key is permitted. In this
 *    case, a new key is registered together with the provided default value.
 *    The GUI utilizes this feature to register additional keys, such as keys
 *    storing shader-relevant parameters that are irrelevant to the emulation
 *    core.
 */
struct DefaultsAPI : API {

    class Defaults *defaults = nullptr;

    DefaultsAPI(Defaults *defaults) : defaults(defaults) { }
    
    ///
    /// @{
    /// @name Loading and saving the key-value storage

public:

    /** @brief  Loads a storage file from disk
     *  @throw  VC64Error (#ERROR_FILE_NOT_FOUND)
     *  @throw  VC64Error (#ERROR_SYNTAX)
     */
    void load(const std::filesystem::path &path);

    /** @brief  Loads a storage file from a stream
     *  @throw  VC64Error (#ERROR_SYNTAX)
     */
    void load(std::ifstream &stream);

    /** @brief  Loads a storage file from a string stream
     *  @throw  VC64Error (#ERROR_SYNTAX)
     */
    void load(std::stringstream &stream);

    /** @brief  Saves a storage file to disk
     *  @throw  VC64Error (#ERROR_FILE_CANT_WRITE)
     */
    void save(const std::filesystem::path &path);

    /** @brief  Saves a storage file to stream
     */
    void save(std::ofstream &stream);

    /** @brief  Saves a storage file to a string stream
     */
    void save(std::stringstream &stream);


    /// @}
    /// @{
    /// @name Reading key-value pairs

public:

    /** @brief  Queries a key-value pair.
     *  @param  key     The key.
     *  @result The value as a string.
     *  @throw  VC64Error (#ERROR\_INVALID\_KEY)
     */
    string getString(const string &key) const;

    /** @brief  Queries a key-value pair.
     *  @param  key     The key.
     *  @result The value as an integer. 0 if the value cannot not be parsed.
     *  @throw  VC64Error (#ERROR\_INVALID\_KEY)
     */
    i64 getInt(const string &key) const;

    /** @brief  Queries a key-value pair.
     *  @param  option  A config option whose name is used as the key.
     *  @result The value as an integer.
     *  @throw  VC64Error (#ERROR\_INVALID\_KEY)
     */
    i64 get(Option option) const;

    /** @brief  Queries a key-value pair.
     *  @param  option  A config option whose name is used as the prefix of the key.
     *  @param  nr      The key is parameterized by adding the value as suffix.
     *  @result The value as an integer.
     *  @throw  VC64Error (#ERROR\_INVALID\_KEY)
     */
    i64 get(Option option, isize nr) const;

    /** @brief  Queries a fallback key-value pair.
     *  @param  key     The key.
     *  @result The value as a string.
     *  @throw  VC64Error (#ERROR\_INVALID\_KEY)
     */
    string getFallbackString(const string &key) const;

    /** @brief  Queries a fallback key-value pair.
     *  @param  key     The key.
     *  @result The value as an integer. 0 if the value cannot not be parsed.
     *  @throw  VC64Error (#ERROR\_INVALID\_KEY)
     */
    i64 getFallbackInt(const string &key) const;

    /** @brief  Queries a fallback key-value pair.
     *  @param  option  A config option whose name is used as the key.
     *  @result The value as an integer.
     *  @throw  VC64Error (#ERROR\_INVALID\_KEY)
     */
    i64 getFallback(Option option) const;

    /** @brief  Queries a fallback key-value pair.
     *  @param  option  A config option whose name is used as the prefix of the key.
     *  @param  nr      The key is parameterized by adding the value as suffix.
     *  @result The value as an integer.
     *  @throw  VC64Error (#ERROR\_INVALID\_KEY)
     */
    i64 getFallback(Option option, isize nr) const;


    /// @}
    /// @{
    /// @name Writing key-value pairs

    /** @brief  Writes a key-value pair into the user storage.
     *  @param  key     The key, given as a string.
     *  @param  value   The value, given as a string.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    void setString(const string &key, const string &value);

    /** @brief  Writes a key-value pair into the user storage.
     *  @param  opt     The option's name forms the keys.
     *  @param  value   The value, given as an integer.
     */
    void set(Option opt, i64 value);

    /** @brief  Writes a key-value pair into the user storage.
     *  @param  opt     The option's name forms the keys.
     *  @param  value   The value, given as an integer.
     *  @param  nr      The key is parameterized by adding the value as suffix.
     */
    void set(Option opt, i64 value, isize nr);

    /** @brief  Writes multiple key-value pairs into the user storage.
     *  @param  opt     The option's name forms the prefix of the keys.
     *  @param  value   The shared value for all pairs, given as an integer.
     *  @param  nrs     The keys are parameterized by adding the vector values as suffixes.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    void set(Option opt, i64 value, std::vector<isize> nrs);

    /** @brief  Writes a key-value pair into the fallback storage.
     *  @param  key     The key, given as a string.
     *  @param  value   The value, given as a string.
     */
    void setFallback(const string &key, const string &value);

    /** @brief  Writes a key-value pair into the fallback storage.
     *  @param  opt     The option's name forms the keys.
     *  @param  value   The value, given as a string.
     */
    void setFallback(Option opt, const string &value);

    /** @brief  Writes a key-value pair into the fallback storage.
     *  @param  opt     The option's name forms the keys.
     *  @param  value   The value, given as an integer.
     */
    void setFallback(Option opt, i64 value);

    /** @brief  Writes a key-value pair into the fallback storage.
     *  @param  opt     The option's name forms the keys.
     *  @param  value   The value, given as a string.
     *  @param  nr      The key is parameterized by adding the value as suffix.
     */
    void setFallback(Option opt, const string &value, isize nr);

    /** @brief  Writes a key-value pair into the fallback storage.
     *  @param  opt     The option's name forms the keys.
     *  @param  value   The value, given as an integer.
     *  @param  nr      The key is parameterized by adding the value as suffix.
     */
    void setFallback(Option opt, i64 value, isize nr);

    /** @brief  Writes multiple key-value pairs into the fallback storage.
     *  @param  opt     The option's name forms the prefix of the keys.
     *  @param  value   The shared value for all pairs.
     *  @param  nrs     The keys are parameterized by adding the vector values as suffixes.
     */
    void setFallback(Option opt, const string &value, std::vector <isize> nrs);

    /// @copydoc setFallback(Option, std::vector <isize>, const string &);
    void setFallback(Option opt, i64 value, std::vector <isize> nrs);


    /// @}
    /// @{
    /// @name Deleting key-value pairs

    /** @brief  Deletes all key-value pairs.
     */
    void remove();

    /** @brief  Deletes a key-value pair
     *  @param  key     The key of the key-value pair.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    void remove(const string &key) throws;

    /** @brief  Deletes a key-value pair
     *  @param  option  The option's name forms the key.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    void remove(Option option) throws;

    /** @brief  Deletes a key-value pair.
     *  @param  option  The option's name forms the prefix of the key.
     *  @param  nr      The key is parameterized by adding this value as suffix.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    void remove(Option option, isize nr) throws;

    /** @brief  Deletes multiple key-value pairs.
     *  @param  option  The option's name forms the prefix of the keys.
     *  @param  nrs     The keys are parameterized by adding the vector values as suffixes.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    void remove(Option option, std::vector <isize> nrs) throws;

    /// @}
};


/** C64 API
 */
struct C64API : public API {

    class C64 *c64 = nullptr;

    /// @name Analyzing the emulator
    /// @{

    /** @brief  Returns the component's current state.
     */
    const C64Info &getInfo() const;
    const C64Info &getCachedInfo() const;

    /** @brief  Returns the current state of an event slot.
     *
     *  @param  nr      Number of the event slot.
     */
    EventSlotInfo getSlotInfo(isize nr) const;

    /** @brief  Returns information about one of the installed Roms
     *
     *  @param  type    The ROM type
     */
    RomTraits getRomTraits(RomType type) const;

    /// @}
    /// @name Resetting the C64
    /// @{

    /** @brief  Performs a hard reset
     *
     *  A hard reset affects all components. The effect is similar to
     *  switching power off and on.
     */
    void hardReset();

    /** @brief  Performs a hard reset
     *
     *  A soft reset emulates a real reset of the C64 which can be initiated
     *  via the reset line on the expansion port.
     */
    void softReset();

    /// @}
    /// @name Setting up auto-inspection
    /// @{

    /** @brief  Returns the current inspection target.
     *
     *  If you open the inspector panel in the Mac app while the emulator
     *  is running, you will see continuous updates of the emulator state.
     *  The displayed information is recorded via the auto-inspection
     *  mechanism. If auto-inspection is active, the emulator schedules an
     *  inspect event which calls function cacheInfo() on the inspection
     *  target in constant intervals. The recorded information is later
     *  picked up by the GUI.
     *
     *  If you change to a different panel in the inspector window, the
     *  emulator will change the inspection target to only record the
     *  information you're seeing in the currently open panel.
     */
    InspectionTarget getInspectionTarget() const;

    /** @brief  Sets the current inspection target.
     */
    void setInspectionTarget(InspectionTarget target);

    /** @brief  Removes the current inspection target.
     */
    void removeInspectionTarget();


    /// @}
    /// @name Handling snapshots
    /// @{

    /** @brief  Takes a snapshot
     *
     *  @return A pointer to the created Snapshot object.
     *
     *  @note   The function transfers the ownership to the caller. It is
     *          his responsibility of the caller to free the object.
     */
    MediaFile *takeSnapshot();

    /** @brief  Loads a snapshot into the emulator.
     *
     *  @param  snapshot    Reference to a snapshot.
     */
    void loadSnapshot(const MediaFile &snapshot);


    /// @}
    /// @name Handling ROMs
    /// @{

    /** @brief  Loads a ROM from a file
     *          The ROM type is determined automatically.
     *
     *  @throw  VC64Error (ERROR_ROM_BASIC_MISSING)
     *          VC64Error (ERROR_FILE_TYPE_MISMATCH)
     */
    void loadRom(const fs::path &path);

    /** @brief  Loads a ROM, provided by a RomFile object
     */
    void loadRom(const MediaFile &file);

    /** @brief  Removes an installed ROM
     *          The ROM contents is overwritten with zeroes.
     */
    void deleteRom(RomType type);

    /** @brief  Saves a ROM to disk
     *
     *  @throw  VC64Error (ERROR_FILE_CANT_WRITE)
     */
    void saveRom(RomType rom, const std::filesystem::path &path);


    /// @}
    /// @name Handling media files
    /// @{

    /** @brief  Flashes a file into memory
     */
    void flash(const MediaFile &file);

    /** @brief  Flashes a file from a collection into memory
     */
    void flash(const MediaFile &file, isize item);

    /** @brief  Flashes a file from a file system into memory
     */
    void flash(const FileSystem &fs, isize item);
    /// @}
};


/** Public API
 *
 * This class declares the emulator's public API. It consists of functions
 * controlling the execution state, such as running or pausing the emulator, as
 * well as functions configuring the various components. The class contains
 * separate sub-APIs for the subcomponents of the emulator. For example, a
 * VICII API provides additional functions that interact directly with the
 * VICII graphics chip.
 */
class VirtualC64 : API {

    //
    // Static methods
    //

public:

    /** @brief  Returns a version string for this release.
     */
    static string version();

    /** @brief  eturns a build-number string for this release.
     */
    static string build();


    //
    // Initializing
    //

public:

    VirtualC64();
    ~VirtualC64();

    /// @name Analyzing the emulator
    /// @{

    /** @brief  Returns the component's current configuration.
     */
    const EmulatorConfig &getConfig() const;

    /** @brief  Returns the component's current state.
     */
    const EmulatorInfo &getInfo() const;
    const EmulatorInfo &getCachedInfo() const;

    /** @brief  Returns statistical information about the components.
     */
    const EmulatorStats &getStats() const;

    /// @}
    /// @name Querying the emulator state
    /// @{

    /** @brief  Returns true iff the emulator if the emulator is powered on.
     */
    bool isPoweredOn();

    /** @brief  Returns true iff the emulator if the emulator is powered off.
     */
    bool isPoweredOff();

    /** @brief  Returns true iff the emulator is in paused state.
     */
    bool isPaused();

    /** @brief  Returns true iff the emulator is running.
     */
    bool isRunning();

    /** @brief  Returns true iff the emulator has been suspended.
     */
    bool isSuspended();

    /** @brief  Returns true iff the emulator has shut down.
     */
    bool isHalted();

    /** @brief  Returns true iff warp mode is active.
     */
    bool isWarping();

    /** @brief  Returns true iff the emulator runs in track mode.
     */
    bool isTracking();

    /** @brief  Checks if the emulator is runnable.
     *  The function checks if the necessary ROMs are installed to lauch the
     *  emulator. On success, the functions returns. Otherwise, an exception
     *  is thrown.
     *
     *  @throw  Error (ERROR_ROM_BASIC_MISSING)
     *  @throw  Error (ERROR_ROM_CHAR_MISSING)
     *  @throw  Error (ERROR_ROM_KERNAL_MISSING)
     *  @throw  Error (ERROR_ROM_CHAR_MISSING)
     *  @throw  Error (ERROR_ROM_MEGA65_MISMATCH)
     */
    void isReady();


    /// @}
    /// @name Controlling the emulator state
    /// @{

    /** @brief  Switches the emulator on
     *
     *  Powering on the emulator changes the interal state to #STATE\_PAUSED,
     *  the same state that is entered when the user hits the pause button.
     *  Calling this function on an already powered-on emulator has no effect.
     *  */
    void powerOn();

    /** @brief  Switches the emulator off
     *
     *  Powering off the emulator changes the interal state of #STATE\_OFF.
     *  Calling this function on an already powered-off emulator has no effect.
     */
    void powerOff();

    /** @brief  Starts emulation
     *
     *  Running the emulator changes the internal state to #STATE\_RUNNING,
     *  which is the normal operation state. Frames are computed at normal
     *  pace, if warp mode is switched off, or as fast as possible, if warp
     *  mode is switched on. If this function is called for a powere-off
     *  emulator, an implicit call to powerOn() will be performed.
     */
    void run();

    /** @brief   Pauses emulation
     *
     * Pausing the emulator changes the interal state from #STATE\_RUNNING
     * to #STATE\_PAUSED after completing the curent frame. The emulator
     * enteres a frozes state where no more frames are computed.
     */
    void pause();

    /** @brief   Terminates the emulator thread
     *
     *  Halting the emulator changes the internal state to #STATE\_HALTED.
     *  This state is part of the shutdown procedure and never entered during
     *  normal operation.
     */
    void halt();

    /** @brief   Suspends the emulator thread
     *
     *  See the \ref vc64::Suspendable "Suspendable" class for a detailes
     *  description of the suspend-resume machanism.
     */
    void suspend();

    /** @brief   Suspends the emulator thread
     *
     *  See the \ref vc64::Suspendable "Suspendable" class for a detailes
     *  description of the suspend-resume machanism.
     */
    void resume();

    /** @brief  Enables warp mode.
     */
    void warpOn(isize source = 0);

    /** @brief  Disables warp mode.
     */
    void warpOff(isize source = 0);

    /** @brief  Enables track mode.
     */
    void trackOn(isize source = 0);

    /** @brief  Disables track mode.
     */
    void trackOff(isize source = 0);


    /// @}
    /// @name Single-stepping
    /// @{

    /** @brief  Steps a single instruction
     *
     *  If the emulator is paused, calling this functions steps the CPU for
     *  a single instruction. Otherwise, calling this function
     *  has no effect. The CPU debugger utilizes this function to implement single
     *  stepping.
     */
    void stepInto();

    /** @brief  Steps over the current instruction
     *
     *  If the emulator is paused, calling this functions runs the emulator
     *  until the instruction following the current
     *  instruction has been reached. Otherwise, calling this function
     *  has no effect. The CPU debugger utilizes this function to implement single
     *  stepping.
     *
     *  stepOver() acts like stepInto() except for branching instructions.
     *  For instance, if the current instruction is a `JSR` instruction, the
     *  subroutine will be executed in a single chunk.
     *
     *  @note If the current instruction is a branch instruction which never
     *  returns, the emulator will remain in running state. Calling this
     *  function will then be equivalent to a calling run(), with the exception
     *  of a small runtime overhead since the emulator will check after the
     *  each instruction if the program counter has reached it's target
     *  location.
     */
    void stepOver();


    /// @}
    /// @name Synchronizing the emulator thread
    /// @{

    /** @brief  Sends a wakeup signal to the emulator thread.
     *
     *  To compute frames at the proper pace, the emulator core expects the GUI
     *  to send a wakeup signal on each VSYNC pulse. Once this signal is
     *  received, the emulator thread starts computing all missing frames. To
     *  minimize jitter, the wakeup signal should be sent right after the
     *  current texture has been handed over to the GPU.
     */
    void wakeUp();


    /// @}
    /// @name Configuring the emulator
    /// @{

    /** @brief  Launches the emulator thread.
     *
     *  This function must be called in the initialization procedure of the
     *  emulator. It launches the emulator thread and registers a callback
     *  function to the message queue. The callback function is a message
     *  receiver processing all incoming messages from the emulator.
     *
     *  @param  listener    An arbitrary pointer which will be passed to the
     *  registered callback function as first argument. You can use the argument
     *  to pass the address of an object into the callback function. Inside the
     *  callback you can utilize the pointer to invoke a message processing
     *  function on that object.
     *  @param  func        The callback function.
     */
    void launch(const void *listener, Callback *func);

    /** @brief  Queries a configuration option.
     *
     *  This is the main function to query a configuration option.
     *
     *  @param option   The option to query
     *
     *  @note Some options require an additional parameter to uniquely
     *  determine the configured component. For those options, this function
     *  must not be called.
     */
    i64 get(Option option) const;

    /** @brief  Queries a configuration option.
     *
     *  This is the main function to query a configuration option.
     *
     *  @param option   The option to query
     *  @param id       The component to query
     *
     *  @note This function must only be called for those options that require
     *  an additional parameter to uniquely determine the configured component.
     */
    i64 get(Option option, long id) const;

    /** Configures the emulator to match a specific C64 model
     *
     *  @param model    The C64 model to emulate
     */
    void set(C64Model model);

    /** @brief  Configures a component.
     *
     *  This is the main function to set an configuration option.
     *
     *  @param opt      The option to set
     *  @param value    The option's value
     *
     *  @note If this function is called for an options that applies to multiple
     *  components, all components are configured with the specified value.
     */
    void set(Option opt, i64 value) throws;

    /** @brief  Configures a component.
     *
     *  This is the main function to set an configuration option.
     *
     *  @param opt      The option to set
     *  @param id       The component to configure
     *  @param value    The option's value
     *
     *  @note This function must only be called for those options that require
     *  an additional parameter to uniquely determine the configured component.
     */
    void set(Option opt, i64 value, long id);


    /** @brief  Exports the current configuration.
     *
     *  The current configuration is exported in form of a RetroShell script.
     *  Reading in the script at a later point will restore the configuration.
     */
    void exportConfig(const std::filesystem::path &path) const;
    void exportConfig(std::ostream& stream) const;


    /// @}
    /// @name Using the command queue
    /// @{

    /** @brief  Feeds a command into the command queue.
     */
    void put(const Cmd &cmd);
    void put(CmdType type, i64 payload = 0, i64 payload2 = 0) { put(Cmd(type, payload, payload2)); }
    void put(CmdType type, ConfigCmd payload)  { put(Cmd(type, payload)); }
    void put(CmdType type, KeyCmd payload)  { put(Cmd(type, payload)); }
    void put(CmdType type, CoordCmd payload)  { put(Cmd(type, payload)); }
    void put(CmdType type, GamePadCmd payload)  { put(Cmd(type, payload)); }
    void put(CmdType type, TapeCmd payload)  { put(Cmd(type, payload)); }
    void put(CmdType type, AlarmCmd payload)  { put(Cmd(type, payload)); }
    /// @}


    /** @brief  Custom APIs of subcomponents
     */
    static DefaultsAPI defaults;
    C64API c64;
    MemoryAPI mem;
    CPUAPI cpu;
    CIAAPI cia1, cia2;
    VICIIAPI vicii;
    SIDAPI sid;
    AudioPortAPI audioPort;
    VideoPortAPI videoPort;
    DmaDebuggerAPI dmaDebugger;
    KeyboardAPI keyboard;
    DatasetteAPI datasette;
    ControlPortAPI controlPort1, controlPort2;
    RecorderAPI recorder;
    ExpansionPortAPI expansionPort;
    SerialPortAPI serialPort;
    DriveAPI drive8, drive9;
    RetroShellAPI retroShell;
};

}
