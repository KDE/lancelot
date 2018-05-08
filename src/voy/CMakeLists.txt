
add_executable (
    voy_test
    main.cpp

    engine/asio/service.cpp
    )

target_link_libraries (
    voy_test
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    -pthread
    )
