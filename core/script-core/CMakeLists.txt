set(this script-core)

project(${this} VERSION 0.1.0 LANGUAGES CSharp)

ucm_add_dirs(source TO sources RECURSIVE ADDITIONAL_EXT cs)

add_library(${this} SHARED ${sources})

set_target_properties(${this} PROPERTIES FOLDER core)