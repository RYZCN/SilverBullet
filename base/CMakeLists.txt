add_library(base SHARED)
target_sources(base PRIVATE singleton.hpp asynclog.cc latch.hpp logger.cc logfile.cc logstream.cc)
target_include_directories(base PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(base PUBLIC pthread)