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
	bool is_valid() const;

	//-----------------------------------------------------------------------------
	/// Gets the id of the selected device.
	//-----------------------------------------------------------------------------
	const std::string& get_device_id() const;

	//-----------------------------------------------------------------------------
	/// Gets vendor.
	//-----------------------------------------------------------------------------
	const std::string& get_vendor() const;

	//-----------------------------------------------------------------------------
	/// Gets get_version.
	//-----------------------------------------------------------------------------
	const std::string& get_version() const;

	//-----------------------------------------------------------------------------
	/// Gets available extensions.
	//-----------------------------------------------------------------------------
	const std::string& get_extensions() const;

	//-----------------------------------------------------------------------------
	/// Enumerate all playback devices available on the system.
	//-----------------------------------------------------------------------------
	static std::vector<std::string> enumerate_playback_devices();

	//-----------------------------------------------------------------------------
	/// Enumerate all capture devices available on the system.
	//-----------------------------------------------------------------------------
	static std::vector<std::string> enumerate_capture_devices();

private:
	/// pimpl idiom
	std::unique_ptr<detail::device_impl> impl_;
};
} // namespace audio