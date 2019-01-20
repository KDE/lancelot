# vim:set softtabstop=3 shiftwidth=3 tabstop=3 expandtab:

add_subdirectory (activities)
add_subdirectory (services)
add_subdirectory (baloo)

if (BLADE_RECOLL_SUPPORT)
    add_subdirectory (recoll)
endif ()

