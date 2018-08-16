#include "logger.h"

namespace audio
{
namespace detail
{
logger_t& get_info_logger()
{
	static logger_t logger;
	return logger;
}
logger_t& get_error_logger()
{
	static logger_t logger;
	return logger;
}
} // namespace detail

void log_info(const std::string& msg)
{
	auto logger = detail::get_info_logger();
	if(logger)
	{
		logger(msg);
	}
}

void set_info_logger(std::function<void(const std::string&)> logger)
{
	detail::get_info_logger() = std::move(logger);
}

void log_error(const std::string& msg)
{
	auto logger = detail::get_error_logger();
	if(logger)
	{
		logger(msg);
	}
}

void set_error_logger(std::function<void(const std::string&)> logger)
{
	detail::get_error_logger() = std::move(logger);
}
} // namespace audio
