#include "check.h"
#include "../exception.h"
#include "../logger.h"
#include <string>

#include <al.h>

namespace audio
{
namespace detail
{

////////////////////////////////////////////////////////////
void al_check_error(const char* file, unsigned int line, const char* expression)
{
    // Get the last error
    ALenum err = alGetError();

    if(err != AL_NO_ERROR)
    {
        std::string file_string = file;
        std::string error = "Unknown error";
        std::string description = "No description";

        // Decode the error code
        switch(err)
        {
            case AL_INVALID_NAME:
            {
                error = "AL_INVALID_NAME";
                description = "A bad name (ID) has been specified.";
                break;
            }

            case AL_INVALID_ENUM:
            {
                error = "AL_INVALID_ENUM";
                description = "An unacceptable value has been specified for an enumerated argument.";
                break;
            }

            case AL_INVALID_VALUE:
            {
                error = "AL_INVALID_VALUE";
                description = "A numeric argument is out of range.";
                break;
            }

            case AL_INVALID_OPERATION:
            {
                error = "AL_INVALID_OPERATION";
                description = "The specified operation is not allowed in the current state.";
                break;
            }

            case AL_OUT_OF_MEMORY:
            {
                error = "AL_OUT_OF_MEMORY";
                description = "There is not enough memory left to execute the command.";
                break;
            }

            default:
            {
                error = "UNKNOWN_ERROR : " + std::to_string(err);
                description = "Unknown error.";
                break;
            }
        }

        // Log the error
        audio::error() << "An internal OpenAL call failed in "
                       << file_string.substr(file_string.find_last_of("\\/") + 1) << "("
                       << std::to_string(line) << ").\nExpression:\n   " << expression
                       << "\nError description:\n   " << error << "\n   " << description;
    }
}

std::thread::id& get_context_thread_id() noexcept
{
    static std::thread::id id{};
    return id;
}

auto has_context_thread() noexcept -> bool
{
    return std::thread::id{} != get_context_thread_id();
}

void al_check_preconditions()
{
    if(!has_context_thread())
    {
        throw audio::exception("OpenAL call. No device/context was created.");
    }

    if(std::this_thread::get_id() != get_context_thread_id())
    {
        throw audio::exception(
            "OpenAL call from a different thread than the one the OpenAL context was created in.");
    }
}

void set_context_thread(const std::thread::id& id) noexcept
{
    get_context_thread_id() = id;
}

} // namespace detail

} // namespace audio
