#!/usr/bin/env python3
"""Prepares the Doxygen XML for moxygen.

Moxygen only knows the standard member sections of a class (public-func,
public-attrib, ...). Members that are collected in a named group with
@name ... @{ ... @} end up in a "user-defined" section, which it skips. This
script copies the XML directory and

  - moves the members of every user-defined section into the matching standard
    section,
  - drops members without a doc comment in the header (constructors, wiring
    pointers, internal helpers), so that the reference only shows what the
    headers document,
  - drops base and derived class references, so that moxygen adds neither a
    "List of all members" nor "Inherited from" tables to the class pages,
  - lists the overloads of a function together, even if the header separates
    them (moxygen takes the order of the members from index.xml, so the index
    is reordered, too),
  - drops unnamed nested structs and unions from the index.

Usage: prepare_xml.py <input-xml-dir> <output-xml-dir>
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

def documented(member):
    for tag in ("briefdescription", "detaileddescription"):
        node = member.find(tag)
        if node is not None and "".join(node.itertext()).strip():
            return True
    return False

def drop_undocumented(compound):
    for section in compound.findall("sectiondef"):
        for member in section.findall("memberdef"):
            if member.get("kind") in ("function", "variable", "typedef", "friend") \
                    and not documented(member):
                section.remove(member)
        if not section.findall("memberdef"):
            compound.remove(section)

def group_by_name(parent, tag):
    """Sorts the children with the given tag by the first occurrence of their name."""
    children = parent.findall(tag)
    order = {}
    for child in children:
        order.setdefault(child.findtext("name"), len(order))
    children.sort(key=lambda c: order[c.findtext("name")])  # stable
    for child in parent.findall(tag):
        parent.remove(child)
    parent.extend(children)

def flatten(path):
    tree = ET.parse(path)
    for compound in tree.getroot().iter("compounddef"):
        for tag in ("basecompoundref", "derivedcompoundref"):
            for ref in compound.findall(tag):
                compound.remove(ref)
        drop_undocumented(compound)
        sections = list(compound.findall("sectiondef"))
        targets = {s.get("kind"): s for s in sections if s.get("kind") != "user-defined"}
        for section in [s for s in sections if s.get("kind") == "user-defined"]:
            for member in section.findall("memberdef"):
                kind = section_kind(member)
                if kind not in targets:
                    new = ET.Element("sectiondef", kind=kind)
                    compound.insert(list(compound).index(section), new)
                    targets[kind] = new
                targets[kind].append(member)
            compound.remove(section)
        for section in compound.findall("sectiondef"):
            group_by_name(section, "memberdef")
    tree.write(path, encoding="utf-8", xml_declaration=True)

def tidy_index(dst):
    path = Path(dst) / "index.xml"
    tree = ET.parse(path)
    root = tree.getroot()
    for compound in root.findall("compound"):
        if "[" in (compound.findtext("name") or ""):
            root.remove(compound)
        else:
            group_by_name(compound, "member")
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
