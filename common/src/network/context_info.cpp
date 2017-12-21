
#include "network/context_info.hpp"

using namespace std::literals;

namespace sk::net {

	const float context_info::pingIntegrator{ 0.9f };
	
	context_info context_info::default_info() {
		context_info info;
		info.pSocket = nullptr;
		info.packetSize = 508;
		info.ping = 100ms;
		info.requestRetry = 50ms;
		info.timeout = 1'000ms;
		info.serverAddress = { localhost_ip, 52'049 };
		return info;
	}

	std::pair<bool, char const*> context_info::is_valid() const {
		if (!pSocket)
			return { false, "Socket must be set" };
		if (packetSize < 200 || packetSize > 1500)
			return { false, "Packet size must be between 200 & 1500" };
		if (ping >= timeout)
			return { false, "Ping must be inferior to timeout" };
		if (requestRetry >= timeout)
			return { false, "Requet retry must be inferior to timeout" };

		return { true, "Network context info valid" };
	}

	void context_info::register_ping(std::chrono::milliseconds delay) {
		const int percent = static_cast<int>(pingIntegrator * 100);
		ping = ping * (100 - percent) + delay * percent;
		ping /= 100;
	}

}
