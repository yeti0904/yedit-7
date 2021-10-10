cc      = g++
out     = bin/yedit
libs    = -lncurses -lstdc++fs
src     = src/main.cc src/fs.cc src/ui.cc src/util.cc
headers = src/colour.hh src/constants.hh src/editmode.hh src/fs.hh src/ui.hh src/util.hh
std     = c++17
include = ./lib
pkgout  = yedit_0.7.2-1_amd64

build: $(src) $(headers)
	@$(cc) $(src) -o $(out) $(libs) -std=$(std) -I$(include)

debug: $(src) $(headers)
	@$(cc) $(src) -o $(out) $(libs) -std=$(std) -I$(include) -g

run: $(out)
	@./$(out)

deb: $(out)
	mkdir $(pkgout)
	mkdir -p $(pkgout)/usr/local/bin $(pkgout)/DEBIAN
	cp $(out) $(pkgout)/usr/local/bin/
	touch $(pkgout)/DEBIAN/control
	curl https://server.mesyeti.uk/deb/yedit-control > $(pkgout)/DEBIAN/control
	dpkg-deb --build --root-owner-group $(pkgout)
	rm -r $(pkgout)
