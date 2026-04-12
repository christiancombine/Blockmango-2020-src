#include "SoundEngine.h"

namespace LORD
{

    void SoundEngine::preloadBackgroundMusic(String strFilePath) {}

    void SoundEngine::playBackgroundMusic(String strFilePath, bool bLoop, float fMinInterval, float fMaxInterval, unsigned int nParam) {}

    void SoundEngine::stopBackgroundMusic(bool bReleaseData, unsigned int nParam) {}

    void SoundEngine::pauseBackgroundMusic() {}

    void SoundEngine::resumeBackgroundMusic() {}

    void SoundEngine::rewindBackgroundMusic() {}

    bool SoundEngine::willPlayBackgroundMusic() { return false; }

    bool SoundEngine::isBackgroundMusicPlaying() { return false; }

    float SoundEngine::getBackgroundMusicVolume() { return 0.0f; }

    void SoundEngine::setBackgroundMusicVolume(float volume) {}

    float SoundEngine::getEffectsVolume() { return 0.0f; }

    void SoundEngine::setEffectsVolume(float volume) {}

    unsigned int SoundEngine::playEffect(String strFilePath, bool bLoop, float fMinInterval, float fMaxInterval, float fVolume,
        const LORD::Vector3& pos, unsigned int nParam)
    {
        return 0;
    }

    void SoundEngine::pauseEffect(unsigned int nSoundId) {}

    void SoundEngine::pauseAllEffects() {}

    void SoundEngine::resumeEffect(unsigned int nSoundId) {}

    void SoundEngine::resumeAllEffects() {}

    void SoundEngine::stopEffect(unsigned int nSoundId) {}

    void SoundEngine::stopAllEffects() {}

    void SoundEngine::preloadEffect(String strFilePath) {}

    void SoundEngine::unloadEffect(String strFilePath) {}

    void SoundEngine::updateListenerPos(const LORD::Vector3& pos, const LORD::Vector3& face) {}

    void SoundEngine::updateEngine() {}

    void SoundEngine::updateSoundPos(unsigned int nSoundId, const LORD::Vector3& pos) {}

    void SoundEngine::release() {}

    SoundEngine::SoundEngine() {}

    SoundEngine::~SoundEngine() {}

    void SoundEngine::end() {}

} // namespace LORD
