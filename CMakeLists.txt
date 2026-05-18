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

# 6. GSettings Schema Compilation Automation
find_program(GLIB_COMPILE_SCHEMAS NAMES glib-compile-schemas REQUIRED)

add_custom_command(
        TARGET 25Plus5Clock PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/com.github.neobliz1.25Plus5Clock.gschema.xml
        ${CMAKE_CURRENT_BINARY_DIR}/
        COMMENT "Syncing schema configuration mapping files..."
)

add_custom_command(
        TARGET 25Plus5Clock POST_BUILD
        COMMAND ${GLIB_COMPILE_SCHEMAS} ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Compiling local GSettings application schemas..."
        VERBATIM
)

# 7. Release Optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(25Plus5Clock PROPERTIES
            COMPILE_FLAGS "-Os"
            LINK_FLAGS "-s")
endif()

# 8. Installation Rules (Standard GNU Paths)
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

# Install the schema layout blueprint straight to the system glib repository
install(FILES com.github.neobliz1.25Plus5Clock.gschema.xml
        DESTINATION ${CMAKE_INSTALL_DATADIR}/glib-2.0/schemas)

# 9. Automated Logic Verification Framework
enable_testing()

# --- Common Test Target Dependencies ---
# Collect files required by multiple test runners to avoid repetition
set(TEST_CORE_SOURCES src/app.c src/callbacks.c)

# ----------------------------------------------------
# Target 1: Core Logic Test Runner
# ----------------------------------------------------
add_executable(run_logic_tests tests/test_logic.c ${TEST_CORE_SOURCES})
target_include_directories(run_logic_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(run_logic_tests PRIVATE ${GTK3_LIBRARIES} ${CANBERRA_LIBRARIES})

# Register and apply structural labels for parallel execution or filtering
add_test(NAME CoreLogicTests COMMAND run_logic_tests)
set_tests_properties(CoreLogicTests PROPERTIES LABELS "logic")
set_tests_properties(CoreLogicTests PROPERTIES ENVIRONMENT "GSETTINGS_SCHEMA_DIR=${CMAKE_CURRENT_BINARY_DIR}")
add_dependencies(run_logic_tests 25Plus5Clock)

# ----------------------------------------------------
# Target 2: GSettings Persistence Test Runner
# ----------------------------------------------------
add_executable(run_gsettings_tests tests/test_timer_gsettings.c ${TEST_CORE_SOURCES})
target_include_directories(run_gsettings_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(run_gsettings_tests PRIVATE ${GTK3_LIBRARIES} ${CANBERRA_LIBRARIES})

# Register and tag the persistence test module
add_test(NAME GSettingsPersistenceTests COMMAND run_gsettings_tests)
set_tests_properties(GSettingsPersistenceTests PROPERTIES LABELS "storage")
set_tests_properties(GSettingsPersistenceTests PROPERTIES ENVIRONMENT "GSETTINGS_SCHEMA_DIR=${CMAKE_CURRENT_BINARY_DIR}")
add_dependencies(run_gsettings_tests 25Plus5Clock)

# 10. Debian Packaging Configuration (CPack Alpha Pre-release)
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