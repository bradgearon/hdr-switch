
project(tray)

include_directories(
  ..
  ../core/include
)

file(GLOB SRC_FILES
  *.cpp
)

add_executable(
  ${PROJECT_NAME}
  ${SRC_FILES}
)

target_link_libraries(${PROJECT_NAME} 
  core
)

set(LIBRARY_OUTPUT_PATH ../output)
