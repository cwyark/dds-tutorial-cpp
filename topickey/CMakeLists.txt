cmake_minimum_required (VERSION 3.0)
project(topickey)

include_directories (
  ${PROJECT_SOURCE_DIR}
  ${OpenSplice_INCLUDE_DIRS}
)

set (IDL_FILES TopicKey.idl)
set (DATAMODEL topickey)

OpenSplice_IDLGEN_FILES(${IDL_FILES} ${DATAMODEL})

add_definitions (
  ${OpenSplice_DEFINITIONS}
  ${DEFINITIONS}
)

set (SOURCES
  topickey.cpp
)

set (EXE 
  topickey_test
)

add_executable (${EXE} ${SOURCES})

target_link_libraries (${EXE}
  ${OpenSplice_LIBRARIES}
  ${DATAMODEL}
)
