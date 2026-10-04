
#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>

#include "sound_system.h"
#include "storage.h"
#include "constants.h"
#include "rng.h"

using Sound = SoundSystem::Sound;

// Main theme, played when the game opens (file name without extension, inside the soundtrack folder)
constexpr const char *THEME_SONG_NAME = "C418 - 0x10c";

std::vector<ActiveSound *> SoundSystem::active_sounds_;
std::unordered_map<Sound, Soundlib::Sound> SoundSystem::sound_map_;
std::vector<std::unique_ptr<Soundlib::Sound>> SoundSystem::soundtrack_;
int SoundSystem::theme_song_index_ = -1;
float SoundSystem::sfx_volume_;
float SoundSystem::music_volume_;

void SoundSystem::Init()
{
    Soundlib::Init();
    Soundlib::SetAttenuationModel(Soundlib::AttenuationModel::INVERSE_DISTANCE);

    LoadSoundtrack();
    sound_map_[Sound::ALIEN_JUMP].LoadSound((Storage::SOUNDS / "alienjump.wav").string());
    sound_map_[Sound::BLOCK_BREAK].LoadSound((Storage::SOUNDS / "blockbreak.wav").string());
    sound_map_[Sound::BLOCK_PLACE].LoadSound((Storage::SOUNDS / "blockplace.wav").string());
    sound_map_[Sound::CRAFT].LoadSound((Storage::SOUNDS / "craft.wav").string());
    sound_map_[Sound::DING].LoadSound((Storage::SOUNDS / "ding.wav").string());
    sound_map_[Sound::DRILL].LoadSound((Storage::SOUNDS / "drill.wav").string());
    sound_map_[Sound::DRILL2].LoadSound((Storage::SOUNDS / "drill2.wav").string());
    sound_map_[Sound::DRILL3].LoadSound((Storage::SOUNDS / "drill3.wav").string());
    sound_map_[Sound::FRIENDLY_SUMMON].LoadSound((Storage::SOUNDS / "friendlysummon.wav").string());
    sound_map_[Sound::HURT].LoadSound((Storage::SOUNDS / "hurt.wav").string());
    sound_map_[Sound::JETPACK].LoadSound((Storage::SOUNDS / "jetpack.wav").string());
    sound_map_[Sound::JUMP].LoadSound((Storage::SOUNDS / "jump.wav").string());
    sound_map_[Sound::LAND].LoadSound((Storage::SOUNDS / "land.wav").string());
    sound_map_[Sound::LASER].LoadSound((Storage::SOUNDS / "lasergun.wav").string());
    sound_map_[Sound::MEDKIT].LoadSound((Storage::SOUNDS / "medkit.wav").string());
    sound_map_[Sound::PICKUP].LoadSound((Storage::SOUNDS / "pickup.wav").string());
    sound_map_[Sound::LOW_BEEP].LoadSound((Storage::SOUNDS / "lowbeep.wav").string());
    sound_map_[Sound::REWARD].LoadSound((Storage::SOUNDS / "reward.wav").string());
    sound_map_[Sound::TELEPORT].LoadSound((Storage::SOUNDS / "teleport.wav").string());

    active_sounds_.reserve(ACTIVE_SOUND_LIMIT);
}

void SoundSystem::Exit()
{
    for (auto active_sound : active_sounds_)
    {
        active_sound->source->Stop();
        delete active_sound;
    }
    soundtrack_.clear();

    Soundlib::Exit();
}

void SoundSystem::Update(Options options)
{
    sfx_volume_ = options.sfx_volume;
    music_volume_ = options.music_volume;

    for (auto it = active_sounds_.begin(); it != active_sounds_.end(); )
    {
        auto active_sound = *it;

        if (active_sound->source->GetState() == Soundlib::SourceState::STOPPED) // Remove finished sounds
        {
            delete active_sound;
            it = active_sounds_.erase(it);
        }
        else
        {
            // Update volumes
            if (IsMusic(active_sound->sound_id))
                active_sound->source->SetGain(music_volume_);
            else
                active_sound->source->SetGain(sfx_volume_);

            // Update positions of global sounds
            if (active_sound->is_global)
                active_sound->source->SetPosition(Soundlib::GetListenerPosition());

            ++it;
        }
    }
}

// Play global sound
ActiveSound *SoundSystem::Play(Sound sound, bool loop)
{
    if (active_sounds_.size() == ACTIVE_SOUND_LIMIT)
        return nullptr;

    auto source = new Soundlib::SoundSource(sound_map_[sound]);
    source->SetPosition(Soundlib::GetListenerPosition());
    source->SetRolloffFactor(0);
    source->SetLooping(loop);
    source->SetGain(IsMusic(sound) ? music_volume_ : sfx_volume_);
    source->Play();

    auto active_sound = new ActiveSound{source, sound, true};
    active_sounds_.push_back(active_sound);
    return active_sound;
}

// Play positioned sound
ActiveSound *SoundSystem::PlayAt(Sound sound, glm::vec3 position, bool loop)
{
    if (active_sounds_.size() == ACTIVE_SOUND_LIMIT)
        return nullptr;

    auto source = new Soundlib::SoundSource(sound_map_[sound]);
    source->SetReferenceDistance(1.0f);
    source->SetRolloffFactor(0.8f);
    source->SetMaxDistance(1.5f * CHUNK_SIZE);
    source->SetPosition({position.x, position.y, position.z});
    source->SetLooping(loop);
    source->SetGain(IsMusic(sound) ? music_volume_ : sfx_volume_);
    source->Play();
    
    auto active_sound = new ActiveSound{source, sound, false};
    active_sounds_.push_back(active_sound);
    return active_sound;
}

void SoundSystem::Stop(ActiveSound *active_sound)
{
    active_sound->source->Stop();
}

// Loads every song in the soundtrack folder, so new tracks can be dropped in without code changes
void SoundSystem::LoadSoundtrack()
{
    std::filesystem::path soundtrack_dir = Storage::SOUNDS / "soundtrack";
    if (!std::filesystem::is_directory(soundtrack_dir))
    {
        std::cerr << "Soundtrack folder not found: " << soundtrack_dir.string() << std::endl;
        return;
    }

    std::vector<std::filesystem::path> song_paths;
    for (const auto &entry : std::filesystem::directory_iterator(soundtrack_dir))
    {
        std::string extension = entry.path().extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        if (entry.is_regular_file() && (extension == ".ogg" || extension == ".mp3" || extension == ".wav" || extension == ".flac"))
            song_paths.push_back(entry.path());
    }
    std::sort(song_paths.begin(), song_paths.end());

    for (const auto &song_path : song_paths)
    {
        auto song = std::make_unique<Soundlib::Sound>();
        song->LoadSound(song_path.string());
        if (song->GetError() != Soundlib::Error::NONE)
        {
            std::cerr << "Failed to load song: " << song_path.string() << std::endl;
            continue;
        }

        if (song_path.stem() == THEME_SONG_NAME)
            theme_song_index_ = (int)soundtrack_.size();
        soundtrack_.push_back(std::move(song));
    }
}

ActiveSound *SoundSystem::PlayMusic(const Soundlib::Sound &song, bool loop)
{
    if (active_sounds_.size() == ACTIVE_SOUND_LIMIT)
        return nullptr;

    auto source = new Soundlib::SoundSource(song);
    source->SetPosition(Soundlib::GetListenerPosition());
    source->SetRolloffFactor(0);
    source->SetLooping(loop);
    source->SetGain(music_volume_);
    source->Play();

    auto active_sound = new ActiveSound{source, Sound::MUSIC, true};
    active_sounds_.push_back(active_sound);
    return active_sound;
}

// Loops until StopMusic is called (e.g. when entering a moon)
void SoundSystem::PlayThemeSong()
{
    if (theme_song_index_ >= 0)
        PlayMusic(*soundtrack_[theme_song_index_], true);
}

// Any song in the soundtrack can be chosen, including the theme
void SoundSystem::PlayRandomSong()
{
    if (!soundtrack_.empty())
        PlayMusic(*soundtrack_[RNG{}.Range<size_t>(0, soundtrack_.size() - 1)]);
}

// Stops every playing song (finished sources are cleaned up by Update)
void SoundSystem::StopMusic()
{
    for (auto active_sound : active_sounds_)
        if (IsMusic(active_sound->sound_id))
            active_sound->source->Stop();
}

void SoundSystem::SetPlayerPosition(glm::vec3 position)
{
    Soundlib::SetListenerPosition({position.x, position.y, position.z});
}

void SoundSystem::SetPlayerOrientation(glm::vec3 forward, glm::vec3 up)
{
    Soundlib::SetListenerOrientation(
        {forward.x, forward.y, forward.z},
        {up.x, up.y, up.z}
    );
}

bool SoundSystem::IsMusic(Sound sound)
{
    return sound == Sound::MUSIC;
}

void SoundSystem::PauseSFX()
{
    for (auto active_sound : active_sounds_)
        if (!IsMusic(active_sound->sound_id))
            active_sound->source->Pause();
}

void SoundSystem::ResumeSFX()
{
    for (auto active_sound : active_sounds_)
        if (!IsMusic(active_sound->sound_id) && active_sound->source->GetState() == Soundlib::SourceState::PAUSED)
            active_sound->source->Play();
}
