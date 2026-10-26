// Sourcey configuration for the VirtualC64 documentation.
//
// Guides:        hand-written Markdown in ./guides
// API Reference: generated from the Doxygen XML that "make" writes to
//                build/xml, with the member groups flattened by
//                flatten_groups.py (see Makefile).
export default {
  name: "VirtualC64",
  // The site is published at https://dirkwhoffmann.github.io/VirtualC64/docs/
  siteUrl: "https://dirkwhoffmann.github.io",
  baseUrl: "/VirtualC64/docs",
  // "mark" tells sourcey that the image is only an icon, so the site name is shown beside it
  logo: { light: "./guides/images/icon.png", mark: true },
  theme: {
    name: "api-first",
    colors: { primary: "#393796" },
    fonts: { sans: "Inter", mono: "system" },
    layout: { content: "48rem" },
  },
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
      { tab: "API Reference", doxygen: { xml: "./build/xml-sourcey", groups: true } },
    ],
  },
};
