cmake_minimum_required (VERSION 3.0)
project (dds-tutorial)

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${OpenSplice_INCLUDE_DIRS}
)

set (idls TempControl.idl)

OpenSplice_IDLGEN_FILES(${idls})

set (PUB_SOURCES
 tspub.cpp
 util.cpp
)
set (SUB_SOURCES
 tssub.cpp
 util.cpp
)

set (PUB_EXE tspub)
set (SUB_EXE tssub)
set (DATAMODEL datamodel)

add_executable (${PUB_EXE} ${PUB_SOURCES})
add_executable (${SUB_EXE} ${SUB_SOURCES})

add_library (${DATAMODEL} SHARED ${OpenSplice_DATAMODEL})

add_definitions (
 ${OpenSplice_DEFINITIONS}
 ${DEFINITIONS}
)

target_link_libraries (
  ${DATAMODEL} 
  ${OpenSplice_LIBRARIES}
)

target_link_libraries (${PUB_EXE}
 ${OpenSplice_LIBRARIES}
 ${DATAMODEL} 
)

target_link_libraries (${SUB_EXE}
 ${OpenSplice_LIBRARIES}
 ${DATAMODEL}
)
