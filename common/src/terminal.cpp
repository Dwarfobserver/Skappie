
#include "terminal.hpp"
#include <iostream>
#include <random>
#include "utils.hpp"


using namespace sk;
using namespace std::literals;

// State

terminal::state::state(std::string const& name) :
	name_{ name }
{}

void terminal::state::add_command(command &&cmd) {
	auto cmdName = cmd.name;
	commands_.emplace(std::make_pair(std::move(cmdName), std::move(cmd)));
}

void terminal::state::add_command(
	std::string&& name,
	std::string&& description,
	std::function<void(std::string const&)>&& action) {
	
	add_command({ std::move(name), std::move(description), std::move(action) });
}

bool terminal::state::has_command(const std::string& name) const {
	return commands_.find(name) != commands_.end();
}

void terminal::state::trigger_command(std::string const &name, std::string&& args) {
	commands_.at(name).action(std::move(args));
}

void terminal::state::print_help() const {
	std::cout << name() << " commands :";
	for (auto const& pair : commands_) {
		std::cout << "\n    " << pair.second.name << " - " << pair.second.description;
	}
	std::cout << "\nType 'quit' to leave the terminal.";
	std::cout << "\nType 'help' to show this description again.\n";
	std::cout << "\n";
}

// Terminal

terminal::terminal() :
	pState_{ nullptr },
	mustQuit_{ false }
{}

terminal::~terminal() noexcept {
	SK_ASSERT(mustQuit_);
}

void terminal::update() {
	if (mustQuit_) return;

	const auto pOldState = pState_;
	run_tasks();
	if (!pOldState) {
		if (!pState_) return;
		// First set_state
		async_console_line();
	}
	if (consoleLine_.wait_for(0s) == std::future_status::ready) {
		execute(consoleLine_.get());
		run_tasks();
		if (mustQuit_) return;
		async_console_line();
	}
}

void terminal::quit() {
	taskQueue_.emplace([this] {
		mustQuit_ = true;
	});
}

void terminal::add_state(terminal::state &&commandsState) {
	taskQueue_.emplace([this, state = std::move(commandsState)]() mutable {
		auto name = state.name();
		states_.emplace(std::make_pair(std::move(name), std::move(state)));
	});
}

void terminal::set_state(std::string const &name) {
	taskQueue_.emplace([this, name] {
		pState_ = &states_.at(name);
		pState_->print_help();
	});
}

void terminal::async_console_line() {
	std::cout << get_console_entry();
	consoleLine_ = std::async(std::launch::async, [this] {
		std::string command;
		std::getline(std::cin, command);
		return command;
	});
}

void terminal::execute(std::string const& command) {
	if (command.empty()) return;

	std::string name;
	std::string args;
	const auto spacePos = command.find(' ');
	if (spacePos < command.size()) {
		name = { command.begin(), command.begin() + spacePos };
		args = { command.begin() + spacePos + 1, command.end() };
	}
	else {
		name = command;
		args = "";
	}
	if (command == "quit") {
		quit();
		return;
	}
	if (command == "help") {
		pState_->print_help();
		return;
	}
	if (!pState_->has_command(name)) {
		std::cout << "There is no command named '" + name + "'. " << get_excuse() << '\n';
		return;
	}
	pState_->trigger_command(name, std::move(args));
}

void terminal::run_tasks() {
	while (!taskQueue_.empty()) {
		taskQueue_.front()();
		taskQueue_.pop();
	}
}

std::string terminal::get_console_entry() const {
	return pState_->name() + '>';
}

char const* terminal::get_excuse() {
	static std::vector<char const*> excuses{
		"Sorry !",
		"Sumimasen !",
		":s",
		":(",
		"*Apologies in asm*"
	};
	static std::random_device rd;
	static std::mt19937 rng(rd());
	static std::uniform_int_distribution<int> genRandom(0, static_cast<int>(excuses.size() - 1));

	return excuses[genRandom(rng)];
}
