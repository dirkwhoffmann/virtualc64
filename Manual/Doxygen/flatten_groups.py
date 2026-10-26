#!/usr/bin/env python3
"""Prepares the Doxygen XML for sourcey.

Sourcey's Doxygen reader only knows the standard member sections of a class
(public-func, public-attrib, ...). Members that are collected in a named group
with @name ... @{ ... @} end up in a "user-defined" section, which it skips.
This script copies the XML directory and moves the members of every
user-defined section into the matching standard section.

Usage: flatten_groups.py <input-xml-dir> <output-xml-dir>
"""
import shutil, sys
import xml.etree.ElementTree as ET
from pathlib import Path

SUFFIX = {"function": "func", "variable": "attrib", "typedef": "type",
          "enum": "type", "friend": "friend"}

def section_kind(member):
    kind = SUFFIX.get(member.get("kind"), "func")
    prot = member.get("prot", "public")
    if kind == "friend":
        return "friend"
    static = "static-" if member.get("static") == "yes" and kind in ("func", "attrib") else ""
    return f"{prot}-{static}{kind}"

def flatten(path):
    tree = ET.parse(path)
    changed = False
    for compound in tree.getroot().iter("compounddef"):
        sections = list(compound.findall("sectiondef"))
        loose = [s for s in sections if s.get("kind") == "user-defined"]
        if not loose:
            continue
        changed = True
        targets = {s.get("kind"): s for s in sections if s.get("kind") != "user-defined"}
        for section in loose:
            for member in section.findall("memberdef"):
                kind = section_kind(member)
                if kind not in targets:
                    new = ET.Element("sectiondef", kind=kind)
                    index = list(compound).index(section)
                    compound.insert(index, new)
                    targets[kind] = new
                targets[kind].append(member)
            compound.remove(section)
    if changed:
        tree.write(path, encoding="utf-8", xml_declaration=True)

def main(src, dst):
    shutil.rmtree(dst, ignore_errors=True)
    shutil.copytree(src, dst)
    for path in Path(dst).glob("*.xml"):
        if path.name.startswith(("class", "struct", "union")):
            flatten(path)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2])
