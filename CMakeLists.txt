cmake_minimum_required(VERSION 3.28)

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

    add_library(vespera)

    file(GLOB_RECURSE VesperaSources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl"
    )

    target_sources(vespera PRIVATE ${VesperaSources})
    target_include_directories(vespera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
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

    add_executable(vespera_test)
    target_link_libraries(vespera_test PRIVATE vespera)
    add_test(NAME vespera_test COMMAND vespera_test)

    file(GLOB_RECURSE VesperaUnitTestSources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.inl"
    )

    list(FILTER VesperaUnitTestSources EXCLUDE REGEX "/tests/death/")

    target_sources(vespera_test PRIVATE ${VesperaUnitTestSources})
    vespera_target_defaults(vespera_test)

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

