# Convenience wrapper around the CMake build.
#
#   make                 build triplane and fokker.dks in $(BUILD_DIR)
#   make run             build and start the game in a window
#   make install         install (PREFIX and DESTDIR are honoured)
#   make test            replay the recorded games in triplane-testsuite/
#   make clean           remove build products, keep the configuration
#   make distclean       remove the whole build directory
#
# Variables: CMAKE, BUILD_DIR, BUILD_TYPE, PREFIX, DESTDIR, ARGS (for run).

CMAKE      ?= cmake
BUILD_DIR  ?= build
BUILD_TYPE ?= RelWithDebInfo
PREFIX     ?= /usr/local
DESTDIR    ?=
ARGS       ?= -w

all: configure
	$(CMAKE) --build $(BUILD_DIR) --parallel

# Re-run the configure step whenever PREFIX or BUILD_TYPE change.
configure:
	$(CMAKE) -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_INSTALL_PREFIX=$(PREFIX)

triplane dksbuild data lvledit pgdview pcx2pgd: configure
	$(CMAKE) --build $(BUILD_DIR) --parallel --target $@

# Dependencies are tracked by CMake; kept for older build scripts.
depend:

run: all
	cd $(BUILD_DIR) && ./triplane $(ARGS)

install: all
	DESTDIR=$(DESTDIR) $(CMAKE) --install $(BUILD_DIR)

test: all
	bash tools/run-all-tests tools/run-one-test $(BUILD_DIR)/triplane triplane-testsuite

clean:
	-$(CMAKE) --build $(BUILD_DIR) --target clean
	rm -f fokker.dks

distclean:
	rm -rf $(BUILD_DIR) fokker.dks

.PHONY: all configure triplane dksbuild data lvledit pgdview pcx2pgd depend run install test clean distclean
