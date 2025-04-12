cmake_minimum_required(VERSION 3.5.0)
project(learn_libnl VERSION 0.1.0 LANGUAGES C CXX)


add_executable(nl80211_show nl80211_show.cpp)
target_include_directories(nl80211_show PUBLIC /usr/include/libnl3/)
target_link_libraries(nl80211_show
  nl-3 
  nl-route-3
  nl-genl-3
)
target_compile_features(nl80211_show PUBLIC cxx_std_23)


add_executable(netdev_list netdev_list.cpp)
target_include_directories(netdev_list PUBLIC /usr/include/libnl3/)
target_link_libraries(netdev_list
  nl-3 
  nl-route-3
  nl-genl-3
)
target_compile_features(netdev_list PUBLIC cxx_std_23)


add_executable(monitor_mode monitor_mode.cpp)
target_include_directories(monitor_mode PUBLIC /usr/include/libnl3/)
target_link_libraries(monitor_mode
  nl-3 
  nl-route-3
  nl-genl-3
)
target_compile_features(monitor_mode PUBLIC cxx_std_23)


add_executable(put_link_updown put_link_updown.cpp)
target_include_directories(put_link_updown PUBLIC /usr/include/libnl3/)
target_link_libraries(put_link_updown
  nl-3 
  nl-route-3
  nl-genl-3
)
target_compile_features(put_link_updown PUBLIC cxx_std_23)


add_executable(remove_netdev remove_netdev.cpp)
target_include_directories(remove_netdev PUBLIC /usr/include/libnl3/)
target_link_libraries(remove_netdev
  nl-3 
  nl-route-3
  nl-genl-3
)
target_compile_features(remove_netdev PUBLIC cxx_std_23)


add_executable(set_channel set_channel.cpp)
target_include_directories(set_channel PUBLIC /usr/include/libnl3/)
target_link_libraries(set_channel
  nl-3 
  nl-route-3
  nl-genl-3
)
target_compile_features(set_channel PUBLIC cxx_std_23)