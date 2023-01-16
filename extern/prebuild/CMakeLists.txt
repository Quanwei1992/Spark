
set(version v0.1.0)
set(url https://github.com/Quanwei1992/SparkEngineDependencies/releases/download/${version}/archive.7z)
set(archive_file_path "${CMAKE_CURRENT_SOURCE_DIR}/archive_${version}.7z")
set(archive_dir "${CMAKE_CURRENT_SOURCE_DIR}/archive_${version}")

if(NOT EXISTS "${archive_file_path}")
    message(STATUS "Download prebuild dependencies")
    file(DOWNLOAD ${url} ${archive_file_path})
endif()


if(NOT EXISTS "${archive_dir}")
    message(STATUS "extract ${archive_file_path} to ${archive_dir}")
    file(ARCHIVE_EXTRACT INPUT  ${archive_file_path} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR} )
    file(RENAME ${CMAKE_CURRENT_SOURCE_DIR}/archive ${archive_dir} )
endif()


# Shaderc
set(this shaderc)
add_library(${this} STATIC IMPORTED GLOBAL)
target_include_directories(${this} INTERFACE 
    $<$<CONFIG:Debug>:${archive_dir}/shaderc_Win64_Debug/include>
    $<$<CONFIG:Release>:${archive_dir}/shaderc_Win64_Release/include>
)
set_target_properties(${this} PROPERTIES IMPORTED_LOCATION_DEBUG ${archive_dir}/shaderc_Win64_Debug/lib/shaderc_combined.lib)
set_target_properties(${this} PROPERTIES IMPORTED_LOCATION_RELEASE ${archive_dir}/shaderc_Win64_Release/lib/shaderc_combined.lib)
set_target_properties(${this} PROPERTIES IMPORTED_LOCATION_MINSIZEREL ${archive_dir}/shaderc_Win64_Release/lib/shaderc_combined.lib)
set_target_properties(${this} PROPERTIES IMPORTED_LOCATION_RELWITHDEBINFO ${archive_dir}/shaderc_Win64_Release/lib/shaderc_combined.lib)