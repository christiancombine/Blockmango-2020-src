#include "Audio/FModAudioEngine.h"
#include "Util/MacroUtil.h"
#include "Util/PlayerPrefs.h"
#include "Audio/SoundSystem.h"

namespace LORD
{

	FModAudioEngine::FModAudioEngine()
	{

	}

	FModAudioEngine::~FModAudioEngine()
	{

	}

	void FModAudioEngine::preloadBackgroundMusic(String strFilePath)
	{
		preloadSound(strFilePath.c_str());
	}

	void FModAudioEngine::playBackgroundMusic(String strFilePath, bool bLoop, float fMinInterval, float fMaxInterval, unsigned int nParam/* = 0*/)
	{
		stopBackgroundMusic(false, nParam);
		m_curMusicChannerId = play2dSound(strFilePath.c_str(), m_curMusicSoundId, m_effectVolume, bLoop, nParam);
		if (!SoundSystem::Instance()->isSwitchOnBackgroundMusic())
			pauseBackgroundMusic();
	}

	void FModAudioEngine::stopBackgroundMusic(bool bReleaseData, unsigned int nParam/* = 0*/)
	{
		if (nParam != 0)
		{
			//unloadSound(m_curMusicSoundId);
			unloadChannel(nParam);
		}
		else
		{
			if (BLOCKMAN::PlayerPrefs::Instance()->getBoolPrefs("StopAndUnloadSound"))
			{
				unloadSound(m_curMusicSoundId);
			}
			unloadChannel(m_curMusicChannerId);
		}
	}

	void FModAudioEngine::pauseBackgroundMusic()
	{
		pauseSound(m_curMusicChannerId);
	}

	void FModAudioEngine::resumeBackgroundMusic()
	{
		resumeSound(m_curMusicChannerId);
	}

	void FModAudioEngine::rewindBackgroundMusic()
	{

	}

	bool FModAudioEngine::willPlayBackgroundMusic()
	{
		return false;
	}

	bool FModAudioEngine::isBackgroundMusicPlaying()
	{
		return isPlaying(m_curMusicChannerId);
	}

	float FModAudioEngine::getBackgroundMusicVolume()
	{
		return getSoundVolume(m_curMusicChannerId);
	}

	void FModAudioEngine::setBackgroundMusicVolume(float volume)
	{
		setSoundVolume(m_curMusicChannerId, volume);
	}

	float FModAudioEngine::getEffectsVolume()
	{
		return m_effectVolume;
	}

	void FModAudioEngine::setEffectsVolume(float volume)
	{
		m_effectVolume = volume;
		for (auto chan : m_channels)
		{
			if (chan.first != m_curMusicChannerId)
			{
				chan.second->setVolume(5*volume);
			}
		}
	}

	unsigned int FModAudioEngine::playEffect(String strFilePath, bool bLoop, float fMinInterval, float fMaxInterval, float fVolume, const LORD::Vector3 & pos, unsigned int nParam/* = 0*/)
	{
		unsigned int soundId = 0;
		if (!BLOCKMAN::PlayerPrefs::Instance()->getBoolPrefs("SoundEffectSwitch"))
		{
			return soundId;
		}
		if (pos.isInvalid())
			return play2dSound(strFilePath.c_str(), soundId, m_effectVolume * fVolume, bLoop, nParam);
		else
			return play3dSound(strFilePath.c_str(), soundId, pos, m_effectVolume * fVolume, bLoop);
	}

	void FModAudioEngine::pauseEffect(unsigned int nSoundId)
	{
		pauseSound(nSoundId);
	}

	void FModAudioEngine::pauseAllEffects()
	{
		for (auto chan : m_channels)
		{

			chan.second->setPaused(true);

		}
	}

	void FModAudioEngine::resumeEffect(unsigned int nSoundId)
	{
		resumeSound(nSoundId);
	}

	void FModAudioEngine::resumeAllEffects()
	{
		for (auto chan : m_channels)
		{

			chan.second->setPaused(false);

		}
	}

	void FModAudioEngine::stopEffect(unsigned int nSoundId)
	{
		stopSound(nSoundId);
	}

	void FModAudioEngine::stopAllEffects()
	{
		LoopCheckBegin(a);
		while (!m_channels.empty())
		{
			LoopCheckDo(a);
			if ((m_channels.begin())->second)
			{
				(m_channels.begin())->second->stop();
			}
			else
			{
				m_channels.erase(m_channels.begin());
				//break;
			}
		}
	}

	void FModAudioEngine::preloadEffect(String strFilePath)
	{
		preloadSound(strFilePath.c_str());
	}

	void FModAudioEngine::unloadEffect(String strFilePath)
	{
		unloadSound(strFilePath.c_str());
	}

	void FModAudioEngine::updateListenerPos(const LORD::Vector3 & pos, const LORD::Vector3& face)
	{
		setListenerPos(pos, face);
	}

	void FModAudioEngine::updateSoundPos(unsigned int nSoundId, const LORD::Vector3 &pos)
	{
		setSoundPos(nSoundId, pos);
	}

	void FModAudioEngine::release()
	{
		if (m_channelGroup != NULL)
		{
			m_channelGroup->release();
			m_channelGroup = NULL;
		}
		if (m_system != NULL)
		{
			m_system->close();
			m_system->release();
			m_system = NULL;
		}
		clearSounds();
	}

	void FModAudioEngine::updateEngine()
	{
		update();
	}

	void FModAudioEngine::end()
	{
		
	}

}//LORD namespace end.
