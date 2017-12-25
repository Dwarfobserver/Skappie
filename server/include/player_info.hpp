
#pragma once

#include "network/udp_socket.hpp"
#include "network/packet.hpp"
#include <map>


namespace sk {

	struct player_account;

	enum class player_status {
		unknown,
		connected
	};

	class player_info {
	public:
		explicit player_info(net::address_type const& address) :
			status{ player_status::unknown },
			address{ address },
			pAccount{ nullptr }
		{}

		player_status status;
		net::address_type address;
		player_account const* pAccount;

		template <class ResponseMsg>
		void store_response(msg::stamp_type stamp, ResponseMsg const& response);

		template <class ResponseMsg>
		bool try_get_response(msg::stamp_type stamp, ResponseMsg& response);
	private:
		std::byte buffer_[500];
		std::map<msg::stamp_type, msg::wrapper> requests_;
	};

	template <class ResponseMsg>
	void player_info::store_response(msg::stamp_type stamp, ResponseMsg const& response) {
		SK_ASSERT(requests_.find(stamp) == requests_.end(),
			"Response with stamp " + std::to_string(stamp) + " already stored");

		const bytes_span span{ buffer_, sizeof(buffer_) };
		requests_.emplace(std::make_pair(stamp, msg::wrapper{ response, span }));
	}

	template <class ResponseMsg>
	bool player_info::try_get_response(msg::stamp_type stamp, ResponseMsg& response) {
		const auto it = requests_.find(stamp);
		if (it == requests_.end())
			return false;

		it->second.extract(response);
		return true;
	}

}
