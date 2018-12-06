find_path(AS_INCLUDE_DIR
    NAMES as/as_version.h
    PATHS ${AS_ROOT_DIR}
    PATH_SUFFIXES include
)

mark_as_advanced(AS_FOUND AS_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AS
    REQUIRED_VARS AS_INCLUDE_DIR
)

if(AS_FOUND)
    set(AS_INCLUDE_DIRS ${AS_INCLUDE_DIR})
endif()

if(AS_FOUND AND NOT TARGET AS::AS)
    add_library(AS::AS INTERFACE IMPORTED)
    set_target_properties(AS::AS PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${AS_INCLUDE_DIRS}"
    )
endif()