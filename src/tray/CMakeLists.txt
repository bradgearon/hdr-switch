
project(hdr_switch_tray)

SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY 
  ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_GENERATOR_PLATFORM}/${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

include_directories(
  ..
  ../core/include
  ../service/include
)

file(GLOB SRC_FILES
  *.cpp
  main.rc
)

add_executable(
  ${PROJECT_NAME}
  ${SRC_FILES}
)

# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/icon.ico" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
# )

target_link_libraries(${PROJECT_NAME} 
  core
  service
)
