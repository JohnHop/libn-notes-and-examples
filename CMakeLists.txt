cmake_minimum_required(VERSION 3.5.0)
project(learn_libnl VERSION 0.1.0 LANGUAGES C CXX)


add_executable(nl80211-list
  nl80211-list.cpp 
)
target_include_directories(nl80211-list
  PUBLIC /usr/include/libnl3/
)
target_link_libraries(nl80211-list
  nl-3 
  nl-route-3
  nl-genl-3
)
target_compile_features(nl80211-list PUBLIC cxx_std_23)


add_executable(netdev-list netdev-list.cpp)
target_include_directories(netdev-list PUBLIC /usr/include/libnl3/)
target_link_libraries(netdev-list
  nl-3 
  nl-route-3
  nl-genl-3
)
target_compile_features(netdev-list PUBLIC cxx_std_23)


add_executable(monitor-mode monitor-mode.cpp)
target_include_directories(monitor-mode PUBLIC /usr/include/libnl3/)
target_link_libraries(monitor-mode
  nl-3 
  nl-route-3
  nl-genl-3
)
target_compile_features(monitor-mode PUBLIC cxx_std_23)


add_executable(put-link-updown put-link-updown.cpp)
target_include_directories(put-link-updown PUBLIC /usr/include/libnl3/)
target_link_libraries(put-link-updown
  nl-3 
  nl-route-3
  nl-genl-3
)
target_compile_features(put-link-updown PUBLIC cxx_std_23)


add_executable(remove-netdev remove-netdev.cpp)
target_include_directories(remove-netdev PUBLIC /usr/include/libnl3/)
target_link_libraries(remove-netdev
  nl-3 
  nl-route-3
  nl-genl-3
)
target_compile_features(remove-netdev PUBLIC cxx_std_23)

add_executable(set_channel set_channel.cpp)
target_include_directories(set_channel PUBLIC /usr/include/libnl3/)
target_link_libraries(set_channel
  nl-3 
  nl-route-3
  nl-genl-3
)
target_compile_features(set_channel PUBLIC cxx_std_23)