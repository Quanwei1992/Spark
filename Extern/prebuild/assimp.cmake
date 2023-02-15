set(this assimp)


set(debug_dir  ${archive_dir}/assimp/lib/win64/debug)
set(release_dir  ${archive_dir}/assimp/lib/win64/release)

add_library(zlib STATIC IMPORTED)
set_target_properties(zlib PROPERTIES 
    IMPORTED_LOCATION ${release_dir}/zlibstatic.lib
    IMPORTED_LOCATION_DEBUG ${debug_dir}/zlibstaticd.lib
)

add_library(${this} STATIC IMPORTED GLOBAL)
target_include_directories(${this} INTERFACE ${archive_dir}/assimp/include)

set_target_properties(${this} PROPERTIES 
    IMPORTED_LOCATION ${release_dir}/assimp-vc143-mt.lib
    IMPORTED_LOCATION_DEBUG ${debug_dir}/assimp-vc143-mtd.lib
    INTERFACE_LINK_LIBRARIES zlib
)

