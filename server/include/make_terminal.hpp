
#pragma once

#include <memory>

namespace sk {

	class terminal;
	std::unique_ptr<terminal> make_terminal();
	
}
