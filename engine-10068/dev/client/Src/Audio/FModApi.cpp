#include "FModApi.h"

#ifdef WIN32
#include "Objbase.h"
#endif

#include "stdio.h"
#include "Resource/ResourceGroupManager.h"
#include "Util/PlayerPrefs.h"

namespace LORD
{
	const float DISTANCEFACTOR = 1.0f;

	unsigned int _Hash(const char *key)
	{
		unsigned int len = strlen(key);
		const char *end = key + len;
		unsigned int hash;

		for (hash = 0; key < end; key++)
		{
			hash *= 16777619;
			hash ^= (unsigned int)(unsigned char)toupper(*key);
		}
		return (hash);
	}


	 FMOD_RESULT F_CALLBACK PlayOverCallBackProxy(FMOD_CHANNELCONTROL *chanControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void *commandData1, void *commandData2)
	{
		FModApi *fmodApi = nullptr;
		FMOD::Channel *chancontrol = (FMOD::Channel *)chanControl;
		chancontrol->getUserData((void**)&fmodApi);
		if (fmodApi)
			return fmodApi->PlayOverCallBack(chanControl, controlType, callbackType, commandData1, commandData2);
		else
			return FMOD_RESULT::FMOD_ERR_INVALID_PARAM;
	}

	FModApi::FModApi()
	{
#ifdef WIN32
		CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
#endif
		m_result = FMOD::System_Create(&m_system);

		m_result = m_system->getVersion(&m_version);

		if (m_version < FMOD_VERSION)
		{
			printf("FMOD lib version %08x doesn't match header version %08x", m_version, FMOD_VERSION);
		}

		m_result = m_system->init(100, FMOD_INIT_NORMAL, 0);

		/*
			Set the distance units. (meters/feet etc).
		*/
		m_result = m_system->set3DSettings(1.0, DISTANCEFACTOR, 1.0f);

		m_system->getMasterChannelGroup(&m_channelGroup);
	}

	FModApi::~FModApi()
	{
		if (m_system != NULL)
		{
			m_system->close();
			m_system->release();
			m_system = NULL;
		}
		if (m_channelGroup != NULL)
		{
			m_channelGroup->release();
			m_channelGroup = NULL;
		}
		clearSounds();
	}


	void FModApi::preloadSound(const char* pszFilePath)
	{
		if (!pszFilePath)
		{
			return;
		}
		unsigned int nID = _Hash(pszFilePath);

		std::ifstream* soundFileStream = LordNewT(std::ifstream);
		soundFileStream->open(pszFilePath, std::ios::in | std::ios::binary);

		if (soundFileStream->fail())
		{
			LordDeleteT(soundFileStream, basic_ifstream);
			return;
		}

		LORD::DataStream* audioStream = LordNew LORD::FileStreamDataStream(soundFileStream);

		if (audioStream == NULL)
		{
			LordLogError("audio file not found.");
			return;
		}

		FMOD_CREATESOUNDEXINFO exinfo;
		memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
		exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		exinfo.length = audioStream->size();
		exinfo.fileoffset = 0;
		char* soundData = (char*)LordMalloc(exinfo.length + 1);
		audioStream->read(soundData, exinfo.length);
		m_result = m_system->createSound(soundData, FMOD_OPENMEMORY | FMOD_3D, &exinfo, &m_sounds[nID]);
		LordSafeFree(soundData);
		LordSafeFree(audioStream);
		LordDeleteT(soundFileStream, basic_ifstream);
		
		if (FMOD_OK != m_result) 	
		{
			LordLogError("audio file load error.code = %d", m_result);
			return;
		}
		m_result = m_sounds[nID]->set3DMinMaxDistance(0.5f * DISTANCEFACTOR, 1000.0f * DISTANCEFACTOR);
	}

	void FModApi::unloadSound(const char* pszFilePath)
	{
		unsigned int nSoundId = _Hash(pszFilePath);
		unloadSound(nSoundId);
	}

	void FModApi::unloadSound(unsigned int nSoundId)
	{
		auto iter = m_sounds.find(nSoundId);
		if (iter != m_sounds.end())
		{
			if (iter->second)
			{
				iter->second->release();
			}
			m_sounds.erase(nSoundId);
		}
	}

	void FModApi::unloadChannel(unsigned int nChannelId)
	{
		stopSound(nChannelId);
		m_channels.erase(nChannelId);
	}

	FMOD_RESULT FModApi::PlayOverCallBack(FMOD_CHANNELCONTROL *chanControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void *commandData1, void *commandData2)
	{
		if ((controlType == FMOD_CHANNELCONTROL_CHANNEL) && (callbackType == FMOD_CHANNELCONTROL_CALLBACK_END))
		{
			FMOD::Channel *chancontrol = (FMOD::Channel *)chanControl;
			// Channel specific functions here...
			unsigned int id = 0;
			for (auto channel : m_channels)
			{
				if (channel.second == chancontrol)
				{
					id = channel.first;
				}
			}
			m_channels.erase(id);

		}
		return FMOD_OK;
	}

	unsigned int  FModApi::play3dSound(const char* pszFilePath, unsigned int & soundId, const LORD::Vector3& pos, float volume, bool isLoop)
	{
		if (!pszFilePath)
		{
			return 0;
		}
		unsigned int nID = _Hash(pszFilePath);

		auto sound = m_sounds[nID];
		if (!sound)
		{
			preloadSound(pszFilePath);
			sound = m_sounds[nID];
			if (!sound)
			{
				soundId = 0;
				return 0;
			}
			else
			{
				soundId = nID;
			}
		}

		m_result = m_system->playSound(sound, m_channelGroup, true, &m_channels[m_channelId]);
		if (BLOCKMAN::PlayerPrefs::Instance()->getBoolPrefs("DebugSound"))
		{
			LordLogDebug("play3dSound %s %d", pszFilePath, soundId);
		}
		if (FMOD_OK != m_result) 
		{
			return 0; 
		}

		FMOD_VECTOR pos0 = { -pos.x, pos.y, pos.z };
		FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
		m_channels[m_channelId]->set3DAttributes(&pos0, &vel);
		m_channels[m_channelId]->getFrequency(&m_speeds[m_channelId]);
		m_channels[m_channelId]->setMode(isLoop ? FMOD_LOOP_NORMAL | FMOD_3D : FMOD_3D);
		m_channels[m_channelId]->set3DMinMaxDistance(1.0f, 100.0f);
		m_channels[m_channelId]->setPaused(false);
		m_channels[m_channelId]->setCallback(PlayOverCallBackProxy);
		m_channels[m_channelId]->setUserData(this);
		m_channels[m_channelId]->setVolume(volume);

		return m_channelId++;
	}

	unsigned int  FModApi::play2dSound(const char* pszFilePath, unsigned int & soundId, float volume, bool isLoop, unsigned int nParam/* = 0*/)
	{
		if (!pszFilePath)
		{
			return 0;
		}
		unsigned int nID = _Hash(pszFilePath);

		auto sound = m_sounds[nID];
		if (!sound)
		{
			preloadSound(pszFilePath);
			sound = m_sounds[nID];
			if (!sound)
			{ 
				soundId = 0;
				return 0;
			} 
			else
			{
				soundId = nID;
			}
		}

		int channelId = m_channelId; //保存之前的channelId
		if (nParam != 0)
		{
			m_channelId = nParam;
		}

		m_result = m_system->playSound(sound, m_channelGroup, true, &m_channels[m_channelId]);
		if (BLOCKMAN::PlayerPrefs::Instance()->getBoolPrefs("DebugSound"))
		{
			LordLogDebug("play2dSound %s %d", pszFilePath, soundId);
		}
		if (FMOD_OK != m_result) 
		{
			return 0; 
		}

		m_channels[m_channelId]->getFrequency(&m_speeds[m_channelId]);
		m_channels[m_channelId]->setMode(isLoop ? FMOD_LOOP_NORMAL | FMOD_2D : FMOD_2D);
		m_channels[m_channelId]->setPaused(false);

		auto mycallback = [this](FMOD_CHANNELCONTROL *chanControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void *commandData1, void *commandData2) {
			return PlayOverCallBack(chanControl, controlType, callbackType, commandData1, commandData2);
		};
		m_channels[m_channelId]->setCallback((FMOD_CHANNELCONTROL_CALLBACK)PlayOverCallBackProxy);
		m_channels[m_channelId]->setUserData(this);
		m_channels[m_channelId]->setVolume(volume);

		if (nParam != 0)
		{
			m_channelId = channelId; //还原channelId
			return nParam;
		}
		else
		{
			return m_channelId++;
		}
	}

	void FModApi::stopSound(unsigned int nChannelId)
	{
		auto iter = m_channels.find(nChannelId);
		if (iter != m_channels.end() && iter->second)
		{
			iter->second->stop();
		}
	}

	void FModApi::pauseSound(unsigned int nSoundId)
	{
		auto iter = m_channels.find(nSoundId);
		if (iter != m_channels.end())
		{
			if (iter->second)
			{
				iter->second->setPaused(true);
			}
		}
	}

	void FModApi::rewindSound(unsigned int nSoundId)
	{
		auto iter = m_channels.find(nSoundId);
		if (iter != m_channels.end())
		{
			if (iter->second)
			{
				iter->second->setPosition(0, FMOD_TIMEUNIT_MS);
			}
		}
	}

	void FModApi::resumeSound(unsigned int nSoundId)
	{
		auto iter = m_channels.find(nSoundId);
		if (iter != m_channels.end())
		{
			if (iter->second)
			{
				iter->second->setPaused(false);
			}
		}
	}

	bool FModApi::isPlaying(unsigned int nSoundId)
	{
		bool isplaying = false;
		auto iter = m_channels.find(nSoundId);
		if (iter != m_channels.end())
		{
			if (iter->second)
			{
				iter->second->isPlaying(&isplaying);
			}
		}
		return isplaying;
	}


	void FModApi::setSoundsVolume(unsigned int nSoundId, float volume)
	{
		auto iter = m_channels.find(nSoundId);
		if (iter != m_channels.end())
		{
			if (iter->second)
			{
				iter->second->setVolume(volume);
			}
		}
	}

	bool FModApi::setSoundPos(unsigned int nSoundId, const LORD::Vector3& pos)
	{
		auto iter = m_channels.find(nSoundId);
		if (iter != m_channels.end())
		{
			if (iter->second)
			{
				FMOD_VECTOR pos0 = { -pos.x, pos.y, pos.z };
				FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
				iter->second->set3DAttributes(&pos0, &vel);
				return true;
			}
		}
		return false;
	}

	void FModApi::setSoundSpeed(unsigned int nSoundId, float percentage)
	{
		auto iter = m_channels.find(nSoundId);
		if (iter != m_channels.end())
		{
			if (iter->second)
			{
				iter->second->setFrequency(m_speeds[nSoundId] * percentage);
			}
		}
	}

	void FModApi::setListenerPos(const LORD::Vector3& pos, const LORD::Vector3& forward)
	{
		FMOD_VECTOR listenerpos = { -pos.x , pos.y , pos.z };
		FMOD_VECTOR forward0 = { -forward.x, forward.y, forward.z };
		FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };
		FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
		m_system->set3DListenerAttributes(0, &listenerpos, &vel, &forward0, &up);
	}

	void FModApi::update()
	{
		m_system->update();
	}

	void FModApi::allMute(bool isMute)
	{
		m_channelGroup->setMute(isMute);
	}

	void FModApi::setGlobalVolume(float volume)
	{
		m_channelGroup->setVolume(volume);
	}

	float FModApi::getSoundVolume(unsigned int nSoundId)
	{
		auto iter = m_channels.find(nSoundId);
		float volume = 0.0f;
		if (iter != m_channels.end())
		{
			if (iter->second)
			{
				iter->second->getVolume(&volume);
			}
		}
		return volume;
	}

	void FModApi::setSoundVolume(unsigned int nSoundId, float volume)
	{
		auto iter = m_channels.find(nSoundId);
		if (iter != m_channels.end())
		{
			if (iter->second)
			{
				iter->second->setVolume(volume);
			}
		}
	}

	void FModApi::clearSounds()
	{
		m_sounds.clear();
		m_channels.clear();
	}

}