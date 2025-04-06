#include <netlink/netlink.h>
#include <netlink/route/route.h>
#include <netlink/route/link.h>

#include <cstdlib>
#include <iostream>
#include <string>


int main(int argc, char* argv[])
{
  std::string ifname;

  // Arguments check
  if(argc == 2) {
    ifname = argv[1];
  }
  else {
    ifname = "enp0s31f6";
  }


  struct nl_sock* nlsock = nl_socket_alloc();
  if(!nlsock) {
    std::cerr << "Error calling nl_socket_alloc()";
    std::exit(EXIT_FAILURE);
  }

  int ret;
  if((ret = nl_connect(nlsock, NETLINK_ROUTE)) < 0) {
    std::cerr << nl_geterror(ret) << std::endl;
    std::exit(EXIT_FAILURE);
  }

  struct nl_cache* nlcache;
  if((ret = rtnl_link_alloc_cache(nlsock, AF_UNSPEC, &nlcache)) < 0) {
    std::cerr << nl_geterror(ret) << std::endl;
    std::exit(EXIT_FAILURE);
  }

  struct rtnl_link* link = rtnl_link_get_by_name(nlcache, ifname.c_str());
  if(!link) {
    std::cerr << "Error: " << ifname << " not found" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  if((ret = rtnl_link_delete(nlsock, link)) < 0) {
    std::cerr << "Error: " << nl_geterror(ret) << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // Cleaning
  rtnl_link_put(link);
  nl_cache_free(nlcache);
  nl_socket_free(nlsock);

  return EXIT_SUCCESS;
}