
#pragma once

#include <cstdint>
#include <string>


namespace sk::cmp {
	
	/// The type of the tag associated with each component type.
	using tag_type = int8_t;

	/// Returns the tag associated to the component type.
	template <class Component>
	constexpr tag_type tag_of = Component::template tag;


	namespace tag {

		/// All tags available to categorize components.
		enum : tag_type {
			invalid = -1,
			_tags_begin = 0,

			body,
			hitbox,
			graphic,

			_tags_end,
			count = _tags_end
		};

		/// Returns true if the tag has a valid value.
		inline bool is_valid(tag_type tag) {
			return tag >= _tags_begin && tag < _tags_end;
		}

		/// Returns the name of the given tag value.
		std::string to_string(tag_type tag);
	}
}
