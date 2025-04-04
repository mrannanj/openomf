
SET(OPUSFILE_SEARCH_PATHS
    /usr/local/
    /usr
    /opt
)

FIND_PATH(OPUSFILE_INCLUDE_DIR opusfile.h
    HINTS ${OPUSFILE_ROOT}
    PATH_SUFFIXES opus include/opus include
    PATHS ${OPUSFILE_SEARCH_PATHS}
)
FIND_LIBRARY(OPUSFILE_LIBRARY opusfile
    HINTS ${OPUSFILE_ROOT}
    PATH_SUFFIXES lib64 lib bin
    PATHS ${OPUSFILE_SEARCH_PATHS}
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments
FIND_PACKAGE_HANDLE_STANDARD_ARGS(opusfile
    REQUIRED_VARS OPUSFILE_LIBRARY OPUSFILE_INCLUDE_DIR
)

mark_as_advanced(OPUSFILE_INCLUDE_DIR OPUSFILE_LIBRARY OPUSFILE_SEARCH_PATHS)
