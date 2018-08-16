#pragma once

#include "sound_data.h"
#include <memory>

namespace audio
{
namespace detail
{
class sound_impl;
}

//-----------------------------------------------------------------------------
/// Storage for audio samples defining a sound.
//-----------------------------------------------------------------------------
class sound
{
public:
	sound();
	~sound();
	sound(sound_data&& data, bool stream = false);
	sound(sound&& rhs) noexcept;
	sound& operator=(sound&& rhs) noexcept;

	sound(const sound& rhs) = delete;
	sound& operator=(const sound& rhs) = delete;

	//-----------------------------------------------------------------------------
	/// Checks whether a sound is valid.
	//-----------------------------------------------------------------------------
	bool is_valid() const;

	//-----------------------------------------------------------------------------
	/// Gets the sound data info.
	//-----------------------------------------------------------------------------
	const sound_info& get_info() const;

	bool load_buffer();

	//-----------------------------------------------------------------------------
	/// Unique identifier of this sound. 0 is invalid
	//-----------------------------------------------------------------------------
	uintptr_t uid() const;

private:
	friend class source;

	/// pimpl idiom
	std::unique_ptr<detail::sound_impl> impl_;

	/// sound info
	sound_info info_;
};
} // namespace audio
