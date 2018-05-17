
set (
    VOY_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PARENT_SCOPE
    )

set (
    VOY_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/engine/asio/service.cpp"
    PARENT_SCOPE
    )

message ("Voy source dir: ${VOY_SOURCE_DIR}")
message ("Voy sources:    ${VOY_SOURCES}")

