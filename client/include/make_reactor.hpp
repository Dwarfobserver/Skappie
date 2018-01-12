
#pragma once
#include <memory>


namespace sk {
	
	class messages_reactor;
	std::unique_ptr<messages_reactor> make_reactor();

}
