cmake_minimum_required(VERSION 3.26.4)
project(triplane)

set(PREFIX "/usr/local" CACHE PATH "Installation prefix")

set(VERSION "1.0.8-vranki")
set(CMAKE_CXX_STANDARD 23)
#CFLAGS      := --std=c++23 -Wall -pedantic -Wextra -Wshadow -Werror -Isrc $(OPTIFLAG) $(CFLAGS_NOSDL) `$(SDL_CONFIG) --cflags` -DHAVE_SDL_MIXER "-DTRIPLANE_DATA=\"$(PREFIX)/share/games/triplane\"" "-DTRIPLANE_VERSION=\"$(VERSION)\""
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wshadow -Werror -O2 -g -DHAVE_SDL_MIXER -DTRIPLANE_DATA=\\\"${PREFIX}/share/games/triplane\\\" -DTRIPLANE_VERSION=\\\"${VERSION}\\\" ")
#"-DTRIPLANE_DATA=\"$(PREFIX)/share/games/triplane\"" "-DTRIPLANE_VERSION=\"$(VERSION)\""
# Set your source files
set(SRCS
        src/gfx/bitmap.cpp
        src/gfx/font.cpp
        src/gfx/gfx.cpp
        src/gfx/fades.cpp
        src/gfx/extra.cpp
        src/util/wutil.cpp
        src/util/random.cpp
        src/io/joystick.cpp
        src/io/sound.cpp
        src/io/sdl_compat.cpp
        src/io/video.cpp
        src/io/mouse.cpp
        src/io/dksfile.cpp
        src/io/timing.cpp
        src/world/tripai.cpp
        src/world/tripmis.cpp
        src/world/plane.cpp
        src/world/tripaudio.cpp
        src/world/terrain.cpp
        src/world/fobjects.cpp
        src/world/tmexept.cpp
        src/menus/tripmenu.cpp
        src/tools/lvledit/lvledit.cpp
        src/triplane.cpp
        src/settings.cpp
        #src/tools/pgdview/pgdview.cpp
        #src/tools/pcx2pgd/pcx2pgd.cpp
        #src/tools/dksbuild/dksbuild.cc
)

# Create an executable target for triplane
add_executable(triplane ${SRCS})

# Specify any additional include directories
target_include_directories(triplane PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/src/tools/dksbuild)

# Specify any additional library directories
link_directories(${PROJECT_SOURCE_DIR}/src/io ${PROJECT_SOURCE_DIR}/src/gfx)

# Link libraries
target_link_libraries(triplane PRIVATE SDL SDL_mixer m)

# Set up installation

set(DESTDIR "" CACHE PATH "Installation destination")

install(TARGETS triplane
        DESTINATION ${PREFIX}/games)

install(FILES fokker.dks
        DESTINATION ${PREFIX}/share/games/triplane)

install(FILES doc/triplane.6
        DESTINATION ${PREFIX}/share/man/man6)

