
#include "make_reactor.hpp"
#include "messages_reactor.hpp"
#include "messages.hpp"
#include "network.hpp"

namespace sk {

	std::unique_ptr<messages_reactor> make_reactor() {
		auto pReactor = std::make_unique<messages_reactor>();

		std::function<msg::connect_response(msg::connect_request const&, player_info&)> connectCallback =
			[] (msg::connect_request const& request, player_info& player) -> msg::connect_response {

			msg::connect_response response;
			response.stamp = request.stamp;

			switch (player.status) {
			case player_status::unknown: {

				const auto pAccount = gAccounts().try_get_account(request.nickname);

				if (request.type == msg::connect_request::new_account) {
					if (pAccount) {
						response.accepted = false;
						response.reason = "Nickname taken";
						return response;
					}
					else {
						player_account account;
						account.nickname = request.nickname;
						account.password = request.password;
						gAccounts().create_account(account);
						player.pAccount = gAccounts().try_get_account(request.nickname);
						player.status = player_status::connected;

						response.accepted = true;
						return response;
					}
				}

				// login & remove_account

				if (!pAccount) {
					response.accepted = false;
					response.reason = "Unknown nickname";
					return response;
				}
				if (request.password != pAccount->password) {
					response.accepted = false;
					response.reason = "Wrong password";
					return response;
				}

				switch (request.type) {
				case msg::connect_request::login:
					player.pAccount = pAccount;
					player.status = player_status::connected;
					response.accepted = true;
					return response;
				case msg::connect_request::remove_account:
					gAccounts().remove_account(request.nickname);
					response.accepted = true;
					return response;
				default:
					std::cout << "WARNING: Wrong request type received\n";
					return response;
				}}
			default:
				response.accepted = false;
				response.reason = "Already connected";
				return response;
			}
		};
		pReactor->set_request_callback(std::move(connectCallback));

		std::function<void(msg::disconnect const&, player_info&)> disconnectCallback =
			[] (msg::disconnect const& request, player_info& player) {

			std::cout << "disconnect ...\n";
			if (!player.pAccount) return;
			std::cout << "success\n";

			player.pAccount = nullptr;
			player.status = player_status::unknown;
		};
		pReactor->set_callback(std::move(disconnectCallback));

		return pReactor;
	}

}
