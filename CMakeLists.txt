cmake_minimum_required(VERSION 3.5)
project(Polysync C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(POLYSYNC_WARNINGS_AS_ERRORS OFF)
option(POLYSYNC_ENABLE_ASAN ON)

set(POLYSYNC_DIR ${CMAKE_SOURCE_DIR})
set(POLYSYNC_SOURCE_DIR ${POLYSYNC_DIR}/source)
set(POLYSYNC_INCLUDE_DIR ${POLYSYNC_DIR}/include)
set(POLYSYNC_LIBRARY_DIR ${POLYSYNC_DIR}/library)

set(CJSON_DIR ${POLYSYNC_LIBRARY_DIR}/cjson)
set(CJSON_INCLUDE_DIR ${CJSON_DIR})

set(CURL_DIR ${POLYSYNC_LIBRARY_DIR}/curl)
set(CURL_INCLUDE_DIR ${CURL_DIR}/include)

set(GIT2_DIR ${POLYSYNC_LIBRARY_DIR}/libgit2)
set(GIT2_INCLUDE_DIR ${GIT2_DIR}/include)

add_subdirectory(${CJSON_DIR})
add_subdirectory(${CURL_DIR})
add_subdirectory(${GIT2_DIR})

file(GLOB_RECURSE POLYSYNC_SOURCE ${POLYSYNC_SOURCE_DIR}/*.c)

add_executable(polysync ${POLYSYNC_SOURCE})

target_include_directories(polysync
    PRIVATE ${POLYSYNC_INCLUDE_DIR}
    PRIVATE ${CJSON_INCLUDE_DIR}
    PRIVATE ${CURL_INCLUDE_DIR}
    PRIVATE ${GIT2_INCLUDE_DIR}
)

target_link_libraries(polysync
    libcurl
    cjson
    git2
)

target_compile_options(polysync PRIVATE
    -Wall
    -Wextra
)

target_compile_definitions(polysync PRIVATE _DEFAULT_SOURCE)

if(POLYSYNC_WARNINGS_AS_ERRORS)
    target_compile_options(polysync PRIVATE -Werror)
endif()

if(POLYSYNC_ENABLE_ASAN)
    target_compile_options(polysync PRIVATE -fsanitize=address)
    target_link_options(polysync PRIVATE -fsanitize=address)
endif()
