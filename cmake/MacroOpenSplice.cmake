##############################################################################
# OpenSplice_IDLGEN(idlfilename)
#
# Macro to generate OpenSplice DDS sources from a given idl file with the
# data structures.
# You must include the extension .idl in the name of the data file.
#
##############################################################################
# Courtersy of Ivan Galvez Junquera <ivgalvez@gmail.com>
##############################################################################


# Macro to create a list with all the generated source files for a given .idl filename

MACRO (DEFINE_OpenSplice_SOURCES idlfilename)
	SET(outsources)
	GET_FILENAME_COMPONENT(it ${idlfilename} ABSOLUTE)
	GET_FILENAME_COMPONENT(nfile ${idlfilename} NAME_WE)
	SET(outsources ${outsources} gen/${nfile}.cpp gen/${nfile}.h)
  SET(outsources ${outsources} gen/${nfile}Dcps.cpp gen/${nfile}Dcps.h)
	SET(outsources ${outsources} gen/${nfile}SplDcps.cpp gen/${nfile}SplDcps.h)
	SET(outsources ${outsources} gen/ccpp_${nfile}.h)
ENDMACRO(DEFINE_OpenSplice_SOURCES)

MACRO (OpenSplice_IDLGEN idlfilename)
	GET_FILENAME_COMPONENT(it ${idlfilename} ABSOLUTE)
	GET_FILENAME_COMPONENT(idlfilename ${idlfilename} NAME)
	DEFINE_OpenSplice_SOURCES(${ARGV})
	ADD_CUSTOM_COMMAND (
		OUTPUT ${outsources}
		COMMAND ${OpenSplice_IDLGEN_BINARY}
    ARGS  -l cpp -d gen ${PROJECT_SOURCE_DIR}/${idlfilename}
		DEPENDS ${it}
	)
ENDMACRO (OpenSplice_IDLGEN)

MACRO (OpenSplice_IDLGEN_FILES _targets target_name)
foreach(idl ${_targets})
  OpenSplice_IDLGEN (${idl})
  string (REGEX REPLACE "\(.*\).idl" "./gen/\\1.cpp" VARS_1 ${idl})
  string (REGEX REPLACE "\(.*\).idl" "./gen/\\1.h" VARS_2 ${idl})
  string (REGEX REPLACE "\(.*\).idl" "./gen/\\1Dcps.h" VARS_4 ${idl})
  string (REGEX REPLACE "\(.*\).idl" "./gen/\\1SplDcps.cpp" VARS_7 ${idl})
  string (REGEX REPLACE "\(.*\).idl" "./gen/\\1SplDcps.h" VARS_8 ${idl})
  string (REGEX REPLACE "\(.*\).idl" "./gen/ccpp_\\1.h" VARS_9 ${idl})
  set(OpenSplice_DATAMODEL ${OpenSplice_DATAMODEL} ${VARS_0} ${VARS_2} ${VARS_3} ${VARS_4} ${VARS_5} ${VARS_6} ${VARS_7} ${VARS_8} ${VARS_9})

  add_library (${target_name} SHARED ${OpenSplice_DATAMODEL})

  target_link_libraries (
    ${target_name}
    ${OpenSplice_LIBRARIES}
  )
endforeach(idl)
ENDMACRO (OpenSplice_IDLGEN_FILES)

add_definitions (
 ${OpenSplice_DEFINITIONS}
 ${DEFINITIONS}
)
