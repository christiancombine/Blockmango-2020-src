//Author:LIGA lijia-liao@outlook.com

#include "fmod.hpp"
#include <stddef.h>
#include <typeinfo>
#include <ctype.h>
#include <string.h>
#include "Core.h"
#include<map>
//using namespace std;


namespace LORD
{
	class FModApi
	{
	protected:
		FMOD::System    *m_system = NULL;
		FMOD_RESULT      m_result;
		unsigned int     m_version;
		std::map<unsigned int, FMOD::Sound* > m_sounds;
		std::map<unsigned int, float > m_speeds;
		unsigned int m_channelId=1;
		FMOD::ChannelGroup *m_channelGroup = NULL;
		std::map<unsigned int, FMOD::Channel* > m_channels;

	public:
		FModApi();
		~FModApi();
		
		void preloadSound(const char* pszFilePath);
		void unloadSound(const char* pszFilePath);
		void unloadSound(unsigned int nSoundId);
		void unloadChannel(unsigned int nChannelId);
		unsigned int play3dSound(const char* pszFilePath, unsigned int & soundId, const LORD::Vector3& pos, float volume, bool isLoop = false);
		unsigned int play2dSound(const char* pszFilePath, unsigned int & soundId, float volume, bool isLoop = false, unsigned int nParam = 0);
		void stopSound(unsigned int nChannelId);
		void setSoundsVolume(unsigned int nSoundId, float volume);
		bool setSoundPos(unsigned int nSoundId, const LORD::Vector3& pos);
		void setSoundSpeed(unsigned int nSoundId, float percentage);
		void resumeSound(unsigned int nSoundId);
		void pauseSound(unsigned int nSoundId);
		void rewindSound(unsigned int nSoundId);
		bool isPlaying(unsigned int nSoundId);
		void setListenerPos(const LORD::Vector3& pos, const LORD::Vector3& forward = LORD::Vector3(0.0F, 0.0F, 1.0F));
		void update();
		void allMute(bool isMute);
		void setGlobalVolume(float volume);
		float getSoundVolume(unsigned int nSoundId);
		void  setSoundVolume(unsigned int nSoundId, float volume);
		void clearSounds();
		FMOD_RESULT   PlayOverCallBack(FMOD_CHANNELCONTROL *chanControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void *commandData1, void *commandData2);

	};
} 
