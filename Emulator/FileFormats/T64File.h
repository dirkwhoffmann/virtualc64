// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _T64FILE_INC
#define _T64FILE_INC

#include "AnyArchive.h"

/*! @class   T64File
 *  @brief   The T64File class declares the programmatic interface for a file
 *           in T64 format.
 */
class T64File : public AnyArchive {
    
    //! @brief    Header signature
    static const u8 magicBytes[];
    
    /*! @brief    Number of the currently selected item
     *  @details  -1, if no item is selected
     */
    long selectedItem = -1;
    
public:
    
    //
    //! @functiongroup Class methods
    //
    
    //! @brief    Returns true iff buffer contains a T64 file
    static bool isT64Buffer(const u8 *buffer, size_t length);
    
    //! Returns true of filename points to a valid file of that type
    static bool isT64File(const char *filename);
    
    
    //
    //! @functiongroup Creating and destructing
    //
    
    //! @brief    Standard constructor
    T64File();
    
    //! @brief    Factory method
    static T64File *makeWithBuffer(const u8 *buffer, size_t length);
    
    //! @brief    Factory method
    static T64File *makeWithFile(const char *path);
    
    /*! @brief    Factory method
     *  @details  otherArchive can be of any archive type
     */
    static T64File *makeT64ArchiveWithAnyArchive(AnyArchive *otherArchive);

    
    //
    //! @functiongroup Methods from AnyC64File
    //
    
    C64FileType type() { return T64_FILE; }
    const char *typeAsString() { return "T64"; }
    const char *getName();
    bool hasSameType(const char *filename) { return isT64File(filename); }
    bool readFromBuffer(const u8 *buffer, size_t length);
    
    
    //
    //! @functiongroup Methods from AnyArchive
    //
    
    int numberOfItems();
    void selectItem(unsigned n);
    const char *getTypeOfItemAsString();
    const char *getNameOfItem();
    size_t getSizeOfItem();
    void seekItem(long offset);
    u16 getDestinationAddrOfItem();

   
    //
    // @functiongroup Scanning and repairing a T64 file
    //
    
    //! @brief Checks if the header contains information at the specified location
    bool directoryItemIsPresent(int n);

    /*! @brief    Checks the file for inconsistencies and tries to repair it
     *  @details  This method can eliminate the following inconsistencies:
     *            number of files:
     *                some archives state falsely in their header that zero
     *                files are present. This value will be fixed.
     *            end loading address:
     *                Archives that are created with CONVC64 often contain a
     *                value of 0xC3C6, which is wrong (e.g., paradrd.t64). This
     *                value will be changed such that getByte() will read until
     *                the end of the physical file.
     * @result    true,  if archive was consistent or could be repaired.
     *            false, if an inconsistency has been detected that could not
     *                   be repaired.
     */
    bool repair();
};

#endif

