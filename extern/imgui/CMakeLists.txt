set(this imgui)


ucm_add_files(  imconfig.h imgui.h imgui.cpp imgui_demo.cpp imgui_draw.cpp
                imgui_internal.h
                imgui_tables.cpp
                imgui_widgets.cpp
                imstb_rectpack.h
                imstb_textedit.h
                imstb_truetype.h
                backends/imgui_impl_glfw.h
                backends/imgui_impl_glfw.cpp
                backends/imgui_impl_opengl3.h
                backends/imgui_impl_opengl3.cpp 
TO sources)

add_library(${this} STATIC ${sources})
target_include_directories(${this} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${this} PRIVATE glfw)
set_target_properties(${this} PROPERTIES FOLDER extern)    
