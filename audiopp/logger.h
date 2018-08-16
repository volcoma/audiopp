#pragma once
#include <functional>
#include <string>
#include <vector>

namespace audio
{
using logger_t = std::function<void(const std::string&)>;

//-----------------------------------------------------------------------------
/// Sets and extern info logging function for the library to use.
//-----------------------------------------------------------------------------
void set_info_logger(logger_t logger);

//-----------------------------------------------------------------------------
/// Sets and extern error logging function for the library to use.
//-----------------------------------------------------------------------------
void set_error_logger(logger_t logger);

//-----------------------------------------------------------------------------
/// Used internally by the library to log informational messages.
//-----------------------------------------------------------------------------
void log_info(const std::string& msg);

//-----------------------------------------------------------------------------
/// Used internally by the library to log error messages.
//-----------------------------------------------------------------------------
void log_error(const std::string& msg);
} // namespace audio
