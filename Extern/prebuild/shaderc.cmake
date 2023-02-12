set(this shaderc)
add_library(${this} STATIC IMPORTED GLOBAL)
target_include_directories(${this} INTERFACE ${archive_dir}/shaderc/include)



set_target_properties(${this} PROPERTIES 
    IMPORTED_LOCATION ${archive_dir}/shaderc/lib/win64/release/shaderc_combined.lib
    IMPORTED_LOCATION_DEBUG ${archive_dir}/shaderc/lib/win64/debug/shaderc_combined.lib
)