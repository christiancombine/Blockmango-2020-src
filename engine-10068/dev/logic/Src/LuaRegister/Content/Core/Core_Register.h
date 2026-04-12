#pragma once

#ifndef __CORE_REGISTER_HEADER_H__
#define __CORE_REGISTER_HEADER_H__

#include "Core.h"
#include "LuaRegister/Template/LuaRegister.h"
#include "Math/IntMath.h"
#include "Item/ItemStack.h"
#include "Setting/GunSetting.h"
#include "Setting/SkillSetting.h"
#include "Setting/MultiLanTipSetting.h"
#include "Setting/MountSetting.h"
#include "Setting/LogicSetting.h"
#include "Audio/SoundManager.h"
#include "GameVersionDefine.h"
#include "Object/Time.h"
#include "Setting/LogicSetting.h"
#include "Setting/ActorSizeSetting.h"
#include "Setting/FilterWord.h"
#include "Util/CPUTimer.h"

using namespace LORD;
using namespace BLOCKMAN;
using namespace LUA_REGISTER;

_BEGIN_REGISTER_CLASS(GunSetting)
_CLASSREGISTER_AddStaticMember(isGunItem, GunSetting::isGunItem)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(MultiLanTipSetting)
_CLASSREGISTER_AddStaticMember(getMessageToLua, MultiLanTipSetting::getMessageToLua)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(EngineVersionSetting)
_CLASSREGISTER_AddStaticMember(getEngineVersion, EngineVersionSetting::getEngineVersion)
_CLASSREGISTER_AddStaticMember(canUseCmd, EngineVersionSetting::canUseCmd)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(CPUTimer)
_CLASSREGISTER_AddStaticMember(PrintResults, CPUTimer::PrintResults)
_CLASSREGISTER_AddStaticMember(SetEnabled, CPUTimer::SetEnabled)
_END_REGISTER_CLASS()


_BEGIN_REGISTER_CLASS(MountSetting)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddStaticMember(addSetting, MountSetting::addSetting)
_CLASSREGISTER_AddVar(id, MountSetting::id)
_CLASSREGISTER_AddVar(size, MountSetting::size)
_CLASSREGISTER_AddVar(sensitivity, MountSetting::sensitivity)
_CLASSREGISTER_AddVar(turnSpeed, MountSetting::turnSpeed)
_CLASSREGISTER_AddVar(straightenSpeed, MountSetting::straightenSpeed)
_CLASSREGISTER_AddVar(moveSpeed, MountSetting::moveSpeed)
_CLASSREGISTER_AddVar(waitStraightenTime, MountSetting::waitStraightenTime)
_CLASSREGISTER_AddVar(cameraOffset, MountSetting::cameraOffset)
_CLASSREGISTER_AddVar(cameraRotCenterOffset, MountSetting::cameraRotCenterOffset)
_CLASSREGISTER_AddVar(riderRenderOffset, MountSetting::riderRenderOffset)
_CLASSREGISTER_AddVar(riderRenderOrientation, MountSetting::riderRenderOrientation)
_CLASSREGISTER_AddVar(modelName, MountSetting::modelName)
_CLASSREGISTER_AddVar(mountBone, MountSetting::mountBone)
_CLASSREGISTER_AddVar(headTextOffset, MountSetting::headTextOffset)
_CLASSREGISTER_AddVar(maxCameraBodyAngle, MountSetting::maxCameraBodyAngle)
_CLASSREGISTER_AddVar(scale, MountSetting::scale)
_CLASSREGISTER_AddVar(rideAnimation, MountSetting::rideAnimation)
_CLASSREGISTER_AddVar(penetrateBlocks, MountSetting::penetrateBlocks)
_CLASSREGISTER_AddVar(spawnSoundId, MountSetting::spawnSoundId)
_CLASSREGISTER_AddVar(deadSoundId, MountSetting::deadSoundId)
_CLASSREGISTER_AddVar(takingSoundId, MountSetting::takingSoundId)
_CLASSREGISTER_AddVar(yOffset, MountSetting::yOffset)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(SoundSetting)
_CLASSREGISTER_AddStaticMember(getSoundSetting, SoundSetting::getSoundSetting)
_CLASSREGISTER_AddVar(nSoundId, SoundSetting::nSoundId)
_CLASSREGISTER_AddVar(bIsRepeat, SoundSetting::bIsRepeat)
_CLASSREGISTER_AddVar(fMinInterval, SoundSetting::fMinInterval)
_CLASSREGISTER_AddVar(fMaxInterval, SoundSetting::fMaxInterval)
_END_REGISTER_CLASS()

//_BEGIN_REGISTER_CLASS(Time)
//_CLASSREGISTER_AddStaticMember(Instance, Time::Instance)
//_CLASSREGISTER_AddMember(getMilliseconds, Time::getMilliseconds)
//_CLASSREGISTER_AddMember(recordMilliTime, Time::recordMilliTime)
//_CLASSREGISTER_AddMember(getMilliTimeInterval, Time::getMilliTimeInterval)
//_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(LogicSetting)
_CLASSREGISTER_AddStaticMember(Instance, LogicSetting::Instance)
_CLASSREGISTER_AddMember(getLordPlatform, LogicSetting::getLordPlatform)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(ActorSize)
_CLASSREGISTER_AddVar(width, ActorSize::width)
_CLASSREGISTER_AddVar(length, ActorSize::length)
_CLASSREGISTER_AddVar(height, ActorSize::height)
_CLASSREGISTER_AddVar(scale, ActorSize::scale)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(ActorSizeSetting)
_CLASSREGISTER_AddStaticMember(getActorSize, ActorSizeSetting::getActorSize)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(FilterWord)
_CLASSREGISTER_AddStaticMember(getReplaceContent, FilterWord::getReplaceContent)
_END_REGISTER_CLASS()

#endif