set(version v0.3.0)
set(url https://github.com/Quanwei1992/SparkEngineDependencies/releases/download/${version}/archive.7z)
set(archive_file_path "${CMAKE_CURRENT_SOURCE_DIR}/archive_${version}.7z")
set(archive_dir "${CMAKE_CURRENT_SOURCE_DIR}/archive_${version}")

if(NOT EXISTS "${archive_file_path}")
    message(STATUS "Downloading ${url}")
    file(DOWNLOAD ${url} ${archive_file_path}.tmp)
    file(RENAME ${archive_file_path}.tmp ${archive_file_path})
endif()


if(NOT EXISTS "${archive_dir}")
    message(STATUS "Extracting ${archive_file_path} to ${archive_dir}")
    file(ARCHIVE_EXTRACT INPUT  ${archive_file_path} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR} )
    file(RENAME ${CMAKE_CURRENT_SOURCE_DIR}/archive ${archive_dir} )
endif()


include(shaderc.cmake)
include(mono.cmake)
include(assimp.cmake)