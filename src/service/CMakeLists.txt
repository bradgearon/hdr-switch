set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

project(service)

ExternalProject_Add(project_ipc
  GIT_REPOSITORY https://github.com/mutouyun/cpp-ipc
  PREFIX ipc
  SOURCE_SUBDIR build
  CONFIGURE_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_COMMAND ""
)

ExternalProject_Get_Property(project_ipc install_dir)
set(ipc_path ${install_dir}/src/project_ipc)

add_library(ipc INTERFACE IMPORTED)
add_dependencies(ipc project_ipc)

message(warning ${ipc_path})

include_directories(
  ${ipc_path}/include
  ${ipc_path}/src
  ../core/include
  ..
)

file(GLOB SRC_FILES
  ${ipc_path}/src/platform/*_win.cpp 
  ${ipc_path}/src/*.cpp
  *.cpp
)

add_library(
  ${PROJECT_NAME} 
  ${SRC_FILES}
)


target_compile_definitions(${PROJECT_NAME}
  PRIVATE __IPC_LIBRARY__
)


target_link_libraries(${PROJECT_NAME} 
  core
)

install(TARGETS service EXPORT libService)

install(EXPORT libService NAMESPACE service::
        DESTINATION ../lib/cmake/service)

export(EXPORT libService NAMESPACE service::)
