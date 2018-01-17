
#include "application_context.hpp"
#include "network/request.hpp"
#include <iostream>

namespace sk {

	application_context::application_context() :
		bufferSpan{ buffer_, sizeof(buffer_) },
		msTimeout{ 1'000 },
		msRetry{ 50 },
		status{ player_status::unknown },
		nextStamp_{ 0 },
		msPing_{ 50 }
	{}

	void application_context::update() {
		std::vector<int> timedOutIndices;
		for (auto i = 0u; i < pRequests_.size(); ++i) {
			auto& request = *pRequests_[i].get();
			request.update();
			if (request.status_ == net::request::status::timed_out) {
				timedOutIndices.push_back(i);
			}
		}
		for (int i = timedOutIndices.size() - 1; i >= 0; --i) {
			pRequests_.erase(pRequests_.begin() + timedOutIndices[i]);
		}
	}

	void application_context::handle_response(msg::wrapper const& response) {

		const auto it = std::find_if(
			pRequests_.begin(), pRequests_.end(),
			[&](std::unique_ptr<net::request> const& pRequest) {

			return pRequest->stamp_ == response.stamp();
		});
		if (it == pRequests_.end()) {
			std::cout << "WARNING : Packet with unknown stamp received\n";
			if (pRequests_.empty()) {
				std::cout << "No requests\n";
			}
			else {
				std::cout << "Stamp " << (int) response.stamp()
					<< " should be " << (int) pRequests_.front()->stamp_ << "\n";
			}
			return;
		}

		auto& request = *it->get();
		if (request.handle(response)) {
			pRequests_.erase(it);
		}
	}

	void application_context::register_ping(std::chrono::milliseconds delay) {
		msPing_ = delay;
	}

}
