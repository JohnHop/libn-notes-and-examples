#include <netlink/netlink.h>
#include <netlink/msg.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <linux/nl80211.h>
#include <netlink/attr.h>

#include <print>
#include <iostream>
#include <cstdlib>

#include "helper.hpp"


/**
 * @brief
 * Prints a list of wireless device like `iw dev` call.
 */


/**
 * @brief Print a single nl80211 device info. This callback is invoked for each
 *        device found on the system.
 * 
 * @ref Based on `print_iface_handler()` function from *iw* source code.
 */
int callback(struct nl_msg* msg, void*)
{
  struct nlattr* attrs[NL80211_ATTR_MAX+1];

  if(int ret; (ret = genlmsg_parse(nlmsg_hdr(msg), 0, attrs, NL80211_ATTR_MAX, nullptr)) < 0) {
    std::println(stderr, "Error: {}.", nl_geterror(ret));
    std::exit(EXIT_FAILURE);
  }
  
  // only consider wireless interface with a valid index
  if(attrs[NL80211_ATTR_IFINDEX])
  {
    std::println("Interface {}", nla_get_string(attrs[NL80211_ATTR_IFNAME]));
    std::println("\tifindex {}", nla_get_u32(attrs[NL80211_ATTR_IFINDEX]));
    if(attrs[NL80211_ATTR_WDEV]) {
      std::println("\twdev 0x{:x}", nla_get_u64(attrs[NL80211_ATTR_WDEV]));
    }
    if(attrs[NL80211_ATTR_MAC]) {
      std::println("\taddr {}", mac_addr_n2str(nla_data(attrs[NL80211_ATTR_MAC])));
    }
    if(attrs[NL80211_ATTR_SSID]) {
      std::println("\tssid {}", get_ssid_escaped(nla_len(attrs[NL80211_ATTR_SSID]), nla_data(attrs[NL80211_ATTR_SSID])));
    }
    if(attrs[NL80211_ATTR_IFTYPE]) {
      std::println("\ttype {}", nl80211_iftype2string.at(nla_get_u32(attrs[NL80211_ATTR_IFTYPE])));
    }
    if(attrs[NL80211_ATTR_WIPHY]) {
      std::println("\twiphy {}", nla_get_u32(attrs[NL80211_ATTR_WIPHY]));
    }
    if(attrs[NL80211_ATTR_WIPHY_FREQ]) {
      auto freq{nla_get_u32(attrs[NL80211_ATTR_WIPHY_FREQ])};
      std::println("\tchannel {} ({} Mhz)", ieee80211_frequency_to_channel(freq), freq);
    }

    std::println();
  }

  return NL_OK;
}


int main(int argc, char* argv[])
{
  struct nl_sock* nlsock = nl_socket_alloc();
  if(!nlsock) {
    std::println(stderr, "Error: Failed to allocate netlink socket.");
    std::exit(EXIT_FAILURE);
  }

  if(int ret; (ret = nl_socket_modify_cb(nlsock, NL_CB_VALID, NL_CB_CUSTOM, callback, nullptr)) < 0) {
    std::println(stderr, "Error: {}.", nl_geterror(ret));
    std::exit(EXIT_FAILURE);
  }

  // connect
  if(int ret; (ret = genl_connect(nlsock)) < 0) {
    std::println(stderr, "Error: {}.", nl_geterror(ret));
    std::exit(EXIT_FAILURE);
  }

  // get the Generic Netlink family identifier
  int nl80211_family_id = genl_ctrl_resolve(nlsock, "nl80211");
  if(nl80211_family_id < 0) {
    std::println(stderr, "Error: {}.", nl_geterror(nl80211_family_id));
    std::exit(EXIT_FAILURE);
  }

  // request
  if(int ret; (ret = genl_send_simple(nlsock, nl80211_family_id, NL80211_CMD_GET_INTERFACE, 0, NLM_F_DUMP)) < 0) {
    std::println(stderr, "Error: {}.", nl_geterror(ret));
    std::exit(EXIT_FAILURE);
  }

  // Instead, to obtain info from a single nl80211 device
  // struct nl_msg* msg = nlmsg_alloc();
  // genlmsg_put(msg, 0, 0, nl80211_family_id, 0, 0, NL80211_CMD_GET_INTERFACE, 0);
  // nla_put_u32(msg, NL80211_ATTR_IFINDEX, ifindex);
  // nl_send_auto_complete(nlsock, msg);

  // send message
  if(nl_recvmsgs_default(nlsock) < 0) {
    std::cerr << "Error calling nl_recvmsgs_default()" << std::endl;
    std::exit(EXIT_FAILURE);
  }  

  // Cleaning
  nl_socket_free(nlsock);

  return EXIT_SUCCESS;
}