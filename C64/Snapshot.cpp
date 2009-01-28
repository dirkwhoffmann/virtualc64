/*
 * (C) 2009 Dirk W. Hoffmann. All rights reserved.
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

#include "C64.h"

bool 
Snapshot::isSnapshot(char *filename)
{
	return false;
}

Snapshot::Snapshot()
{
	major = 0;
	minor = 0;
	memset(data, 0, sizeof(data));
}

Snapshot::~Snapshot()
{
}

bool 
Snapshot::intitWithContentsOfFile(char *filename)
{
	return false;
}


bool 
Snapshot::initWithContentsOfC64(C64 *c64)
{
	uint8_t *ptr = data;
	major = 1;
	minor = 0;
	c64->load(&ptr);
	printf("initWithContentsOfC64: Loaded %d bytes\n", ptr - data);
	return true;
}


//! Save snapshot to file
bool 
Snapshot::writeToFile(char *filename)
{
	return false;
}
