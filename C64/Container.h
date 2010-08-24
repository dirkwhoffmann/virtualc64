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
	
	//! Discard previously loaded contents if present
	virtual void cleanup() = 0;

	//! Check file type
	/*! Returns true, iff the specifies file is a valid archive file. */
	virtual bool fileIsValid(const char *filename) = 0;

	//! Load container data from file
	virtual bool loadFromFile(FILE *file, struct stat fileProperties) = 0;

public:
	
	//! Constructor
	Container();
	
	//! Destructor
	virtual ~Container();
		
	//! Return physical name
	 const char *getPath();

	//! Return logical name (can be overwritten by sub classes)
	virtual const char *getName();
	
	//! Type of container in plain text (T64, D64, PRG, ...)
	virtual const char *getTypeOfContainer() = 0;
	
	//! Get container data from file
	bool loadFile(const char *filename);
	
};

#endif
