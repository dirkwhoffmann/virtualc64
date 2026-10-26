#!/usr/bin/env python3
"""Removes the summary table of the "Public Methods" section.

Sourcey starts the "Public Methods" section of a class page with a table that
lists every method again, followed by the full description of each method. The
table only repeats the first sentence of every description, so it is removed.

The script edits the generated HTML pages in place and leaves a page alone if
its structure is not the expected one.

Usage: strip_summaries.py <site-dir>
"""
import re, sys
from pathlib import Path

SUMMARY = re.compile(
    r'(<h2 id="public-methods">[^<]*</h2>\s*)<div class="table-wrap"[^>]*><table>.*?</table></div>',
    re.S)

def main(site):
    removed = 0
    for page in (Path(site) / "api-reference").glob("*.html"):
        html = page.read_text()
        new, count = SUMMARY.subn(r"\1", html, count=1)
        if count:
            page.write_text(new)
            removed += count
    print(f"Removed {removed} method summaries")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1])
