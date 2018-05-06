
add_executable (
    voy_test
    main.cpp
    )

target_link_libraries (
    voy_test
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    -pthread
    )
