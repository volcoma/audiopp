#include "device.h"
#include "impl/device_impl.h"
#include "logger.h"

namespace audio
{

namespace
{
static auto get_empty() -> const std::string&
{
	static std::string empty;
	return empty;
}
} // namespace

device::device(const std::string& id)
	: impl_(std::make_unique<detail::device_impl>(id))
{
}

device::~device() = default;

void device::enable()
{
	if(impl_)
	{
		impl_->enable();
	}
}

void device::disable()
{
	if(impl_)
	{
		impl_->disable();
	}
}

auto device::is_valid() const -> bool
{
	return impl_ && impl_->is_valid();
}

auto device::get_device_id() const -> const std::string&
{
	if(impl_)
	{
		return impl_->get_device_id();
	}
	return get_empty();
}

auto device::get_vendor() const -> const std::string&
{
	if(impl_)
	{
		return impl_->get_vendor();
	}
	return get_empty();
}

auto device::get_version() const -> const std::string&
{
	if(impl_)
	{
		return impl_->get_version();
	}
	return get_empty();
}

auto device::get_extensions() const -> const std::string&
{
	if(impl_)
	{
		return impl_->get_extensions();
	}
	return get_empty();
}

auto device::enumerate_playback_devices() -> std::vector<std::string>
{
	return detail::device_impl::enumerate_playback_devices();
}

auto device::enumerate_capture_devices() -> std::vector<std::string>
{
	return detail::device_impl::enumerate_capture_devices();
}

auto device::default_playback_device() -> std::string
{
	return detail::device_impl::default_playback_device();
}

auto device::default_capture_device() -> std::string
{
    return detail::device_impl::default_capture_device();
}

void device::print_devices()
{
	info() << "------------------------------------------";

	auto playback_devices = enumerate_playback_devices();
	info() << "Supported audio playback devices:";
	for(const auto& dev : playback_devices)
	{
		info() << "-- " << dev;
	}
	auto default_playback = default_playback_device();
	info() << "Default audio playback device:";
	info() << "-- " << default_playback;

	auto capture_devices = enumerate_capture_devices();
	info() << "Supported audio capture devices:";
	for(const auto& dev : capture_devices)
	{
		info() << "-- " << dev;
	}

	auto default_capture = default_capture_device();
	info() << "Default audio capture device:";
	info() << "-- " << default_capture;
	info() << "------------------------------------------";
}
} // namespace audio
