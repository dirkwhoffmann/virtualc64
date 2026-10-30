#!/usr/bin/env python3
"""Prepares the Markdown written by moxygen for VitePress.

  - Doxygen's anchor lines ("{#id}") are removed; VitePress derives the same
    anchors from the headings.
  - The front matter is rewritten: the title is the page heading (moxygen uses
    the Doxygen id), the kind (class, struct, ...) is kept, and the
    machine-generated description is dropped.
  - The namespace page vc64 (all types and enumerations) gets a proper title and lists
    its enumerations in "On this page".
  - Moxygen documents all enumerations on the namespace page. Each one is moved
    to a page of its own (enum-<Name>.md), and all links to it are redirected.
  - The pages are sorted into guide/ (the guides) and api/ (the overview page and
    everything else), so that VitePress can show a sidebar for each. All links between pages are made absolute.
    index.md is the landing page (home.md). The images of the guides are copied
    next to the pages.

Usage: fix_markdown.py <docs-dir> <guide-images-dir>
"""
import re, shutil, sys
from pathlib import Path

def fix(text):
    text = re.sub(r"(?m)^\{#[^}\n]*\}[ \t]*\n(?:[ \t]*\n)?", "", text)
    front = re.match(r"---\n(.*?)\n---\n", text, re.S)
    kind = re.search(r"(?m)^kind: .*$", front.group(1)) if front else None
    body = re.sub(r"\A---\n.*?\n---\n", "", text, count=1, flags=re.S)
    heading = re.search(r"(?m)^# (.+?)(?:\s*\{#[^}]*\})?\s*$", body)
    title = heading.group(1).replace('"', "'") if heading else "VirtualC64"
    # moxygen numbers the anchors of members ("#getinfo-7"); the headings that
    # VitePress generates carry no such number.
    slugs = {re.sub(r"[^\w-]", "", h.lower().replace(" ", "-"))
             for h in re.findall(r"(?m)^#{1,4} (.+?)\s*$", body)}
    def anchor(match):
        name = match.group(1)
        return f"](#{name})" if name in slugs else match.group(0)
    body = re.sub(r"\]\(#([\w]+)-\d+\)", anchor, body)
    return f'---\ntitle: "{title}"\n{kind.group(0) + chr(10) if kind else ""}---\n{body}'

ENUM_ROW = re.compile(r"^\| \[`(\w+)`\]\(#([\w-]+)\)", re.M)

def split_enums(docs):
    """Moves every enumeration of vc64.md to enum-<Name>.md."""
    page = docs / "vc64.md"
    text = page.read_text()
    start = text.index("\n## Enumerations\n")
    end = text.index("\n## ", start + 1)
    section = text[start:end]
    anchors = {anchor: name for name, anchor in ENUM_ROW.findall(section)}
    table_end = section.index("\n### ")
    table = section[:table_end]
    for block in re.split(r"\n(?=### )", section[table_end:].strip("\n")):
        name = re.match(r"### (\w+)", block).group(1)
        body = re.sub(r"^### \w+\n", "", block)
        body = re.sub(r"(\n---\s*)+$", "", body.rstrip()) + "\n"
        (docs / f"enum-{name}.md").write_text(f'---\ntitle: "{name}"\n---\n# {name}\n{body}')
    page.write_text(text[:start] + table.rstrip("\n") + "\n" + text[end:])
    return anchors

def redirect(docs, anchors):
    """Points links to an enumeration at its page."""
    for page in docs.glob("*.md"):
        # Inside the namespace page and the enumeration pages, "#name" is a link
        # to the namespace page. Elsewhere it is a link within the page.
        local = page.name == "vc64.md" or page.name.startswith("enum-")
        def link(match):
            file, anchor = match.group(1), match.group(2)
            if anchor in anchors and (file or local):
                return f"](enum-{anchors[anchor]}.md)"
            return match.group(0)
        text = page.read_text()
        new = re.sub(r"\]\((vc64\.md)?#([\w-]+)\)", link, text)
        if new != text:
            page.write_text(new)

def organize(docs):
    """Moves the pages to guide/ and api/ and repairs the links."""
    target = {}
    for page in docs.glob("*.md"):
        name = page.name
        if name.startswith("page-api_"):
            target[name] = "api/" + name[len("page-api_"):].replace("_", "-")
        elif name.startswith("page-guide_"):
            target[name] = "guide/" + name[len("page-guide_"):].replace("_", "-")
        else:
            target[name] = "api/" + name
    def link(match):
        file, anchor = match.group(1), match.group(2) or ""
        if file not in target:
            return match.group(0)
        if target[file].startswith("guide/"):
            anchor = ""  # the anchors of Doxygen pages do not exist
        return f"](/{target[file]}{anchor})"
    for name, new in target.items():
        text = re.sub(r"\]\(([\w.-]+\.md)(#[^)]*)?\)", link, (docs / name).read_text())
        destination = docs / new
        destination.parent.mkdir(exist_ok=True)
        destination.write_text(text)
        (docs / name).unlink()

def main(docs, images):
    docs = Path(docs)
    for page in docs.glob("*.md"):
        page.write_text(fix(page.read_text()))
    namespace = docs / "vc64.md"
    namespace.write_text(namespace.read_text().replace('title: "vc64"', 'title: "Types and Enumerations"\noutline: [2, 3]', 1)
                         .replace("\n# vc64\n", "\n# Types and Enumerations\n", 1))
    redirect(docs, split_enums(docs))
    (docs / "api.md").unlink(missing_ok=True)
    organize(docs)
    shutil.copy(Path(__file__).parent / "home.md", docs / "index.md")
    for image in Path(images).iterdir():
        if image.suffix in (".svg", ".png") and image.name != "icon.png":
            shutil.copy(image, docs / "guide")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(*sys.argv[1:])
