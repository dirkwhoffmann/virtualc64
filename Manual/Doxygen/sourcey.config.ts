// Sourcey configuration for the VirtualC64 documentation.
//
// Guides:        hand-written Markdown in ./guides
// API Reference: generated from the Doxygen XML that "make" writes to
//                build/xml, with the member groups flattened by
//                flatten_groups.py (see Makefile).
export default {
  name: "VirtualC64",
  logo: "./guides/images/icon.png",
  navigation: {
    tabs: [
      {
        tab: "Guides",
        slug: "guides",
        groups: [
          { group: "Getting started", pages: ["guides/index", "guides/api-overview"] },
          { group: "Concepts", pages: ["guides/threading", "guides/state", "guides/communication"] },
          { group: "Examples", pages: ["guides/headless"] },
        ],
      },
      { tab: "API Reference", doxygen: { xml: "./build/xml-sourcey", groups: false } },
    ],
  },
};
