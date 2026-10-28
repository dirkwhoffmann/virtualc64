# Experimental: API reference with Doxide (C++ comments -> Markdown) and MkDocs.
#
#   make          Build the site into build/site
#   make serve    Live preview at http://localhost:8002 (rebuilds the Markdown first)
#   make clean
#
# Requires doxide and a Python environment with mkdocs-material in .venv:
#   python3 -m venv .venv && .venv/bin/pip install mkdocs mkdocs-material

MKDOCS = .venv/bin/mkdocs

all: markdown
	$(MKDOCS) build -q -d build/site
	@echo "Open build/site/index.html"

markdown:
	python3 prepare_sources.py build/src
	doxide build

serve: markdown
	$(MKDOCS) serve -a localhost:8002

clean:
	rm -rf build docs/vc64 docs/utl docs/retro docs/index.md

.PHONY: all markdown serve clean
