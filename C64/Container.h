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

//! Loadable object
class Container {
	
private:
	
	//! Physical name of archive
	char *path;
	
	//! Logical name of archive
	char *name;
		
	//! Free allocated memory
	virtual void dealloc() = 0;
	
protected:

	//! Check file type
	/*! Returns true, iff the specified file is a valid file of this container type. */
	virtual bool fileIsValid(const char *filename) = 0;

public:
	
    enum ContainerType {
        CRT_CONTAINER = 1, // Cartridge
        V64_CONTAINER,     // Snapshot
        T64_CONTAINER,
        D64_CONTAINER,
        PRG_CONTAINER,
        P00_CONTAINER,
        FILE_CONTAINER
    };
    
	//! Constructor
	Container();
	
	//! Destructor
	virtual ~Container();
			
	//! Get physical name
    const char *getPath() { return path ? path : ""; }

    //! Set physical name
    void setPath(const char *path);

	//! Get logical name (can be overwritten by sub classes)
    virtual const char *getName() { return name ? name : ""; }

    //! Type of container
    virtual Container::ContainerType getType() = 0;

    //! Type of container in plain text ("T64", "D64", "PRG", ...)
	virtual const char *getTypeAsString() = 0;
	
	//! Read container data from memory buffer
	virtual bool readFromBuffer(const uint8_t *buffer, unsigned length) = 0;
	
	//! Read container data from file
	bool readFromFile(const char *filename);

	//! Write container data to memory buffer
    /*! Returns the number of bytes written.
     *  If buffer is NULL, a test run is performed. Nothing is written and the return value can be used
     *  to determine the prospected container size. 
     *  The function returns 0 to indicate an error. 
     */
    // TODO: REMOVE DEFAULT IMPLEMENTATION, FORCE EACH CONTAINER TO IMPLEMENT THIS FUNCTION
	virtual unsigned writeToBuffer(uint8_t *buffer);

	//! Write container data to file
	bool writeToFile(const char *filename);
	
	
};

#endif
