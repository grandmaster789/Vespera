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

    add_executable(vespera_test)
    add_test(NAME vespera_test COMMAND vespera_test)

    file(GLOB_RECURSE VesperaUnitTestSources CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.inl"
    )

    target_sources(vespera_test PRIVATE ${VesperaUnitTestSources})
    vespera_target_defaults(vespera_test)
endif()
