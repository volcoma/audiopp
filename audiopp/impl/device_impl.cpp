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
	return (alcIsExtensionPresent(dev, ext) == AL_TRUE); // ALC_TRUE
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

device_impl::device_impl(int devnum)
{
	if(has_context_thread())
	{
		throw audio::exception("OpenAL call. Device/context was already created.");
	}
	set_context_thread(std::this_thread::get_id());

	// device name
	auto playback_devices = enumerate_playback_devices();
	log_info("Supported audio playback devices:");
	for(const auto& dev : playback_devices)
	{
		log_info("-- " + dev);
	}
	auto default_playback = enumerate_default_playback_device();
	log_info("Default audio playback device:");
	log_info("-- " + default_playback);

	auto capture_devices = enumerate_capture_devices();
	log_info("Supported audio capture devices:");
	for(const auto& dev : capture_devices)
	{
		log_info("-- " + dev);
	}

	auto default_capture = enumerate_default_capture_device();
	log_info("Default audio capture device:");
	log_info("-- " + default_capture);

	if(devnum >= 0 && devnum < int(playback_devices.size()))
	{
		device_id_ = playback_devices[std::size_t(devnum)];
	}

	// select device
	device_.reset(alcOpenDevice(device_id_.empty() ? nullptr : device_id_.c_str()));

	if(device_ == nullptr)
	{
		log_error("Cant open audio playback device: " + device_id_);
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
	log_info("Selected audio playback device:");
	log_info("-- " + device_id_);

	// create context
	context_.reset(alcCreateContext(device_.get(), nullptr));

	if(context_ == nullptr)
	{
		log_error("Cant create audio context for playback device: " + device_id_);
		throw audio::exception("Cant create audio context for playback device: " + device_id_);
	}
	enable();

	version_ = openal::al_version();
	vendor_ = openal::al_vendor();
	extensions_ = openal::al_extensions();

	log_info(version_);
	log_info(vendor_);
	log_info(extensions_);
	log_info(openal::alc_extensions(device_.get()));

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

auto device_impl::enumerate_default_capture_device() -> std::string
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

auto device_impl::enumerate_default_playback_device() -> std::string
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
