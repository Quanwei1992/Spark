set(this Engine)
ucm_add_dirs(source TO sources RECURSIVE)
add_library(${this} STATIC ${sources})
target_precompile_headers(${this} PRIVATE source/skpch.h)

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

target_include_directories(${this} PUBLIC source)

if(WIN32)
    target_compile_definitions(${this} PUBLIC SK_PLATFORM_WINDOWS)
    # For mono rumtime
    target_link_libraries(${this} PUBLIC bcrypt.lib Mswsock.lib ws2_32.lib psapi.lib version.lib winmm.lib)
endif()

set_target_properties(${this} PROPERTIES FOLDER core)