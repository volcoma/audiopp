#pragma once
#include <memory>
#include <string>
#include <vector>

namespace audio
{
namespace detail
{
class device_impl;
}
//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// Wrapper over the audio device and audio context. Can enumerate and query
/// device properties.
//-----------------------------------------------------------------------------
class device
{
public:
	device(int devnum = 0);
	~device();

	//-----------------------------------------------------------------------------
	/// Sets the current context.
	//-----------------------------------------------------------------------------
	void enable();

	//-----------------------------------------------------------------------------
	/// Un - sets the current context.
	//-----------------------------------------------------------------------------
	void disable();

	//-----------------------------------------------------------------------------
	/// Checks whether the device and context are valid.
	//-----------------------------------------------------------------------------
	auto is_valid() const -> bool;

	//-----------------------------------------------------------------------------
	/// Gets the id of the selected device.
	//-----------------------------------------------------------------------------
	auto get_device_id() const -> const std::string&;

	//-----------------------------------------------------------------------------
	/// Gets vendor.
	//-----------------------------------------------------------------------------
	auto get_vendor() const -> const std::string&;

	//-----------------------------------------------------------------------------
	/// Gets get_version.
	//-----------------------------------------------------------------------------
	auto get_version() const -> const std::string&;

	//-----------------------------------------------------------------------------
	/// Gets available extensions.
	//-----------------------------------------------------------------------------
	auto get_extensions() const -> const std::string&;

	//-----------------------------------------------------------------------------
	/// Enumerate all playback devices available on the system.
	//-----------------------------------------------------------------------------
	static auto enumerate_playback_devices() -> std::vector<std::string>;

	//-----------------------------------------------------------------------------
	/// Enumerate all capture devices available on the system.
	//-----------------------------------------------------------------------------
	static auto enumerate_capture_devices() -> std::vector<std::string>;

	//-----------------------------------------------------------------------------
	/// Enumerate the default playback device available on the system.
	//-----------------------------------------------------------------------------
	static auto enumerate_default_playback_device() -> std::string;

	//-----------------------------------------------------------------------------
	/// Enumerate the dafault capture devices available on the system.
	//-----------------------------------------------------------------------------
	static auto enumerate_default_capture_device() -> std::string;

private:
	/// pimpl idiom
	std::unique_ptr<detail::device_impl> impl_;
};
} // namespace audio
