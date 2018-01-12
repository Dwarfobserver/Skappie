
#include "network/request.hpp"
#include "application_context.hpp"

using namespace std::chrono;

namespace sk::net {

	request::request() :
		stamp_{ 0 },
		status_(status::running),
		gotResponse_(false)
	{}

	void request::update() {
		SK_ASSERT(status_ == status::running,
			"A finished request must not be updated");

		const auto tNow = high_resolution_clock::now();
		if (tNow - firstSend_ > gContext().msTimeout) {
			callbackTimeout_();
			status_ = status::timed_out;
			return;
		}
		if (tNow - lastSend_ > gContext().msRetry) {
			gSocket().push(message_, address_);
			lastSend_ = tNow;
		}
	}

	bool request::handle(msg::wrapper const& message) {
		SK_ASSERT(status_ == status::running,
			"A finished request must not handle messages");

		if (!gotResponse_) {
			const auto tNow = high_resolution_clock::now();
			gContext().register_ping(duration_cast<milliseconds>(tNow - firstSend_));
			gotResponse_ = true;
		}

		const bool resolved = callback_(message);
		if (resolved) status_ = status::resolved;
		return resolved;
	}

}
