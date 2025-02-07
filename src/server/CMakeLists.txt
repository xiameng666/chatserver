# ʹ�� pkg-config ���� MySQL
find_package(PkgConfig REQUIRED)
pkg_search_module(MYSQL REQUIRED mysqlclient)

# ����SRC_LIST������������Ŀ¼������Դ�ļ�
set(SRC_LIST 
    chatserver.cpp
    chatservice.cpp
    usermodel.cpp
    main.cpp
    db/db.cpp
)

# ���� RPATH
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "/usr/lib/x86_64-linux-gnu")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# ָ�����ɿ�ִ���ļ�
add_executable(ChatServer ${SRC_LIST})

# ָ��ͷ�ļ�����·��
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
include_directories(${MYSQL_INCLUDE_DIRS})
include_directories(/usr/include/mysql)  # ��ȷ���� MySQL ͷ�ļ�·��

# ��������
target_link_libraries(ChatServer 
    muduo_net 
    muduo_base 
    mysqlclient
    pthread
)