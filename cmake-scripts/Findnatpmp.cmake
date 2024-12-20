
SET(LIBNATPMP_SEARCH_PATHS
    /usr/local/
    /usr
    /opt
)

FIND_PATH(LIBNATPMP_INCLUDE_DIR natpmp.h
    HINTS
    PATH_SUFFIXES include
    PATHS ${LIBNATPMP_SEARCH_PATHS}
)
FIND_LIBRARY(LIBNATPMP_LIBRARY natpmp
    HINTS
    PATH_SUFFIXES lib64 lib bin
    PATHS ${LIBNATPMP_SEARCH_PATHS}
)

IF (LIBNATPMP_INCLUDE_DIR AND LIBNATPMP_LIBRARY)
    SET(LIBNATPMP_FOUND TRUE)
ENDIF (LIBNATPMP_INCLUDE_DIR AND LIBNATPMP_LIBRARY)

IF (LIBNATPMP_FOUND)
    MESSAGE(STATUS "Found libnatpmp: ${LIBNATPMP_LIBRARY}")
ELSE (LIBNATPMP_FOUND)
    MESSAGE(WARNING "Could not find libnatpmp")
ENDIF (LIBNATPMP_FOUND)

mark_as_advanced(LIBNATPMP_INCLUDE_DIR LIBNATPMP_LIBRARY LIBNATPMP_SEARCH_PATHS)
