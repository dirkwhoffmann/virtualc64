#!/usr/bin/env python3
"""Prepares the Markdown written by moxygen for VitePress.

  - Doxygen's anchor lines ("{#id}") are removed; VitePress derives the same
    anchors from the headings.
  - The front matter is rewritten: the title is the page heading (moxygen uses
    the Doxygen id), and the machine-generated description is dropped.
  - The namespace page vc64 (all types and enumerations) gets a proper title and lists
    its enumerations in "On this page".
  - The mainpage becomes index.md, and the images of the guides are copied
    next to the pages.

Usage: fix_markdown.py <docs-dir> <guide-images-dir>
"""
import re, shutil, sys
from pathlib import Path

def fix(text):
    text = re.sub(r"(?m)^\{#[^}\n]*\}[ \t]*\n(?:[ \t]*\n)?", "", text)
    body = re.sub(r"\A---\n.*?\n---\n", "", text, count=1, flags=re.S)
    heading = re.search(r"(?m)^# (.+?)(?:\s*\{#[^}]*\})?\s*$", body)
    title = heading.group(1).replace('"', "'") if heading else "VirtualC64"
    return f'---\ntitle: "{title}"\n---\n{body}'

def main(docs, images):
    docs = Path(docs)
    for page in docs.glob("*.md"):
        page.write_text(fix(page.read_text()))
    namespace = docs / "vc64.md"
    namespace.write_text(namespace.read_text().replace('title: "vc64"', 'title: "Types and Enumerations"\noutline: [2, 3]', 1)
                         .replace("\n# vc64\n", "\n# Types and Enumerations\n", 1))
    (docs / "page-index.md").replace(docs / "index.md")
    (docs / "api.md").unlink(missing_ok=True)
    for image in Path(images).iterdir():
        if image.suffix in (".svg", ".png") and image.name != "icon.png":
            shutil.copy(image, docs)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(*sys.argv[1:])
