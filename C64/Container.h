/*
 * (C) 2010 Dirk W. Hoffmann. All rights reserved.
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

#ifndef _CONTAINER_INC
#define _CONTAINER_INC

#include "basic.h"

/*!
 * @class Container
 * @brief Base class for all loadable objects.
 */
class Container {
	
private:
	
    /*! @brief The physical name of the archive.
     *  @discussion The physical name indicates where the archive is stores in the file system (full path name)
     */
    char *path;
	
    /*! @brief The logical name of the archive.
     *  @discussion Most archives store a logical name in their header section. An exception is the D64 format that only contains the raw track and sector data. This archive type uses the physical name (without path and extension) as its logical name.
     */
	char *name;
		
    //! @brief Frees all memory allocated by this object.
	virtual void dealloc() = 0;
	
protected:

	/*! @brief Check file integrity
     *  @param filename The location of a file containing a binary representation of the container.
	 *  @return true, iff the specified file is a valid file of this container type. 
     */
	virtual bool fileIsValid(const char *filename) = 0;

public:
	
    /*! @brief The type of a container
     *  @constant CRT_CONTAINER Cartridge that can be plugged into the expansion port.
     *  @constant V64_CONTAINER Snapshot file. Contains a frozes C64 state.
     *  @constant T64_CONTAINER Tape archive. Can contain multiply files.
     *  @constant D64_CONTAINER Floppy Disk Image Format. Can contain multiply files.
     *  @constant PRG_CONTAINER Program archive. Contains a single file, only.
     *  @constant P00_CONTAINER Program archive. Contains a single file, only.
     *  @constant FILE_CONTAINER An arbitrary file. Interpreted as the raw data of a single file.
     */
    enum ContainerType {
        CRT_CONTAINER = 1,
        V64_CONTAINER,
        T64_CONTAINER,
        D64_CONTAINER,
        PRG_CONTAINER,
        P00_CONTAINER,
        FILE_CONTAINER
    };
    
	Container();
	virtual ~Container();
			
	//! @brief Returns the physical name of this container object.
    //! @result The physical name of the containter; "" if no name is stored.
    const char *getPath() { return path ? path : ""; }

    //! @brief Sets the physical name of this container object.
    void setPath(const char *path);

    //! @brief Returns the logical name of this container object.
    //! @result The logical name of the containter; "" if no name is stored.
    virtual const char *getName() { return name ? name : ""; }

    //! @brief Returns the type of this container object.
    virtual Container::ContainerType getType() = 0;

    //! @brief Returns the type of this container object as plain text.
    //! @result The type of the container, e.g., "T64" or "D64".
	virtual const char *getTypeAsString() = 0;
	
    /*! @brief Read in container contents from memory.
     *  @param buffer The address of a binary representation of the container in memory.
     *  @param length The size of the binary representation.
     *  @result true if the operation was successful; otherwise, false.
     */
	virtual bool readFromBuffer(const uint8_t *buffer, unsigned length) = 0;
	
    /*! @brief Read in container contents from file.
     *  @param filename The location of a file containing a binary representation of the container.
     *  @discussion This is a generic function that requires no custom implementation. It first reads in the file contents in memory and invokes @link readFromBuffer @/link afterwards.
     *  @result true if the operation was successful; otherwise, false.
     */
	bool readFromFile(const char *filename);

    /*! @brief Write container contents into memory.
     *  @discussion The function need to be implemented null pointer safe.
     *  @param buffer The address in memory where the binary representation of the container will be written. If a NULL pointer is passed in, a test run is performed. Such a test run is performed, e.g., by @link writeToFile @/link to determine the size of the binary representation before allocating the proper amount of memory.
     *  @result The size of the binary representation.
     */
    // TODO: Remove default implementation. Force each container to implement this method.
	virtual unsigned writeToBuffer(uint8_t *buffer);

    /*! @brief Write container contents to file.
     *  @param filename The location of a file that will contain a binary representation of the container.
     *  @discussion This is a generic function that requires no custom implementation. It first invokes @link writeToBuffer @/link to write a binary representation into memory and writes the data to disk afterwards.
     *  @result true if the operation was successful; otherwise, false.
     */
	bool writeToFile(const char *filename);
	
	
};

#endif
