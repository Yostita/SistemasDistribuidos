cmake_minimum_required(VERSION 3.5)

project(http_server_mysql LANGUAGES CXX)

set(PROJECT_HTTPSERVER http_server)
set(PROJECT_PRUEBACLASE rpc_pruebaclase)
set(PROJECT_REMOTEFILE rpc_remotefile)
set(PROJECT_FILEMANAGER rpc_filemanager)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
 set(CMAKE_BUILD_TYPE Debug)


project(${PROJECT_PRUEBACLASE})
add_executable(${PROJECT_PRUEBACLASE} "main_slave.cpp"
    "pruebaclase_imp.cpp" "pruebaclase_imp.h"
    "pruebaclase.cpp" "pruebaclase.h"
    "mpi_manager.h" "mpi_manager.cpp")

project(${PROJECT_REMOTEFILE})
add_executable(${PROJECT_REMOTEFILE} "main_remotefile.cpp"
    "remotefile.h" "remotefile.cpp"
    "remotefile_imp.h" "remotefile_imp.cpp"
    "mpi_manager.h" "mpi_manager.cpp")


project(${PROJECT_FILEMANAGER})
add_executable(${PROJECT_FILEMANAGER}
    "main_fm.cpp" "filemanager.cpp" "filemanager.h"
    "filemanager_imp.cpp" "filemanager_imp.h"
    "mpi_manager.h" "mpi_manager.cpp")


project(${PROJECT_HTTPSERVER})
add_executable(${PROJECT_HTTPSERVER} "main.cpp"
    "httpserver.cpp" "httpserver.h"
    "pruebaclase_stub.cpp" "pruebaclase_stub.h"
    "remotefile_stub.h" "remotefile_stub.cpp"
    "mpi_manager.h" "mpi_manager.cpp"
    "utils.cpp" "utils.h" "filemanager_stub.h" "filemanager_stub.cpp")


find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
target_link_libraries(${PROJECT_HTTPSERVER} ${MPI_LIBRARIES})
target_link_libraries(${PROJECT_PRUEBACLASE} ${MPI_LIBRARIES})
target_link_libraries(${PROJECT_REMOTEFILE} ${MPI_LIBRARIES})
target_link_libraries(${PROJECT_FILEMANAGER} ${MPI_LIBRARIES})


link_directories(/usr/lib/x86_64-linux-gnu/)
target_link_libraries(http_server mysqlcppconn mysqlclient pthread z m rt atomic dl)
