
#include "components/cmp_tags.hpp"
#include "utils.hpp"
#include <unordered_map>


namespace sk::cmp {

	namespace tag {

		std::string to_string(tag_type tag) {

#define WRITE_PAIR(name) { name, #name }
			static std::unordered_map<tag_type, std::string> names{
				WRITE_PAIR(body),
				WRITE_PAIR(hitbox),
				WRITE_PAIR(graphic),
			};
#undef WRITE_PAIR

			SK_ASSERT(names.size() == _tags_end - _tags_begin,
				"Tag names were forgotten");

			if (!is_valid(tag))
				return "invalid";

			return names[tag];
		}

	}

}
