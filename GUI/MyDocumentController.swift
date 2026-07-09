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

    override func openDocument(withContentsOf url: URL,
                               display: Bool,
                               completionHandler: @escaping (NSDocument?, Bool, Error?) -> Void)
    {
        loginfo(.lifetime, "openDocument(withContentsOf: \(url)")

        let commandLine = CommandLine.arguments

        for i in 1..<commandLine.count {
            let argument = commandLine[i]

            // Check only arguments that follow an option like "-svm"
            if commandLine[i - 1].hasPrefix("-") {

                let argumentURL = URL(fileURLWithPath: argument).standardizedFileURL
                let openedURL = url.standardizedFileURL

                if argumentURL == openedURL {
                    loginfo(.lifetime, "Skipping command-line file \(url)")
                    completionHandler(nil, false, nil)
                    return
                }
            }
        }

        super.openDocument(withContentsOf: url,
                           display: display,
                           completionHandler: completionHandler)
    }
    
    override func makeDocument(withContentsOf url: URL,
                               ofType typeName: String) throws -> NSDocument {

        var doc : NSDocument!

        loginfo(.lifetime, "makeDocument(withContentsOf: \(url), ofType: \(typeName)")

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
