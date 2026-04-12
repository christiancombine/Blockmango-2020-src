//
//  IOSSoundEngine.cpp
//  ZhanShenDemo
//
//  Created by zhangyi on 13-11-22.
//  Copyright (c) 2013年 zhangyi. All rights reserved.
//

#include "IOSSoundEngine.h"
#include "SimpleAudioEngine.h"

namespace LORD
{
    
    
	void IOSSoundEngine::preloadBackgroundMusic( String strFilePath )
	{
		SimpleAudioEngine::sharedEngine()->playBackgroundMusic(strFilePath.c_str());
	}
    
	void IOSSoundEngine::playBackgroundMusic( String strFilePath, bool bLoop )
	{
		SimpleAudioEngine::sharedEngine()->playBackgroundMusic(strFilePath.c_str(), bLoop);
	}
    
	void IOSSoundEngine::stopBackgroundMusic( bool bReleaseData )
	{
		SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(bReleaseData);
	}
    
	void IOSSoundEngine::pauseBackgroundMusic()
	{
		SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
	}
    
	void IOSSoundEngine::resumeBackgroundMusic()
	{
        SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
	}
    
	void IOSSoundEngine::rewindBackgroundMusic()
	{
		SimpleAudioEngine::sharedEngine()->rewindBackgroundMusic();
	}
    
	bool IOSSoundEngine::willPlayBackgroundMusic()
	{
		return SimpleAudioEngine::sharedEngine()->willPlayBackgroundMusic();
	}
    
	bool IOSSoundEngine::isBackgroundMusicPlaying()
	{
		return SimpleAudioEngine::sharedEngine()->isBackgroundMusicPlaying();
	}
    
	float IOSSoundEngine::getBackgroundMusicVolume()
	{
		return SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume();
	}
    
	void IOSSoundEngine::setBackgroundMusicVolume( float volume )
	{
		SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(volume);
	}
    
	float IOSSoundEngine::getEffectsVolume()
	{
		return SimpleAudioEngine::sharedEngine()->getEffectsVolume();
	}
    
	void IOSSoundEngine::setEffectsVolume( float volume )
	{
		SimpleAudioEngine::sharedEngine()->setEffectsVolume(volume);
	}
    
	unsigned int IOSSoundEngine::playEffect( String strFilePath, bool bLoop )
	{
		return SimpleAudioEngine::sharedEngine()->playEffect(strFilePath.c_str(), bLoop);
	}
    
	void IOSSoundEngine::pauseEffect( unsigned int nSoundId )
	{
		SimpleAudioEngine::sharedEngine()->pauseEffect(nSoundId);
	}
    
	void IOSSoundEngine::pauseAllEffects()
	{
        SimpleAudioEngine::sharedEngine()->pauseAllEffects();
	}
    
	void IOSSoundEngine::resumeEffect( unsigned int nSoundId )
	{
		SimpleAudioEngine::sharedEngine()->resumeEffect(nSoundId);
	}
    
	void IOSSoundEngine::resumeAllEffects()
	{
		SimpleAudioEngine::sharedEngine()->resumeAllEffects();
	}
    
	void IOSSoundEngine::stopEffect( unsigned int nSoundId )
	{
		SimpleAudioEngine::sharedEngine()->stopEffect(nSoundId);
	}
    
	void IOSSoundEngine::stopAllEffects()
	{
		SimpleAudioEngine::sharedEngine()->stopAllEffects();
	}
    
	void IOSSoundEngine::preloadEffect( String strFilePath )
	{
		SimpleAudioEngine::sharedEngine()->preloadEffect(strFilePath.c_str());
	}
    
	void IOSSoundEngine::unloadEffect( String strFilePath )
	{
		SimpleAudioEngine::sharedEngine()->playBackgroundMusic(strFilePath.c_str());
	}
    
    
	IOSSoundEngine::IOSSoundEngine()
	{
        
	}
    
	IOSSoundEngine::~IOSSoundEngine()
	{
        
	}
    
    void IOSSoundEngine::end()
    {
        SimpleAudioEngine::sharedEngine()->end();
    }
}//LORD namespace end.

