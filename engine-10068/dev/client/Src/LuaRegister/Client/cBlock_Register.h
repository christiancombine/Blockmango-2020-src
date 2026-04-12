#pragma once
#ifndef __CBLOCK_REGISTER_H__
#define __CBLOCK_REGISTER_H__

#include "LuaRegister/Template/LuaRegister.h"
#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"

using namespace LUA_REGISTER;
using namespace BLOCKMAN;

_BEGIN_REGISTER_CLASS(cBlock)
_CLASSREGISTER_AddMember(getAtlasName, cBlock::getAtlasName)
_CLASSREGISTER_AddMember(setSpriteName, cBlock::setSpriteName)
_CLASSREGISTER_AddMember(getSpriteName, cBlock::getSpriteName)
_CLASSREGISTER_AddMember(setStepSound, cBlock::setStepSound)
_CLASSREGISTER_AddMember(getStepSound, cBlock::getStepSound)
_CLASSREGISTER_AddMember(setNeedRender, cBlock::setNeedRender)
_CLASSREGISTER_AddMember(isNeedRender, cBlock::isNeedRender)
_CLASSREGISTER_AddMember(setPositionValue, cBlock::setPositionValue)
_CLASSREGISTER_AddMember(getPositionValue, cBlock::getPositionValue)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(cBlockManager)
_CLASSREGISTER_AddStaticMember(cGetBlockById, cBlockManager::cGetBlockById)
_END_REGISTER_CLASS()

#endif