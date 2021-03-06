project(external C CXX)

set(BUILD_SHARED_LIBS_BCKP ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Force disable of shared libraries" FORCE)

set(BUILD_TESTING_BCKP ${BUILD_TESTING})
set(BUILD_TESTING OFF CACHE BOOL "Force disable of tests for external dependencies" FORCE)

set(CUSTOM_COVERAGE_EXCLUDE ${CUSTOM_COVERAGE_EXCLUDE} "external" PARENT_SCOPE) # Replaced in CTestCustom.cmake.in

#Use fmtlib for formatting
add_subdirectory(fmt EXCLUDE_FROM_ALL)
add_library(external::fmt ALIAS fmt)

#Use spdlog for logging, and tell it to use our version of fmtlib
add_subdirectory(spdlog EXCLUDE_FROM_ALL)
target_compile_definitions(spdlog INTERFACE SPDLOG_FMT_EXTERNAL)
add_library(external::spdlog ALIAS spdlog)

#Doctest for unit tests
add_library(doctest INTERFACE)
target_include_directories(
    doctest
    INTERFACE
    doctest/doctest # note : will expose the parts/ folder...
)
add_library(external::doctest ALIAS doctest)

add_subdirectory(StormLib EXCLUDE_FROM_ALL)
target_set_warnings(storm DISABLE ALL)
add_library(external::storm ALIAS storm)

#Add new external (non-interface) targets to this list
set_target_properties(fmt storm PROPERTIES FOLDER "external")

set(BUILD_TESTING ${BUILD_TESTING_BCKP} CACHE BOOL "Build tests (default variable for CTest)" FORCE) #Set it back to its past value
set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_BCKP} CACHE BOOL "Cause all libraries to be built shared unless the library was explicitly added as a static library." FORCE)