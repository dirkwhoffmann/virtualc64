// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import UniformTypeIdentifiers

class MyDocumentController: NSDocumentController {
    
    override func makeDocument(withContentsOf url: URL,
                               ofType typeName: String) throws -> NSDocument {
        
        // For media files, attach the file to a new untitled document
        if typeName != UTType.vc64.identifier {

            let doc = try super.makeUntitledDocument(ofType: typeName)

            if let mydoc = doc as? MyDocument {

                mydoc.launchUrl = url
                return mydoc
            }
        }
        
        // For snapshot files, follow the standard procedure
        return try super.makeDocument(withContentsOf: url, ofType: typeName)
    }
}
