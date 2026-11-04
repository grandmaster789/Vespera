cmake_minimum_required(VERSION 3.28)

project(Vespera
        VERSION 0.1.0
        DESCRIPTION "Game engine + VR vertical bullet-hell"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    enable_testing()

    add_test(NAME vespera_test COMMAND vespera_test)

    file(GLOB_RECURSE VesperaUnitTestSources CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.inl"
    )

    target_sources(vespera_test PRIVATE ${VesperaUnitTestSources})
endif()