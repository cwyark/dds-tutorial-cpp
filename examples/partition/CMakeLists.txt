cmake_minimum_required (VERSION 3.0)
project(partition)

include_directories (
  ${PROJECT_SOURCE_DIR}
  ${OpenSplice_INCLUDE_DIRS}
)

set (IDL_FILES BlockData.idl)
set (DATAMODEL blockdata)

OpenSplice_IDLGEN_FILES(${IDL_FILES} ${DATAMODEL})

add_definitions (
 ${OpenSplice_DEFINITIONS}
 ${DEFINITIONS}
)

# Define helloworld_pub 
set (PUB_SOURCES
  floor_pub.cpp
)
set (PUB_EXE
  floor_pub
)
add_executable (${PUB_EXE} ${PUB_SOURCES})
target_link_libraries (${PUB_EXE}
  ${OpenSplice_LIBRARIES}
  ${DATAMODEL}
)

# Define helloworld_sub
set (SUB_SOURCES
  floor_sub.cpp
)
set (SUB_EXE
  floor_sub
)
add_executable (${SUB_EXE} ${SUB_SOURCES})
target_link_libraries (${SUB_EXE}
  ${OpenSplice_LIBRARIES}
  ${DATAMODEL}
)
