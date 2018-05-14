
include_directories (
    ../../3rdparty/azmq
    )

foreach (
        test_name IN ITEMS
        VALUES_TEST
        PROCESS_TEST
        DELAYED_TEST
        DELAYED_VALS_TEST
        FILTER_TEST
        SLICE_TEST
        MERGE_TEST
        TCP_TEST
        ZMQ_SUB_TEST
        ZMQ_PUB_TEST
        ZMQ_PUBSUB_TEST
        )

    message("This is a test: ${test_name}")

    add_executable (
        "voy_test_${test_name}"
        tests_simple.cpp

        engine/asio/service.cpp
        )

    target_compile_definitions (
        "voy_test_${test_name}"
        PUBLIC "-D${test_name}"
        )

    target_link_libraries (
        "voy_test_${test_name}"
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_REGEX_LIBRARY}
        ${ZMQ_LIBRARIES}
        -pthread
        )

endforeach()

# add_executable (
#     voy_test
#     main.cpp
#
#     engine/asio/service.cpp
#     )
