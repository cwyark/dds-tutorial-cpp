cmake_minimum_required (VERSION 3.0)
project(lifecycle)

include_directories (
  ${PROJECT_SOURCE_DIR}
  ${OpenSplice_INCLUDE_DIRS}
)

set (IDL_FILES LifecycleData.idl)
set (DATAMODEL lifecycle)

OpenSplice_IDLGEN_FILES(${IDL_FILES} ${DATAMODEL})

add_definitions (
 ${OpenSplice_DEFINITIONS}
 ${DEFINITIONS}
)

set (PUB_SOURCES
  publisher.cpp
)
set (PUB_EXE
  publisher
)
add_executable (${PUB_EXE} ${PUB_SOURCES})
target_link_libraries (${PUB_EXE}
  ${OpenSplice_LIBRARIES}
  ${DATAMODEL}
)

set (SUB_SOURCES
  subscriber.cpp
)
set (SUB_EXE
  subscriber
)
add_executable (${SUB_EXE} ${SUB_SOURCES})
target_link_libraries (${SUB_EXE}
  ${OpenSplice_LIBRARIES}
  ${DATAMODEL}
)
