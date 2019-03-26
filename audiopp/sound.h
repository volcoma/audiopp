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
	sound(sound&& rhs) noexcept = default;
	sound& operator=(sound&& rhs) noexcept = default;

	sound(const sound& rhs) = delete;
	sound& operator=(const sound& rhs) = delete;

	//-----------------------------------------------------------------------------
	/// Checks whether a sound is valid.
	//-----------------------------------------------------------------------------
	auto is_valid() const -> bool;

	//-----------------------------------------------------------------------------
	/// Gets the sound data info.
	//-----------------------------------------------------------------------------
	auto get_info() const -> const sound_info&;

	//-----------------------------------------------------------------------------
	/// Loads the next chunk of the buffer if streaming is enabled.
	/// If already loaded, it does nothing.
	//-----------------------------------------------------------------------------
	auto load_chunk() -> bool;

	//-----------------------------------------------------------------------------
	/// Adds a pcm data chunk
	//-----------------------------------------------------------------------------
	void append_chunk(const std::vector<uint8_t>& data);

	//-----------------------------------------------------------------------------
	/// Unique identifier of this sound. 0 is invalid
	//-----------------------------------------------------------------------------
	auto uid() const -> uintptr_t;

private:
	friend class source;

	/// pimpl idiom
	std::unique_ptr<detail::sound_impl> impl_;
};
} // namespace audio
