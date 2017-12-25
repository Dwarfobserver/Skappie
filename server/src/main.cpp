
#include <iostream>
#include "network.hpp"
#include "messages.hpp"
#include "terminal.hpp"
#include "players_database.hpp"
#include "player_info.hpp"
#include "messages_reactor.hpp"
#include "globals.hpp"

using namespace std::literals;

int main() {
	sk::terminal terminal;
	try {
		std::cout << "   +++ Skappie server +++\n\n";

		sk::gInit();

		auto netContext = sk::net::context_info::default_info();
		{
			netContext.pSocket = &sk::gSocket();
			const auto pair = netContext.is_valid();
			if (!pair.first) throw std::runtime_error{ pair.second };
		}
		std::map<sk::net::address_type, sk::player_info> players;

		sk::terminal::state onlineState{ "online" };
		onlineState.add_command(
			"status",
			"Prints the server status",
			[&](std::string const& args) {

			std::cout << "Players online : (" << players.size() << ")\n";
			for (auto const& pair : players) {
				auto pAccount = pair.second.pAccount;
				if (!pAccount) continue;
				std::cout << "    At " << pair.first << " - "
					<< pair.second.pAccount->nickname << '\n';
			}
		});
		onlineState.add_command(
			"accounts",
			"Show all accounts",
			[&](std::string const& args) {

			auto const& data = sk::gAccounts().accounts();
			std::cout << "Accounts : (" << data.size() << ")\n";
			for (auto const& pair : data) {
				std::cout << "    " << pair.first << " - password = '" << pair.second.password << "'\n";
			}

			for (auto const& pair : players) {
				auto& address = pair.first;
				sk::msg::disconnect msg;
				sk::gSocket().push(msg, address);
			}
			players.clear();
		});
		onlineState.add_command(
			"disconnect",
			"Disconnects all players",
			[&](std::string const& args) {

			for (auto const& pair : players) {
				auto& address = pair.first;
				sk::msg::disconnect msg;
				sk::gSocket().push(msg, address);
			}
			players.clear();
		});
		terminal.add_state(std::move(onlineState));
		terminal.set_state("online");

		sk::messages_reactor reactor;
		std::function<sk::msg::connect_response(sk::msg::connect_request const&, sk::player_info&)> connectCallback =
			[&] (sk::msg::connect_request const& request, sk::player_info& player) -> sk::msg::connect_response {

			sk::msg::connect_response response;
			response.stamp = request.stamp;

			switch (player.status) {
			case sk::player_status::unknown: {

				const auto pAccount = sk::gAccounts().try_get_account(request.nickname);

				if (request.type == sk::msg::connect_request::new_account) {
					if (pAccount) {
						response.accepted = false;
						response.reason = "Nickname taken";
						return response;
					}
					else {
						sk::player_account account;
						account.nickname = request.nickname;
						account.password = request.password;
						sk::gAccounts().create_account(account);
						player.pAccount = sk::gAccounts().try_get_account(request.nickname);
						player.status = sk::player_status::connected;

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
				case sk::msg::connect_request::login:
					player.pAccount = pAccount;
					player.status = sk::player_status::connected;
					response.accepted = true;
					return response;
				case sk::msg::connect_request::remove_account:
					sk::gAccounts().remove_account(request.nickname);
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
		reactor.set_request_callback(std::move(connectCallback));

		std::function<void(sk::msg::disconnect const&, sk::player_info&)> disconnectCallback =
			[&](sk::msg::disconnect const& request, sk::player_info& player) {

			if (!player.pAccount) return;

			player.pAccount = nullptr;
			player.status = sk::player_status::unknown;
		};
		reactor.set_callback(std::move(disconnectCallback));

		sk::gSocket().bind(netContext.serverAddress.port());
		std::cout << "bind at port " << sk::gSocket().port() << '\n';
		terminal.update();

		sk::net::packet packet;
		sk::net::address_type address;
		std::vector<sk::msg::wrapper> wrappers;

		while (!terminal.is_quitting()) {
			while (sk::gSocket().try_get_packet(packet, address)) {
				auto it = players.find(address);
				if (it == players.end()) {
					it = players.insert(it, std::make_pair(address, sk::player_info{ address }));
				}
				auto& player = it->second;
				for (auto& wrapper : packet.extract_messages()) {
					reactor.handle(wrapper, player);
				}
			}
			sk::gSocket().send_packets();
			std::this_thread::sleep_for(10ms);
			terminal.update();
		}
		for (auto const& pair : players) {
			address = pair.first;
			sk::msg::disconnect msg;
			sk::gSocket().push(msg, address);
		}
		sk::gSocket().send_packets();
	}
	catch(std::exception& e) {
		std::cout << "ERROR: " << e.what() << '\n';
		terminal.quit();
		terminal.update();
		std::cin.get();
	}
}
