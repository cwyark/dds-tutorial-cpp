cmake_minimum_required (VERSION 3.0)
project(chatroom)

include_directories (
  ${PROJECT_SOURCE_DIR}
  ${OpenSplice_INCLUDE_DIRS}
)

set (IDL_FILES ChatRoom.idl)
set (DATAMODEL chatroom)

OpenSplice_IDLGEN_FILES(${IDL_FILES} ${DATAMODEL})

add_definitions (
 ${OpenSplice_DEFINITIONS}
 ${DEFINITIONS}
)

# Define helloworld_pub 
set (USERLOAD_SOURCES
  userload.cpp
)
set (USERLOAD_EXE
  userload
)
add_executable (${USERLOAD_EXE} ${USERLOAD_SOURCES})
target_link_libraries (${USERLOAD_EXE}
  ${OpenSplice_LIBRARIES}
  ${DATAMODEL}
)


# Define helloworld_pub 
set (MESSAGEBOARD_SOURCES
  messageboard.cpp
)
set (MESSAGEBOARD_EXE
  messageboard
)
add_executable (${MESSAGEBOARD_EXE} ${MESSAGEBOARD_SOURCES})
target_link_libraries (${MESSAGEBOARD_EXE}
  ${OpenSplice_LIBRARIES}
  ${DATAMODEL}
)


set (CHATTER_SOURCES
  chatter.cpp
)
set (CHATTER_EXE
  chatter
)
add_executable (${CHATTER_EXE} ${CHATTER_SOURCES})
target_link_libraries (${CHATTER_EXE}
  ${OpenSplice_LIBRARIES}
  ${DATAMODEL}
)
