add_definitions(-DBOOST_LOG_DYN_LINK)
find_package(Boost 1.54 COMPONENTS log REQUIRED)
find_package(Threads)
add_definitions(-Wall -Wconversion)

add_executable (ircvpn main.cxx tun_device.cxx)
target_link_libraries(ircvpn ${Boost_LOG_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
