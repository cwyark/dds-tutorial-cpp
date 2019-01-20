cmake_minimum_required(VERSION 3.0)

set(target log4cpp)

include(ExternalProject)

find_program(MAKE make)

ExternalProject_Add(${target}
  BUILD_IN_SOURCE ON
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/log4cpp/
  CONFIGURE_COMMAND ./configure --prefix=<INSTALL_DIR>
  BUILD_COMMAND ${MAKE}
  INSTALL_COMMAND ${MAKE} install
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)
