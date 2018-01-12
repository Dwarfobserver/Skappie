
#include "make_terminal.hpp"
#include "terminal.hpp"
#include "messages.hpp"
#include "globals.hpp"
#include "application_context.hpp"
#include <iostream>
#include <optional>

namespace sk {
	
	std::unique_ptr<terminal> make_terminal() {
		auto pTerminal = std::make_unique<terminal>();
		auto ptrTerminal = pTerminal.get();

		auto parseArgs = [&](std::string const& args) -> std::optional<msg::connect_request> {
			auto it = find(args.begin(), args.end(), ' ');
			if (it == args.end()) {
				std::cout << "Usage : login <nickname> <password>\n";
				return {};
			}
			msg::connect_request request;
			request.nickname = std::string{ args.begin(), it };
			request.password = std::string{ it + 1, args.end() };
			return request;
		};

		terminal::state launcherState{ "launcher" };
		launcherState.add_command(
			"login",
			"Connect with an account : <nickname> <password>",
			[&, ptrTerminal](std::string const& args) {

			auto optRequest = parseArgs(args);
			if (!optRequest) return;

			msg::connect_request request = *optRequest;
			request.type = msg::connect_request::login;

			std::function<bool(msg::connect_response const& response)> callback =
				[&, ptrTerminal]
				(msg::connect_response const& response) -> bool {

				if (response.accepted) {
					ptrTerminal->set_state("online");
					gContext().status = player_status::connected;
					std::cout << "Connected !\n";
				}
				else {
					std::cout << "Login failed : " << response.reason << '\n';
				}
				return true;
			};
			gContext().make_request(
				request,
				net::server_address,
				std::move(callback),
				[] { std::cout << "Login request timed out\n"; }
			);
		});
		launcherState.add_command(
			"register",
			"Create an account : <nickname> <password>",
			[&, ptrTerminal] (std::string const& args) {

			auto optRequest = parseArgs(args);
			if (!optRequest) return;

			msg::connect_request request = *optRequest;
			request.type = msg::connect_request::new_account;

			std::function<bool(msg::connect_response const& response)> callback =
				[request, ptrTerminal]
				(msg::connect_response const& response) -> bool {

				if (response.accepted) {
					ptrTerminal->set_state("online");
					gContext().status = player_status::connected;
					std::cout << "Account created ! "
						"nickname = " << request.nickname <<
						", password = " << request.password << '\n';
				}
				else {
					std::cout << "Account creation failed : " << response.reason << '\n';
				}
				return true;
			};
			gContext().make_request(
				request,
				net::server_address,
				std::move(callback),
				[] { std::cout << "Register request timed out\n"; }
			);
		});
		launcherState.add_command(
			"delete",
			"Delete an account : <nickname> <password>",
			[&](std::string const& args) {

			auto optRequest = parseArgs(args);
			if (!optRequest) return;

			msg::connect_request request = *optRequest;
			request.type = msg::connect_request::remove_account;

			std::function<bool(msg::connect_response const& response)> callback =
				[request](msg::connect_response const& response) -> bool {

				if (response.accepted) {
					std::cout << "Account deleted !\n";
				}
				else {
					std::cout << "Account suppression failed : " << response.reason << '\n';
				}
				return true;
			};
			gContext().make_request(
				request,
				net::server_address,
				std::move(callback),
				[] { std::cout << "Delete request timed out\n"; }
			);
		});

		terminal::state onlineState{ "online" };
		onlineState.add_command(
			"logout",
			"Disconnect from the server",
			[&, ptrTerminal](std::string const& args) {

			gSocket().push(msg::disconnect{}, net::server_address);
			gContext().status = player_status::unknown;
			ptrTerminal->set_state("launcher");
		});

		pTerminal->add_state(std::move(launcherState));
		pTerminal->add_state(std::move(onlineState));
		pTerminal->set_state("launcher");
		return pTerminal;
	}

}
