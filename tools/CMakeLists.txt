cmake_minimum_required(VERSION 3.0)

include(ExternalProject)

ExternalProject_Add(log4cpp
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/log4cpp/
  CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/log4cpp/autogen.sh 
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/log4cpp/configure --prefix=<INSTALL_DIR>
  BUILD_COMMAND ${MAKE}
  INSTALL_COMMAND ${MAKE} install
  EXCLUDE_FROM_ALL ON
)
