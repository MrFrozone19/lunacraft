#pragma once

#include <vector>
#include <tuple>
#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>
#include <soundlib/soundlib.h>

#include "options.h"

#define ACTIVE_SOUND_LIMIT 128

struct ActiveSound;

class SoundSystem
{
    public:
        enum class Sound
        {
            MUSIC, // Any song from the soundtrack folder
            ALIEN_JUMP,
            BLOCK_BREAK,
            BLOCK_PLACE,
            CRAFT,
            DING,
            DRILL,
            DRILL2,
            DRILL3,
            FRIENDLY_SUMMON,
            HURT,
            JETPACK,
            JUMP,
            LAND,
            LASER,
            MEDKIT,
            PICKUP,
            LOW_BEEP,
            REWARD,
            TELEPORT
        };
        
    public:
        SoundSystem() = delete;
        static void Init();
        static void Exit();
        static void Update(Options options);
        static ActiveSound *Play(Sound sound, bool loop = false);
        static ActiveSound *PlayAt(Sound sound, glm::vec3 position, bool loop = false);
        static void Stop(ActiveSound *active_sound);
        static void PlayThemeSong();
        static void PlayRandomSong();
        static void StopMusic();
        static void SetPlayerPosition(glm::vec3 position);
        static void SetPlayerOrientation(glm::vec3 forward, glm::vec3 up);
        static void PauseSFX();
        static void ResumeSFX();

    private:
        static bool IsMusic(Sound sound);
        static void LoadSoundtrack();
        static ActiveSound *PlayMusic(const Soundlib::Sound &song, bool loop = false);

    private:
        static std::vector<ActiveSound *> active_sounds_;
        static std::unordered_map<Sound, Soundlib::Sound> sound_map_;
        static std::vector<std::unique_ptr<Soundlib::Sound>> soundtrack_;
        static int theme_song_index_;
        static float sfx_volume_;
        static float music_volume_;
};

struct ActiveSound
{
    friend class SoundSystem;

    public:
        ActiveSound(Soundlib::SoundSource* source, SoundSystem::Sound sound, bool is_global)
        : source(source),
          sound_id(sound),
          is_global(is_global) {}

        ~ActiveSound() { delete source; }
    
    private:
        Soundlib::SoundSource *source;
        SoundSystem::Sound sound_id;
        bool is_global;
};
