# Builds the API reference.
#
#   make          Doxygen HTML reference (build/html) and XML (build/xml)
#   make sourcey  Sourcey site (build/sourcey): guides + API reference
#   make dev      Sourcey live preview (http://localhost:4400)
#   make serve    Serve build/sourcey at http://localhost:8000

all:
	doxygen Doxyfile
	@echo "Open build/html/index.html"

sourcey: all
	python3 flatten_groups.py build/xml build/xml-sourcey
	sourcey build -o build/sourcey
	@echo "Open build/sourcey/index.html"

dev: all
	python3 flatten_groups.py build/xml build/xml-sourcey
	sourcey dev

serve:
	@echo "Open http://localhost:8000"
	python3 -m http.server 8000 --directory build/sourcey

clean:
	rm -rf build

.PHONY: all sourcey dev serve clean
