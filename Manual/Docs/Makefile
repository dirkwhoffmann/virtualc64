# API documentation: Doxygen -> Moxygen -> VitePress.
#
#   make          Build the site into build/site
#   make serve    Live preview (VitePress dev server, http://localhost:5173)
#   make clean
#
# The headers are parsed by Doxygen (Doxyfile), the XML is prepared by
# prepare_xml.py and turned into Markdown by moxygen (fix_markdown.py sorts
# the pages into the guide and the reference). VitePress renders the site.
# moxygen and VitePress are installed with "npm install".

GENERATED = docs/*.md docs/guide docs/reference

markdown:
	mkdir -p build
	doxygen Doxyfile
	python3 prepare_xml.py build/doxygen/xml build/xml
	rm -rf $(GENERATED)
	npx moxygen -c -p -a -f -F github -o 'docs/%s.md' build/xml > /dev/null
	python3 fix_markdown.py docs guides/images

all: markdown
	npx vitepress build docs
	@echo "Open build/site/index.html"

serve: markdown
	npx vitepress dev docs

clean:
	rm -rf build $(GENERATED) moxygen.log docs/.vitepress/cache

.PHONY: all markdown serve clean
