
message ("Voy source dir: ${VOY_SOURCE_DIR}")
message ("Voy sources:    ${VOY_SOURCES}")

# Simple tests

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
        ASSOCIATIVITY_TEST
        )

    message("This is a test: ${test_name}")

    add_executable (
        "voy_test_${test_name}"
        ${VOY_SOURCES}
        tests_simple.cpp
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

# Multiprocess test

foreach (
        test_name IN ITEMS
        TEST_FRONTEND
        TEST_BACKEND_1
        TEST_BACKEND_2
        )

    message("This is a multiprocess test: ${test_name}")

    add_executable (
        "voy_test_multiprocess_${test_name}"
        ${VOY_SOURCES}
        tests_multiprocess.cpp
        )

    target_compile_definitions (
        "voy_test_multiprocess_${test_name}"
        PUBLIC "-D${test_name}"
        )

    target_link_libraries (
        "voy_test_multiprocess_${test_name}"
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_REGEX_LIBRARY}
        ${ZMQ_LIBRARIES}
        -pthread
        )

endforeach()


# Separate-thread test

add_executable (
    "voy_test_separate_thread"
    ${VOY_SOURCES}
    tests_thread.cpp
    )

target_link_libraries (
    "voy_test_separate_thread"
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${ZMQ_LIBRARIES}
    -pthread
    )

