# Experimental: API reference with Doxygen -> Moxygen -> VitePress.
#
#   make          Build the site into build/site
#   make serve    Live preview (VitePress dev server, http://localhost:5173)
#   make clean
#
# Doxygen runs with the settings of ../Doxygen/Doxyfile. moxygen and VitePress are
# installed with "npm install".

GENERATED = docs/*.md docs/*.svg docs/vc-*.png

# The XML is generated without the modules of groups.dox (they exist for
# sourcey): the API classes get their own pages and the sidebar lists them.
# The namespace page then documents all types and enumerations.
markdown:
	cd ../Doxygen && python3 guides_for_doxygen.py guides build/guides
	cd ../Doxygen && (cat Doxyfile; echo "EXCLUDE = groups.dox"; \
	    echo "OUTPUT_DIRECTORY = ../Moxygen/build/doxygen"; echo "GENERATE_HTML = NO") | doxygen -
	python3 ../Doxygen/flatten_groups.py --keep-ids --keep-callouts --keep-namespace build/doxygen/xml build/xml
	rm -f $(GENERATED)
	npx moxygen -c -p -a -f -F github -o 'docs/%s.md' build/xml > /dev/null
	python3 fix_markdown.py docs ../Doxygen/guides/images

all: markdown
	npx vitepress build docs
	@echo "Open build/site/index.html"

serve: markdown
	npx vitepress dev docs

clean:
	rm -rf build $(GENERATED) moxygen.log docs/.vitepress/cache

.PHONY: all markdown serve clean
