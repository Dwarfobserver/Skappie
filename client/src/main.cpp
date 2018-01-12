
#include <iostream>
#include "globals.hpp"
#include "terminal.hpp"
#include "network.hpp"
#include "messages.hpp"
#include "messages_reactor.hpp"
#include "make_terminal.hpp"
#include "make_reactor.hpp"
#include "application_context.hpp"

using namespace std::literals;

int main() {
	try {
		std::cout << "   +++ Skappie client +++\n\n";

		sk::gInit();

		auto pReactor  = sk::make_reactor();
		auto pTerminal = sk::make_terminal();

		const int alea = std::chrono::system_clock::now().time_since_epoch().count() % 1'000;
		sk::gSocket().bind(sk::net::server_address.port() + alea + 1);
		std::cout << "bind at port " << sk::gSocket().port() << '\n';

		pTerminal->update();

		sk::net::packet packet;
		sk::net::address_type address;

		while (!pTerminal->is_quitting()) {

			sk::gContext().update();

			while (sk::gSocket().try_get_packet(packet, address)) {
				if (address != sk::net::server_address)
					throw std::runtime_error{ "Not implemented" };

				for (auto& wrapper : packet.extract_messages()) {
					const auto family = sk::msg::tag::family_of(wrapper.tag());

					if (family == sk::msg::family::request) {
						sk::gContext().handle_response(wrapper);
					}
					else {
						pReactor->handle(wrapper);
					}
				}
			}
			sk::gSocket().send_packets();
			std::this_thread::sleep_for(10ms);
			pTerminal->update();
		}
		if (sk::gContext().status != sk::player_status::unknown) {
			sk::gSocket().push(sk::msg::disconnect{}, sk::net::server_address);
			sk::gSocket().send_packets();
		}
	}
	catch(std::exception& e) {
		std::cout << "ERROR : " << e.what() << '\n';
		std::cin.get();
	}

}
