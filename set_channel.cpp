#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <net/if.h>
#include <linux/nl80211.h>

#include <iostream>
#include <string>
#include <cstring>
#include <print>

#include "helper.hpp"


void print_usage()
{
  std::println("Usage: ./set_channel <wireless interface> <channel number>");
  std::exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[])
{
  std::string ifname;
  int channel;

  // Argument checks
  if(argc != 3) {
    print_usage();
  }
  else {
    ifname = argv[1];
    channel = std::stoi(argv[2]);
  }

  struct nl_sock* nlsock = nl_socket_alloc();
  if(!nlsock) {
    std::println(stderr, "Error: Failed to allocate netlink socket.");
    std::exit(EXIT_FAILURE);
  }

  if(int ret; ret = genl_connect(nlsock) < 0) {
    std::println(stderr, "Error: {}.", nl_geterror(ret));
    std::exit(EXIT_FAILURE);
  }

  int nl80211_family_id = genl_ctrl_resolve(nlsock, "nl80211");
  if(nl80211_family_id < 0) {
    std::println(stderr, "Error: {}.", nl_geterror(nl80211_family_id));
    std::exit(EXIT_FAILURE);
  }

  int ifindex = if_nametoindex(ifname.c_str());
  if(ifindex == 0) {
    std::println(stderr, "Error: {}.", strerror(errno));
    std::exit(EXIT_FAILURE);
  }

  struct nl_msg* msg = nlmsg_alloc();
  if(!msg) {
    std::println(stderr, "Error: {}.", std::strerror(errno));
    std::exit(EXIT_FAILURE);
  }

  // Set channel frequency
  if(!genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, nl80211_family_id, 0, 0, NL80211_CMD_SET_WIPHY, 0)) {
    std::println(stderr, "Error: genlmsg_put() failed");
    std::exit(EXIT_FAILURE);
  }

  nla_put_u32(msg, NL80211_ATTR_IFINDEX, ifindex);
  nla_put_u32(msg, NL80211_ATTR_WIPHY_FREQ, ieee80211_channel_to_frequency(channel));

  if(int ret; (ret = nl_send_auto(nlsock, msg)) < 0) {
    std::println(stderr, "Error: {}", nl_geterror(ret));
    std::exit(EXIT_FAILURE);
  }

  if(int ret; (ret = nl_recvmsgs_default(nlsock)) < 0) {
    std::println(stderr, "Error: {}.", nl_geterror(ret));
    std::exit(EXIT_FAILURE);
  }

  // Request actual channel number
  if(!genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, nl80211_family_id, 0, 0, NL80211_CMD_GET_WIPHY, 0)) {
    std::println(stderr, "Error: genlmsg_put() failed");
    std::exit(EXIT_FAILURE);
  }

  nla_put_u32(msg, NL80211_ATTR_IFINDEX, ifindex);
  nla_put_u32(msg, NL80211_ATTR_WIPHY_FREQ, ieee80211_channel_to_frequency(channel));

  if(int ret; (ret = nl_send_auto(nlsock, msg)) < 0) {
    std::println(stderr, "Error: {}", nl_geterror(ret));
    std::exit(EXIT_FAILURE);
  }

  if(int ret; (ret = nl_recvmsgs_default(nlsock)) < 0) {
    std::println(stderr, "Error: {}.", nl_geterror(ret));
    std::exit(EXIT_FAILURE);
  }

  // Cleaning
  nlmsg_free(msg);
  nl_socket_free(nlsock);
}