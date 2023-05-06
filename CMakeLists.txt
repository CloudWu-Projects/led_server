cmake_minimum_required (VERSION 3.8)

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

project ("led_server")



find_package(httplib CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_path(CPP_BASE64_INCLUDE_DIRS "cpp-base64/base64.cpp")

file(GLOB_RECURSE source_cpp "./source/*.cpp")

add_executable(${PROJECT_NAME} ${source_cpp} "lib/LedDll.cpp" "${CPP_BASE64_INCLUDE_DIRS}/cpp-base64/base64.cpp")

set_target_properties(${PROJECT_NAME}
  PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib  ${CPP_BASE64_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE httplib::httplib 
                                            spdlog::spdlog_header_only
                                           
                                            tinyxml2::tinyxml2)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/bin)

install (TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/lib/dll/"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)