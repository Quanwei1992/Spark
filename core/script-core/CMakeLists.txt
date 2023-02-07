set(this script-core)

project(${this} VERSION 0.1.0 LANGUAGES CSharp)

add_library(${this} SHARED source/main.cs)

set_target_properties(${this} PROPERTIES FOLDER core)