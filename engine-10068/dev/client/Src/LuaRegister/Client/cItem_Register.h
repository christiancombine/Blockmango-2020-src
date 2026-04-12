#ifndef __CITEM_REGISTER_H__
#define __CITEM_REGISTER_H__

#include "LuaRegister/Template/LuaRegister.h"
#include "cItem/cItem.h"

using namespace LUA_REGISTER;
using namespace BLOCKMAN;

_BEGIN_REGISTER_CLASS(cItem)
_CLASSREGISTER_AddStaticMember(getClientItemById, cItem::getClientItemById)
_CLASSREGISTER_AddMember(getAtlasName, cItem::getAtlasName)
_CLASSREGISTER_AddMember(setAtlasName, cItem::setAtlasName)
_END_REGISTER_CLASS()

#endif