# - Find Log4cpp
# Find the native Log4cpp includes and library
#
#  log4cpp_INCLUDE_DIR - where to find header files, etc.
#  log4cpp_LIBRARIES   - List of libraries when using log4cpp.
#  log4cpp_FOUND       - True if log4cpp found.


include(ExternalProject)

ExternalProject_Get_Property(log4cpp install_dir)

set(log4cpp_INCLUDE_DIR ${install_dir}/include)
set(log4cpp_LIBRARY ${install_dir}/lib)

if (log4cpp_INCLUDE_DIR AND log4cpp_LIBRARY)
  set(log4cpp_FOUND TRUE)
  set(log4cpp_LIBRARIES ${log4cpp_LIBRARY} CACHE INTERNAL "" FORCE)
  set(log4cpp_INCLUDE_DIRS ${log4cpp_INCLUDE_DIR} CACHE INTERNAL "" FORCE)
else ()
  set(log4cpp_FOUND FALSE CACHE INTERNAL "" FORCE)
  set(log4cpp_LIBRARY "" CACHE INTERNAL "" FORCE)
  set(log4cpp_LIBRARIES "" CACHE INTERNAL "" FORCE)
  set(log4cpp_INCLUDE_DIR "" CACHE INTERNAL "" FORCE)
  set(log4cpp_INCLUDE_DIRS "" CACHE INTERNAL "" FORCE)
endif ()

if (log4cpp_FOUND)
  message(STATUS "Found log4cpp: ${log4cpp_LIBRARIES}")
else ()
  if (log4cpp_FIND_REQUIRED)
    message(STATUS "Looked for log4cpp libraries named log4cpp.")
    message(FATAL_ERROR "Could NOT find log4cpp library")
  endif ()
endif ()

mark_as_advanced(
  log4cpp_LIBRARIES
  log4cpp_INCLUDE_DIRS
)
