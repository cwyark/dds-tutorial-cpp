cmake_minimum_required(VERSION 3.0)

include(ExternalProject)

ExternalProject_Add(log4cpp
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/log4cpp/
  CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/log4cpp/configure 
  BUILD_COMMAND make
  INSTALL_COMMAND make install
  EXCLUDE_FROM_ALL ON
)
