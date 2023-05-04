cmake_minimum_required (VERSION 3.8)

project ("led_server")

add_executable(${PROJECT_NAME} "main.cpp" "lib/LedDll.cpp")
set_target_properties(${PROJECT_NAME}
  PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/bin)

install (TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/lib/dll/"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)