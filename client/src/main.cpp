
#include <iostream>
#include <optional>
#include "terminal.hpp"
#include "network.hpp"
#include "messages.hpp"

using namespace std::literals;

int main() {
	std::cout << "   +++ Skappie client +++\n\n";

	sk::net::udp_socket socket;
	auto netContext = sk::net::context_info::default_info();
	{
		netContext.pSocket = &socket;
		const auto pair = netContext.is_valid();
		if (!pair.first) throw std::runtime_error{ pair.second };
	}

	sk::msg::stamp_type nextStamp;
	std::vector<std::unique_ptr<sk::net::request>> pRequests;
	bool connected = false;

	auto parseArgs = [&] (std::string const& args) -> std::optional<sk::msg::connect_request> {
		auto it = find(args.begin(), args.end(), ' ');
		if (it == args.end()) {
			std::cout << "Usage : login <nickname> <password>\n";
			return {};
		}
		sk::msg::connect_request request;
		request.nickname = std::string{ args.begin(), it };
		request.password = std::string{ it, args.end() };
		return request;
	};

	sk::terminal terminal;
	sk::terminal::state launcherState{ "launcher" };
	launcherState.add_command(
		"login",
		"Connect with an account : <nickname> <password>",
		[&](std::string const& args) {

		auto optRequest = parseArgs(args);
		if (!optRequest) return;

		sk::msg::connect_request request = *optRequest;
		request.type = sk::msg::connect_request::login;
		request.stamp = nextStamp++;

		std::function<bool(sk::msg::connect_response const& response)> callback =
			[&](sk::msg::connect_response const& response) -> bool {

			if (response.accepted) {
				connected = true;
				std::cout << "Connected !\n";
			}
			else {
				std::cout << "Login failed : " << response.reason << '\n';
			}
			return true;
		};
		pRequests.push_back(make_request(
			netContext,
			request,
			netContext.serverAddress,
			std::move(callback),
			[] { std::cout << "Login request timed out\n"; }
		));
	});
	launcherState.add_command(
		"register",
		"Create an account : <nickname> <password>",
		[&](std::string const& args) {

		auto optRequest = parseArgs(args);
		if (!optRequest) return;

		sk::msg::connect_request request = *optRequest;
		request.type = sk::msg::connect_request::new_account;
		request.stamp = nextStamp++;

		std::function<bool(sk::msg::connect_response const& response)> callback =
			[request](sk::msg::connect_response const& response) -> bool {

			if (response.accepted) {
				std::cout << "Account created ! "
					"nickname = " << request.nickname <<
					", password = " << request.password << '\n';
			}
			else {
				std::cout << "Account creation failed : " << response.reason << '\n';
			}
			return true;
		};
		pRequests.push_back(make_request(
			netContext,
			request,
			netContext.serverAddress,
			std::move(callback),
			[] { std::cout << "Register request timed out\n"; }
		));
	});
	launcherState.add_command(
		"delete",
		"Delete an account : <nickname> <password>",
		[&](std::string const& args) {

		auto optRequest = parseArgs(args);
		if (!optRequest) return;

		sk::msg::connect_request request = *optRequest;
		request.type = sk::msg::connect_request::remove_account;
		request.stamp = nextStamp++;

		std::function<bool(sk::msg::connect_response const& response)> callback =
			[request](sk::msg::connect_response const& response) -> bool {

			if (response.accepted) {
				std::cout << "Account deleted !\n";
			}
			else {
				std::cout << "Account suppression failed : " << response.reason << '\n';
			}
			return true;
		};
		pRequests.push_back(make_request(
			netContext,
			request,
			netContext.serverAddress,
			std::move(callback),
			[] { std::cout << "Delete request timed out\n"; }
		));
	});
	launcherState.add_command(
		"logout",
		"Disconnect from the server",
		[&](std::string const& args) {

		socket.push(sk::msg::disconnect{}, netContext.serverAddress);
		connected = false;
	});
	terminal.add_state(std::move(launcherState));
	terminal.set_state("launcher");

	socket.bind(netContext.serverAddress.port() + (rand() % 1'000) + 1);
	terminal.update();

	sk::net::packet packet;
	sk::net::address_type address;
	try {
		while (!terminal.is_quitting()) {

			std::vector<int> timedOutIndices;
			for (auto i = 0u; i < pRequests.size(); ++i) {
				auto& request = *pRequests[i].get();
				request.update();
				if (request.get_status() == sk::net::request::status::timed_out)
					timedOutIndices.push_back(i);
			}
			for (int i = timedOutIndices.size() - 1; i >= 0; --i) {
				pRequests.erase(pRequests.begin() + timedOutIndices[i]);
			}

			while (socket.try_get_packet(packet, address)) {
				if (address != netContext.serverAddress)
					throw std::runtime_error{ "Not implemented" };

				for (auto& wrapper : packet.extract_messages()) {
					const auto family = sk::msg::tag::family_of(wrapper.tag());
					switch (family) {
					case sk::msg::family::state_sync:
						throw std::runtime_error{ "Not implemented" };
					case sk::msg::family::event:
						if (wrapper.tag() != sk::msg::tag::disconnect)
							throw std::runtime_error{ "Not implemented" };

						std::cout << "Server went offline\n";
						pRequests.clear();
						connected = false;
						break;
					case sk::msg::family::request: {
						if (wrapper.tag() != sk::msg::tag::connect)
							throw std::runtime_error{ "Not implemented" };

						const auto it = std::find_if(
							pRequests.begin(), pRequests.end(),
							[&] (std::unique_ptr<sk::net::request> const& pRequest) {

							return pRequest->stamp() == wrapper.stamp();
						});
						if (it == pRequests.end())
							throw std::runtime_error{ "Received response with unknown stamp" };

						auto& request = *it->get();
						if (request.handle(wrapper)) {
							pRequests.erase(it);
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
		if (connected) {
			socket.push(sk::msg::disconnect{}, netContext.serverAddress);
			socket.send_packets();
		}
	}
	catch(std::exception& e) {
		std::cout << "ERROR : " << e.what() << '\n';
		terminal.quit();
		terminal.update();
	}

}
