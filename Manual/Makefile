# API documentation: Doxygen -> Moxygen -> VitePress.
#
#   make          Build the site into build/site
#   make serve    Live preview (VitePress dev server, http://localhost:5173)
#   make dev      Like serve, but also rebuilds the pages whenever a header or
#                 guide changes (reload happens automatically)
#   make publish  Build for the web site and copy it over the old documentation
#   make clean
#
# The color scheme is selected with SCHEME (catppuccin, dracula, nord or
# tokyo-night), e.g. "make SCHEME=nord" or "make serve SCHEME=dracula".
#
# The headers are parsed by Doxygen (Doxyfile), the XML is prepared by
# prepare_xml.py and turned into Markdown by moxygen (fix_markdown.py sorts
# the pages into the guide and the API). VitePress renders the site.
# moxygen and VitePress are installed with "npm install".

SCHEME ?= tokyo-night
export SCHEME

GENERATED = docs/*.md docs/guide docs/api

markdown:
	mkdir -p build
	doxygen Doxyfile
	python3 prepare_xml.py build/doxygen/xml build/xml
	rm -rf $(GENERATED)
	@# moxygen warns "namespace mismatch" for every class: it reads the namespace of a
	@# class before it has been set. The result is correct, so the warning is filtered.
	npx moxygen -c -p -a -f -F github -o 'docs/%s.md' build/xml > /dev/null 2> build/moxygen.err || { cat build/moxygen.err; exit 1; }
	@grep -v 'namespace mismatch' build/moxygen.err || true
	python3 fix_markdown.py docs guides/images

all: markdown
	npx vitepress build docs
	@echo "Open build/site/index.html"

serve: markdown
	npx vitepress dev docs

# Polls the sources once a second and regenerates the Markdown on a change.
# New or removed pages show up in the sidebar after restarting the server.
dev: markdown
	@touch build/.stamp
	@( while sleep 1; do \
	     if [ -n "$$(find guides ../VCCore Doxyfile prepare_xml.py fix_markdown.py \( -name '*.h' -o -name '*.md' -o -name Doxyfile -o -name '*.py' \) -newer build/.stamp 2>/dev/null | head -1)" ]; then \
	       touch build/.stamp; echo "Sources changed, regenerating..."; \
	       $(MAKE) --no-print-directory markdown 2>&1 | tail -3; \
	     fi; \
	   done ) & WATCHER=$$!; trap "kill $$WATCHER" EXIT INT TERM; \
	npx vitepress dev docs

# The web site is a clone of dirkwhoffmann.github.io next to the Retro folder
PUBLISH_DIR ?= ../../dirkwhoffmann.github.io/VirtualC64/docs

publish: markdown
	BASE=/VirtualC64/docs/ npx vitepress build docs
	rm -rf $(PUBLISH_DIR)
	cp -R build/site $(PUBLISH_DIR)
	rm -rf build/site
	@echo "Copied the site to $(PUBLISH_DIR). Commit it there."

clean:
	rm -rf build $(GENERATED) moxygen.log docs/.vitepress/cache

.PHONY: all markdown serve dev publish clean
