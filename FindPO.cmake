find_path(PO_INCLUDE_DIR
    NAMES ProgramOptions.hxx
    PATHS ${PO_ROOT_DIR}
    PATH_SUFFIXES include
)

mark_as_advanced(PO_FOUND PO_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PO
    REQUIRED_VARS PO_INCLUDE_DIR
)

if(PO_FOUND)
    set(PO_INCLUDE_DIRS ${PO_INCLUDE_DIR})
endif()

if(PO_FOUND AND NOT TARGET PO::PO)
    add_library(PO::PO INTERFACE IMPORTED)
    set_target_properties(PO::PO PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${PO_INCLUDE_DIR}"
    )
endif()