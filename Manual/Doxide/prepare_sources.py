#!/usr/bin/env python3
"""Copies the public API headers (the INPUT list of ../Doxygen/Doxyfile) to src/.

Doxide stops parsing a header that contains a "/// @file" comment, and it does
not know @brief, so those are removed from the copies. The original headers stay untouched.

Usage: prepare_sources.py <output-dir>
"""
import re, shutil, sys
from pathlib import Path

DOXYFILE = Path(__file__).parent.parent / "Doxygen" / "Doxyfile"

def main(dst):
    root = DOXYFILE.parent
    block = re.search(r"^INPUT\s*=(.*?)(?=^\S)", DOXYFILE.read_text(), re.S | re.M).group(1)
    shutil.rmtree(dst, ignore_errors=True)
    count = 0
    for name in re.findall(r"[^\s\\]+\.h", block):
        source = (root / name).resolve()
        target = Path(dst) / name.replace("../", "")
        target.parent.mkdir(parents=True, exist_ok=True)
        lines = [l.replace("@brief ", "") for l in source.read_text().splitlines()
                 if l.strip() != "/// @file"]
        target.write_text("\n".join(lines) + "\n")
        count += 1
    print(f"Copied {count} headers")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1])
