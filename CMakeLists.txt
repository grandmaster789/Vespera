cmake_minimum_required(VERSION 3.28)

# shaped-core's guard for the Windows developer environment. It has to run before
# project() detects the compilers: it records which VS environment this build
# directory was configured in, and refuses a later configure from a different one.
# A mixed configure can pair cached and re-detected tools and hang the build.
# On Linux it does nothing.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/extern/shaped-core/tools/cmake/ConfigureEnvironment.cmake")
    include("${CMAKE_CURRENT_SOURCE_DIR}/extern/shaped-core/tools/cmake/ConfigureEnvironment.cmake")
endif()

project(Vespera
        VERSION 0.1.0
        DESCRIPTION "Game engine + VR vertical bullet-hell"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# Shared settings for every Vespera target. Warnings are errors; locally you can
# opt out for one configure with `cmake --compile-no-warning-as-error`.
function(vespera_target_defaults target)
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE /W4 -Wpedantic)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    set_target_properties(${target} PROPERTIES COMPILE_WARNING_AS_ERROR ON)
endfunction()



if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    enable_testing()

    # shaped-core, pinned as a git submodule. It must be added before any Vespera
    # target exists.
    #   SC_LIBRARIES     only these libraries (and what they depend on) get configured;
    #                    later milestones add to the list
    #   EXCLUDE_FROM_ALL build only what Vespera links, not shaped-core's tools
    #   SYSTEM           its headers can't trip Vespera's warnings-as-errors
    # shaped-core needs Clang 19 or newer (it builds and tests with 21). Older Clang
    # fails deep inside clean-core's headers, so stop here with a clear message.
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19)
        message(FATAL_ERROR
            "Clang ${CMAKE_CXX_COMPILER_VERSION} is too old for shaped-core; it needs 19 or newer.\n"
            "Pick a newer one: cmake --preset <name> --fresh -DCMAKE_C_COMPILER=clang-21 -DCMAKE_CXX_COMPILER=clang++-21")
    endif()

    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/extern/shaped-core/CMakeLists.txt")
        message(FATAL_ERROR
            "extern/shaped-core is empty: the repository was cloned without its submodules.\n"
            "Run: git submodule update --init --recursive")
    endif()
    set(SC_LIBRARIES "clean-core;typed-geometry")
    add_subdirectory(extern/shaped-core EXCLUDE_FROM_ALL SYSTEM)

    # Upstream bug workaround; see the header for when to delete it.
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_compile_options(clean-core PRIVATE
            "$<$<COMPILE_LANGUAGE:CXX>:-include${CMAKE_CURRENT_SOURCE_DIR}/cmake/shaped_core_workarounds.h>")
    endif()

    add_library(vespera)

    file(GLOB_RECURSE VesperaSources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl"
    )

    target_sources(vespera PRIVATE ${VesperaSources})
    target_include_directories(vespera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(vespera PUBLIC clean-core typed-geometry)
    vespera_target_defaults(vespera)

    add_executable(vespera_app)

    file(GLOB_RECURSE VesperaAppSources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/app/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/app/*.inl"
    )
    target_sources(vespera_app PRIVATE ${VesperaAppSources})
    target_link_libraries(vespera_app PRIVATE vespera)
    vespera_target_defaults(vespera_app)

    # Unit tests: each tests/<dir>/<name>.cpp is its own executable named
    # <dir>_<name> (e.g. tests/core/test_logger.cpp -> core_test_logger).
    file(GLOB_RECURSE VesperaUnitTestSources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )
    list(FILTER VesperaUnitTestSources EXCLUDE REGEX "/tests/death/")

    foreach(source IN LISTS VesperaUnitTestSources)
        file(RELATIVE_PATH name ${CMAKE_CURRENT_SOURCE_DIR}/tests ${source})
        string(REGEX REPLACE "\\.cpp$" "" name ${name})
        string(REPLACE "/" "_" name ${name})

        add_executable(${name} ${source})
        target_link_libraries(${name} PRIVATE vespera)
        vespera_target_defaults(${name})
        add_test(NAME ${name} COMMAND ${name})
    endforeach()

    # Death tests: each tests/death/<name>.cpp is its own executable that must
    # die from a failed assert/check (see cmake/death_test.cmake).
    string(REPLACE ";" "|" VesperaEmulator "${CMAKE_CROSSCOMPILING_EMULATOR}")
    file(GLOB VesperaDeathTestSources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/death/*.cpp"
    )
    foreach(source IN LISTS VesperaDeathTestSources)
        get_filename_component(name ${source} NAME_WE)

        add_executable(death_${name} ${source})
        target_link_libraries(death_${name} PRIVATE vespera)
        vespera_target_defaults(death_${name})

        add_test(
            NAME death_${name}
            COMMAND ${CMAKE_COMMAND}
                -DEXE=$<TARGET_FILE:death_${name}>
                -DEMULATOR=${VesperaEmulator}
                -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/death_test.cmake
        )

    endforeach()
endif()

