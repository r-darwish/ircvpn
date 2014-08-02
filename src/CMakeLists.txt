add_definitions(-DBOOST_LOG_DYN_LINK)
find_package(Boost 1.54 COMPONENTS log system REQUIRED)
find_package(Threads)
add_definitions(-Wall -Wconversion)

add_executable (ircvpn main.cpp irc_vpn.cpp tun_device.cpp irc_client.cpp)
target_link_libraries(ircvpn b64 ${Boost_LOG_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
