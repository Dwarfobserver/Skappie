
#include "make_terminal.hpp"
#include "terminal.hpp"
#include "globals.hpp"
#include "messages.hpp"
#include "network.hpp"
#include "players_database.hpp"
#include "player_info.hpp"
#include <iostream>

namespace sk {

	std::unique_ptr<terminal> make_terminal() {
		auto pTerminal = std::make_unique<terminal>();

		terminal::state onlineState{ "online" };
		onlineState.add_command(
			"status",
			"Prints the server status",
			[&](std::string const& args) {

			std::cout << "Players online : (" << gPlayers().size() << ")\n";
			for (auto const& pair : gPlayers()) {
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

			auto const& data = gAccounts().accounts();
			std::cout << "Accounts : (" << data.size() << ")\n";
			for (auto const& pair : data) {
				std::cout << "    " << pair.first << " - password = '" << pair.second.password << "'\n";
			}

			for (auto const& pair : gPlayers()) {
				auto& address = pair.first;
				msg::disconnect msg;
				gSocket().push(msg, address);
			}
			gPlayers().clear();
		});
		onlineState.add_command(
			"disconnect",
			"Disconnects all players",
			[&](std::string const& args) {

			for (auto const& pair : gPlayers()) {
				auto& address = pair.first;
				msg::disconnect msg;
				gSocket().push(msg, address);
			}
			gPlayers().clear();
		});

		pTerminal->add_state(std::move(onlineState));
		pTerminal->set_state("online");
		return pTerminal;
	}
	
}
