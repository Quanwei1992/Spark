set(this imguizmo)


ucm_add_files(  ImGuizmo.h  ImGuizmo.cpp
TO sources)

add_library(${this} STATIC ${sources})
target_include_directories(${this} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${this} PRIVATE imgui)
set_target_properties(${this} PROPERTIES FOLDER extern)    
