#if !defined(HELPER_H)
#define HELPER_H


#include <linux/nl80211.h>

#include <vector>
#include <string>


/**
 * @brief `enum nl80211_iftype` to string. Se line 3468 from <linux/nl80211.h>.
 * 
 * @example
 * std::println("type: {}", nl80211_iftype2string[NL80211_IFTYPE_STATION]);
 */
std::vector<std::string> const nl80211_iftype2string
{
  "unspecified", // NL80211_IFTYPE_UNSPECIFIED
  "independent BSS member", // NL80211_IFTYPE_ADHOC
	"managed BSS member", // NL80211_IFTYPE_STATION
	"access point", // NL80211_IFTYPE_AP
	"VLAN", // NL80211_IFTYPE_AP_VLAN
	"wireless distribution", // NL80211_IFTYPE_WDS
	"monitor", // NL80211_IFTYPE_MONITOR
	"mesh point", // NL80211_IFTYPE_MESH_POINT
	"P2P client", // NL80211_IFTYPE_P2P_CLIENT
	"P2P group owner", // NL80211_IFTYPE_P2P_GO
	"P2P device", // NL80211_IFTYPE_P2P_DEVICE
	"Outside Context of a BSS", // NL80211_IFTYPE_OCB
	"NAN", // NL80211_IFTYPE_NAN
};


/**
 * @brief From `iw` source code. Returns the frequency equivalents to a channel.
 */
inline
int ieee80211_channel_to_frequency(int chan)
{
	if (chan < 14)
		return 2407 + chan * 5;

	if (chan == 14)
		return 2484;

	/* FIXME: dot11ChannelStartingFactor (802.11-2007 17.3.8.3.2) */
	return (chan + 1000) * 5;
}


/**
 * From iw source code.
 */
int ieee80211_frequency_to_channel(int freq)
{
	if (freq == 2484)
		return 14;

	if (freq < 2484)
		return (freq - 2407) / 5;

	/* FIXME: dot11ChannelStartingFactor (802.11-2007 17.3.8.3.2) */
	if (freq < 45000)
		return freq/5 - 1000;

	if (freq >= 58320 && freq <= 64800)
		return (freq - 56160) / 2160;

	return 0;
}


/**
 * @brief 
 * 
 * @param mac_addr A 20 byte unsigned char C-style array.
 * @param arg Pointer to the payload returned by 
 * 						`nla_data(tb_msg[NL80211_ATTR_MAC])` call.
 * 
 * Based on `mac_addr_n2a()` iw source code.
 */
inline
std::string mac_addr_n2str(void* ptr)
{
	constexpr int ETH_ALEN{6};
	char buffer[21]{};
	int l{};	// fill counter
	auto* arg = reinterpret_cast<unsigned char*>(ptr);

	for(int i = 0; i < ETH_ALEN; ++i) 
	{
		std::sprintf(buffer+l, "%02x", arg[i]);
		l += 2;
		if(i < ETH_ALEN-1) {
			std::sprintf(buffer+l, ":");
			l += 1;
		}
	}

	return std::string{buffer};
}

/**
 * Based on `print_ssid_escaped()` iw source code.
 */
std::string get_ssid_escaped(uint8_t const len, void const* ptr)
{
	auto* data = reinterpret_cast<uint8_t const*>(ptr);
	char ssid[33]{};
	int l{};

	for(int i = 0; i < len; ++i) 
	{
		if(isprint(data[i]) && data[i] != ' ' && data[i] != '\\') {
			std::sprintf(ssid+l, "%c", data[i]);
			l += 1;
		}
		else if(data[i] == ' ' && (i != 0 && i != len -1)) {
			std::sprintf(ssid+l, " ");
			l += 1;
		}
		else {
			std::sprintf(ssid+l, "\\x%.2x", data[i]);
			l += 3;
		}
	}

	return std::string{ssid};
}


#endif // HELPER_H
