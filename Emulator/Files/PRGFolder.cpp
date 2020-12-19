// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"
#include <dirent.h>

bool
PRGFolder::isPRGFolder(const char *path)
{
    DIR *dir;
    
    // We accept all directories
    if ((dir = opendir(path)) == NULL) return false;
    
    closedir(dir);
    return true;
}

PRGFolder *
PRGFolder::makeWithFolder(const char *path)
{
    assert(path != NULL);
    
    DIR *dir; struct dirent *entry; PRGFile *prg;

    // Open directory
    printf("makeWithFolder: %s\n", path); 
    if ((dir = opendir(path)) == NULL) return NULL;

    // Create new PRGFolder object
    PRGFolder *archive = new PRGFolder();

    // Use the directory name as the name for the archive
    char *dirname = extractFilename(path);
    strncpy(archive->name, dirname, 255);
    free(dirname);
    
    // Iterate through all files
    size_t pathLength = strlen(path);
    while ((entry = readdir(dir)) != NULL) {
        
        // Create the full path for this file
        char *filename = new char[pathLength + strlen(entry->d_name) + 1];
        sprintf(filename, "%s/%s", path, entry->d_name);
        
        // Get information for this file
        struct stat st;
        if(stat(filename, &st) == -1) continue;

        // Skip directories
        if ((st.st_mode & S_IFMT) == S_IFDIR) continue;

        // Skip files that are not PRGs
        if ((prg = PRGFile::makeWithFile(filename)) == NULL) continue;

        // Add the PRG
        archive->add(prg);
        delete prg;
    }

    return archive;
}

PRGFolder *
PRGFolder::makeWithAnyArchive(AnyArchive *other)
{
    if (other == NULL) return NULL;
    
    PRGFolder *archive = new PRGFolder();
    archive->plaintrace(FILE_DEBUG, "Creating %s from %s...\n",
                        archive->typeString(), other->typeString());
    
    // Iterate through all items
    for (int i = 0; i < other->numberOfItems(); i++) {
        
        // Try to create a PRG
        PRGFile *prg = PRGFile::makeWithAnyArchive(other, i);
        if (prg == NULL) continue;
        
        // Add the PRG
        archive->add(prg);
        delete prg;
    }
   
    return archive;
}

int
PRGFolder::numberOfItems()
{
    return (int)items.size();
}

void
PRGFolder::selectItem(unsigned item)
{
    assert(item < items.size());
    
    selectedItem = item;
    data = items[selectedItem].data;
    size = items[selectedItem].size;
    iEof = items[selectedItem].size;
    iFp = 0;
}

const char *
PRGFolder::getNameOfItem()
{
    assert(selectedItem != -1);

    return items[selectedItem].name;
}

size_t
PRGFolder::getSizeOfItem()
{
    assert(selectedItem != -1);

    return items[selectedItem].size;
}

void
PRGFolder::seekItem(long offset)
{
    assert(selectedItem != -1);
    assert(offset < (long)items[selectedItem].size);
    
    iFp = offset;
}

u16
PRGFolder::getDestinationAddrOfItem()
{
    assert(selectedItem != -1);
    
    return items[selectedItem].loadAddr;
}

void
PRGFolder::add(PRGFile *prg)
{
    return add(prg, items.size());
}

void
PRGFolder::add(PRGFile *prg, long at)
{
    assert(prg != NULL);
    
    Item newItem;
    
    // TODO: Add error checks here
    // Limit the number of items to 144
    // Limit the size of a single item to <MAX C64 FILE SIZE>
        
    // Store name
    strncpy(newItem.name, prg->getNameOfItem(), sizeof(newItem.name) - 1);
    newItem.name[sizeof(newItem.name) - 1] = 0;
    
    // Allocate memory
    prg->selectItem(0);
    newItem.size = prg->getSizeOfItem();
    newItem.data = new u8[newItem.size];

    // Copy data
    int i = 0, c;
    while ((c = prg->readItem()) != -1) newItem.data[i++] = c;
    assert(prg->readItem() == -1);

    // Store load address
    newItem.loadAddr = prg->getDestinationAddrOfItem();
    
    items.insert(items.begin() + at, newItem);
    dumpDirectory();
}
 
bool
PRGFolder::remove(long at)
{
    // Only proceed if the provided index is valid
    if (at < 0 || at >= (long)items.size()) return false;
    
    items.erase(items.begin() + at);
    return true;
}

bool
PRGFolder::swap(long at1, long at2)
{
    // Only proceed if the provided indices are valid
    if (at1 < 0 || at1 >= (long)items.size()) return false;
    if (at2 < 0 || at2 >= (long)items.size()) return false;
    
    if (at1 != at2) {
        iter_swap(items.begin() + at1, items.begin() + at2);
    }
    return true;
}
