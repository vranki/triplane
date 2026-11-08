cmake_minimum_required(VERSION 3.24)
project(triplane CXX)

set(VERSION "1.0.8-vranki")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

# --- Dependencies -----------------------------------------------------------
# SDL3 must be installed (e.g. `brew install sdl3`, or a distro package).
find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3)

# libmikmod plays the MOD music (e.g. `brew install libmikmod`).
find_package(PkgConfig REQUIRED)
pkg_check_modules(MIKMOD REQUIRED IMPORTED_TARGET libmikmod)

# --- Common settings ----------------------------------------------------------
add_library(triplane_options INTERFACE)
target_include_directories(triplane_options INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_compile_definitions(triplane_options INTERFACE
  TRIPLANE_DATA="${CMAKE_INSTALL_PREFIX}/share/games/triplane"
  TRIPLANE_VERSION="${VERSION}")
if(NOT MSVC)
  target_compile_options(triplane_options INTERFACE -Wall -Wextra -pedantic -Wshadow -Werror)
endif()
if(APPLE)
  # The macOS SDK marks sprintf() and friends as deprecated.
  target_compile_options(triplane_options INTERFACE -Wno-deprecated-declarations)
endif()

# --- Engine: graphics, input, sound and data file access --------------------
# Shared by the game and the tools below.
add_library(triplane_engine STATIC
        src/gfx/bitmap.cpp
        src/gfx/font.cpp
        src/gfx/gfx.cpp
        src/util/wutil.cpp
        src/util/random.cpp
        src/io/keys.cpp
        src/io/sdl_compat.cpp
        src/io/video.cpp
        src/io/mouse.cpp
        src/io/dksfile.cpp
        src/io/timing.cpp
)
target_link_libraries(triplane_engine PUBLIC triplane_options SDL3::SDL3 PkgConfig::MIKMOD)

# --- Game -------------------------------------------------------------------
add_executable(triplane
        src/triplane.cpp
        src/settings.cpp
        src/gfx/fades.cpp
        src/gfx/extra.cpp
        src/io/joystick.cpp
        src/io/sound.cpp
        src/menus/tripmenu.cpp
        src/world/tripai.cpp
        src/world/tripmis.cpp
        src/world/plane.cpp
        src/world/tripaudio.cpp
        src/world/terrain.cpp
        src/world/fobjects.cpp
        src/world/tmexept.cpp
)
target_link_libraries(triplane PRIVATE triplane_engine)

# --- Tools --------------------------------------------------------------------
# Level editor (see doc/lvledit-keys.txt).
add_executable(lvledit src/tools/lvledit/lvledit.cpp)
target_link_libraries(lvledit PRIVATE triplane_engine)

# Viewer for .pgd images inside fokker.dks.
add_executable(pgdview src/tools/pgdview/pgdview.cpp)
target_link_libraries(pgdview PRIVATE triplane_engine)

# Converts PCX images to .pgd (used by tools/build-data-from-source).
add_executable(pcx2pgd src/tools/pcx2pgd/pcx2pgd.cpp)
target_link_libraries(pcx2pgd PRIVATE triplane_options)

# --- Data file --------------------------------------------------------------
add_executable(dksbuild src/tools/dksbuild/dksbuild.cc)
target_link_libraries(dksbuild PRIVATE triplane_options)

file(STRINGS data/fokker.lst _dks_lines)
set(DKS_INPUTS data/fokker.lst)
foreach(_line IN LISTS _dks_lines)
  if(_line MATCHES "^(data/[^ \t]+)")
    list(APPEND DKS_INPUTS ${PROJECT_SOURCE_DIR}/${CMAKE_MATCH_1})
  endif()
endforeach()

# dksbuild reads the short relative paths in data/fokker.lst, so it runs in
# the source tree. The result is copied next to the binary as well.
add_custom_command(
  OUTPUT ${PROJECT_SOURCE_DIR}/fokker.dks
  COMMAND dksbuild data/fokker.lst
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  DEPENDS dksbuild ${DKS_INPUTS}
  COMMENT "Building fokker.dks")
add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/fokker.dks
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/fokker.dks ${CMAKE_BINARY_DIR}/fokker.dks
  DEPENDS ${PROJECT_SOURCE_DIR}/fokker.dks)
add_custom_target(data ALL DEPENDS ${CMAKE_BINARY_DIR}/fokker.dks)

# --- Install ----------------------------------------------------------------
# Use -DCMAKE_INSTALL_PREFIX=... (default /usr/local) to pick the prefix.
install(TARGETS triplane DESTINATION games)
install(FILES ${PROJECT_SOURCE_DIR}/fokker.dks DESTINATION share/games/triplane)
install(FILES doc/triplane.6 DESTINATION share/man/man6)
