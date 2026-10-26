#!/usr/bin/env python3
"""Prepares the Doxygen XML for sourcey.

Sourcey's Doxygen reader only knows the standard member sections of a class
(public-func, public-attrib, ...). Members that are collected in a named group
with @name ... @{ ... @} end up in a "user-defined" section, which it skips.
This script copies the XML directory and moves the members of every
user-defined section into the matching standard section.

Members without a doc comment in the header (constructors, wiring pointers,
internal helpers) are dropped, so that the reference only shows what the
headers document.

It also tidies the compound list: unnamed nested structs and unions are
dropped, and the groups are listed in the order of GROUP_ORDER (the order in
which the sidebar shows them).

Usage: flatten_groups.py <input-xml-dir> <output-xml-dir>
"""
import shutil, sys
import xml.etree.ElementTree as ET
from pathlib import Path

# Sourcey sorts the pages by the length of the Doxygen id (group__<name>) and
# keeps the order of index.xml for equal lengths. The group names in groups.dox
# are therefore chosen so that their length does not decrease along this list.
GROUP_ORDER = ["grp_api", "grp_config", "grp_states", "grp_messages", "grp_exceptions",
               "grp_mediafiles", "grp_enumerations", "grp_commandqueue", "grp_miscellaneous"]

SUFFIX = {"function": "func", "variable": "attrib", "typedef": "type",
          "enum": "type", "friend": "friend"}

def section_kind(member):
    kind = SUFFIX.get(member.get("kind"), "func")
    prot = member.get("prot", "public")
    if kind == "friend":
        return "friend"
    static = "static-" if member.get("static") == "yes" and kind in ("func", "attrib") else ""
    return f"{prot}-{static}{kind}"

def documented(member):
    for tag in ("briefdescription", "detaileddescription"):
        node = member.find(tag)
        if node is not None and "".join(node.itertext()).strip():
            return True
    return False

def drop_undocumented(compound):
    removed = False
    for section in compound.findall("sectiondef"):
        for member in section.findall("memberdef"):
            if member.get("kind") in ("function", "variable", "typedef", "friend") \
                    and not documented(member):
                section.remove(member)
                removed = True
        if not section.findall("memberdef"):
            compound.remove(section)
    return removed

def flatten(path):
    tree = ET.parse(path)
    changed = False
    for compound in tree.getroot().iter("compounddef"):
        changed |= drop_undocumented(compound)
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

def tidy_index(dst):
    path = Path(dst) / "index.xml"
    tree = ET.parse(path)
    root = tree.getroot()
    compounds = root.findall("compound")
    for c in compounds:
        name = c.findtext("name") or ""
        if "[" in name or (c.get("kind") == "namespace" and name == "vc64"):
            root.remove(c)
    groups = [c for c in root.findall("compound") if c.get("kind") == "group"]
    for c in groups:
        root.remove(c)
    def rank(c):
        name = c.findtext("name")
        return GROUP_ORDER.index(name) if name in GROUP_ORDER else len(GROUP_ORDER)
    first = len(list(root))
    for c in sorted(groups, key=rank):
        root.insert(0, c) if False else root.append(c)
    tree.write(path, encoding="utf-8", xml_declaration=True)

def main(src, dst):
    shutil.rmtree(dst, ignore_errors=True)
    shutil.copytree(src, dst)
    for path in Path(dst).glob("*.xml"):
        if path.name.startswith(("class", "struct", "union")):
            flatten(path)
    tidy_index(dst)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2])
