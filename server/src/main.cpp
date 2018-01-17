
#include <iostream>
#include "messages.hpp"
#include "terminal.hpp"
#include "players_database.hpp"
#include "player_info.hpp"
#include "globals.hpp"
#include "make_reactor.hpp"
#include "messages_reactor.hpp"
#include "make_terminal.hpp"

using namespace std::literals;

int main() {
	try {
		std::cout << "   +++ Skappie server +++\n\n";

		sk::gInit();

		auto pTerminal = sk::make_terminal();
		auto pReactor = sk::make_reactor();

		sk::gSocket().bind(sk::net::server_address.port());
		std::cout << "bind at port " << sk::gSocket().port() << '\n';
		pTerminal->update();

		sk::net::packet packet;
		sk::net::address_type address;
		std::vector<sk::msg::wrapper> wrappers;

		while (!pTerminal->is_quitting()) {
			while (sk::gSocket().try_get_packet(packet, address)) {
				auto it = sk::gPlayers().find(address);
				if (it == sk::gPlayers().end()) {
					it = sk::gPlayers().insert(it, std::make_pair(address, sk::player_info{ address }));
				}
				auto& player = it->second;
				for (auto& wrapper : packet.extract_messages()) {
					pReactor->handle(wrapper, player);
				}
			}
			sk::gSocket().send_packets();
			std::this_thread::sleep_for(10ms);
			pTerminal->update();
		}
		for (auto const& pair : sk::gPlayers()) {
			address = pair.first;
			sk::msg::disconnect msg;
			sk::gSocket().push(msg, address);
		}
		sk::gSocket().send_packets();
	}
	catch(std::exception& e) {
		std::cout << "ERROR: " << e.what() << '\n';
		std::cin.get();
	}
}
