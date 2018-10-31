cmake_minimum_required (VERSION 3.0)
project(coherent)

include_directories (
  ${PROJECT_SOURCE_DIR}
  ${OpenSplice_INCLUDE_DIRS}
)

set (IDL_FILES CoherentData.idl)
set (DATAMODEL coherent)

OpenSplice_IDLGEN_FILES(${IDL_FILES} ${DATAMODEL})

add_definitions (
 ${OpenSplice_DEFINITIONS}
 ${DEFINITIONS}
)

set (PUB_SOURCES
  publisher.cpp
)
set (PUB_EXE
  coherent_pub
)
#find_package (Threads REQUIRED)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable (${PUB_EXE} ${PUB_SOURCES})
target_link_libraries (${PUB_EXE}
  ${OpenSplice_LIBRARIES}
  ${DATAMODEL}
  #Threads::Threads
)

set (SUB_SOURCES
  subscriber.cpp
)
set (SUB_EXE
  coherent_sub
)
add_executable (${SUB_EXE} ${SUB_SOURCES})
target_link_libraries (${SUB_EXE}
  ${OpenSplice_LIBRARIES}
  ${DATAMODEL}
)
