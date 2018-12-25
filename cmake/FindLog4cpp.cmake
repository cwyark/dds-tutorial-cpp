# - Find Log4cpp
# Find the native Log4cpp includes and library
#
#  Log4cpp_INCLUDE_DIR - where to find header files, etc.
#  Log4cpp_LIBRARIES   - List of libraries when using Log4cpp.
#  Log4cpp_FOUND       - True if Log4cpp found.


find_path(Log4cpp_INCLUDE_DIR 
  NAMES Category.hh
  PATH_SUFFIXES log4cpp
  PATHS
    /opt/local/include
    /usr/local/include
    /usr/include
)

find_library(Log4cpp_LIBRARY
  NAMES log4cpp
  PATHS /usr/local/lib /opt/local/lib /usr/lib/x86_64-linux-gnu
)

if (Log4cpp_INCLUDE_DIR AND Log4cpp_LIBRARY)
  set(Log4cpp_FOUND TRUE)
  set(Log4cpp_LIBRARIES ${Log4cpp_LIBRARY} CACHE INTERNAL "" FORCE)
  set(Log4cpp_INCLUDE_DIRS ${Log4cpp_INCLUDE_DIR} CACHE INTERNAL "" FORCE)
else ()
  set(Log4cpp_FOUND FALSE CACHE INTERNAL "" FORCE)
  set(Log4cpp_LIBRARY "" CACHE INTERNAL "" FORCE)
  set(Log4cpp_LIBRARIES "" CACHE INTERNAL "" FORCE)
  set(Log4cpp_INCLUDE_DIR "" CACHE INTERNAL "" FORCE)
  set(Log4cpp_INCLUDE_DIRS "" CACHE INTERNAL "" FORCE)
endif ()

if (Log4cpp_FOUND)
  message(STATUS "Found Log4cpp: ${Log4cpp_LIBRARIES}")
else ()
  if (Log4cpp_FIND_REQUIRED)
    message(STATUS "Looked for Log4cpp libraries named Log4cpp.")
    message(FATAL_ERROR "Could NOT find Log4cpp library")
  endif ()
endif ()

mark_as_advanced(
  Log4cpp_LIBRARIES
  Log4cpp_INCLUDE_DIRS
)
