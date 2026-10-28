#!/usr/bin/env python3
"""Prepares the Doxygen XML for sourcey.

Sourcey's Doxygen reader only knows the standard member sections of a class
(public-func, public-attrib, ...). Members that are collected in a named group
with @name ... @{ ... @} end up in a "user-defined" section, which it skips.
This script copies the XML directory and moves the members of every
user-defined section into the matching standard section.

Notes and warnings (@note, @warning) are turned into titled paragraphs.
Moxygen would render them as ":::warning" containers, which sourcey only
understands in hand-written Markdown, so they would show up as literal text.

Members without a doc comment in the header (constructors, wiring pointers,
internal helpers) are dropped, so that the reference only shows what the
headers document.

Overloads of a function are listed together, even if the header
separates them.

Base class and derived class references are dropped. Sourcey would otherwise
add a "List of all members" table and "Inherited from" tables to every class,
which repeats the members of the page (and suspend()/resume() of the base class
API on every API page).

It also tidies the compound list: unnamed nested structs and unions are
dropped, and the groups are listed in the order of GROUP_ORDER (the order in
which the sidebar shows them).

Usage: flatten_groups.py <input-xml-dir> <output-xml-dir>
"""
import re, shutil, sys
import xml.etree.ElementTree as ET
from pathlib import Path

# The groups in the order of the sidebar (see groups.dox). Sourcey sorts the
# pages by the length of the Doxygen id, so lengthen_group_ids() pads the ids
# of the groups until their length grows along this list.
GROUP_ORDER = ["grp_emulator", "grp_cpu", "grp_memory", "grp_vicii", "grp_sid", "grp_cia", "grp_audiovideo", "grp_controlports", "grp_expansionport", "grp_userserial", "grp_datasette", "grp_drives", "grp_keyboard", "grp_retroshell", "grp_remoteservers", "grp_messages", "grp_commands", "grp_errors", "grp_media"]

SUFFIX = {"function": "func", "variable": "attrib", "typedef": "type",
          "enum": "type", "friend": "friend"}

def section_kind(member):
    kind = SUFFIX.get(member.get("kind"), "func")
    prot = member.get("prot", "public")
    if kind == "friend":
        return "friend"
    static = "static-" if member.get("static") == "yes" and kind in ("func", "attrib") else ""
    return f"{prot}-{static}{kind}"

CALLOUTS = {"note": "Note", "warning": "Warning", "attention": "Attention",
            "remark": "Remark", "deprecated": "Deprecated"}

def retitle_callouts(path):
    """Turns callout sections into titled paragraphs ("#### Warning")."""
    tree = ET.parse(path)
    changed = False
    for section in tree.getroot().iter("simplesect"):
        title = CALLOUTS.get(section.get("kind"))
        if title:
            heading = ET.Element("title")
            heading.text = title
            section.insert(0, heading)
            section.set("kind", "par")
            changed = True
    if changed:
        tree.write(path, encoding="utf-8", xml_declaration=True)

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

def group_overloads(compound):
    """Puts the overloads of a function next to each other, at the place of the first one."""
    for section in compound.findall("sectiondef"):
        members = section.findall("memberdef")
        order = {}
        for member in members:
            order.setdefault(member.findtext("name"), len(order))
        members.sort(key=lambda m: order[m.findtext("name")])  # stable
        for member in section.findall("memberdef"):
            section.remove(member)
        section.extend(members)

def flatten(path):
    tree = ET.parse(path)
    changed = False
    for compound in tree.getroot().iter("compounddef"):
        # Without base classes, sourcey adds neither "List of all members" nor
        # the "Inherited from" tables to the class pages
        for tag in ("basecompoundref", "derivedcompoundref"):
            for ref in compound.findall(tag):
                compound.remove(ref)
                changed = True
        changed |= drop_undocumented(compound)
        group_overloads(compound)
        changed = True
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
        group_overloads(compound)
    if changed:
        tree.write(path, encoding="utf-8", xml_declaration=True)

def tidy_index(dst):
    path = Path(dst) / "index.xml"
    tree = ET.parse(path)
    root = tree.getroot()
    compounds = root.findall("compound")
    # Moxygen takes the order of the members from the index, so the overloads
    # are grouped there, too
    for c in compounds:
        members = c.findall("member")
        order = {}
        for m in members:
            order.setdefault(m.findtext("name"), len(order))
        members.sort(key=lambda m: order[m.findtext("name")])
        for m in c.findall("member"):
            c.remove(m)
        c.extend(members)
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
    for c in sorted(groups, key=rank):
        root.append(c)
    tree.write(path, encoding="utf-8", xml_declaration=True)

def lengthen_group_ids(dst):
    """Pads the ids of the groups so that their length grows along GROUP_ORDER."""
    root = ET.parse(Path(dst) / "index.xml").getroot()
    ids = {c.findtext("name"): c.get("refid") for c in root.findall("compound")
           if c.get("kind") == "group"}
    base = max((len(i) for i in ids.values()), default=0)
    renames = {}
    for rank, name in enumerate(GROUP_ORDER):
        if name in ids:
            renames[ids[name]] = ids[name].ljust(base + rank + 1, "z")
    if not renames:
        return
    pattern = re.compile("|".join(re.escape(k) + "(?![A-Za-z0-9])"
                                  for k in sorted(renames, key=len, reverse=True)))
    for path in Path(dst).glob("*.xml"):
        text = path.read_text()
        new = pattern.sub(lambda m: renames[m.group(0)], text)
        if new != text:
            path.write_text(new)
    for old, new in renames.items():
        if (Path(dst) / f"{old}.xml").exists():
            (Path(dst) / f"{old}.xml").rename(Path(dst) / f"{new}.xml")

def main(src, dst):
    shutil.rmtree(dst, ignore_errors=True)
    shutil.copytree(src, dst)
    for path in Path(dst).glob("*.xml"):
        if path.name.startswith(("class", "struct", "union")):
            flatten(path)
        if path.name != "index.xml":
            retitle_callouts(path)
    tidy_index(dst)
    lengthen_group_ids(dst)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2])
