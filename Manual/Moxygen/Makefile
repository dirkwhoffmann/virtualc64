# Experimental: API reference with Doxygen -> Moxygen -> VitePress.
#
#   make          Build the site into build/site
#   make serve    Live preview (VitePress dev server, http://localhost:5173)
#   make clean
#
# The Doxygen XML comes from ../Doxygen (make there). moxygen and VitePress are
# installed with "npm install".

GENERATED = docs/*.md docs/*.svg docs/vc-*.png

markdown:
	$(MAKE) -C ../Doxygen all
	python3 ../Doxygen/flatten_groups.py --keep-ids --keep-callouts ../Doxygen/build/xml build/xml
	rm -f $(GENERATED)
	npx moxygen -g -c -p -a -f -F github -o 'docs/%s.md' build/xml > /dev/null
	python3 fix_markdown.py docs ../Doxygen/guides/images

all: markdown
	npx vitepress build docs
	@echo "Open build/site/index.html"

serve: markdown
	npx vitepress dev docs

clean:
	rm -rf build $(GENERATED) moxygen.log docs/.vitepress/cache

.PHONY: all markdown serve clean
