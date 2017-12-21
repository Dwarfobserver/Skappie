
#include <iostream>
#include "network.hpp"
#include "messages.hpp"
#include "terminal.hpp"
#include "players_database.hpp"

using namespace std::literals;

int main() {
	std::cout << "   +++ Skappie server +++\n\n";

	sk::net::udp_socket socket;
	auto netContext = sk::net::context_info::default_info();
	{
		netContext.pSocket = &socket;
		const auto pair = netContext.is_valid();
		if (!pair.first) throw std::runtime_error{ pair.second };
	}
	sk::players_database playersDatabase;
	std::map<sk::net::address_type, sk::players_database::info const*> players;

	sk::terminal terminal;
	sk::terminal::state onlineState{ "online" };
	onlineState.add_command(
		"status",
		"Prints the server status",
		[&](std::string const& args) {

		std::cout << "Players online : (" << players.size() << ")\n";
		for (auto const& pair : players) {
			const auto pInfo = pair.second;
			std::cout << "At " << pair.first << " - " << pInfo->nickname << " (password: " << pInfo->password << ")\n";
		}
		std::cout << '\n';
	});
	terminal.add_state(std::move(onlineState));
	terminal.set_state("online");

	socket.bind(netContext.serverAddress.port());
	terminal.update();

	sk::net::packet packet;
	sk::net::address_type address;
	std::vector<sk::msg::wrapper> wrappers;

	try {
		while (!terminal.is_quitting()) {
			while (socket.try_get_packet(packet, address)) {
				for (auto& wrapper : packet.extract_messages()) {
					const auto family = sk::msg::tag::family_of(wrapper.tag());
					switch (family) {
					case sk::msg::family::state_sync:
						throw std::runtime_error{ "Not implemented" };
					case sk::msg::family::event: {
						if (wrapper.tag() != sk::msg::tag::disconnect)
							throw std::runtime_error{ "Not implemented" };

						const auto it = players.find(address);
						if (it != players.end()) {
							players.erase(it);
						}
						break; }
					case sk::msg::family::request: {
						if (wrapper.tag() != sk::msg::tag::connect)
							throw std::runtime_error{ "Not implemented" };

						// TODO Handle request copies

						sk::msg::connect_request request;
						wrapper.extract(request);

						const auto pInfo = playersDatabase.try_get_infos(request.nickname);

						sk::msg::connect_response response;
						response.stamp = request.stamp;

						auto sendResponse = [&](bool success, std::string const& reason) {
							response.accepted = success;
							response.reason = reason;
							socket.push(response, address);
						};

						switch (request.type) {
						case sk::msg::connect_request::login:
							if (!pInfo) {
								sendResponse(false, "Wrong nickname");
								break;
							}
							if (pInfo->password != request.password) {
								sendResponse(false, "Wrong password");
								break;
							}
							players.emplace(address, pInfo);
							sendResponse(true, "");
							break;
						case sk::msg::connect_request::new_account:
							if (pInfo) {
								sendResponse(false, "Nickname taken");
								break;
							}
							playersDatabase.try_register({ request.nickname, request.password });
							sendResponse(true, "");
							break;
						case sk::msg::connect_request::remove_account:
							if (!pInfo) {
								sendResponse(false, "Wrong nickname");
								break;
							}
							if (pInfo->password != request.password) {
								sendResponse(false, "Wrong password");
								break;
							}
							playersDatabase.try_remove(request.nickname);
							sendResponse(true, "");
							break;
						default:
							throw std::runtime_error{ "Invalid request type" };
						}

						break; }
					default:
						throw std::runtime_error{ "Invalid tag" };
					}
				}

			}
			socket.send_packets();
			std::this_thread::sleep_for(10ms);
			terminal.update();
		}
		for (auto const& pair : players) {
			address = pair.first;
			sk::msg::disconnect msg;
			socket.push(msg, address);
		}
		socket.send_packets();
	}
	catch(std::exception& e) {
		std::cout << "ERROR : " << e.what() << '\n';
		terminal.quit();
		terminal.update();
	}
}
