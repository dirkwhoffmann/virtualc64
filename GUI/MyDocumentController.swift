// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// import UniformTypeIdentifiers

class MyDocumentController: NSDocumentController {

    override func makeDocument(withContentsOf url: URL,
                               ofType typeName: String) throws -> NSDocument {

        var doc : NSDocument!

        debug(.lifetime, "makeDocument(withContentsOf: \(url), ofType: \(typeName)")

        if typeName.components(separatedBy: ".").last?.lowercased() != "vc64" {

            // For media files, attach the file to a new untitled document
            doc = try super.makeUntitledDocument(ofType: typeName)

        } else {

            // For workspaces, follow the standard procedure
            doc = try super.makeDocument(withContentsOf: url, ofType: typeName)
        }

        (doc as? MyDocument)?.launchURL = url
        return doc
    }
}
