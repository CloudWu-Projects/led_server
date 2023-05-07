cmake_minimum_required (VERSION 3.8)


add_executable(tt t.cpp )

add_library(xHook SHARED IMPORTED)

set_target_properties(xHook PROPERTIES IMPORTED_LOCATION
  ${CMAKE_SOURCE_DIR}/lib/dll/ledplayer7.so)

set_target_properties(tt
  PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
target_include_directories(tt PUBLIC ${CMAKE_SOURCE_DIR}/lib  ${CPP_BASE64_INCLUDE_DIRS})
target_link_libraries(tt PRIVATE httplib::httplib 
                                            spdlog::spdlog_header_only
                                          xHook
                                            tinyxml2::tinyxml2)
