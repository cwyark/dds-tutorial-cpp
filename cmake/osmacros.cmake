IF (WIN32)
  SET  (DEFINITIONS "/DSIMD_NOSTARTOSPL=0")
ELSE (WIN32)
  SET (DEFINITIONS "-DNDEBUG -g -O2 -std=c++11  -pipe -Wall")
ENDIF (WIN32)
