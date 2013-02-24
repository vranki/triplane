PREFIX ?= /usr/local
DESTDIR ?=

CXX	     ?= g++
OPTIFLAG     = -O2 -g
SDL_CONFIG  ?= sdl-config
VERSION      = 1.0.8
CFLAGS      := -Wall -Isrc $(OPTIFLAG) $(CFLAGS_NOSDL) `$(SDL_CONFIG) --cflags` -DHAVE_SDL_MIXER "-DTRIPLANE_DATA=\"$(PREFIX)/share/games/triplane\"" "-DTRIPLANE_VERSION=\"$(VERSION)\""
LDFLAGS      = 
LIBS        := `$(SDL_CONFIG) --libs` -lSDL_mixer -lm
INSTALL_DATA     ?= install -m 644
INSTALL_PROGRAM  ?= install

COMMON_OBJS = src/gfx/bitmap.o src/gfx/font.o \
	src/gfx/gfx.o src/util/wutil.o src/util/random.o \
	src/io/sdl_compat.o src/io/video.o \
	src/io/mouse.o src/io/dksfile.o src/io/timing.o
TRIPLANE_OBJS = src/triplane.o src/world/tripai.o \
	src/world/tripmis.o src/gfx/fades.o src/menus/tripmenu.o \
	src/world/terrain.o src/world/fobjects.o src/world/tmexept.o \
	src/gfx/extra.o src/settings.o src/world/plane.o src/io/joystick.o src/io/sound.o \
	src/world/tripaudio.o
LVLEDIT_OBJS = src/tools/lvledit/lvledit.o
PGDVIEW_OBJS = src/tools/pgdview/pgdview.o
PCX2PGD_OBJS = src/tools/pcx2pgd/pcx2pgd.o

COMMON_SRCS = $(COMMON_OBJS:.o=.cpp)
TRIPLANE_SRCS = $(TRIPLANE_OBJS:.o=.cpp)
LVLEDIT_SRCS = $(LVLEDIT_OBJS:.o=.cpp)
PGDVIEW_SRCS = $(PGDVIEW_OBJS:.o=.cpp)
PCX2PGD_SRCS = $(PCX2PGD_OBJS:.o=.cpp)
ALLSRCS = $(COMMON_SRCS) $(TRIPLANE_SRCS) $(LVLEDIT_SRCS) $(PGDVIEW_SRCS) $(PCX2PGD_SRCS)

all: checkdepend triplane tools/dksbuild fokker.dks tools/lvledit tools/pgdview

checkdepend:
	@[ -f .depend ] || ( echo 'Please run "make depend" first!'; exit 1 )

depend: $(ALLSRCS)
	rm -f .depend
	touch .depend
	(set -e; \
	for src in $(ALLSRCS); do \
		$(CXX) -MM -MT "`echo "$$src" | sed 's/\\.cpp$$/.o/'`" $(CFLAGS) "$$src" >>.depend; \
	done)
	echo "fokker.dks: tools/dksbuild data/fokker.lst \\" >>.depend
	sed '1d;  /^;/d;  s/ .*$$//;  s/$$/ \\/;  $$s/ \\//;  s/^/  /;' data/fokker.lst >> .depend

clean:
	rm -f .depend
	rm -f src/*.o
	rm -f src/*/*.o
	rm -f src/*/*/*.o
	rm -f triplane tools/dksbuild tools/lvledit tools/pgdview tools/pcx2pgd
	rm -f fokker.dks

fokker.dks:
	./tools/dksbuild data/fokker.lst

%.o: %.cpp
	$(CXX) -o $@ $(CFLAGS) -c $<

triplane: $(TRIPLANE_OBJS) $(COMMON_OBJS)
	$(CXX) -o $@ $(CFLAGS) $(LDFLAGS) $^ $(LIBS)

tools/lvledit: $(LVLEDIT_OBJS) $(COMMON_OBJS)
	$(CXX) -o $@ $(CFLAGS) $(LDFLAGS) $^ $(LIBS)

tools/pgdview: $(PGDVIEW_OBJS) $(COMMON_OBJS)
	$(CXX) -o $@ $(CFLAGS) $(LDFLAGS) $^ $(LIBS)

tools/pcx2pgd: $(PCX2PGD_OBJS) $(COMMON_OBJS)
	$(CXX) -o $@ $(CFLAGS) $(LDFLAGS) $^ $(LIBS)

tools/dksbuild: src/tools/dksbuild/dksbuild.cc
	$(CXX) -o tools/dksbuild -g src/tools/dksbuild/dksbuild.cc

install:
	mkdir -p $(DESTDIR)$(PREFIX)/games
	$(INSTALL_PROGRAM) triplane $(DESTDIR)$(PREFIX)/games/triplane
	mkdir -p $(DESTDIR)$(PREFIX)/share/games/triplane
	$(INSTALL_DATA) fokker.dks $(DESTDIR)$(PREFIX)/share/games/triplane/fokker.dks
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man6
	$(INSTALL_DATA) doc/triplane.6 $(DESTDIR)$(PREFIX)/share/man/man6/triplane.6
test:
	if [ ! -d triplane-testsuite ]; then echo Please darcs get http://iki.fi/lindi/darcs/triplane-testsuite; false; fi
	SDL_VIDEODRIVER=dummy bash tools/run-all-tests tools/run-one-test ./triplane triplane-testsuite

build-data-from-source: tools/pcx2pgd
	tools/build-data-from-source

dist:
	make clean
	test -z "`git ls-files -d -o -m -u -s`" || (echo "There are uncommitted changes" >&2; /bin/false)
	test -z "`git diff --cached`" || (echo "There are uncommitted changes in the index" >&2; /bin/false)
	test -z "`find -type d -empty ! -path "./.git/*"`" || (echo "There are empty directories" >&2; /bin/false)
	tar --transform "s/^./triplane-$(VERSION)/" --exclude=.git --exclude=debian -zcf ../triplane-$(VERSION).tar.gz .

# man -Tps doc/triplane.6 > triplane.ps
# ps2pdf triplane.ps
.PHONY: all checkdepend depend clean install test
-include .depend
