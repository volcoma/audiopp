#pragma once
#include "types.h"
#include <memory>

namespace audio
{
namespace detail
{
class listener_impl;
}

//-----------------------------------------------------------------------------
/// Receiver of a sound. It can control 3d spatial and listener properties like
/// 3d position, 3d orientation, volume etc.
//-----------------------------------------------------------------------------
class listener
{
public:
	listener();
	~listener();

	//-----------------------------------------------------------------------------
	/// A value of 1.0 means unattenuated. Each division by 2 equals an attenuation
	/// of about -6dB. Each multiplicaton by 2 equals an amplification of about +6dB.
	/// A value of 0.0 is meaningless with respect to a logarithmic scale; it is
	/// silent.
	//-----------------------------------------------------------------------------
	void set_volume(float volume);

	//-----------------------------------------------------------------------------
	/// The listener location in three dimensional space.
	//-----------------------------------------------------------------------------
	void set_position(const float3& position);

	//-----------------------------------------------------------------------------
	/// Specifies the current velocity in local space.
	//-----------------------------------------------------------------------------
	void set_velocity(const float3& velocity);

	//-----------------------------------------------------------------------------
	/// Effectively two three dimensional vectors. The first vector is the front (or
	/// "at") and the second is the top (or "up").
	//-----------------------------------------------------------------------------
	void set_orientation(const float3& direction, const float3& up);

private:
	/// pimpl idiom
	std::unique_ptr<detail::listener_impl> impl_;
};
} // namespace audio
