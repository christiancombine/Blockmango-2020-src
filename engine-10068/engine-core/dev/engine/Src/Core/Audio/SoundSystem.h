/********************************************************************
filename: 	SoundSystem.h
file path:	dev\engine\Src\Core\Audio\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef  _SOUNDSYSTEM_H_
#define  _SOUNDSYSTEM_H_

#include "Core.h"
#include "Object/Singleton.h"
#include "Resource/ResourceGroupManager.h"
#include "SoundManager.h"
#include "SoundDef.h"

namespace LORD
{
	class SoundEngine;
	class BaseThreadEvent;
	class LORD_CORE_API SoundSystem : public Singleton<SoundSystem>, public ObjectAlloc
	{
	public:
		SoundSystem();
		~SoundSystem();

		inline static SoundSystem* Instance()
		{
			return ms_pSingleton;
		}

		void SetSoundEngine(SoundEngine* pSoundEngine);

		void RemoveSoundEngine();

		void EndSoundEngine();

		void setSoundOn(bool setting) {

			if (setting)
			{
				resumeAllEffects();
				resumeBackgroundMusic();
			}
			else
			{
				pauseBackgroundMusic();
				pauseAllEffects();
			}

			m_soundSwitch = setting;
		}

		void setSoundPause(bool bPause) { m_soundPause = bPause; }

		bool isSoundOn() const { return m_soundSwitch; }

		void setBackgroundMusicSwitch(bool isOn) {
			if (isOn)
			{
				resumeBackgroundMusic();
			}
			else
			{
				pauseBackgroundMusic();
			}
			m_backgroundMusicSwitch = isOn;
		}

		bool isSwitchOnBackgroundMusic() {
			return m_backgroundMusicSwitch;
		}
		bool checkDownload(const String& filePath);
		bool onProcess(const String& path, int soundType);
		bool onRespond(const String& path, int soundType);


	protected:
		SoundEngine* m_pSoundEngine;
		SoundManager		m_soundManager;

		bool				m_soundSwitch;
		bool                m_soundPause = false;
		bool				m_backgroundMusicSwitch = true;

		SoundType			m_curBgmSoundType = SoundType::ST_Loading;

		String getFilePath(const String& name);

		std::map<String, BaseThreadEvent*> m_eventMap;

	public:
		/**
		@brief Play cur background music
		*/
		void playBackgroundMusic();

		/**
		@brief Stop playing background music
		*/
		void stopBackgroundMusic();

		/**
		@brief Pause playing background music
		*/
		void pauseBackgroundMusic();

		/**
		@brief Resume playing background music
		*/
		void resumeBackgroundMusic();

		/**
		@brief Rewind playing background music
		*/
		void rewindBackgroundMusic();

		bool willPlayBackgroundMusic();

		/**
		@brief Whether the background music is playing
		@return If is playing return true,or return false
		*/
		bool isBackgroundMusicPlaying();

		// properties
		/**
		@brief The volume of the background music max value is 1.0,the min value is 0.0
		*/
		float getBackgroundMusicVolume();

		/**
		@brief set the volume of background music
		@param volume must be in 0.0~1.0
		*/
		void setBackgroundMusicVolume(float volume);

		/**
		@brief The volume of the effects max value is 1.0,the min value is 0.0
		*/
		float getEffectsVolume();

		/**
		@brief set the volume of sound effecs
		@param volume must be in 0.0~1.0
		*/
		void setEffectsVolume(float volume);

		// for sound effects
		// ֱ��ͨ����Ч����ID����������
		unsigned int playEffectByType(SoundType soundType, const LORD::Vector3& pos = LORD::Vector3::INVALID, unsigned int nParam = 0);

		unsigned int playEffectByType(int soundType, const LORD::Vector3& pos = LORD::Vector3::INVALID) { return playEffectByType((SoundType)soundType, pos); }

		/**
		@brief Pause playing sound effect
		@param nSoundId The return value of function playEffect
		*/
		void pauseEffect(unsigned int nSoundId);

		/**
		@brief Pause all playing sound effect
		@param nSoundId The return value of function playEffect
		*/
		void pauseAllEffects();

		/**
		@brief Resume playing sound effect
		@param nSoundId The return value of function playEffect
		*/
		void resumeEffect(unsigned int nSoundId);

		/**
		@brief Resume all playing sound effect
		@param nSoundId The return value of function playEffect
		*/
		void resumeAllEffects();

		/**
		@brief Stop playing sound effect
		@param nSoundId The return value of function playEffect
		*/
		void stopEffect(unsigned int nSoundId);

		/**
		@brief Stop all playing sound effects
		*/
		void stopAllEffects();

		/**
		@brief          preload a compressed audio file
		@details        the compressed audio will be decode to wave, then write into an
		internal buffer in SimpleaudioEngine
		*/
		void preloadEffect(const String& soundName);

		/**
		@brief          unload the preloaded effect from internal buffer
		@param[in]        pszFilePath        The path of the effect file,or the FileName of T_SoundResInfo
		*/
		void unloadEffect(const String& soundName);

		void update3DSoundListenerPos(const LORD::Vector3& pos, const LORD::Vector3& face);

		void updateEngine();

		void updateSoundPos(unsigned int nSoundId, const LORD::Vector3& pos);
	};
}

#endif
