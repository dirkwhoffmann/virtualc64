#!/usr/bin/env python3
"""Converts the Markdown guides into a form Doxygen understands.

The guides are written for sourcey. This script writes a copy to an output
directory in which

  - the front matter is replaced by a Doxygen page title,
  - links to the API reference (/api-reference/...) become Doxygen references,
  - images are included with \\image html.

Links between the guides (threading.md etc.) are resolved by Doxygen itself.

Usage: guides_for_doxygen.py <guides-dir> <output-dir>
"""
import re, sys
from pathlib import Path

# Enumerations are documented in modules, so their links name the enumeration
ENUMS = {"opt": "vc64::Opt", "execstate": "vc64::ExecState", "msg": "vc64::Msg"}

def identifiers(headers):
    """Maps the lower-case spelling of every identifier in the headers to the real one."""
    table = {}
    for path in headers:
        for word in set(re.findall(r"[A-Za-z_][A-Za-z0-9_]*", path.read_text(errors="ignore"))):
            table.setdefault(word.lower(), set()).add(word)
    return table

def doxygen_headers(doxyfile):
    text = Path(doxyfile).read_text()
    block = re.search(r"^INPUT\s*=(.*?)(?=^\S)", text, re.S | re.M).group(1)
    root = Path(doxyfile).parent
    return [root / p for p in re.findall(r"[^\s\\]+\.h", block)]

def convert(text, ids, page):
    def ref(match):
        label, target, anchor = match.group(1), match.group(2), match.group(3)
        if target.startswith("grp_"):
            name = ENUMS[anchor]
        else:
            name = target.replace("-", "::")
            if anchor:
                real = ids.get(anchor, set())
                # Prefer member functions (lower-case start) over enumerators and types
                for keep in (lambda w: w[0].islower(), lambda w: not w.isupper(),
                             lambda w: w != anchor):
                    if len(real) > 1 and any(keep(w) for w in real):
                        real = {w for w in real if keep(w)}
                if len(real) != 1:
                    sys.exit(f"{page}: cannot resolve member '{anchor}' of {name}: {real}")
                name += "::" + next(iter(real))
        return f"[{label}](@ref {name})"

    text = re.sub(r"\[([^\]]+)\]\(/api-reference/([\w-]+)\.html(?:#(\w+))?\)", ref, text)

    def image(match):
        alt, path = match.group(1), match.group(2)
        if path.startswith("http"):
            return match.group(0)
        return f'@image html {Path(path).name} "{alt}"'

    return re.sub(r"!\[([^\]]*)\]\(([^)]+)\)", image, text)

def main(src, dst, doxyfile):
    ids = identifiers(doxygen_headers(doxyfile))
    dst = Path(dst)
    dst.mkdir(parents=True, exist_ok=True)
    for page in sorted(Path(src).glob("*.md")):
        text = page.read_text()
        front = re.match(r"---\n(.*?)\n---\n", text, re.S)
        title = re.search(r"^title:\s*(.*)$", front.group(1), re.M).group(1).strip()
        body = convert(text[front.end():], ids, page.name)
        (dst / page.name).write_text(f"# {title} {{#guide_{page.stem.replace('-', '_')}}}\n{body}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2], Path(__file__).parent / "Doxyfile")
