#include "sound.h"
#include "impl/sound_impl.h"

namespace audio
{

sound::sound() = default;

sound::~sound() = default;

sound::sound(sound_data&& data, bool stream)
	: impl_(std::make_unique<detail::sound_impl>(std::move(data.data), data.info, stream))
{
}

auto sound::is_valid() const -> bool
{
	return impl_ && impl_->is_valid();
}

auto sound::get_info() const -> const sound_info&
{
	if(impl_)
	{
		return impl_->get_info();
	}
	static sound_info empty;
	return empty;
}

void sound::append_chunk(const std::vector<uint8_t>& data)
{
	if(impl_)
	{
		impl_->append_chunk(data);
	}
}

auto sound::uid() const -> uintptr_t
{
	return reinterpret_cast<uintptr_t>(impl_.get());
}

} // namespace audio
