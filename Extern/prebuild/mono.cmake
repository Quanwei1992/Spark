set(this mono)

add_library(${this} STATIC IMPORTED GLOBAL)
target_include_directories(${this} INTERFACE ${archive_dir}/mono/include)


set(win64_debug_lib_dir  ${archive_dir}/mono/lib/win64/debug)

set(mono_debug_libs 
    ${win64_debug_lib_dir}/libmono-static-sgen.lib
    ${win64_debug_lib_dir}/eglib.lib
    ${win64_debug_lib_dir}/libgcmonosgen.lib
    ${win64_debug_lib_dir}/libmonoruntime-sgen.lib
    ${win64_debug_lib_dir}/libmonoutils.lib
    ${win64_debug_lib_dir}/mono-2.0-sgen.lib
    ${win64_debug_lib_dir}/MonoPosixHelper.lib
)
set(win64_release_lib_dir  ${archive_dir}/mono/lib/win64/release)
set(mono_release_libs 
    ${win64_release_lib_dir}/libmono-static-sgen.lib
    ${win64_release_lib_dir}/eglib.lib
    ${win64_release_lib_dir}/libgcmonosgen.lib
    ${win64_release_lib_dir}/libmonoruntime-sgen.lib
    ${win64_release_lib_dir}/libmonoutils.lib
    ${win64_release_lib_dir}/mono-2.0-sgen.lib
    ${win64_release_lib_dir}/MonoPosixHelper.lib
)

set_target_properties(${this} PROPERTIES 
    IMPORTED_LOCATION ${mono_release_libs}
    IMPORTED_LOCATION_DEBUG ${mono_debug_libs}
)