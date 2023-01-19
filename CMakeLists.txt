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


set(this engine)
ucm_add_dirs(include src TO sources RECURSIVE)
add_library(${this} STATIC ${sources})
target_precompile_headers(${this} PRIVATE src/skpch.h)

target_link_libraries(${this} PUBLIC 
    box2d 
    EnTT 
    glad 
    glfw 
    glm 
    imgui
    imguizmo
    magicenum 
    spdlog 
    stbimage 
    typelist 
    yamlcpp
    spriv_cross
    shaderc
    mono
)

target_include_directories(${this} PUBLIC include)
target_include_directories(${this} PRIVATE src)

if(WIN32)
    target_compile_definitions(${this} PUBLIC SK_PLATFORM_WINDOWS)
    # For mono rumtime
    target_link_libraries(${this} PUBLIC bcrypt.lib Mswsock.lib ws2_32.lib psapi.lib version.lib winmm.lib)
endif()

set_target_properties(${this} PROPERTIES FOLDER engine) 

add_subdirectory(apps/sandbox)
add_subdirectory(apps/editor)

utils_get_all_targets(all_targets)

set_property(TARGET ${all_targets} PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")




