
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

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/icon.ico" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

target_link_libraries(${PROJECT_NAME} 
  core
)
