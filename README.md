[![Build Status](https://travis-ci.org/volcoma/audiopp.svg)](https://travis-ci.org/volcoma/audiopp)
[![Build status](https://ci.appveyor.com/api/projects/status/ytbigal46vf5kr2t?svg=true)](https://ci.appveyor.com/project/volcoma/audiopp)

## audiopp c++14 audio library
- Supports .wav/.ogg formats
- Supports 3d sounds
- Basically a thin wrapper over OpenAL


```c++
int main()
{
    // Try to load the sound data
    std::string err;
    audio::sound_data data;
    std::string sample = "some_sample.wav"; // .ogg is supported as well
  
    if(!audio::load_from_file(sample, data, err))
    {
        audio::log_error("Failed to load sound data : " + err);
        return -1;
    }
    
    // initialize the audio device
    audio::device device;
    
    // create a listener. There can only be one listener
    // per application. 
    audio::listener listener;
    
    // 3d positioning only work with mono sounds
    // so if you want this functionality make sure you
    // load a mono sound
    listener.set_position({0, 0, 0});
    
    // create the sound from the sound data
    audio::sound sound(std::move(data));
    audio::source source;

    // bind the sound to a source
    source.bind(sound);
    source.play();
    while(source.is_playing())
    {
        std::this_thread::sleep_for(1667us);
    }
    
    return 0;
```
