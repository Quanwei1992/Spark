cmake_minimum_required(VERSION 3.25)

include(cmake/ucm/ucm.cmake)
include(cmake/utils.cmake)

if (POLICY CMP00091)
  # Enable MSVC Runtime Library Property
  cmake_policy(SET CMP0091 NEW)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON) 
set(CMAKE_CXX_STANDARD 17)
ucm_set_runtime(STATIC)
ucm_print_flags()

project(Spark)

add_subdirectory(extern)
add_subdirectory(core/engine)
add_subdirectory(core/script-core)
add_subdirectory(apps/sandbox)
add_subdirectory(apps/editor)

utils_get_all_targets(all_targets)

set_property(TARGET ${all_targets} PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")




