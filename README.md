# Netlink Library (libnl) notes and examples

Website: https://www.infradead.org/~tgr/libnl/

| **Library**                              | **Developer's Guide**          | **API Reference** |
|------------------------------------------|--------------------------------|-------------------|
| **Core Library (libnl)**                 | [Core Library Developer's Guide](https://www.infradead.org/~tgr/libnl/doc/core.html) | [API Reference](https://www.infradead.org/~tgr/libnl/doc/api/group__core.html)     |
| **Routing Family (libnl-route)**         | [Core Library Developer's Guide](https://www.infradead.org/~tgr/libnl/doc/route.html) | [API Reference](https://www.infradead.org/~tgr/libnl/doc/api/group__rtnl.html)     |
| **Generic Netlink Library (libnl-genl)** |                                | [API Reference](https://www.infradead.org/~tgr/libnl/doc/api/group__genl.html)     |

## Installation, configuration and use

### Development libraries

Installation:

`sudo apt install libnl-3-dev libnl-route-3-dev libnl-genl-3-dev`

A `CMakeLists.txt` example

```cmake
add_executable(main 
  main.cpp
)
target_include_directories(main
  PUBLIC /usr/include/libnl3/
)
target_link_libraries(main
  nl-3 
  nl-route-3
  nl-genl-3
)
```

For debugging: you can start a program with the environment variable `NLDBG` set to a debugger level from 0 to 4 and print to `stderr` debugging messages, like this

```bash
NLDBG=2 ./main
```

Or set `NLDBG=debug` to print all Netlink messages exchanged.

### Utilities

`sudo apt install libnl-utils`

| Program                | Description
|------------------------|-------------------------------------------------------------------------------------------------------------------|
| `genl-ctrl-list`       | Print a list of all Generic Netlink families in the form `{GN_FAMILY_ID}`, `{GN_FAMILY_NAME}`, `{REGISTRERED_VERSION}` |
| `nl-route-list`        | TODO |
| `nl-link-list`         | Print a list of *links* (also called network device or *netdev*) on the system including the *index number* (starting from 1) and *device name* |
| `nl-link-name2ifindex` | Get the link index from his name |

## Fundamentals (libnl)

In a nutshell: socket handling, sending and receiving, message construction and parsing.

### Socket handling [[Doc](https://www.infradead.org/~tgr/libnl/doc/core.html#core_sockets), [API Ref](https://www.infradead.org/~tgr/libnl/doc/api/group__socket.html)]

First, you need to create a Netlink socket

```C++
#include <netlink/socket.h>

struct nl_sock* nlsock = nl_socket_alloc();
if(!nlsock) {
  std::println(stderr, "Error: Failed to allocate Netlink socket.");
  std::exit(EXIT_FAILURE);
}

nl_socket_free(nlsock);
```

Then, you can connect to any `AF_NETLINK` address with a specified protocol like `NETLINK_ROUTE` or `NETLINK_GENERIC`.

```C++
#include <netlink/socket.h>
#include <netlink/errno.h>

if(int ret; (ret = nl_connect(nlsock, Netlink_ROUTE)) < 0) {
  std::println(stderr, "Error: {}.", nl_geterror(ret));
  std::exit(EXIT_FAILURE);
}

nl_close(nlsock);
```

If you want to close the connection and then free the socket, you can directly call `nl_socket_free()`.

### Sending and receiving data [[Doc](https://www.infradead.org/~tgr/libnl/doc/core.html#_message_parsing_amp_construction), [API Ref](https://www.infradead.org/~tgr/libnl/doc/api/group__msg.html)]

`struct nl_msg` is the type of Netlink message. You allocate a message with `nlmsg_alloc()` and destroy it with `nlmsg_free()`. After setting up the message (command and attributes), usually you send a message with `nl_send_auto()` using the automatic message completion functionality. 

For trivial messages, you can use `nl_send_simple()` and avoid the message allocation. This function accepts some arguments like the type of message and flags.

After, to receive a message using the callback configured for the socket, call `nl_recvmsgs_default()`.

### Callbacks [[Doc](https://www.infradead.org/~tgr/libnl/doc/core.html#core_cb), [API Ref](https://www.infradead.org/~tgr/libnl/doc/api/group__cb.html)]

You had to use callbacks to handle incoming messages. A callback is a custom function of type `nl_recvmsg_msg_cb_t`, an alias for this signature

```C++
int fun(struct nl_msg* msg, void* arg)
```

Each socket is provided with a set of callback, also named callback configuration, which controls the socket behavior of the socket. 

The default callback is invoked when `nl_recvmsgs()` is called. It does nothing. To change this, you had to use [`nl_socket_modify_cb()`](https://www.infradead.org/~tgr/libnl/doc/api/group__socket.html#gaeee66d6edef118209c7e7f1e3d393448). For example

```C++
if(int ret; (ret = nl_socket_modify_cb(nlsock, NL_CB_VALID, NL_CB_CUSTOM, callback, nullptr)) < 0) {
  std::println(stderr, "Error: {}.", nl_geterror(ret));
  std::exit(EXIT_FAILURE);
}
```

where

- `NL_CB_VALID` is the callback type and means that it will handle valid message replies
- `NL_CB_CUSTOM` is the kind and means that the handler is specified by the user

### Cache System [[Doc](https://www.infradead.org/~tgr/libnl/doc/core.html#core_cache), [API Ref](https://www.infradead.org/~tgr/libnl/doc/api/group__cache__mngt.html)]

A cache (`struct nl_cache`) represent the state of an object in the kernel. Usually, you will deal with function like `nl_<obj name>_alloc_cache()` that, automatically, allocate a cache, send a specific message to the kernel, parse the response and fill the result into the cache.

For example, the `rtnl_link_alloc_cache()` get a list of links (or *netdev*s) in the cache as a sequence of `struct rtnl_link`.

You can check if the cache is empty with `nl_cache_is_empty()`; or get the number of elements with `nl_cache_nitems()`. Each element is a `struct nl_object`.

### Attributes [[Doc](https://www.infradead.org/~tgr/libnl/doc/core.html#core_attr), [API Reference](https://www.infradead.org/~tgr/libnl/doc/api/group__attr.html)]

Each attribute is stored in `struct nlattr` data type, made from two members: a length and a type. Usually, attributes are retrieved inside a callback, defining an array of `struct nlattr*` of a specific size and calling a function from the `nlmsg_parse()` family. This function will fill the array with pointers so that you can check the presence of an attribute simply with 

Types are defined in `Netlink/attr.h` and [here](https://www.infradead.org/~tgr/libnl/doc/api/group__attr.html#gadf764cbdea00d65edcd07bb9953ad2b7). How to read them properly? Unfortunately, I can't find a clear rule about that the type of each attribute. Anyway `iw` source code, I found some specified hint, but nothing more. For example

- for `NL80211_ATTR_WIPHY`, `NL80211_ATTR_IFINDEX`, `NL80211_ATTR_IFTYPE`, use `nla_get_u32()`
- for `NL80211_ATTR_IFNAME`, use `nla_get_string()`
- for `NL80211_ATTR_WDEV`, use `nla_get_u64()`.

## Routing Family Library (libnl-route)

TODO

## Generic Netlink Library (libnl-genl) [[API Ref](https://www.infradead.org/~tgr/libnl/doc/api/group__genl.html)]

**genl** is an extended version of the Netlink protocol. Indeed, there are some similarities with the core library.

After allocate the socket, you can connect directly and in a more semantic way to the Generic API subsystem with

```C++
#include <netlink/socket.h>
#include <netlink/genl/genl.h>

if(genl_connect(nlsock) < 0) {
  std::println(stderr, "Error: {}.", nl_geterror(ret));
  std::exit(EXIT_FAILURE);
}

nl_close(nlsock);
```

A simple way to send a generic Netlink header-only message is through `genl_send_simple()`. To receive the response, simply use `nl_recvmsgs_default()` and handle the result inside the callback function. Example

```C++
if(int ret; (ret = genl_send_simple(nlsock, gn_family_id, NL80211_CMD_GET_INTERFACE, 0, NLM_F_DUMP)) < 0) {
  std::println(stderr, "Error: {}.", nl_geterror(ret));
  std::exit(EXIT_FAILURE);
}
```

where

- *26* is the id of the nl80211 generic Netlink family
- `NL80211_CMD_GET_INTERFACE` is a nl80211 command to request an interface's configuration
- `NLM_F_DUMP` means `NLM_F_ROOT|NLM_F_MATCH` and means "don't stop at the first result". More info at [Netlink(7)](https://man7.org/linux/man-pages/man7/Netlink.7.html)

The response is a `struct nl_msg` consisting of a `struct nlmsghdr` and the payload. In this case, the payload is a `struct genlmsghdr` and an optional payload. The best way to handle the result is through the `genlmsg_parse()` function, like this

```C++
struct nlattr* attrs[NL80211_ATTR_MAX+1];

if(int ret; (ret = genlmsg_parse(nlmsg_hdr(msg), 0, attrs, NL80211_ATTR_MAX, nullptr)) < 0) {
  std::println(stderr, "Error: {}.", nl_geterror(ret));
  std::exit(EXIT_FAILURE);
}
```

where

- due to we sent a `NL80211_CMD_GET_INTERFACE` message to request an interface configuration, we need to allocate a `struct nlattr*` array of size equal to `NL80211_ATTR_MAX+1`: `NL80211_ATTR_MAX` (defined in `<linux/nl80211.h>`) is the max number of `nl80211_attrs` attributes defined and the +1 is for the "attribute after the last" (kind of *end* iterator in STL).
- `nlmsg_hdr(msg)` returns the message header (just a pointer cast)

The `genlmsg_parse()` function will parse the Generic Netlink message and fill the attribute array, so we can read some data from it. The fourth parameter is maximum attribute expected, so `NL80211_ATTR_MAX` is the last one from the `nl80211_attrs` enum defined in `<linux/nl80211.h>`.

### Submodule Controller (Resolver)

In the kernel there is a component called Controller that you can query in order to resolve Generic Netlink family names to their numeric identifiers. For example, with `genl_ctrl_resolve(nlsocket, "nl80211")` (from `<netlink/genl/ctrl.h>`) you can communicate directly with the kernel and query about the numeric family identifier from his name. Execute `genl-ctrl-list` from `libnl-utils` program to get the complete list of families.