set(this glm)

ucm_add_dirs(glm TO sources)
ucm_add_files(util/glm.natvis TO sources)

add_library(${this} INTERFACE ${sources})
target_include_directories(${this} INTERFACE glm)
set_target_properties(${this} PROPERTIES FOLDER extern)    
