[![Build Status](https://travis-ci.org/volcoma/audiopp.svg?branch=master)](https://travis-ci.org/volcoma/audiopp)
[![Build status](https://ci.appveyor.com/api/projects/status/ytbigal46vf5kr2t?svg=true)](https://ci.appveyor.com/project/volcoma/audiopp)

## audiopp c++14 cross-platform audio library
- Supports loading of .wav/.ogg/.mp3/.flac formats
- Supports 3d sounds
- Basically a thin wrapper over OpenAL


```c++
int main()
{
    // Load the sound data. This can also be done on seperate threads.
    std::string err;
    audio::sound_data data;
    if(!audio::load_from_file("some_sample.wav", data, err))
    {
        audio::error() << "Failed to load sound data : " << err;
        return -1;
    }
    
    // initialize the audio device
    audio::device device;
    
    // create a listener. There can only be one listener
    // per application. 
    audio::listener listener;
    
    // adjust listener properties (optional)
    listener.set_volume(0.8f);
    // 3d positioning only work with mono sounds
    // so if you want this functionality make sure you
    // play a mono sound
    listener.set_position({0, 0, 0});
    
    // create the sound from the sound data
    audio::sound sound(std::move(data));

    // create a source
    audio::source source;
    
    // adjust source properties (optional)
    source.set_volume(0.8f);
    source.set_pitch(1.0f);
    // 3d positioning only work with mono sounds
    // so if you want this functionality make sure you
    // play a mono sound
    source.set_position({0, 0, 0});

    // bind the sound to the source
    source.bind(sound);
    // play it
    source.play();
    while(source.is_playing())
    {
        std::this_thread::sleep_for(1667us);
    }
    
    return 0;
}
```
