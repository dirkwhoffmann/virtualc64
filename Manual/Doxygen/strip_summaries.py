#!/usr/bin/env python3
"""Post-processes the generated class pages.

1. Removes the summary table of the "Public Methods" section.

Sourcey starts the "Public Methods" section of a class page with a table that
lists every method again, followed by the full description of each method. The
table only repeats the first sentence of every description, so it is removed.

2. Fixes the entries of overloaded functions. Sourcey gives every overload the
same heading id, so the links in "On this page" all jump to the first one and
the list shows the same name several times. Later overloads get the unique id
of the anchor that precedes them, and the repeated names are dropped from the
list.

The script edits the generated HTML pages in place and leaves a page alone if
its structure is not the expected one.

Usage: strip_summaries.py <site-dir>
"""
import re, sys
from pathlib import Path

SUMMARY = re.compile(
    r'(<h2 id="public-methods">[^<]*</h2>\s*)<div class="table-wrap"[^>]*><table>.*?</table></div>',
    re.S)

OVERLOAD = re.compile(r'(<p><a id="([^"]+)"></a></p>\s*)<h3 id="([^"]+)">')
TOC_ENTRY = re.compile(r'<li><a href="#([^"]+)"[^>]*>[^<]*</a></li>')

def fix_overloads(html):
    seen = set()
    def heading(match):
        anchor, current = match.group(2), match.group(3)
        if current in seen:
            current = anchor
        seen.add(current)
        return f'{match.group(1)}<h3 id="{current}">'
    html = OVERLOAD.sub(heading, html)
    start = html.find('<nav aria-labelledby="sourcey-toc-label">')
    end = html.find('</nav>', start)
    if start < 0:
        return html
    listed = set()
    def entry(match):
        if match.group(1) in listed:
            return ""
        listed.add(match.group(1))
        return match.group(0)
    return html[:start] + TOC_ENTRY.sub(entry, html[start:end]) + html[end:]

def main(site):
    removed = 0
    for page in (Path(site) / "api-reference").glob("*.html"):
        html = page.read_text()
        new, count = SUMMARY.subn(r"\1", html, count=1)
        new = fix_overloads(new)
        if new != html:
            page.write_text(new)
        removed += count
    print(f"Removed {removed} method summaries")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1])
