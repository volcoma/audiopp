#include "device_impl.h"

#include "../exception.h"
#include "../logger.h"
#include "check.h"

#include "al.h"
#include "efx.h"
#include <cstdint>
#include <sstream>

namespace audio
{
namespace detail
{

namespace openal
{
static auto al_has_extension(ALCdevice* dev, const char* ext) -> bool
{
	return (alcIsExtensionPresent(dev, ext) == ALC_TRUE); // ALC_TRUE
}

static auto al_get_string(ALCdevice* dev, ALenum e) -> std::string
{
	auto result = reinterpret_cast<const char*>(alcGetString(dev, e));
	if(result)
	{
		return result;
	}

	return {};
}

static auto al_get_strings(ALCdevice* dev, ALenum e) -> std::vector<std::string>
{
	std::vector<std::string> output;

	auto result = reinterpret_cast<const char*>(alcGetString(dev, e));

	if(result)
	{
		while(!std::string(result).empty())
		{
			output.emplace_back(result);
			result += output.back().size() + 1;
		}
	}

	return output;
}

static auto al_version() -> std::string
{
	std::stringstream ss;
	auto version = alGetString(AL_VERSION);
	if(version != nullptr)
	{
		ss << "OpenAL version: " << version;
	}
	return ss.str();
}

static std::string al_vendor()
{
	std::stringstream ss;
	auto vendor = alGetString(AL_VERSION);
	if(vendor != nullptr)
	{
		ss << "OpenAL vendor: " << vendor;
	}
	return ss.str();
}

static auto al_extensions() -> std::string
{
	std::stringstream ss;
	auto extensions = alGetString(AL_EXTENSIONS);
	if(extensions != nullptr)
	{
		ss << "OpenAL extensions: " << extensions;
	}
	return ss.str();
}

static auto alc_extensions(ALCdevice* dev) -> std::string
{
	std::stringstream ss;
	auto extensions = alcGetString(dev, ALC_EXTENSIONS);
	if(extensions != nullptr)
	{
		ss << "OpenALC extensions: " << extensions;
	}

	return ss.str();
}
} // namespace openal

device_impl::device_impl(const std::string& id)
{
	if(has_context_thread())
	{
		throw audio::exception("OpenAL call. Device/context was already created.");
	}
	set_context_thread(std::this_thread::get_id());

	// select device
	device_.reset(alcOpenDevice(id.empty() ? nullptr : id.c_str()));

	device_id_ = id;

	if(device_ == nullptr)
	{
		error() << "Cant open audio playback device: " << device_id_;
		throw audio::exception("Cant open audio playback device: " + device_id_);
	}

	bool can_enumerate = openal::al_has_extension(device_.get(), "ALC_ENUMERATE_ALL_EXT");

	if(can_enumerate)
	{
		device_id_ = openal::al_get_string(device_.get(), ALC_ALL_DEVICES_SPECIFIER);
	}
	else
	{
		device_id_ = openal::al_get_string(device_.get(), ALC_DEVICE_SPECIFIER);
	}
	info() << "Selected audio playback device:";
	info() << "-- " << device_id_;

	// create context
	context_.reset(alcCreateContext(device_.get(), nullptr));

	if(context_ == nullptr)
	{
		error() << "Cant create audio context for playback device: " << device_id_;
		throw audio::exception("Cant create audio context for playback device: " + device_id_);
	}
	enable();

	version_ = openal::al_version();
	vendor_ = openal::al_vendor();
	extensions_ = openal::al_extensions();

	info() << "-- " << version_;
	info() << "-- " << vendor_;
	info() << "-- " << extensions_;
	info() << "-- " << openal::alc_extensions(device_.get());

	al_check(alDistanceModel(AL_LINEAR_DISTANCE));
}

device_impl::~device_impl() = default;

void device_impl::enable()
{
	al_check(alcMakeContextCurrent(context_.get()));
}

void device_impl::disable()
{
	al_check(alcMakeContextCurrent(nullptr));
}

auto device_impl::is_valid() const -> bool
{
	return (device_ != nullptr) && (context_ != nullptr);
}

auto device_impl::get_device_id() const -> const std::string&
{
	return device_id_;
}

auto device_impl::get_version() const -> const std::string&
{
	return version_;
}

const std::string& device_impl::get_vendor() const
{
	return vendor_;
}

auto device_impl::get_extensions() const -> const std::string&
{
	return extensions_;
}

auto device_impl::enumerate_capture_devices() -> std::vector<std::string>
{
	return openal::al_get_strings(nullptr, ALC_CAPTURE_DEVICE_SPECIFIER);
}

auto device_impl::default_capture_device() -> std::string
{
	return openal::al_get_string(nullptr, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
}

auto device_impl::enumerate_playback_devices() -> std::vector<std::string>
{
	bool can_enumerate = openal::al_has_extension(nullptr, "ALC_ENUMERATE_ALL_EXT");

	if(can_enumerate)
	{
		return openal::al_get_strings(nullptr, ALC_ALL_DEVICES_SPECIFIER);
	}

	return openal::al_get_strings(nullptr, ALC_DEVICE_SPECIFIER);
}

auto device_impl::default_playback_device() -> std::string
{
	bool can_enumerate = openal::al_has_extension(nullptr, "ALC_ENUMERATE_ALL_EXT");

	if(can_enumerate)
	{
		return openal::al_get_string(nullptr, ALC_DEFAULT_ALL_DEVICES_SPECIFIER);
	}

	return openal::al_get_string(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
}

void device_impl::deleter::operator()(ALCdevice* obj)
{
	if(obj != nullptr)
	{
		alcCloseDevice(obj);
	}
}

void device_impl::deleter::operator()(ALCcontext* obj)
{
	if(obj != nullptr)
	{
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(obj);
	}
}

} // namespace detail
} // namespace audio
