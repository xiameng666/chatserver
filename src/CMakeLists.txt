# ?? CMake ??????
cmake_minimum_required(VERSION 3.10)

# ??????
project(chat)

# ?? C++ ??
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ???????????
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)

# ?????????
include_directories(${PROJECT_SOURCE_DIR}/../include)
include_directories(${PROJECT_SOURCE_DIR}/../include/server)
include_directories(${PROJECT_SOURCE_DIR}/../include/server/db)
include_directories(${PROJECT_SOURCE_DIR}/../include/server/redis)
include_directories(${PROJECT_SOURCE_DIR}/../thirdparty)
# ������Ŀ¼
add_subdirectory(server)

# ??????????
install(TARGETS ChatServer
    RUNTIME DESTINATION bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                GROUP_READ GROUP_EXECUTE
                WORLD_READ WORLD_EXECUTE
)

