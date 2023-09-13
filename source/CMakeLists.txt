cmake_minimum_required(VERSION 3.8)





file(GLOB_RECURSE source_cpp "./*.cpp")

file(GLOB_RECURSE nonCmake_cpp "${CMAKE_SOURCE_DIR}/thirdpart/nonCmake/*.cpp")
if(WIN32)
    list(APPEND source_cpp "${CMAKE_SOURCE_DIR}/lib/LedDll.cpp")
endif()


add_executable(${PROJECT_NAME} ${source_cpp})

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/lib
    ${CPP_BASE64_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/thirdpart/include
    ${CMAKE_SOURCE_DIR}/thirdpart/include/spdlog
    ${CMAKE_SOURCE_DIR}/thirdpart/thirdpart/nonCmake
    )
target_link_libraries(${PROJECT_NAME} PRIVATE 
      
    )


set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/bin)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/lib/dll/"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/webPage/"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/webPage/")



    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin)