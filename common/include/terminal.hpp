
#pragma once

#include <map>
#include <queue>
#include <future>
#include <functional>

/**
* A terminal class which get commands from std::cin with an async task.
* He is actualised with yield() and does not consume another thread.
* quit, add_state and set_state functions are deferred until a yield call.
* The command line is also treated during the yield call.
*
* He executes 'quit', 'help' commands and those from it's current state.
* A state is composed of commands (a name, a description and an action with a
* string argument).
*/

namespace sk {

	class terminal {
	public:
		struct command {
			std::string name;
			std::string description;
			std::function<void(std::string const&)> action;
		};
		class state {
			friend class terminal;
		public:
			explicit state(std::string const& name);

			void add_command(command&& cmd);
			void add_command(std::string&& name, std::string&& description, std::function<void(std::string const&)>&& action);
			std::string const& name() const { return name_; }
		private:
			bool has_command(std::string const& name) const;
			void trigger_command(std::string const& name, std::string&& args);
			void print_help() const;

			std::string name_;
			std::map<std::string, command> commands_;
		};

		terminal();
		~terminal() noexcept;

		void update();

		void quit();
		bool is_quitting() const { return mustQuit_; }
		void add_state(state&& commandsState);
		void set_state(std::string const& name);

		std::string get_console_entry() const;

		static char const* get_excuse();
	private:
		void async_console_line();
		void execute(std::string const& command);
		void run_tasks();

		std::queue<std::function<void()>> taskQueue_;
		std::map<std::string, state> states_;
		state* pState_;
		bool mustQuit_;
		std::future<std::string> consoleLine_;
	};

}
