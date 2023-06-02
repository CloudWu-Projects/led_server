cmake_minimum_required(VERSION 3.8)

find_package(spdlog CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_path(CPP_BASE64_INCLUDE_DIRS "cpp-base64/base64.cpp")
find_package(libhv CONFIG REQUIRED)

file(GLOB_RECURSE source_cpp "./*.cpp")

if(WIN32)
    list(APPEND source_cpp "${CMAKE_SOURCE_DIR}/lib/LedDll.cpp")
endif()

list(APPEND source_cpp "${CPP_BASE64_INCLUDE_DIRS}/cpp-base64/base64.cpp")

add_executable(${PROJECT_NAME} ${source_cpp})

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/lib
    ${CPP_BASE64_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE 
    spdlog::spdlog_header_only    
    tinyxml2::tinyxml2)

if(UNIX)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/lib/dll/libledplayer7.so" hv_static)
    else()
    target_link_libraries(${PROJECT_NAME} PUBLIC  hv)
endif()

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