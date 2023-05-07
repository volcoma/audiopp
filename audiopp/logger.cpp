#include "logger.h"

namespace audio
{
namespace detail
{
auto get_info_logger() -> log_func_t&
{
    static log_func_t logger;
    return logger;
}
auto get_error_logger() -> log_func_t&
{
    static log_func_t logger;
    return logger;
}
auto get_trace_logger() -> log_func_t&
{
    static log_func_t logger;
    return logger;
}
auto get_verbose_logger() -> log_func_t&
{
    static log_func_t logger;
    return logger;
}
} // namespace detail

void set_info_logger(const log_func_t& logger)
{
    detail::get_info_logger() = logger;
}

void set_error_logger(const log_func_t& logger)
{
    detail::get_error_logger() = logger;
}
void set_trace_logger(const log_func_t& logger)
{
    detail::get_trace_logger() = logger;
}

void set_verbose_logger(const log_func_t& logger)
{
    detail::get_verbose_logger() = logger;
}

logger::logger(const log_func_t& func)
    : func_(func)
{
}

logger::~logger()
{
    if(func_)
    {
        auto msg = str_.str();
        if(!msg.empty())
        {
            func_(msg);
        }
    }
}

auto info() -> logger
{
    return {detail::get_info_logger()};
}
auto error() -> logger
{
    return {detail::get_error_logger()};
}
auto trace() -> logger
{
    return {detail::get_trace_logger()};
}
auto verbose() -> logger
{
    return {detail::get_verbose_logger()};
}

} // namespace audio
