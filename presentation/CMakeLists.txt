cmake_minimum_required (VERSION 3.0)
project(presentation)

include_directories (
  ${PROJECT_SOURCE_DIR}
  ${OpenSplice_INCLUDE_DIRS}
)

set (IDL_FILES PresentationData.idl)
set (DATAMODEL presentation)

OpenSplice_IDLGEN_FILES(${IDL_FILES} ${DATAMODEL})

add_definitions (
 ${OpenSplice_DEFINITIONS}
 ${DEFINITIONS}
)

# Define helloworld_pub 
set (PRESENT_PUB_SOURCES
  presentation_pub.cpp
)
set (PRESENT_PUB_EXE
  presentation_pub
)
add_executable (${PRESENT_PUB_EXE} ${PRESENT_PUB_SOURCES})
target_link_libraries (${PRESENT_PUB_EXE}
  ${OpenSplice_LIBRARIES}
  ${DATAMODEL}
)

# Define helloworld_sub
set (PRESENT_SUB_SOURCES
  presentation_sub.cpp
)
set (PRESENT_SUB_EXE
  presentation_sub
)
add_executable (${PRESENT_SUB_EXE} ${PRESENT_SUB_SOURCES})
target_link_libraries (${PRESENT_SUB_EXE}
  ${OpenSplice_LIBRARIES}
  ${DATAMODEL}
)
