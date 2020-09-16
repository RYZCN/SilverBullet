add_library(net STATIC)
target_sources(net PRIVATE webserver.cc )
target_include_directories(net PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(net PUBLIC util base)