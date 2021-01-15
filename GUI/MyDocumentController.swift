// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class MyDocumentController: NSDocumentController {
    
    override func makeDocument(withContentsOf url: URL,
                               ofType typeName: String) throws -> NSDocument {

        track("typeName: \(typeName)")
        
        // For media files, attach the file to a new untitled document
        if typeName.uppercased() != "V64" {

            let doc = try super.makeUntitledDocument(ofType: typeName)
            if let mydoc = doc as? MyDocument {
                try? mydoc.createAttachment(from: url)
                return mydoc
            }
        }
        
        // For snapshot files, follow the standard procedure
        return try super.makeDocument(withContentsOf: url, ofType: typeName)
    }
}
