cmake_minimum_required(VERSION 4.2)
project(25Plus5Clock C)

set(CMAKE_C_STANDARD 11)

# 1. Find tools and libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(CANBERRA REQUIRED libcanberra-gtk3)

# 2. GResource Compilation
# This converts resources.xml + your assets into a .c file
find_program(GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED)
set(RESOURCES_C "${CMAKE_CURRENT_BINARY_DIR}/resources.c")

add_custom_command(
        OUTPUT ${RESOURCES_C}
        COMMAND ${GLIB_COMPILE_RESOURCES} --target=${RESOURCES_C} --generate-source ${CMAKE_CURRENT_SOURCE_DIR}/resources.xml
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resources.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/style.css
        ${CMAKE_CURRENT_SOURCE_DIR}/icon.svg
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM
)

# 3. Setup search paths
include_directories(${GTK3_INCLUDE_DIRS} ${CANBERRA_INCLUDE_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER} ${CANBERRA_CFLAGS_OTHER})

# 4. Create executable
# Note: ${RESOURCES_C} is added here so it compiles into your app
add_executable(25Plus5Clock
        main.c
        src/app.c
        src/ui.c
        src/callbacks.c
        ${RESOURCES_C})

# 5. Link libraries
target_link_libraries(25Plus5Clock ${GTK3_LIBRARIES} ${CANBERRA_LIBRARIES})

# 6. Release Optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(25Plus5Clock PROPERTIES
            COMPILE_FLAGS "-Os"
            LINK_FLAGS "-s")
endif()

# 7. Installation Rules (Standard GNU Paths)
include(GNUInstallDirs)

# Install the executable binary to /usr/local/bin
install(TARGETS 25Plus5Clock DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install the .desktop file to /usr/local/share/applications
install(FILES com.github.neobliz1.Clock25Plus5.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

# Install your icon to the system icon cache so the launcher can find it
install(FILES icon.svg
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
        RENAME com.github.neobliz1.Clock25Plus5.svg)

# 8. Automated Logic Verification Framework
enable_testing()

# Define the automated execution file target mapping
add_executable(run_logic_tests tests/test_logic.c src/app.c src/callbacks.c)

# Add inclusion directory lookups to map your header configurations
target_include_directories(run_logic_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link your GTK and Canberra compilation symbols
target_link_libraries(run_logic_tests PRIVATE ${GTK3_LIBRARIES} ${CANBERRA_LIBRARIES})

# Register the testing target under the system CMake index engine
add_test(NAME CoreLogicTests COMMAND run_logic_tests)

# 9. Debian Packaging Configuration (CPack Alpha Pre-release)
set(CPACK_PACKAGE_NAME "25plus5clock")
set(CPACK_PACKAGE_VENDOR "neobliz1")

# Explicitly tags the output artifact package filename and inner metadata as Alpha
set(CPACK_PACKAGE_VERSION "1.0.0-alpha1")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A professional Pomodoro timer (Alpha Pre-release)")
set(CPACK_PACKAGE_CONTACT "Your Email <your.email@example.com>")

# Target distribution archive format type selector
set(CPACK_GENERATOR "DEB")

# Mandatory package requirements mapping for system package managers
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libgtk-3-0, libcanberra-gtk3-0")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

include(CPack)