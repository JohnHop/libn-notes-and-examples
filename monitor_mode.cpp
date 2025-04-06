#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <linux/nl80211.h>
#include <net/if.h>

#include <cstdlib>
#include <iostream>
#include <cstring>
#include <print>


/**
 * @brief
 * Set wireless network card type to managed or monitor.
 */

void print_usage()
{
  std::println("Usage: ./monitor-mode <wireless interface name> <monitor|managed>");
  std::exit(EXIT_SUCCESS);
}


int main(int argc, char const* argv[])
{
  std::string ifname;
  bool action;  // true: monitor; false: managed

  // check usage
  if(argc != 3) {
    print_usage();
  }
  else {
    ifname = argv[1];

    if(std::strcmp(argv[2], "monitor") == 0) {
      action = true;
    }
    else if(std::strcmp(argv[2], "managed") == 0) {
      action = false;
    }
    else {
      print_usage();
    }
  }

  struct nl_sock* nlsock = nl_socket_alloc();
  if(!nlsock) {
    std::println(stderr, "Error: Failed to allocate netlink socket.");
    std::exit(EXIT_FAILURE);
  }

  if(int ret; (ret = genl_connect(nlsock)) < 0) {
    std::println(stderr, "Error: {}.", nl_geterror(ret));
    std::exit(EXIT_FAILURE);
  }

  int nl80211_family_id = genl_ctrl_resolve(nlsock, "nl80211");
  if(nl80211_family_id < 0) {
    std::println(stderr, "Error: {}.", nl_geterror(nl80211_family_id));
    std::exit(EXIT_FAILURE);
  }

  int ifindex;
  if((ifindex = if_nametoindex(ifname.c_str())) == 0) {
    std::println(stderr, "Error: {}.", std::strerror(errno));
    std::exit(EXIT_FAILURE);
  }

  struct nl_msg* msg = nlmsg_alloc();
  if(!msg) {
    std::println(stderr, "Error: Failed to allocate netlink message.");
    std::exit(EXIT_FAILURE);
  }

  // Costruzione del messaggio per creare una nuova interfaccia
  if(!genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, nl80211_family_id, 0, 0, NL80211_CMD_SET_INTERFACE, 0)) {
    std::println(stderr, "Error in genlmsg_put().");
    std::exit(EXIT_FAILURE);
  }

  nla_put_u32(msg, NL80211_ATTR_IFINDEX, ifindex);
  // nla_put_string(msg, NL80211_ATTR_IFNAME, "wlp0s20f3mon");
  nla_put_u32(msg, NL80211_ATTR_IFTYPE, action ? NL80211_IFTYPE_MONITOR : NL80211_IFTYPE_STATION);

  // Invio del messaggio
  if(int ret; (ret = nl_send_auto(nlsock, msg)) < 0) {
    std::println(stderr, "Error: {}.", nl_geterror(ret));
    std::exit(EXIT_FAILURE);
  }

  if(int ret; (ret = nl_recvmsgs_default(nlsock)) < 0) {
    std::println(stderr, "Error: {}.", nl_geterror(ret));
    std::exit(EXIT_FAILURE);
  }

  // Cleaning
  nlmsg_free(msg);
  nl_socket_free(nlsock);

  return EXIT_SUCCESS;
}