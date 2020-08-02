set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

project(settings)

ExternalProject_Add(winlamb
  GIT_REPOSITORY https://github.com/rodrigocfd/winlamb
  PREFIX winlamb
  SOURCE_SUBDIR build
  CONFIGURE_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_COMMAND ""
)

ExternalProject_Get_Property(winlamb install_dir)
set(winlamb_path ${install_dir}/src)

add_library(libWinlamb INTERFACE IMPORTED)
add_dependencies(libWinlamb winlamb)

include_directories(
  ${winlamb_path}
  ../core/include
  ..
)

file(GLOB SRC_FILES
  *.cpp
)

add_library(
  ${PROJECT_NAME} 
  ${SRC_FILES}
)

target_link_libraries(${PROJECT_NAME} 
  libWinlamb
)

install(TARGETS settings EXPORT libSettings)

install(EXPORT libSettings NAMESPACE settings::
        DESTINATION ../lib/cmake/settings)

export(EXPORT libSettings NAMESPACE settings::)
