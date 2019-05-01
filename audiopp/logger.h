#pragma once
#include <functional>
#include <string>
#include <vector>
#include <sstream>

namespace audio
{
using log_func_t = std::function<void(const std::string&)>;

//-----------------------------------------------------------------------------
/// Sets and extern info logging function for the library to use.
//-----------------------------------------------------------------------------
void set_info_logger(const log_func_t& logger);

//-----------------------------------------------------------------------------
/// Sets and extern error logging function for the library to use.
//-----------------------------------------------------------------------------
void set_error_logger(const log_func_t& logger);

class logger
{
public:
    logger(const log_func_t& func);
	~logger();
	template <typename T>
	logger& operator<<(T&& val)
	{
        if(func_)
        {
            str_ << val;
        }
		return *this;
	}

private:

    const log_func_t& func_;
	std::stringstream str_;
};

auto info() -> logger;
auto error() -> logger;
auto trace() -> logger;

} // namespace audio
