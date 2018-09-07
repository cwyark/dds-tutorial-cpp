cmake_minimum_required (VERSION 3.0)
project(hello-world)

include_directories (
  ${PROJECT_SOURCE_DIR}
  ${OpenSplice_INCLUDE_DIRS}
)

set (IDL_FILES HelloWorld.idl)
set (DATAMODEL helloworld)

OpenSplice_IDLGEN_FILES(${IDL_FILES} ${DATAMODEL})

add_definitions (
 ${OpenSplice_DEFINITIONS}
 ${DEFINITIONS}
)

# Define helloworld_pub 
set (PUB_SOURCES
  helloworld_pub.cpp
)
set (PUB_EXE
  helloworld_pub
)
add_executable (${PUB_EXE} ${PUB_SOURCES})
target_link_libraries (${PUB_EXE}
  ${OpenSplice_LIBRARIES}
  ${DATAMODEL}
)

# Define helloworld_sub
set (SUB_SOURCES
  helloworld_sub.cpp
)
set (SUB_EXE
  helloworld_sub
)
add_executable (${SUB_EXE} ${SUB_SOURCES})
target_link_libraries (${SUB_EXE}
  ${OpenSplice_LIBRARIES}
  ${DATAMODEL}
)
