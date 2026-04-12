#include "SoundSystem.h"
#include "SoundEngine.h"
#include "Core.h"
#include "Object/Root.h"
//#include "Resource/ZipResourceManager.h"
//#include "Resource/ThreadEvents.h"

namespace LORD
{
	SoundSystem::SoundSystem()
		: m_pSoundEngine(NULL)
		, m_soundSwitch(true)
	{
		m_soundManager.scanSounds();
	}

	SoundSystem::~SoundSystem()
	{
		/*for (const auto& item : m_eventMap)
		{
			if (item.second->m_pRequest)
			{
				item.second->m_pRequest->Abort();
				item.second->SetDeath();
			}
		}*/
		m_eventMap.clear();
		RemoveSoundEngine();
	}

	void SoundSystem::SetSoundEngine(SoundEngine* pSoundEngine)
	{
		if (m_pSoundEngine != NULL)
		{
			RemoveSoundEngine();
		}
		m_pSoundEngine = pSoundEngine;
	}

	void SoundSystem::RemoveSoundEngine()
	{
		stopAllEffects();
		stopBackgroundMusic();
		EndSoundEngine();
		if (m_pSoundEngine != NULL)
		{
			m_pSoundEngine->release();
			LordDelete m_pSoundEngine;
			m_pSoundEngine = NULL;
		}
	}

	bool SoundSystem::checkDownload(const String& filePath)
	{
		/*if (!FileResourceManager::Instance() || !ResourceDownloadManager::Instance() || filePath.empty()) return false;

		if (FileResourceManager::Instance()->CheckMD5(filePath))
		{
			if (!ResourceDownloadManager::Instance()->resourceIsDownloading(filePath))
			{
				return ResourceDownloadManager::Instance()->tryDownloadResource(filePath, this);
			}
			return true;
		}*/
		return false;
	}

	bool SoundSystem::onProcess(const String& path, int soundType)
	{
		if (!FileResourceManager::Instance() || path.empty()) return true;

		if (!FileResourceManager::Instance()->ResourceExistInAny(path))
			return false;

		return true;
	}

	bool SoundSystem::onRespond(const String& path, int soundType)
	{
		if (FileResourceManager::Instance() && FileResourceManager::Instance()->ResourceExistInAny(path))
		{
			if (soundType == m_curBgmSoundType)
			{
				playEffectByType(soundType);
			}
		}
		if (m_eventMap.find(path) != m_eventMap.end())
		{
			m_eventMap.erase(path);
		}
		return true;
	}

	String SoundSystem::getFilePath(const String& name)
	{
		return m_soundManager.getRandomSoundFile(name);
	}

	void SoundSystem::playBackgroundMusic()
	{
		playEffectByType(m_curBgmSoundType);
	}

	void SoundSystem::stopBackgroundMusic()
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->stopBackgroundMusic();
		}
	}

	void SoundSystem::pauseBackgroundMusic()
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->pauseBackgroundMusic();
		}
	}

	void SoundSystem::resumeBackgroundMusic()
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->resumeBackgroundMusic();
		}
	}

	void SoundSystem::rewindBackgroundMusic()
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->rewindBackgroundMusic();
		}
	}

	bool SoundSystem::willPlayBackgroundMusic()
	{
		if (m_pSoundEngine)
		{
			return m_pSoundEngine->willPlayBackgroundMusic();
		}

		return false;
	}

	bool SoundSystem::isBackgroundMusicPlaying()
	{
		if (m_pSoundEngine)
		{
			return m_pSoundEngine->isBackgroundMusicPlaying();
		}

		return false;
	}

	float SoundSystem::getBackgroundMusicVolume()
	{
		if (m_pSoundEngine)
		{
			return m_pSoundEngine->getBackgroundMusicVolume();
		}

		return 0.0f;
	}

	void SoundSystem::setBackgroundMusicVolume(float volume)
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->setBackgroundMusicVolume(volume);
		}
	}

	float SoundSystem::getEffectsVolume()
	{
		if (m_pSoundEngine)
		{
			return m_pSoundEngine->getEffectsVolume();
		}

		return 0.0f;
	}

	void SoundSystem::setEffectsVolume(float volume)
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->setEffectsVolume(volume);
		}
	}

	unsigned int SoundSystem::playEffectByType(SoundType soundType, const LORD::Vector3& pos, unsigned int nParam/* = 0*/)
	{
		if (soundType <= ST_Invalid || soundType >= ST_Total)
			return 0;

		if (soundType >= ST_BgmStart)
		{
			m_curBgmSoundType = soundType;  // save bgm sound type
		}

		if (m_soundPause)
			return 0;

		if (m_pSoundEngine == NULL)
			return 0;

		SoundSetting* pSetting = SoundSetting::getSoundSetting((int)soundType);
		if (!pSetting)
		{
			LordLogWarning("SoundType [%d] does not exist!!!", (int)soundType);
			return 0;
		}

		String filePath = pSetting->GetRandomPath();
		if (FileResourceManager::Instance()->ResourceExistInAny(filePath))
		{
			if (soundType >= ST_BgmStart)
			{
				m_pSoundEngine->playBackgroundMusic(filePath, pSetting->bIsRepeat, pSetting->fMinInterval, pSetting->fMaxInterval, nParam);
			}
			else
			{
				return m_pSoundEngine->playEffect(filePath, pSetting->bIsRepeat, pSetting->fMinInterval, pSetting->fMaxInterval, pSetting->fVolume, pos, nParam);
			}
		}
		else
		{
			LordLogWarning("sound file of sound type %d is not found", (int)soundType);
		}

		return 0;
	}

	void SoundSystem::pauseEffect(unsigned int nSoundId)
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->pauseEffect(nSoundId);
		}
	}

	void SoundSystem::pauseAllEffects()
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->pauseAllEffects();
		}
	}

	void SoundSystem::resumeEffect(unsigned int nSoundId)
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->resumeEffect(nSoundId);
		}
	}

	void SoundSystem::resumeAllEffects()
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->resumeAllEffects();
		}
	}

	void SoundSystem::stopEffect(unsigned int nSoundId)
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->stopEffect(nSoundId);
		}
	}

	void SoundSystem::stopAllEffects()
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->stopAllEffects();
		}
	}

	void SoundSystem::preloadEffect(const String& soundName)
	{
		if (m_pSoundEngine)
		{
			auto filePath = getFilePath(soundName);
			if (FileResourceManager::Instance()->ResourceExistInAny(filePath))
			{
				m_pSoundEngine->preloadEffect(filePath);
			}
			else
			{
				LordLogWarning("sound file is not found: %s", filePath.c_str());
				checkDownload(filePath);
			}
		}
	}

	void SoundSystem::unloadEffect(const String& soundName)
	{
		if (m_pSoundEngine)
		{
			auto filePath = getFilePath(soundName);
			if (FileResourceManager::Instance()->ResourceExistInAny(filePath))
			{
				m_pSoundEngine->unloadEffect(filePath);
			}
			else
			{
				LordLogWarning("sound file is not found: %s", filePath.c_str());
				checkDownload(filePath);
			}
		}
	}

	void SoundSystem::update3DSoundListenerPos(const LORD::Vector3& pos, const LORD::Vector3& face)
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->updateListenerPos(pos, face);
		}
	}

	void SoundSystem::updateEngine()
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->updateEngine();
		}

	}

	void SoundSystem::updateSoundPos(unsigned int nSoundId, const LORD::Vector3& pos)
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->updateSoundPos(nSoundId, pos);
		}
	}

	void SoundSystem::EndSoundEngine()
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->end();
		}
	}

}//LORD namespace end.
