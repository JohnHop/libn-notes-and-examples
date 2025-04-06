#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <netlink/route/link.h>
#include <netlink/route/addr.h>

#include <cstddef>
#include <print>


/**
 * @brief
 * Print a list of network device available on the system in the format
 * {if index}: {if name}, {if mac address}.
 */

int main()
{
  struct nl_sock* nlsocket = nl_socket_alloc();
  if(!nlsocket) {
    std::println(stderr, "Error: Failed to allocate netlink socket.");
    std::exit(EXIT_FAILURE);
  }

  if(int ret; (ret = nl_connect(nlsocket, NETLINK_ROUTE)) < 0) {
    std::println(stderr, "Error: {}.", nl_geterror(ret));
    std::exit(EXIT_FAILURE);
  }

  // netdev list request
  struct nl_cache* nlcache;
  if(int ret; (ret = rtnl_link_alloc_cache(nlsocket, AF_UNSPEC, &nlcache)) < 0) {
    std::println(stderr, "Error: {}.", nl_geterror(ret));
    std::exit(EXIT_FAILURE);
  }

  // Result
  for(auto obj = nl_cache_get_first(nlcache); obj != nullptr; obj = nl_cache_get_next(obj)) 
  {
    struct rtnl_link* link = reinterpret_cast<rtnl_link*>(obj);
    char buff[18]{};

    std::println("{}: {}, {}", 
      rtnl_link_get_ifindex(link), 
      rtnl_link_get_name(link),
      nl_addr2str(rtnl_link_get_addr(link), buff, std::size(buff))
    );
  }

  // Cleaning
  nl_cache_free(nlcache);
  nl_socket_free(nlsocket);

  return EXIT_SUCCESS;
}