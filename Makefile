PREFIX = /usr/local
DESTDIR = ""

CXX	     = g++
OPTIFLAG     = -O2 -g
CFLAGS      := -Wall -Isrc $(OPTIFLAG) $(CFLAGS_NOSDL) `sdl-config --cflags` -DHAVE_SDL_MIXER "-DTRIPLANE_DATA=\"$(PREFIX)/share/games/triplane-classic\""
LDFLAGS      = 
LIBS        := `sdl-config --libs` -lSDL_mixer -lm

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

all: checkdepend triplane-classic tools/dksbuild fokker.dks tools/lvledit tools/pgdview

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
	rm -f triplane-classic tools/dksbuild tools/lvledit tools/pgdview tools/pcx2pgd
	rm -f fokker.dks

fokker.dks:
	./tools/dksbuild data/fokker.lst

%.o: %.cpp
	$(CXX) -o $@ $(CFLAGS) -c $<

triplane-classic: $(TRIPLANE_OBJS) $(COMMON_OBJS)
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
	install -D triplane-classic $(DESTDIR)$(PREFIX)/games/triplane-classic
	install -D -m 644 fokker.dks $(DESTDIR)$(PREFIX)/share/games/triplane-classic/fokker.dks
test:
	if [ ! -d triplane-testsuite ]; then echo Please darcs get http://iki.fi/lindi/darcs/triplane-testsuite; false; fi
	bash tools/run-all-tests tools/run-one-test ./triplane-classic triplane-testsuite

build-data-from-source: tools/pcx2pgd
	tools/build-data-from-source

# man -Tps doc/triplane-classic.6 > triplane-classic.ps
# ps2pdf triplane-classic.ps
.PHONY: all checkdepend depend clean install test
-include .depend
