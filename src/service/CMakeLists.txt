include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

project(service)

set(qtroot "S:/Qt/Qt5.12.9/5.12.9/msvc2017_64")
set(Qt5Core_DIR ${qtroot}/lib/cmake/Qt5Core)
set(Qt5_DIR ${qtroot}/lib/cmake/Qt5)
set(QT_QMAKE_EXECUTABLE ${qtroot}/bin/qmake.exe)

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
  ${NVAPI_PATH}
  ${ipc_path}/include
  ${ipc_path}/src
  ..
)

file(GLOB SRC_FILES
  ${ipc_path}/src/platform/*_win.cpp 
  ${ipc_path}/src/*.cpp
  *.cpp
)

add_executable(
  ${PROJECT_NAME} 
  ${SRC_FILES}
)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE __IPC_LIBRARY__
)

target_link_libraries(${PROJECT_NAME} 
  nvapi
  core
  #ipc
)

set(LIBRARY_OUTPUT_PATH ../output)
