function(utils_add_lib target_name include_dir source_dir taregt_floder filter_pop)
    ucm_add_dirs(${include_dir} ${source_dir} TO sources RECURSIVE FILTER_POP ${filter_pop} )
    add_library(${target_name} STATIC ${sources})

    target_include_directories(${target_name} PUBLIC ${include_dir} )
    target_include_directories(${target_name} PRIVATE ${source_dir} )
    set_target_properties(${target_name} PROPERTIES FOLDER ${taregt_floder})    
endfunction(utils_add_lib)

function(utils_add_interface target_name include_dir taregt_floder filter_pop)
    ucm_add_dirs(${include_dir}  TO sources RECURSIVE FILTER_POP ${filter_pop})
    add_library(${target_name} INTERFACE ${sources})

    target_include_directories(${target_name} INTERFACE ${include_dir} )
    set_target_properties(${target_name} PROPERTIES FOLDER ${taregt_floder})    
    set_target_properties(${target_name} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endfunction(utils_add_interface)

function(utils_get_all_targets var)
    set(targets)
    utils_get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
    set(${var} ${targets} PARENT_SCOPE)
endfunction()

macro(utils_get_all_targets_recursive targets dir)
    get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirectories})
        utils_get_all_targets_recursive(${targets} ${subdir})
    endforeach()

    get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
    list(APPEND ${targets} ${current_targets})
endmacro()