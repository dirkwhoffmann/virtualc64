# Builds the API reference.
#
#   make          Doxygen HTML reference (build/html) and XML (build/xml)
#   make sourcey  Sourcey site (build/sourcey): guides + API reference
#   make dev      Sourcey live preview (http://localhost:4400). Shows the guides
#                 and the styling; the API pages are not post-processed there
#                 (strip_summaries.py), so use "make serve" to see the final result.
#   make site     Both references side by side in build/site:
#                   build/site/VirtualC64/api   Doxygen
#                   build/site/VirtualC64/docs  Sourcey
#                 (Sourcey is built for the public path /VirtualC64/docs)
#   make serve    Serve build/site at http://localhost:8000/VirtualC64/

all:
	python3 guides_for_doxygen.py guides build/guides
	doxygen Doxyfile
	@echo "Open build/html/index.html"

sourcey: all
	python3 flatten_groups.py build/xml build/xml-sourcey
	sourcey build -o build/sourcey
	python3 strip_summaries.py build/sourcey
	@echo "Open build/sourcey/index.html"

dev: all
	python3 flatten_groups.py build/xml build/xml-sourcey
	sourcey dev

site: sourcey
	rm -rf build/site
	mkdir -p build/site/VirtualC64
	cp -R build/html build/site/VirtualC64/api
	cp -R build/sourcey build/site/VirtualC64/docs
	@echo "Doxygen: build/site/VirtualC64/api, Sourcey: build/site/VirtualC64/docs"

serve: site
	@echo "Doxygen: http://localhost:8000/VirtualC64/api/index.html"
	@echo "Sourcey: http://localhost:8000/VirtualC64/docs/index.html"
	python3 -m http.server 8000 --directory build/site

clean:
	rm -rf build

.PHONY: all sourcey dev site serve clean
