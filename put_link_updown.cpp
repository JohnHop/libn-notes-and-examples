#include <cstdlib>
#include <print>
#include <cstring>

#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <netlink/errno.h>  // for error codes and `nl_geterror()`
#include <netlink/route/route.h>
#include <netlink/route/link.h>
#include <linux/if_arp.h> // ?
#include <linux/if.h> // for `enum net_device_flags`

/**
 * @brief 
 * 
 */

void print_usage()
{
  std::println("Usage: ./put-link-updown <netdev> <up|down>");
  std::exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[])
{
  std::string ifname{};
  bool action;  // true = UP, false = DOWN

  // argument check
  if(argc != 3) {
    print_usage();
  }
  else 
  {
    ifname = argv[1];

    if(std::strcmp(argv[2], "up") == 0) {
      action = true;
    }
    else if(std::strcmp(argv[2], "down") == 0) {
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

  if(int ret = nl_connect(nlsock, NETLINK_ROUTE); ret < 0) {
    std::println(stderr, "Error: {}.", nl_geterror(ret));
    std::exit(EXIT_FAILURE);
  }

  // get the link list
  struct nl_cache* nlcache;
  if(int ret; (ret = rtnl_link_alloc_cache(nlsock, AF_UNSPEC, &nlcache)) < 0) {
    std::println(stderr, "Error: {}.", nl_geterror(ret));
    std::exit(EXIT_FAILURE);
  }

  // get the index from name
  int link_index = rtnl_link_name2i(nlcache, ifname.c_str());
  if(!link_index) {
    std::println(stderr, "Error: link {} not found.", ifname);
    std::exit(EXIT_FAILURE);
  }

  // get the link from his index
  struct rtnl_link* link = rtnl_link_get(nlcache, link_index);
  if(!link) {
    std::println(stderr, "Error: link with index {} not found.", link_index);
    std::exit(EXIT_FAILURE);
  }

  // read and modify link state
  struct rtnl_link* request = rtnl_link_alloc();
  // int flag = rtnl_link_str2flags("up");
  action ? rtnl_link_set_flags(request, IFF_UP) : rtnl_link_unset_flags(request, IFF_UP);
  auto link_state = rtnl_link_get_operstate(link);
  
  if(action != static_cast<bool>(link_state & IF_OPER_UP))  // logical XOR
  {
    if(int ret; (ret = rtnl_link_change(nlsock, link, request, 0)) < 0) {
      std::println(stderr, "Error: {}.", nl_geterror(ret));
      std::exit(EXIT_FAILURE);
    }
  }
  else {
    std::println("Link {} already {}", ifname, argv[2]);
  }
    
  // Cleaning
  rtnl_link_put(request);
  rtnl_link_put(link);
  nl_cache_free(nlcache);
  nl_socket_free(nlsock);

  return EXIT_SUCCESS;
}