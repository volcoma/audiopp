#pragma once

#include "effect.h"
#include <memory>

namespace audio
{

namespace detail
{
class builtin_effect_impl;
}

template<typename T>
struct effect_param
{
    explicit effect_param(const T& v, const T& min, const T& max)
    {
        min_ = min;
        max_ = max;
        set_value(v);
    }

    effect_param& operator=(const T& v)
    {
        set_value(v);
        return *this;
    }

    operator T() const
    {
        return value_;
    }

    const T& get() const
    {
        return value_;
    }

    T& get()
    {
        return value_;
    }

    const T& min()
    {
        return min_;
    }

    const T& max()
    {
        return max_;
    }
private:
    void set_value(const T& v)
    {
        value_ = v;
        value_ = std::min<T>(value_, max_);
        value_ = std::max<T>(value_, min_);
    }
    T min_{};
    T max_{};
    T value_{};
};

//Reference : https://usermanual.wiki/Pdf/Effects20Extension20Guide.90272296/view
class builtin_effect : public effect
{
public:
    using ptr = std::shared_ptr<builtin_effect>;

    using param = effect_param<float>;
    using param_hz = effect_param<float>;
    using param_sec = effect_param<float>;

    using param_degrees = effect_param<int>;
    using param_phoneme_type = effect_param<int>;
    using param_semitones = effect_param<int>;
    using param_cents = effect_param<int>;

    using param_vec3 = effect_param<std::array<float, 3>>;
    using param_state = bool;

    struct eaxreverb
    {
        param density{1.0f, 0.0f, 1.0f};

        param diffusion{1.0f, 0.0f, 1.0f};

        param gain{0.32f, 0.0f, 1.0f};
        param gainhf{0.89f, 0.0f, 1.0f};
        param gainlf{0.0f, 0.0f, 1.0f};

        param_sec decay_time{1.49f, 0.1f, 20.0f};
        param decay_hfratio{0.83f, 0.1f, 2.0f};
        param decay_lfratio{1.0f, 0.1f, 2.0f};

        param reflections_gain{0.05f, 0.0f, 3.16f};
        param_sec reflections_delay{0.007f, 0.0f, 0.3f};
        param_vec3 reflections_pan{{0.0f, 0.0f, 0.0f}, {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

        param late_reverb_gain{1.26f, 0.0f, 10.0f};
        param_sec late_reverb_delay{0.011f, 0.0f, 0.1f};
        param_vec3 late_reverb_pan{{0.0f, 0.0f, 0.0f}, {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

        param echo_time{0.25f, 0.075f, 0.25f};
        param echo_depth{0.0f, 0.0f, 1.0f};

        param_sec modulation_time{0.25f, 0.004f, 4.0f};
        param modulation_depth{0.0f, 0.0f, 1.0f};

        param air_absorption_gainhf{0.994f, 0.892f, 1.0f};

        param_hz hfreference{5000.0f, 1000.0f, 20000.0f};
        param_hz lfreference{250.0f, 20.0f, 1000.0f};

        param room_rolloff_factor{0.0f, 0.0f, 1.0f};

        param_state decay_hflimit{true};
    };

    struct reverb
    {
        param density{1.0f, 0.0f, 1.0f};

        param diffusion{1.0f, 0.0f, 1.0f};

        param gain{0.32f, 0.0f, 1.0f};
        param gainhf{0.89f, 0.0f, 1.0f};

        param_sec decay_time{1.49f, 0.1f, 20.0f};
        param decay_hfratio{0.83f, 0.1f, 2.0f};

        param reflections_gain{0.05f, 0.0f, 3.16f};
        param_sec reflections_delay{0.007f, 0.0f, 0.3f};

        param late_reverb_gain{1.26f, 0.0f, 10.0f};
        param_sec late_reverb_delay{0.011f, 0.0f, 0.1f};

        param air_absorption_gainhf{0.994f, 0.892f, 1.0f};

        param room_rolloff_factor{0.0f, 0.0f, 10.0f};

        param_state decay_hflimit{true};
    };

    struct chorus
    {
        enum class waveform_t { sinusoid, triangle};
        waveform_t waveform{waveform_t::triangle};
        param_degrees phase{90, -180, 180};
        param_hz rate{1.1f, 0.0f, 10.0f};
        param depth{0.1f, 0.0f, 1.0f};
        param feedback{0.25f, -1.0f, 1.0f};
        param_sec delay{0.016f, 0.0f, 0.016f};
    };

    struct distortion
    {
        param edge{0.2f, 0.0f, 1.0f};
        param gain{0.05f, 0.01f, 1.0f};
        param_hz lowpass_cutoff{8000.0f, 80.0f, 24000.0f};
        param_hz eqcenter{3600.0f, 80.0f, 24000.0f};
        param_hz eqbandwidth{3600.0f, 80.0f, 24000.0f};
    };

    struct echo
    {
        param_sec delay{0.1f, 0.0f, 0.207f};
        param_sec lrdelay{0.1f, 0.0f, 0.404f};
        param damping{0.5f, 0.0f, 0.99f};
        param feedback{0.5f, 0.0f, 1.0f};
        param spread{-1.0f, -1.0f, 1.0f};
    };

    struct flanger
    {
        enum class waveform_t { sinusoid, triangle };
        waveform_t waveform{waveform_t::triangle};
        param_degrees phase{90, -180, 180};
        param rate{0.27f, 0.0f, 10.0f};
        param depth{1.0f, 0.0f, 1.0f};
        param feedback{-0.5f, -1.0f, 1.0f};
        param_sec delay{0.002f, 0.0f, 0.004f};
    };

    struct frequency_shifter
    {
        enum class direction_t { down, up, off };
        param_hz frequency{0.0f, 0.0f, 24000.0f};
        direction_t left_direction{direction_t::down};
        direction_t right_direction{direction_t::down};
    };

    struct vocal_morpher
    {
        enum class waveform_t { sinusoid, triangle, saw };
        param_phoneme_type phonemea{0, 0, 29};
        param_phoneme_type phonemeb{10, 0, 29};
        param_semitones phonemea_coarse_tuning{0, -24, 24};
        param_semitones phonemeb_coarse_tuning{0, -24, 24};
        waveform_t waveform{waveform_t::sinusoid};
        param_hz rate{1.41f, 0.0f, 10.0f};
    };

    struct pitch_shifter
    {
        param_semitones coarse_tune{12, -12, 12};
        param_cents fine_tune{0, -50, 50};
    };

    struct ring_modulator
    {
        enum class waveform_t { sinusoid, saw, square };
        param_hz frequency{440.0f, 0.0f, 8000.0f};
        param_hz highpass_cutoff{800.0f, 0.0f, 24000.0f};
        waveform_t waveform{waveform_t::sinusoid};
    };

    struct autowah
    {
        param_sec attack_time{0.0001f, 0.06f, 1.0f};
        param_sec release_time{0.0001f, 0.06f, 1.0f};
        param resonance{1000.0f, 2.0f, 1000.0f};
        param peak_gain{11.22f, 0.00003f, 31621.0f};
    };

    struct compressor
    {
        enum class state_t { on, off };
        state_t onoff{state_t::off};
    };

    struct equalizer
    {
        param low_gain{1.0f, 0.126f, 7.943f};
        param_hz low_cutoff{200.0f, 50.0f, 800.0f};
        param mid1_gain{1.0f, 0.126f, 7.943f};
        param_hz mid1_center{500.0f, 200.0f, 3000.0f};
        param mid1_width{1.0f, 0.01f, 1.0f};
        param mid2_gain{1.0f, 0.126f, 7.943f};
        param mid2_center{3000.0f, 1000.0f, 8000.0f};
        param mid2_width{1.0f, 0.01f, 1.0f};
        param high_gain{1.0f, 0.126f, 7.943f};
        param_hz high_cutoff{6000.0f, 4000.0f, 16000.0f};
    };

    builtin_effect(const builtin_effect&) = delete;
    builtin_effect& operator=(const builtin_effect&) = delete;
    builtin_effect(builtin_effect&&) noexcept;
    builtin_effect& operator=(builtin_effect&&) noexcept;
    ~builtin_effect() override;

    //-----------------------------------------------------------------------------
    /// Creates builtin effect.
    //-----------------------------------------------------------------------------
    static ptr create(const eaxreverb& param);
    static ptr create(const reverb& param);
    static ptr create(const chorus& param);
    static ptr create(const distortion& param);
    static ptr create(const echo& param);
    static ptr create(const flanger& param);
    static ptr create(const frequency_shifter& param);
    static ptr create(const vocal_morpher& param);
    static ptr create(const pitch_shifter& param);
    static ptr create(const ring_modulator& param);
    static ptr create(const autowah& param);
    static ptr create(const compressor& param);
    static ptr create(const equalizer& param);

    void set_param(const eaxreverb& param);
    void set_param(const reverb& param);
    void set_param(const chorus& param);
    void set_param(const distortion& param);
    void set_param(const echo& param);
    void set_param(const flanger& param);
    void set_param(const frequency_shifter& param);
    void set_param(const vocal_morpher& param);
    void set_param(const pitch_shifter& param);
    void set_param(const ring_modulator& param);
    void set_param(const autowah& param);
    void set_param(const compressor& param);
    void set_param(const equalizer& param);

    //-----------------------------------------------------------------------------
    /// Checks whether the effect is valid, i.e. usable.
    //-----------------------------------------------------------------------------
    auto is_valid() const noexcept -> bool;

    //-----------------------------------------------------------------------------
    /// Appplies the configured type & (optional) parameters.
    /// If parameters are set after that, they will not be accounted
    /// until apply is called again.
    //-----------------------------------------------------------------------------
    auto apply() noexcept -> bool;

protected:
    auto bind_impl(source& src) noexcept -> bool override;
    void unbind_impl(source& src) noexcept override;
    void unbind_all_impl() noexcept override;

    std::unique_ptr<detail::builtin_effect_impl> impl_;

    builtin_effect();
};
} //end of namespace audio
