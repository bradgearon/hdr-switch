cmake_minimum_required(VERSION 3.10)

project(hdr-switch)

set(CMAKE_CXX_STANDARD 17)

set (qtroot "S:/Qt/Qt5.12.9/5.12.9/msvc2017_64")
set(Qt5Core_DIR ${qtroot}/lib/cmake/Qt5Core)
set(Qt5_DIR ${qtroot}/lib/cmake/Qt5)
set(QT_QMAKE_EXECUTABLE ${qtroot}/bin/qmake.exe)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(NVAPI_DL_PATH "g:/users/bradg/Downloads/R430-developer.zip")

set(NVAPI_PROJECT_NAME ext_nvapi)
set(NVAPI_PROJECT_PATH ${CMAKE_CURRENT_BINARY_DIR}/nvapi)
set(NVAPI_PATH ${NVAPI_PROJECT_PATH}/src/${NVAPI_PROJECT_NAME})

ExternalProject_Add( ${NVAPI_PROJECT_NAME}
  URL ${NVAPI_DL_PATH}
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/nvapi
  CONFIGURE_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_COMMAND ""
)

add_library(nvapi STATIC IMPORTED)
set_property(TARGET nvapi PROPERTY IMPORTED_LOCATION ${NVAPI_PATH}/amd64/nvapi64.lib)
add_dependencies(nvapi ext_nvapi)

ExternalProject_Add(project_ipc
  GIT_REPOSITORY https://github.com/mutouyun/cpp-ipc
  PREFIX ipc
  SOURCE_SUBDIR build
  CONFIGURE_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_COMMAND ""
)

ExternalProject_Get_Property(project_ipc install_dir)

add_library(ipc INTERFACE IMPORTED)
# add_library(ipc STATIC IMPORTED)
# set_property(TARGET ipc PROPERTY IMPORTED_LOCATION ipc/src/project_ipc)
add_dependencies(ipc project_ipc)

message(warning ${ipc_src_files})

include_directories(
  ${NVAPI_PATH}
  ipc/src/project_ipc/include
  ipc/src/project_ipc/src
  ../../src
)

file(GLOB SRC_FILES
  ipc/src/project_ipc/src/platform/*_win.cpp 
  ipc/src/project_ipc/src/*.cpp
  ../src/service/*.cpp
  ../src/*.cpp
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
  #ipc
)

set(LIBRARY_OUTPUT_PATH ../output)
