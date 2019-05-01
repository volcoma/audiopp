#pragma once
#include <memory>
#include <string>
#include <vector>

#include "alc.h"
#include <map>

namespace audio
{
namespace detail
{
class device_impl
{
public:
	device_impl(const std::string& id = {});
	~device_impl();

	void enable();
	void disable();

	auto is_valid() const -> bool;

	auto get_device_id() const -> const std::string&;
	auto get_version() const -> const std::string&;
	auto get_vendor() const -> const std::string&;
	auto get_extensions() const -> const std::string&;

	static auto enumerate_playback_devices() -> std::vector<std::string>;
	static auto enumerate_capture_devices() -> std::vector<std::string>;
    static auto default_playback_device() -> std::string;
    static auto default_capture_device() -> std::string;


private:
	struct deleter
	{
		void operator()(ALCdevice* obj);
		void operator()(ALCcontext* obj);
	};

	std::unique_ptr<ALCdevice, deleter> device_;
	std::unique_ptr<ALCcontext, deleter> context_;

	std::string device_id_;

	std::string version_;
	std::string vendor_;
	std::string extensions_;
};
} // namespace detail
} // namespace audio
