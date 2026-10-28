# Moxygen

[![CI](https://github.com/sourcey/moxygen/actions/workflows/ci.yml/badge.svg)](https://github.com/sourcey/moxygen/actions/workflows/ci.yml)
[![npm](https://img.shields.io/npm/v/moxygen)](https://www.npmjs.com/package/moxygen)
[![Node](https://img.shields.io/node/v/moxygen)](https://nodejs.org)
[![License](https://img.shields.io/npm/l/moxygen)](https://github.com/sourcey/moxygen/blob/master/LICENCE)

Doxygen XML to Markdown converter. Doxygen's parser is solid; its HTML output looks like 1998. Every tool that tried to replace the parser (Standardese, hdoc, DoxyPress) stalled. The other option is a four-tool Breathe + Exhale + Sphinx pipeline. Moxygen takes a simpler approach: parse the XML, emit clean Markdown, let your docs tool handle the rest.

Used as the C++ documentation engine in [Sourcey](https://github.com/sourcey/sourcey).

## Features

- **Multi-language** - C++ and Java supported out of the box
- **Multi-page output** - single file, per-group, per-class, or per-page
- **Internal linking** - anchors in comments and function definitions
- **Markdown comments** - Markdown in Doxygen comments is rendered
- **Doxygen groups** - [grouping](http://www.doxygen.nl/manual/grouping.html) support for organised docs
- **Grouped and ungrouped codebases** - works with explicit grouped compounds and normal namespace/class output
- **Modern C++ signatures** - class template parameters, default arguments, typedef aliases, friend declarations, `constexpr`/`consteval`, `[[nodiscard]]`, conditional `noexcept`, ref qualifiers, trailing returns, and `requires` clauses
- **Reference-depth sections** - source locations, examples, inherited members, all-member indexes, referenced-by links, and reimplementation relationships when present in the XML
- **Structured landing pages** - namespace and group pages separate nested namespaces, types, and enums
- **Custom templates** - modify the Handlebars templates to suit your needs
- **Optional index** - optionally render a top-level index

## Install

```
npm install moxygen -g
```

Requires Node.js 20+.

## Usage

1. Add `GENERATE_XML=YES` to your `Doxyfile` (see [Icey's Doxyfile](https://github.com/nilstate/icey/blob/main/Doxyfile) for a real-world example)
2. Run `doxygen` to generate the XML documentation
3. Run `moxygen` pointing to the XML output directory

```
moxygen [options] <doxygen XML directory>

Options:
  -V, --version          output the version number
  -o, --output <file>    output file, must contain "%s" when using groups or classes
  -g, --groups           output doxygen groups into separate files
  -c, --classes          output doxygen classes into separate files
  -p, --pages            output doxygen pages into separate files
  -n, --noindex          disable generation of the index
  -a, --anchors          add anchors to internal links
  -H, --html-anchors     add HTML anchors to internal links
  -l, --language <lang>  programming language (default: cpp)
  -F, --flavour <name>   markdown dialect for admonitions: pandoc or github (default: pandoc)
  -t, --templates <dir>  custom templates directory
  -s, --source-root <dir>
                         source root used to resolve file-level group membership
                         when Doxygen group XML is sparse
  -f, --frontmatter      prepend YAML frontmatter to output files
  -L, --logfile [file]   output log messages to file (default: moxygen.log)
  -q, --quiet            quiet mode
  -h, --help             display help
```

## Grouping Modes

Moxygen supports both of the common Doxygen documentation styles:

1. Explicit grouped compounds.
   - `@defgroup`, `@ingroup`, and related tags produce normal Doxygen group XML.
   - Moxygen renders those groups directly.
   - The root `api.md` page lists top-level groups before classes, and nested group pages list their child groups with breadcrumbs.
2. File-level grouped code.
   - Some codebases group files with `@addtogroup` blocks or `@file` comments with `@ingroup`, but Doxygen emits sparse group XML and leaves the real compound ownership in file XML.
   - Moxygen can recover those grouped classes, namespaces, and members when you pass `--source-root`.
   - Shared umbrella namespaces across multiple groups are preserved without dropping group-owned root classes, and cross-page refs are resolved correctly for both markdown mirrors and generated multi-page output.

Use `--source-root` when your Doxygen XML is generated outside the source tree or when grouped output depends on file-level grouping comments:

```
moxygen --groups \
  --source-root /path/to/project/src \
  --output api-%s.md \
  /path/to/doxygen/xml
```

If your XML already contains fully populated groups, `--source-root` is optional.

## Examples

Single file output:
```
moxygen --anchors /path/to/doxygen/xml
```

Multi-file grouped output:
```
moxygen --anchors --groups --output api-%s.md /path/to/doxygen/xml
```

Grouped output writes group files such as `api-mygroup.md` plus `api.md` for root-level globals and the top-level group/class index, unless `--noindex` is set.

Grouped output recovered from file-level grouping comments:
```
moxygen --anchors --groups --source-root /path/to/project/src \
  --output api-%s.md /path/to/doxygen/xml
```

Per-class files:
```
moxygen --classes --output api-%s.md /path/to/doxygen/xml
```

Per-class output writes class files plus `api.md` for root-level globals and the class/namespace index, unless `--noindex` is set.

Java project:
```
moxygen --language java --anchors /path/to/doxygen/xml
```

## Custom Templates

Moxygen uses Handlebars templates for output. The default modern templates produce clean Markdown with code-block signatures and parameter tables.

To use the classic (pre-1.0) template style:
```
moxygen --templates ./templates/classic /path/to/xml
```

To create your own templates, copy the `templates/cpp/` directory and modify. Templates receive the full parsed compound data including structured parameter info, inheritance, and more.

## Programmatic API

Two entry points: `run()` writes Markdown files to disk, `generate()` returns structured page objects for library consumers.

```typescript
import { run, generate } from 'moxygen';

// Write files to disk (CLI equivalent)
await run({
  directory: '/path/to/doxygen/xml',
  output: 'api.md',
  anchors: true,
});

await run({
  directory: '/path/to/doxygen/xml',
  output: 'api-%s.md',
  groups: true,
  sourceRoot: '/path/to/project/src',
});

// Library API - returns GeneratedPage[] (no disk I/O)
// This is what Sourcey uses internally
const pages = await generate({
  directory: '/path/to/doxygen/xml',
  language: 'cpp',
});

for (const page of pages) {
  console.log(page.slug, page.title, page.kind);
  console.log(page.markdown); // rendered markdown body
}
```

## Development

```bash
npm install
npm run build
npm test
```

To test against the example:
```bash
npm run example
```

## More Information

For more details, visit [0state.com/moxygen](https://0state.com/moxygen).

## Licence

MIT
