/*
 * Written 2015 by Dirk W. Hoffmann
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

#ifndef _DISK525_INC
#define _DISK525_INC

#include "VirtualComponent.h"
#include "D64Archive.h"

class C64;

class Disk525 : VirtualComponent {
    
public:
    
    //! Constructor
    Disk525();

    //! Destructor
    ~Disk525();

    //! Reset
    void reset(C64 *c64);

    //! Size of internal state
    uint32_t stateSize();
    
    //! Dump current configuration into message queue
    // void ping() { };

    //! Load state
    void loadFromBuffer(uint8_t **buffer);
    
    //! Save state
    void saveToBuffer(uint8_t **buffer);
    
    //! Dump current state into logfile
    // void dumpState();
 
    
    // -----------------------------------------------------------------------------------------------
    //                                     Types and constants
    // -----------------------------------------------------------------------------------------------
    
    // The VC1541 drive head can move between position 1 and 85.
    // The odd numbers between 1 and 70 mark the 35 tracks that are used by VC1541 DOS. This means
    // that DOS moves the drive head always two positions up or down. If programmed manually, the
    // can also be position on half tracks and on tracks beyond 35. This leads to the following layout:
    //
    //                      -----------------------------------------------------------------------------
    // Track layout:        |  1  | 1.5 |  2  | 2.5 |  3  | 3.5 | ... |  35  | 35.5 | ... |  42  | 42.5 |
    //                      -----------------------------------------------------------------------------
    // Halftrack addressing:   1  |  2  |  3  |  4  |  5  |  6  |     |  69  |  70  |     |  83  |  84
    // Track addressing:       1  |     |  2  |     |  3  |     |     |  35  |      |     |  42  |
    
    //! Datatype for methods using halftrack addressing
    typedef unsigned Halftrack;
    bool isHalftrackNumber(unsigned nr) { return 1 <= nr && nr <= 84; }

    //! Datatype for methods using track addressing
    typedef unsigned Track;
    bool isTrackNumber(unsigned nr) { return 1 <= nr && nr <= 42; }

    //! Maximum number of files that can be stored on a single disk
    /*! VC1541 DOS allows up to 18 directory sectors, each containing 8 files. */
    const static unsigned MAX_FILES_ON_DISK = 144;
    
private:
    
    //! GCR encoding table
    /*! On disk, data is stored GCR encoded. Four data bytes expand to five GCR bytes. */
    
    const uint16_t gcr[16] = {
        0x0a, 0x0b, 0x12, 0x13,
        0x0e, 0x0f, 0x16, 0x17,
        0x09, 0x19, 0x1a, 0x1b,
        0x0d, 0x1d, 0x1e, 0x15
    };

public:
    
    //! Disk data
    /*! Each tracks can store a maximum of 7928 bytes. The number varies depends on the track number
        (inner tracks contain fewer bytes) and the actual write speed of a drive.
        The first valid track and halftrack number is 1. Hence, the entries [0][x] are unused. 
        data.halftack[i] : pointer to the first byte of halftrack i
        data.track[i]    : pointer to the first byte of track i */
    uint8_t olddata[85][7928];
    
    union {
        struct {
            uint8_t _pad[7928];
            uint8_t halftrack[85][7928];
        };
        uint8_t track[43][2 * 7928];
    } data;

// private:
    //! Length of each halftrack in bytes
    /*! length.halftack[i] : length of halftrack i
        length.track[i][0] : length of track i
        length.track[i][1] : length of halftrack above track i */
     
    uint16_t oldlength[85];

    union {
        struct {
            uint16_t _pad;
            uint16_t halftrack[85];
        };
        uint16_t track[43][2];
    } length;

    
public:
    
    //! Returns true if track/offset indicates a valid disk position on disk
    bool isValidDiskPositon(Halftrack ht, uint16_t offset) { return isHalftrackNumber(ht) && offset < oldlength[ht]; }
    
    
    //! Total number of tracks on this disk
    // DEPRECATED (ADD METHOD emptyTrack(Track nr);) (Scans track for data, returns true/false numTracks())
    uint8_t numTracks;
    
   

  
    
    
    //private:
public:
    
    //! Converts a track number into a halftrack number
    /*! The mapping is: 1->1, 2->3, 3->5, 5->7, ..., 41->81, 42->83 */
    // DEPRECATED
    static unsigned trackToHalftrack(unsigned track) {
        assert(track >= 1 && track <= 42); return (2 * track) - 1; }
    
    //! Converts a halftrack number into a track number
    /*! The mapping is: 1->1, 2->1, 3->2, 4->2, ..., 83->42, 84->42 */
    // DEPRECATED
    static unsigned halftrackToTrack(unsigned halftrack) {
        assert(halftrack >= 1 && halftrack <= 84); return (halftrack + 1) / 2; }
    
    //! Return start address of a given halftrack (1...84)
    // DEPRECATED
    inline uint8_t *startOfHalftrack(unsigned halftrack) {
        assert(halftrack >= 1 && halftrack <= 84); return olddata[halftrack - 1]; }
    
    //! Return start address of a given track (1...42)
    // DEPRECATED
    inline uint8_t *startOfTrack(unsigned track) {
        assert(track >= 1 && track <= 42); return startOfHalftrack(2 * track - 1); }
    
    //! Returns the length of a halftrack
    // DEPRECATED
    inline uint16_t lengthOfHalftrack(unsigned halftrack) {
        assert(halftrack >= 1 && halftrack <= 84); return oldlength[halftrack - 1];
    }
    
    //! Returns the length of a track
    // DEPRECATED
    inline uint16_t lengthOfTrack(unsigned track) {
        assert(track >= 1 && track <= 42); return lengthOfHalftrack(trackToHalftrack(track));
    }
    
    //! Returns the length of a halftrack
    // DEPRECATED
    inline void setLengthOfHalftrack(unsigned halftrack, unsigned len) {
        assert(halftrack >= 1 && halftrack <= 84); oldlength[halftrack - 1] = len;
    }
    
    //! Returns the length of a track
    // DEPRECATED
    inline void setLengthOfTrack(unsigned track, unsigned len) {
        assert(track >= 1 && track <= 42); setLengthOfHalftrack(trackToHalftrack(track), len);
    }
    
    //! Zero out a single halftrack
    /*! If the additional track size parameter is provided, the track size is also adjusted.
     Othwerwise, the current track size is kept. */
    void clearHalftrack(Halftrack ht);
    
    //! Zero out all tracks on a disk
    void clearDisk();
    
    //! For debugging
    void dumpTrack(Halftrack ht, unsigned min = 0, unsigned max = UINT_MAX, unsigned highlight = UINT_MAX);


    // ---------------------------------------------------------------------------------------------
    //                               Data encoding and decoding
    // ---------------------------------------------------------------------------------------------
    
public:
    
    //! Converts a D64 archive to real disk data
    /*! The methods creates sync marks, GRC encoded header and data blocks, checksums and gaps */
    void encodeDisk(D64Archive *a);
    
    //! Converts real disk data into a byte stream compatible with the D64 format
    /*! Returns the number of bytes written.
     If dest is NULL, a test run is performed (used to determine how many bytes will be written).
     If something went wrong, an error code is written to 'error' (0 = no error = success) */
    unsigned decodeDisk(uint8_t *dest, int *error = NULL);
    
    
private:
    
    //! Encode a single track
    /*! This function translates the logical byte sequence of a single track into the native VC1541
     byte representation. The native representation includes sync marks, GCR data etc.
     tailGapEven is the number of tail bytes follwowing sectors with even sector numbers.
     tailGapOdd is the number of tail bytes follwowing sectors with odd sector numbers.
     Returns the number of bytes written */
    unsigned encodeTrack(D64Archive *a, uint8_t track, int *sector, uint8_t tailGapEven, uint8_t tailGapOdd);
    
    //! Encode a single sector
    /*! This function translates the logical byte sequence of a single sector into the native VC1541
     byte representation. The sector is closed by 'gap' tail gap bytes.
     Returns the number of bytes written. */
    unsigned encodeSector(D64Archive *a, uint8_t track, uint8_t sector, uint8_t *dest, int gap);
    
    //! Write five SYNC bytes
    void encodeSync(uint8_t *dest) { for (unsigned i = 0; i < 5; i++) dest[i] = 0xFF; }
    
    //! Write interblock gap
    void encodeGap(uint8_t *dest, unsigned size) { for (unsigned i = 0; i < size; i++) dest[i] = 0x55; }
    
    //! Translate 4 data bytes into 5 GCR encodes bytes
    void encodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t *dest);
    
    //! Decodes all sectors of a single GCR encoded track
    unsigned decodeTrack(uint8_t *source, uint8_t *dest, int *error = NULL);
    
    //! Decodes a single GCR encoded sector and write out its 256 data bytes
    void decodeSector(uint8_t *source, uint8_t *dest);
    
    //! Decodes 5 GCR bytes into its 4 corrrsponding data bytes
    void decodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t *dest);
    
};
    
    

#endif
