set(CMAKE_CXX_STANDARD 20)

project(core)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(NVAPI_DL_PATH "g:/users/bradg/Downloads/R430-developer.zip")

set(NVAPI_PROJECT_NAME ext_nvapi)
set(NVAPI_PROJECT_PATH ${CMAKE_CURRENT_BINARY_DIR}/nvapi)
set(NVAPI_PATH ${NVAPI_PROJECT_PATH}/src/${NVAPI_PROJECT_NAME})

ExternalProject_Add(${NVAPI_PROJECT_NAME}
  URL ${NVAPI_DL_PATH}
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/nvapi
  CONFIGURE_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_COMMAND ""
)

add_library(nvapi STATIC IMPORTED)
target_compile_definitions(nvapi
  INTERFACE _WIN32
)


if(${CMAKE_GENERATOR_PLATFORM} STREQUAL "win32")
  set(nvlib x86/nvapi.lib)
else()
  set(nvlib amd64/nvapi64.lib)
endif()


set_property(TARGET nvapi PROPERTY IMPORTED_LOCATION ${NVAPI_PATH}/${nvlib})
add_dependencies(nvapi ext_nvapi)

include_directories(
  ${NVAPI_PATH}
)

file(GLOB SRC_FILES
  *.cpp
)

add_library(
  ${PROJECT_NAME} 
  ${SRC_FILES}
)

target_link_libraries(${PROJECT_NAME} 
  nvapi
)

install(TARGETS core EXPORT libCore)

install(EXPORT libCore NAMESPACE core::
        DESTINATION ../lib/cmake/core)

export(EXPORT libCore NAMESPACE core::)
