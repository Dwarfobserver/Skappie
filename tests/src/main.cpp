
#define CATCH_CONFIG_RUNNER
#include <catch.hpp>


int main(int argc, char* argv[]) {
	const int result = Catch::Session().run(argc, argv);
	std::cout << "Press a touch to exit ...\n";
	std::cin.get();
	return result;
}
