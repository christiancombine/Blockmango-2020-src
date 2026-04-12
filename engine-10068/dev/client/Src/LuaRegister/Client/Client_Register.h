#pragma once

#ifndef _CLIENT_REGISTER_H__
#define _CLIENT_REGISTER_H__

#include "LuaRegister/Template/LuaRegister.h"
#include "cWorld/WorldClient.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSP.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Util/LanguageManager.h"
#include "Audio/SoundSystem.h"
#include "Setting/GuideSystemSetting.h"
#include "Util/UserManager.h"
#include "game.h"
#include "Render/HeadTextRenderer.h"
#include "Render/WorldImageRender.h"
#include "Render/TextureAtlas.h"
#include "cEntity/EntityOtherPlayerMP.h"
#include "EntityRenders/EntityRenderManager.h"
#include "Actor/ActorObject.h"
#include "Util/ClientCmdMgr.h"
#include "World/GameSettings.h"
#include "World/WorldEffectManager.h"
#include "Render/RenderGlobal.h"
#include "Object/Root.h"
#include "Scene/SceneManager.h"
#include "Util/FriendManager.h"

using namespace LUA_REGISTER;
using namespace GameClient;
using namespace BLOCKMAN;

_BEGIN_REGISTER_CLASS(CGame)
_CLASSREGISTER_AddStaticMember(Instance, CGame::Instance)
_CLASSREGISTER_AddMember(isEnableIndie, CGame::isEnableIndie)
_CLASSREGISTER_AddMember(enableIndie, CGame::enableIndie)
_CLASSREGISTER_AddMember(getShellInterface, CGame::getShellInterface)
_CLASSREGISTER_AddMember(getPlayerName, CGame::getPlayerName)
_CLASSREGISTER_AddMember(getPlatformUserId, CGame::getPlatformUserId)
_CLASSREGISTER_AddMember(SetMaxFps, CGame::SetMaxFps)
_CLASSREGISTER_AddMember(getPicUrl, CGame::getPicUrl)
_CLASSREGISTER_AddMember(getBaseUrl, CGame::getBaseUrl)
_CLASSREGISTER_AddMember(getGameType, CGame::getGameType)
_CLASSREGISTER_AddMember(exitGame, CGame::exitGame)
_CLASSREGISTER_AddMember(toggleDebugMessageShown, CGame::toggleDebugMessageShown)
_CLASSREGISTER_AddMember(handleTouchClick, CGame::handleTouchClick)
_CLASSREGISTER_AddMember_Override(resetGame, CGame::resetGame, void, String, ui64, String)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(GameSettings)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddVar(usePole, GameSettings::usePole)
_CLASSREGISTER_AddVar(gammaSetting, GameSettings::gammaSetting)
_CLASSREGISTER_AddVar(playerActivityGuiSize, GameSettings::playerActivityGuiSize)
_CLASSREGISTER_AddMember(setFovSetting, GameSettings::setFovSetting)
_CLASSREGISTER_AddMember(setCameraSensitive, GameSettings::setCameraSensitive)
_CLASSREGISTER_AddMember(isMouseMoving, GameSettings::isMouseMoving)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(Blockman)
_CLASSREGISTER_AddStaticMember(Instance, Blockman::Instance)
_CLASSREGISTER_AddMember(getWorld, Blockman::getWorld)
_CLASSREGISTER_AddMember(getPlayer, Blockman::getPlayer)
_CLASSREGISTER_AddMember(setPersonView, Blockman::setPersonView)
_CLASSREGISTER_AddMember(getCurrPersonView, Blockman::getCurrPersonView)
_CLASSREGISTER_AddMember(switchPerspece, Blockman::switchPerspece)
_CLASSREGISTER_AddMember(setBloomThreshold, Blockman::setBloomThreshold)
_CLASSREGISTER_AddMember(setBloomSaturation, Blockman::setBloomSaturation)
_CLASSREGISTER_AddMember(setBloomIntensity, Blockman::setBloomIntensity)
_CLASSREGISTER_AddMember(setBloomBlurDeviation, Blockman::setBloomBlurDeviation)
_CLASSREGISTER_AddMember(setBloomBlurMultiplier, Blockman::setBloomBlurMultiplier)
_CLASSREGISTER_AddMember(enableFullscreenBloom, Blockman::enableFullscreenBloom)
_CLASSREGISTER_AddMember(setBlockLightEnhancement, Blockman::setBlockLightEnhancement)
_CLASSREGISTER_AddMember(setBlockBloomOption, Blockman::setBlockBloomOption)
_CLASSREGISTER_AddVar(m_gameSettings, Blockman::m_gameSettings)
_CLASSREGISTER_AddVar(m_globalRender, Blockman::m_globalRender)
_CLASSREGISTER_AddVar(renderViewEntity, Blockman::renderViewEntity)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(RenderGlobal)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(luaAddWallText, RenderGlobal::luaAddWallText)
_CLASSREGISTER_AddMember(deleteWallText, RenderGlobal::deleteWallText)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(WorldClient)
_CLASSREGISTER_AddBaseClass(World)
_CLASSREGISTER_AddMember_Override(setBlock, WorldClient::setBlock, bool, const BlockPos &, int, int, int, bool)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(EntityPlayerSP)
_CLASSREGISTER_AddBaseClass(EntityPlayer)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(EntityPlayerSPMP)
_CLASSREGISTER_AddBaseClass(EntityPlayerSP)
_CLASSREGISTER_AddMember(swingItem, EntityPlayerSPMP::swingItem)
_CLASSREGISTER_AddMember(catchEntity, EntityPlayerSPMP::catchEntity)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(EntityOtherPlayerMP)
_CLASSREGISTER_AddBaseClass(EntityPlayer)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(EntityRenderManager)
_CLASSREGISTER_AddStaticMember(Instance, EntityRenderManager::Instance)
_CLASSREGISTER_AddMember(getEntityActor, EntityRenderManager::getEntityActor)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(ActorObject)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(GetScale, ActorObject::GetScale)
//_CLASSREGISTER_AddMember(GetTotalScale, ActorObject::GetTotalScale)
//_CLASSREGISTER_AddMember(GetEntityScale, ActorObject::GetEntityScale)
_CLASSREGISTER_AddMember(AddPluginEffect, ActorObject::AddPluginEffect)
//_CLASSREGISTER_AddMember(AddPluginEffect1, ActorObject::AddPluginEffect1)
_CLASSREGISTER_AddMember(DelPluginEffect, ActorObject::DelPluginEffect)
_CLASSREGISTER_AddMember(PlaySkill, ActorObject::PlaySkill)
_CLASSREGISTER_AddMember(GetSkillTimeLength, ActorObject::GetSkillTimeLength)
_CLASSREGISTER_AddMember(setActorHide, ActorObject::setActorHide)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(LanguageManager)
_CLASSREGISTER_AddStaticMember(Instance, LanguageManager::Instance)
_CLASSREGISTER_AddMember(getString, LanguageManager::getString)
_CLASSREGISTER_AddMember(getCurrentLanguage, LanguageManager::getCurrentLanguage)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(SoundSystem)
_CLASSREGISTER_AddStaticMember(Instance, SoundSystem::Instance)
_CLASSREGISTER_AddMember_Override(playEffectByType, SoundSystem::playEffectByType, unsigned int, int, const LORD::Vector3 &)
_CLASSREGISTER_AddMember(setBackgroundMusicSwitch, SoundSystem::setBackgroundMusicSwitch)
_CLASSREGISTER_AddMember(setBackgroundMusicVolume, SoundSystem::setBackgroundMusicVolume)
_CLASSREGISTER_AddMember(setEffectsVolume, SoundSystem::setEffectsVolume)
_CLASSREGISTER_AddMember(stopEffect, SoundSystem::stopEffect)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(GuideSystemSetting)
_CLASSREGISTER_AddStaticMember(onWindowShown, GuideSystemSetting::onWindowShown)
_CLASSREGISTER_AddStaticMember(onGuideDone, GuideSystemSetting::onGuideDone)
_CLASSREGISTER_AddStaticMember_Override(onGuideTriggered, GuideSystemSetting::onGuideTriggered, void, int)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(UserManager)
_CLASSREGISTER_AddStaticMember(Instance, UserManager::Instance)
_CLASSREGISTER_AddMember(findUser, UserManager::findUser)
_CLASSREGISTER_AddMember(getAllUser, UserManager::getAllUser)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(User)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddVar(sex, User::sex)
_CLASSREGISTER_AddVar(vip, User::vip)
_CLASSREGISTER_AddVar(teamId, User::teamId)
_CLASSREGISTER_AddVar(userId, User::userId)
_CLASSREGISTER_AddVar(isSend, User::isSend)
_CLASSREGISTER_AddVar(isFriend, User::isFriend)
_CLASSREGISTER_AddVar(isFriendRequest, User::isFriendRequest)
_CLASSREGISTER_AddVar(userName, User::userName)
_CLASSREGISTER_AddVar(teamName, User::teamName)
_CLASSREGISTER_AddVar(picUrl, User::picUrl)
_CLASSREGISTER_AddVar(message, User::message)
_CLASSREGISTER_AddVar(country, User::country)
_CLASSREGISTER_AddVar(language, User::language)
_CLASSREGISTER_AddVar(teamViewName, User::teamViewName)
_CLASSREGISTER_AddVar(isRobot, User::isRobot)
_END_REGISTER_CLASS()


_BEGIN_REGISTER_CLASS(ClientCmdMgr)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(WorldEffectManager)
_CLASSREGISTER_AddStaticMember(Instance, WorldEffectManager::Instance)
_CLASSREGISTER_AddMember(addSimpleEffect, WorldEffectManager::addSimpleEffect)
_CLASSREGISTER_AddMember(removeSimpleEffect, WorldEffectManager::removeSimpleEffect)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(SimpleEffect)
_CLASSREGISTER_AddVar(mEffectName, SimpleEffect::mEffectName)
_CLASSREGISTER_AddVar(mTemplateName, SimpleEffect::mTemplateName)
_CLASSREGISTER_AddVar(mPosition, SimpleEffect::mPosition)
_CLASSREGISTER_AddVar(mYaw, SimpleEffect::mYaw)
_CLASSREGISTER_AddVar(mPitch, SimpleEffect::mPitch)
_CLASSREGISTER_AddVar(mDuration, SimpleEffect::mDuration)
_CLASSREGISTER_AddVar(mScaling, SimpleEffect::mScaling)
_CLASSREGISTER_AddVar(mAutoScale, SimpleEffect::mAutoScale)
_CLASSREGISTER_AddVar(mEnableLocalPos, SimpleEffect::mEnableLocalPos)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(WorldImageRender)
_CLASSREGISTER_AddStaticMember(Instance, WorldImageRender::Instance)
_CLASSREGISTER_AddMember(addImageRender, WorldImageRender::addImageRender)
_CLASSREGISTER_AddMember(addFloatNumber, WorldImageRender::addFloatNumber)
_CLASSREGISTER_AddMember(removeAllFloatNumber, WorldImageRender::removeAllFloatNumber)
_CLASSREGISTER_AddMember(addStaticNumber, WorldImageRender::addStaticNumber)
_CLASSREGISTER_AddMember(removeStaticNumber, WorldImageRender::removeStaticNumber)
_CLASSREGISTER_AddMember(addStaticImage, WorldImageRender::addStaticImage)
_CLASSREGISTER_AddMember(removeStaticImage, WorldImageRender::removeStaticImage)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(TextureAtlasRegister)
_CLASSREGISTER_AddStaticMember(Instance, TextureAtlasRegister::Instance)
_CLASSREGISTER_AddMember(addTextureAtlas, TextureAtlasRegister::addTextureAtlas)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(SceneManager)
_CLASSREGISTER_AddStaticMember(Instance, SceneManager::Instance)
_CLASSREGISTER_AddMember(getMainCamera, SceneManager::getMainCamera)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(Camera)
_CLASSREGISTER_AddMember(getPosition, Camera::getPosition)
_CLASSREGISTER_AddMember(getDirection, Camera::getDirection)
_CLASSREGISTER_AddMember(getRight, Camera::getRight)
_CLASSREGISTER_AddMember(getUp, Camera::getUp)
_END_REGISTER_CLASS()

#endif