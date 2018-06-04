find_path(PMC_INCLUDE_DIR
    NAMES pmc/pmc.h
    PATHS ${PMC_ROOT_DIR}
    PATH_SUFFIXES include
)

find_library(PMC_LIBRARY
    NAMES pmc
    PATHS ${PMC_ROOT_DIR}
    PATH_SUFFIXES lib lib64
)

mark_as_advanced(PMC_FOUND PMC_INCLUDE_DIR PMC_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PMC
    REQUIRED_VARS PMC_INCLUDE_DIR PMC_LIBRARY
)

if(PMC_FOUND)
    set(PMC_INCLUDE_DIRS ${PMC_INCLUDE_DIR})
    set(PMC_LIBRARIES ${PMC_LIBRARY})
endif()

if(PMC_FOUND AND NOT TARGET Pmc::pmc)
    add_library(Pmc::pmc INTERFACE IMPORTED)
    set_target_properties(Pmc::pmc PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${PMC_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "${PMC_LIBRARY}"
    )
endif()