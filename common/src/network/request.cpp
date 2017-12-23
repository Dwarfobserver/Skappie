
#include "network/request.hpp"

using namespace std::chrono;

namespace sk::net {

	void request::update() {
		SK_ASSERT(status_ == status::running,
			"A finished request must not be updated");

		const auto tNow = high_resolution_clock::now();
		if (tNow - firstSend_ > context_.timeout) {
			callbackTimeout_();
			status_ = status::timed_out;
			return;
		}
		if (tNow - lastSend_ > context_.requestRetry) {
			context_.pSocket->push(message_, address_);
			lastSend_ = tNow;
		}
	}

	bool request::handle(msg::wrapper const& message) {
		SK_ASSERT(status_ == status::running,
			"A finished request must not handle messages");

		if(!gotResponse_) {
			const auto tNow = high_resolution_clock::now();
			context_.register_ping(duration_cast<milliseconds>(tNow - firstSend_));
			gotResponse_ = true;
		}

		const bool resolved = callback_(message);
		if (resolved) status_ = status::resolved;
		return resolved;
	}

	request::request(context_info& context) :
		context_(context),
		status_(status::running),
		gotResponse_(false)
	{}

}
