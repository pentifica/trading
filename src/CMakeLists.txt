add_library(trading "")

configure_file(Version.h.in Version.h)

target_sources(trading
    PRIVATE
        Version.h
)

add_subdirectory(exch)

target_include_directories(
    trading
    PUBLIC
        "${PROJECT_BINARY_DIR}/../src/"
)

