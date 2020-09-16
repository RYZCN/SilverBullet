add_library(util STATIC)
target_sources(util PRIVATE httpdata.cc threadpool.cc util.cc timer.cc eventloopthread.cc eventloop.cc eventloopthreadpool.cc epoller.cc channel.cc)
target_include_directories(util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(util PUBLIC base)

add_subdirectory(test)
