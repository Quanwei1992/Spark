set(this editor)
ucm_add_dirs(src assets resources TO editor_sources RECURSIVE ADDITIONAL_EXT scene glsl png)
add_executable(${this} ${editor_sources})
target_link_libraries(${this} PRIVATE engine)
set_target_properties(${this} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${this} PROPERTIES FOLDER apps)
add_dependencies(${this} script-core)

add_custom_command(TARGET ${this} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    "$<TARGET_FILE:script-core>" "${CMAKE_CURRENT_SOURCE_DIR}/resources/scripts" 
)
