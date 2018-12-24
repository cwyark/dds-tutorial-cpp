cmake_minimum_required (VERSION 3.0)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_MACOSX_RPATH 1)
include (MacroOpenSplice)
include (osmacros)

find_package (OpenSplice REQUIRED)
find_package (Log4cpp REQUIRED)

add_subdirectory(examples)
add_subdirectory(tools)

# Debug use
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
