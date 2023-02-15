set(this Sandbox)
ucm_add_dirs(source assets TO sandbox_sources RECURSIVE ADDITIONAL_EXT scene glsl png)
add_executable(${this} ${sandbox_sources})
target_link_libraries(${this} PRIVATE Engine)
set_target_properties(${this} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${this} PROPERTIES FOLDER apps)

add_dependencies(${this} ScriptCore)

add_custom_command(TARGET ${this} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    "$<TARGET_FILE:ScriptCore>" "${CMAKE_CURRENT_SOURCE_DIR}/resources/scripts/ScriptCore.dll" 
)