cmake_minimum_required (VERSION 3.0)
project(contentfilter)

include_directories (
  ${PROJECT_SOURCE_DIR}
  ${OpenSplice_INCLUDE_DIRS}
)

set (IDL_FILES ContentFilter.idl)
set (DATAMODEL contentfilter)

OpenSplice_IDLGEN_FILES(${IDL_FILES} ${DATAMODEL})

add_definitions (
  ${OpenSplice_DEFINITIONS}
  ${DEFINITIONS}
)

set (PUB_SOURCES
  contentfilter_pub.cpp
)

set (PUB_EXE 
  contentfilter_pub
)

add_executable (${PUB_EXE} ${PUB_SOURCES})

target_link_libraries (${PUB_EXE}
  ${OpenSplice_LIBRARIES}
  ${DATAMODEL}
)


set (SUB_SOURCES
  contentfilter_sub.cpp
)

set (SUB_EXE 
  contentfilter_sub
)

add_executable (${SUB_EXE} ${SUB_SOURCES})

target_link_libraries (${SUB_EXE}
  ${OpenSplice_LIBRARIES}
  ${DATAMODEL}
)
